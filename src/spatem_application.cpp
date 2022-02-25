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


#include "spatem_application.hpp"
#include "vanetza-extension/asn1/spatem.hpp"
#include "vanetza-extension/facilities/cam_functions.hpp"
#include "vanetza-extension/asn1/packet_visitor.hpp"
#include <vanetza/btp/ports.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>


using namespace std::chrono;

SpatemApplication::SpatemApplication(vanetza::PositionProvider& positioning, vanetza::Runtime& rt) :
    positioning_(positioning), runtime_(rt), spat_interval_(seconds(1))
{
    schedule_timer();
}

void SpatemApplication::set_interval(vanetza::Clock::duration interval)
{
    spat_interval_ = interval;
    runtime_.cancel(this);
    schedule_timer();
}

void SpatemApplication::print_generated_message(bool flag)
{
    print_tx_msg_ = flag;
}

void SpatemApplication::print_received_message(bool flag)
{
    print_rx_msg_ = flag;
}

SpatemApplication::PortType SpatemApplication::port()
{
    return vanetza::btp::ports::SPAT;
}

void SpatemApplication::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    vanetzaExtension::asn1::PacketVisitor<vanetzaExtension::asn1::Spatem> visitor;
    //type check of packet
    std::shared_ptr<const vanetzaExtension::asn1::Spatem> spat = boost::apply_visitor(visitor, *packet);

    std::cout << "SPATEM application received a packet with " << (spat ? "decodable" : "broken") << " content" << std::endl;
    if (spat && print_rx_msg_) {
        std::cout << "Received SPATEM contains\n";
        //print_indented(std::cout, *spat, "  ", 1);
    }
}

void SpatemApplication::schedule_timer()
{
    runtime_.schedule(spat_interval_, std::bind(&SpatemApplication::on_timer, this, std::placeholders::_1), this);
}

//send message on timer
void SpatemApplication::on_timer(vanetza::Clock::time_point)
{
    schedule_timer();

    auto message = createMessageSkeleton();

    std::string error;
    if (!message.validate(error)) {
        throw std::runtime_error("Invalid high frequency SPATEM: %s" + error);
    }

    if (print_tx_msg_) {
        std::cout << "Generated SPATEM contains\n";
        //print_indented(std::cout, message, "  ", 1);
    }

    DownPacketPtr packet { new vanetza::DownPacket() };
    packet->layer(vanetza::OsiLayer::Application) = std::move(message);

    DataRequest request;
    request.transport_type = vanetza::geonet::TransportType::SHB;
    request.communication_profile = vanetza::geonet::CommunicationProfile::ITS_G5;

    auto confirm = Application::request(request, std::move(packet));
    if (!confirm.accepted()) {
        throw std::runtime_error("SPATEM application data request failed");
    }
}

vanetzaExtension::asn1::Spatem SpatemApplication::createMessageSkeleton()
{   
    vanetzaExtension::asn1::Spatem message;

    message->header.protocolVersion = 2;
    message->header.messageID = ItsPduHeader__messageID_spatem;
    message->header.stationID = 1; // some dummy value
    
    SPAT_t& spat = message->spat;
    IntersectionStateList_t& intersections = spat.intersections;
    
    auto intersection = vanetzaExtension::asn1::allocate<IntersectionState_t>();
    intersection->id.id = 1;
    intersection->revision = 1;
    intersection->status.buf = static_cast<uint8_t*>(vanetzaExtension::asn1::allocate(4));
    intersection->status.size = 2;
    intersection->status.bits_unused = 0;
    auto intersectionState = IntersectionStatusObject_manualControlIsEnabled;
    intersection->status.buf[0] |= (intersectionState & (1 << 0));
    intersection->status.buf[0] |= (intersectionState & (1 << 1));
    intersection->status.buf[1] |= (intersectionState & (1 << 2));
    intersection->status.buf[1] |= (intersectionState & (1 << 3));

    auto state = vanetzaExtension::asn1::allocate<MovementState_t>();
    state->signalGroup = 1;

    auto event = vanetzaExtension::asn1::allocate<MovementEvent_t>();
    event->eventState = 6;
    
    asn_sequence_add(&state->state_time_speed, event);
    asn_sequence_add(&intersection->states, state);
    asn_sequence_add(&spat.intersections, intersection);

    return message;
}
