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


#include "denm_application_parser.hpp"
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

static const auto microdegree = vanetza::units::degree * boost::units::si::micro;


void DenmApplicationParser::print_received_message(bool flag)
{
    print_rx_msg_ = flag;
}

DenmApplicationParser::PortType DenmApplicationParser::port()
{
    return vanetza::btp::ports::DENM;
}

void DenmApplicationParser::parse(UpPacketPtr packet)
{
    vanetzaExtension::asn1::PacketVisitor<vanetzaExtension::asn1::Denm> visitor;
    
    std::shared_ptr<const vanetzaExtension::asn1::Denm> denm = boost::apply_visitor(visitor, *packet);

    std::cout << "DENM application received a packet with " << (denm ? "decodable" : "broken") << " content" << std::endl;
    if (denm && print_rx_msg_) {
        std::cout << "Received DENM contains\n";
        vanetzaExtension::facilities::denm::print_indented(std::cout, *denm, "  ", 1);
    }
}
