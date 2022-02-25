/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/cohda.cpp>
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


#include "cohda.hpp"
#include <vanetza/access/access_category.hpp>
#include <vanetza/access/g5_link_layer.hpp>
#include <vanetza/common/serialization_buffer.hpp>
#include <vanetza/dcc/mapping.hpp>
#include <cassert>
#include <llc-api.h>

namespace vanetza
{

void insert_cohda_tx_header(const access::DataRequest& request, std::unique_ptr<ChunkPacket>& packet)
{
    access::G5LinkLayer link_layer;
    access::ieee802::dot11::QosDataHeader& mac_header = link_layer.mac_header;
    mac_header.destination = request.destination_addr;
    mac_header.source = request.source_addr;
    mac_header.qos_control.user_priority(request.access_category);

    ByteBuffer link_layer_buffer;
    serialize_into_buffer(link_layer, link_layer_buffer);
    assert(link_layer_buffer.size() == access::G5LinkLayer::length_bytes);
    packet->layer(OsiLayer::Link) = std::move(link_layer_buffer);

    const std::size_t payload_size = packet->size();
    const std::size_t total_size = sizeof(tMKxTxPacket) + payload_size;

    tMKxTxPacket phy = { 0 };
    phy.Hdr.Type = MKXIF_TXPACKET;
    phy.Hdr.Len = total_size;
    phy.TxPacketData.TxAntenna = MKX_ANT_DEFAULT;
    phy.TxPacketData.TxFrameLength = payload_size;
    auto phy_ptr = reinterpret_cast<const uint8_t*>(&phy);
    packet->layer(OsiLayer::Physical) = ByteBuffer { phy_ptr, phy_ptr + sizeof(tMKxTxPacket) };
}

boost::optional<EthernetHeader> strip_cohda_rx_header(CohesivePacket& packet)
{
    static const std::size_t min_length = sizeof(tMKxRxPacket) + access::G5LinkLayer::length_bytes +
        access::ieee802::dot11::fcs_length_bytes;
    if (packet.size(OsiLayer::Physical) < min_length) {
        return boost::none;
    }

    packet.set_boundary(OsiLayer::Physical, sizeof(tMKxRxPacket));
    auto phy = reinterpret_cast<const tMKxRxPacket*>(&*packet[OsiLayer::Physical].begin());
    if (phy->Hdr.Type != MKXIF_RXPACKET) {
        return boost::none;
    }

    // Sanity check that sizes reported by Cohda LLC are correct, since we rely on Cohda's FCS checking
    if (phy->Hdr.Len != packet.size() || phy->RxPacketData.RxFrameLength != packet.size() - sizeof(tMKxRxPacket)) {
        return boost::none;
    }

    if (!phy->RxPacketData.FCSPass) {
        return boost::none;
    }

    packet.trim(OsiLayer::Link, packet.size() - access::ieee802::dot11::fcs_length_bytes);
    packet.set_boundary(OsiLayer::Link, access::G5LinkLayer::length_bytes);
    access::G5LinkLayer link_layer;
    deserialize_from_range(link_layer, packet[OsiLayer::Link]);
    if (!access::check_fixed_fields(link_layer)) {
        return boost::none;
    }

    EthernetHeader eth;
    eth.destination = link_layer.mac_header.destination;
    eth.source = link_layer.mac_header.source;
    eth.type = link_layer.llc_snap_header.protocol_id;
    return eth;
}

} // namespace vanetza
