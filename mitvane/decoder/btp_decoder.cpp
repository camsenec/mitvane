/* 
 * (C) 2022 Tomoya Tanaka <deepsky2221@gmail.com> 
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


#include "btp_decoder.hpp"
#include <vanetza/btp/port_dispatcher.hpp>
#include <cassert>


using namespace vanetza;
using namespace vanetza::btp;

namespace mitvane {

BtpDecoder::BtpDecoder(DetectionContext& detection_context) : 
    m_detection_context(detection_context){}
    
boost::optional<DataIndication> parse_btp_header(const geonet::DataIndication& gn_ind, PacketVariant& packet, DetectionContext& detection_context)
{
    boost::optional<DataIndication> indication;

    switch (gn_ind.upper_protocol) {
        case geonet::UpperProtocol::BTP_A: {
            HeaderA hdr = parse_btp_a(packet);
            indication = DataIndication(gn_ind, hdr);
            
            // Store btp type
            detection_context.btp_data.btp_type = geonet::UpperProtocol::BTP_A;
            // Store source port
            detection_context.btp_data.source_port = hdr.source_port;
            // Store destination port
            detection_context.btp_data.destination_port = hdr.destination_port;
            }
            break;
        case geonet::UpperProtocol::BTP_B: {
            HeaderB hdr = parse_btp_b(packet);
            indication = DataIndication(gn_ind, hdr);

            // Store btp type
            detection_context.btp_data.btp_type = geonet::UpperProtocol::BTP_B; 
            // Store destination port
            detection_context.btp_data.destination_port = hdr.destination_port;
            // Store destination port info
            detection_context.btp_data.destination_port_info = hdr.destination_port_info;
            }
            break;
        default:
            break;
    }

    return indication;
}


void BtpDecoder::decode(const vanetza::geonet::DataIndication& gn_ind, std::unique_ptr<UpPacket> packet)
{
    assert(packet);
    boost::optional<DataIndication> btp_ind = parse_btp_header(gn_ind, *packet, m_detection_context);
    m_detection_context.app_layer_parser_port = btp_ind->destination_port;
    packet.release();
}
} // namespace mitvane