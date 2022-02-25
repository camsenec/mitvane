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


#include "denm_application.hpp"
#include "vanetza-extension/facilities/functions.hpp"
#include "vanetza-extension/facilities/denm_functions.hpp"
#include "vanetza-extension/asn1/denm.hpp"
#include "vanetza-extension/asn1/packet_visitor.hpp"
#include <vanetza/btp/ports.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>


using namespace std::chrono;

static const auto microdegree = vanetza::units::degree * boost::units::si::micro;

DenmApplication::DenmApplication(vanetza::PositionProvider& positioning, vanetza::Runtime& rt) :
    positioning_(positioning), runtime_(rt), denm_interval_(seconds(1))
{
    schedule_timer();
}

void DenmApplication::set_interval(vanetza::Clock::duration interval)
{
    denm_interval_ = interval;
    runtime_.cancel(this);
    schedule_timer();
}

void DenmApplication::print_generated_message(bool flag)
{
    print_tx_msg_ = flag;
}

void DenmApplication::print_received_message(bool flag)
{
    print_rx_msg_ = flag;
}

DenmApplication::PortType DenmApplication::port()
{
    return vanetza::btp::ports::DENM;
}

void DenmApplication::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    vanetzaExtension::asn1::PacketVisitor<vanetzaExtension::asn1::Denm> visitor;
    //type check of packet
    std::shared_ptr<const vanetzaExtension::asn1::Denm> denm = boost::apply_visitor(visitor, *packet);

    std::cout << "DENM application received a packet with " << (denm ? "decodable" : "broken") << " content" << std::endl;
    if (denm && print_rx_msg_) {
        std::cout << "Received DENM contains\n";
        vanetzaExtension::facilities::denm::print_indented(std::cout, *denm, "  ", 1);
    }
}

void DenmApplication::schedule_timer()
{
    runtime_.schedule(denm_interval_, std::bind(&DenmApplication::on_timer, this, std::placeholders::_1), this);
}

void DenmApplication::on_timer(vanetza::Clock::time_point)
{
    schedule_timer();

    auto position = positioning_.position_fix();

    if (!position.confidence) {
        std::cerr << "Skipping DENM, because no good position is available, yet." << std::endl;
        return;
    }
    
    auto message = createMessageSkeleton(position);

    std::string error;
    if (!message.validate(error)) {
        throw std::runtime_error("Invalid high frequency DENM: %s" + error);
    }

    if (print_tx_msg_) {
        std::cout << "Generated DENM contains\n";
        vanetzaExtension::facilities::denm::print_indented(std::cout, message, "  ", 1);
    }

    DownPacketPtr packet { new vanetza::DownPacket() };
    packet->layer(vanetza::OsiLayer::Application) = std::move(message);

    DataRequest request;
    request.its_aid = vanetza::aid::DEN;
    request.transport_type = vanetza::geonet::TransportType::SHB;
    request.communication_profile = vanetza::geonet::CommunicationProfile::ITS_G5;

    auto confirm = Application::request(request, std::move(packet));
    if (!confirm.accepted()) {
        throw std::runtime_error("DENM application data request failed");
    }
}

vanetzaExtension::asn1::Denm DenmApplication::createMessageSkeleton(const vanetza::PositionFix& position)
{   
    vanetzaExtension::asn1::Denm message;

    message->header.protocolVersion = 2;
    message->header.messageID = ItsPduHeader__messageID_denm;
    message->header.stationID = 1; // some dummy value

    message->denm.management.actionID.originatingStationID = 1000; //some dummy value
    message->denm.management.actionID.sequenceNumber = 32764; //some dummy value
    int ret = 0;
    ret += asn_ulong2INTEGER(&message->denm.management.detectionTime, TimestampIts_utcStartOf2004);
    ret += asn_ulong2INTEGER(&message->denm.management.referenceTime, TimestampIts_utcStartOf2004);
    assert(ret == 0);
    
    // Temporal implementation
    message->denm.management.eventPosition.altitude.altitudeValue = AltitudeValue_unavailable;
    message->denm.management.eventPosition.altitude.altitudeConfidence = AltitudeConfidence_unavailable;
    message->denm.management.eventPosition.longitude = vanetzaExtension::facilities::round(position.longitude, microdegree) * Longitude_oneMicrodegreeEast;
    message->denm.management.eventPosition.latitude = vanetzaExtension::facilities::round(position.latitude, microdegree) * Latitude_oneMicrodegreeNorth;
    message->denm.management.eventPosition.positionConfidenceEllipse.semiMajorOrientation = HeadingValue_unavailable;
    message->denm.management.eventPosition.positionConfidenceEllipse.semiMajorConfidence = SemiAxisLength_unavailable;
    message->denm.management.eventPosition.positionConfidenceEllipse.semiMinorConfidence = SemiAxisLength_unavailable;

    return message;
}
