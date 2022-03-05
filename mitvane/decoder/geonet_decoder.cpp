/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/geonet/router.cpp>
 * at 2022-02-25.
 * 
 * This file is part of Mitvane.
 *
 * Mitvane is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or 
 * any later version.
 * Mitvane is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License and 
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * and GNU Lesser General Public License along with Mitvane. 
 * If not, see <https://www.gnu.org/licenses/>. 
 */

/* 
 * State Changes
 * - Several functions which are not necessary for decoding are removed.
 * - Data storing for detection module in IDS/IPS are added.
 * - Function names are changed from indicate_[basic|common|extended|secured]
 *   to decode_[basic|common|extended|secured]
*/


#include "geonet_decoder.hpp"
#include <vanetza/geonet/indication_context.hpp>



using namespace vanetza;
using namespace vanetza::geonet;
using namespace vanetza::security;

namespace mitvane {

GeonetDecoder::GeonetDecoder(const vanetza::geonet::MIB& mib, DetectionContext& detection_context) :  
    m_mib(mib), m_detection_context(detection_context){}

void inspect_signer_info(const SecuredMessage* secured_message, DetectionContext& detection_context)
{
    detection_context.geonet_data.payload_type = secured_message->payload.type;
    detection_context.geonet_data.secure_protocol_version = secured_message->protocol_version();
    
    const SignerInfo* signer_info = secured_message->header_field<HeaderFieldType::Signer_Info>();

    if(signer_info){
        HashedId8 signer_hash;
        signer_hash.fill(0x00);
        SignerInfoType signer_info_type = get_type(*signer_info);
        detection_context.geonet_data.signer_info_type = signer_info_type;
        switch(signer_info_type){
            case SignerInfoType::Certificate:
            {
                signer_hash = calculate_hash(boost::get<Certificate>(*signer_info));
                detection_context.geonet_data.certificate_id = signer_hash;
                break;
            }
            case SignerInfoType::Certificate_Digest_With_SHA256:
            {
                signer_hash = boost::get<HashedId8>(*signer_info);
                detection_context.geonet_data.certificate_id = signer_hash;
                break;
            }
            case SignerInfoType::Certificate_Chain:
            {
                std::list<Certificate> chain = boost::get<std::list<Certificate>>(*signer_info);
                detection_context.geonet_data.chain_size = chain.size();
                signer_hash = calculate_hash(chain.back());
                detection_context.geonet_data.certificate_id = signer_hash;
                break;
            }
            default:
                // logging
                break;
        }
    }

}

void GeonetDecoder::decode(UpPacketPtr packet, const vanetza::MacAddress& sender, const vanetza::MacAddress& destination)
{
    assert(packet);

    struct indication_visitor : public boost::static_visitor<>
    {
        indication_visitor(GeonetDecoder& decoder, const IndicationContext::LinkLayer& link_layer, UpPacketPtr packet) :
            m_decoder(decoder), m_link_layer(link_layer), m_packet(std::move(packet))
        {
        }

        void operator()(vanetza::CohesivePacket& packet)
        {
            IndicationContextDeserialize ctx(std::move(m_packet), packet, m_link_layer);
            m_decoder.decode_basic(ctx);
        }

        void operator()(vanetza::ChunkPacket& packet)
        {
            IndicationContextCast ctx(std::move(m_packet), packet, m_link_layer);
            m_decoder.decode_basic(ctx);
        }

        GeonetDecoder& m_decoder;
        const IndicationContext::LinkLayer& m_link_layer;
        UpPacketPtr m_packet;
    };

    IndicationContext::LinkLayer link_layer;
    link_layer.sender = sender;
    link_layer.destination = destination;

    UpPacket* packet_ptr = packet.get();
    indication_visitor visitor(*this, link_layer, std::move(packet));
    boost::apply_visitor(visitor, *packet_ptr);
}

void GeonetDecoder::decode_basic(IndicationContextBasic& ctx)
{
    const BasicHeader* basic = ctx.parse_basic();
    if (!basic) {
        // logging
    } else {
        m_detection_context.geonet_data.protocol_version = basic->version.raw();
        // Store lifetime
        m_detection_context.geonet_data.remaining_packet_lifetime = basic->lifetime;
        // Store RHL (Remaining Hop Limit)
        m_detection_context.geonet_data.remaining_hop_limit = basic->hop_limit;

        if (basic->next_header == NextHeaderBasic::Secured) {
            m_detection_context.geonet_data.security_info = SecurityInfo::Signed;
            decode_secured(ctx, *basic);
        } else if (basic->next_header == NextHeaderBasic::Common) {
            m_detection_context.geonet_data.security_info = SecurityInfo::NoSecurity;
            decode_common(ctx, *basic);
        } else {
            // logging
        }
    }
}

void GeonetDecoder::decode_common(IndicationContext& ctx, const BasicHeader& basic)
{
    const CommonHeader* common = ctx.parse_common();
    if (!common) {
        // logging
    } else {
        DataIndication& indication = ctx.service_primitive();
        indication.traffic_class = common->traffic_class;
        switch (common->next_header)
        {
            case NextHeaderCommon::BTP_A:
                indication.upper_protocol = UpperProtocol::BTP_A;
                break;
            case NextHeaderCommon::BTP_B:
                indication.upper_protocol = UpperProtocol::BTP_B;
                break;
            case NextHeaderCommon::IPv6:
                indication.upper_protocol = UpperProtocol::IPv6;
                break;
            default:
                indication.upper_protocol = UpperProtocol::Unknown;
                break;
        }

        // execute steps depending on extended header type
        decode_extended(ctx, *common);
    }
}

void GeonetDecoder::decode_secured(IndicationContextBasic& ctx, const BasicHeader& basic)
{   
    
    struct secured_payload_visitor : public boost::static_visitor<>
    {
        secured_payload_visitor(GeonetDecoder& decoder, IndicationContextBasic& ctx, const BasicHeader& basic) :
            m_decoder(decoder), m_context(ctx), m_basic(basic)
        {
        }

        void operator()(ChunkPacket& packet)
        {
            IndicationContextSecuredCast ctx(m_context, packet);
            m_decoder.decode_common(ctx, m_basic);
        }

        void operator()(CohesivePacket& packet)
        {
            IndicationContextSecuredDeserialize ctx(m_context, packet);
            m_decoder.decode_common(ctx, m_basic);
        }

        GeonetDecoder& m_decoder;
        IndicationContextBasic& m_context;
        const BasicHeader& m_basic;
    };

    auto secured_message = ctx.parse_secured();
    if (!secured_message) {
        // logging
    } else {
        inspect_signer_info(secured_message, m_detection_context);
        secured_payload_visitor visitor(*this, ctx, basic);
        PacketVariant plaintext_payload = std::move(secured_message->payload.data);
        boost::apply_visitor(visitor, plaintext_payload);
    }
}

void GeonetDecoder::decode_extended(IndicationContext& ctx, const CommonHeader& common)
{
    struct extended_header_visitor : public boost::static_visitor<>
    {
        extended_header_visitor(GeonetDecoder& router, IndicationContext& ctx, const UpPacket& packet, mitvane::DetectionContext& detection_context) :
            m_decoder(router), m_context(ctx), m_packet(packet), m_detection_context(detection_context)
        {
        }

        void operator()(const ShbHeader& shb)
        {
            DataIndication& indication = m_context.service_primitive();
            indication.transport_type = TransportType::SHB;
            indication.source_position = static_cast<ShortPositionVector>(shb.source_position);
            
            // Store Header Type
            m_detection_context.geonet_data.header_type = TransportType::SHB;
            // Store Source Position Vector
            m_detection_context.geonet_data.source_position = static_cast<ShortPositionVector>(shb.source_position);
        }

        void operator()(const GeoBroadcastHeader& gbc)
        {
            DataIndication& indication = m_context.service_primitive();
            indication.transport_type = TransportType::GBC;
            indication.source_position = static_cast<ShortPositionVector>(gbc.source_position);
            indication.destination = gbc.destination(m_context.pdu().common().header_type);
            
            // Store Header Type
            m_detection_context.geonet_data.header_type = TransportType::GBC;
            // Store Source Position Vector
            m_detection_context.geonet_data.source_position = static_cast<ShortPositionVector>(gbc.source_position);
            // Store destination
            m_detection_context.geonet_data.destination = gbc.destination(m_context.pdu().common().header_type);
        }

        void operator()(const BeaconHeader& beacon)
        {
        }

        GeonetDecoder& m_decoder;
        IndicationContext& m_context;
        const UpPacket& m_packet;
        mitvane::DetectionContext& m_detection_context;
    };

    auto extended = ctx.parse_extended(common.header_type);
    UpPacketPtr packet = ctx.finish();
    // Store payload data
    m_detection_context.payload = packet.get();
    assert(packet);

    if (!extended) {
        // logging
    } else {
        extended_header_visitor visitor(*this, ctx, *packet, m_detection_context);
        boost::apply_visitor(visitor, *extended);
        pass_up(ctx.service_primitive(), std::move(packet));
    }
}

void GeonetDecoder::pass_up(const DataIndication& ind, UpPacketPtr packet)
{    
    BtpDecoder btp_decoder(m_detection_context);
    btp_decoder.decode(ind, std::move(packet));
}

} // namespace mitvane