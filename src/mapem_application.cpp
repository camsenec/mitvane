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


#include "mapem_application.hpp"
#include "vanetza-extension/asn1/mapem.hpp"
#include "vanetza-extension/facilities/functions.hpp"
#include "vanetza-extension/facilities/mapem_functions.hpp"
#include "vanetza-extension/asn1/packet_visitor.hpp"
#include <vanetza/btp/ports.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <utility>


using namespace std::chrono;

MapemApplication::MapemApplication(vanetza::PositionProvider& positioning, vanetza::Runtime& rt) :
    positioning_(positioning), runtime_(rt), map_interval_(seconds(1))
{
    schedule_timer();
}

void MapemApplication::set_interval(vanetza::Clock::duration interval)
{
    map_interval_ = interval;
    runtime_.cancel(this);
    schedule_timer();
}

void MapemApplication::print_generated_message(bool flag)
{
    print_tx_msg_ = flag;
}

void MapemApplication::print_received_message(bool flag)
{
    print_rx_msg_ = flag;
}

MapemApplication::PortType MapemApplication::port()
{
    return vanetza::btp::ports::SPAT;
}

void MapemApplication::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    vanetzaExtension::asn1::PacketVisitor<vanetzaExtension::asn1::Mapem> visitor;
    //type check of packet
    std::shared_ptr<const vanetzaExtension::asn1::Mapem> map = boost::apply_visitor(visitor, *packet);

    std::cout << "MAPEM application received a packet with " << (map ? "decodable" : "broken") << " content" << std::endl;
    if (map && print_rx_msg_) {
        std::cout << "Received MAPEM contains\n";
        vanetzaExtension::facilities::mapem::print_indented(std::cout, *map, "  ", 1);
    }
}

void MapemApplication::schedule_timer()
{
    runtime_.schedule(map_interval_, std::bind(&MapemApplication::on_timer, this, std::placeholders::_1), this);
}

//send message on timer
void MapemApplication::on_timer(vanetza::Clock::time_point)
{
    schedule_timer();
    
    auto message = createMessageSkeleton();
    

    std::string error;
    if (!message.validate(error)) {
        throw std::runtime_error("Invalid high frequency MapEM: %s" + error);
    }

    if (print_tx_msg_) {
        std::cout << "Generated MapEM contains\n";
        vanetzaExtension::facilities::mapem::print_indented(std::cout, message, "  ", 1);
    }

    DownPacketPtr packet { new vanetza::DownPacket() };
    packet->layer(vanetza::OsiLayer::Application) = std::move(message);

    DataRequest request;
    request.transport_type = vanetza::geonet::TransportType::SHB;
    request.communication_profile = vanetza::geonet::CommunicationProfile::ITS_G5;

    auto confirm = Application::request(request, std::move(packet));
    if (!confirm.accepted()) {
        throw std::runtime_error("MapEM application data request failed");
    }
}

vanetzaExtension::asn1::Mapem MapemApplication::createMessageSkeleton()
{   
    vanetzaExtension::asn1::Mapem message;

    message->header.protocolVersion = 2;
    message->header.messageID = ItsPduHeader__messageID_mapem;
    message->header.stationID = 1; // some dummy value
    
    MapData_t& map = message->map;
    map.msgIssueRevision = 0;

    return message;
}
