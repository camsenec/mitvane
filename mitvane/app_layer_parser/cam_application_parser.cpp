/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/cam_application.cpp>
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



#include "cam_application_parser.hpp"
#include "vanetza-extension/asn1/cam.hpp"
#include "vanetza-extension/facilities/functions.hpp"
#include "vanetza-extension/facilities/cam_functions.hpp"
#include "vanetza-extension/asn1/packet_visitor.hpp"
#include <vanetza/btp/ports.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>

using namespace vanetza;

void CamApplicationParser::print_received_message(bool flag)
{
    print_rx_msg_ = flag;
}

CamApplicationParser::PortType CamApplicationParser::port()
{
    return vanetza::btp::ports::CAM;
}

void CamApplicationParser::parse(UpPacketPtr packet)
{
    vanetzaExtension::asn1::PacketVisitor<vanetzaExtension::asn1::Cam> visitor;
    std::shared_ptr<const vanetzaExtension::asn1::Cam> cam = boost::apply_visitor(visitor, *packet);

    std::cout << "CAM application received a packet with " << (cam ? "decodable" : "broken") << " content" << std::endl;
    if (cam && print_rx_msg_) {
        std::cout << "Received CAM contains\n";
        vanetzaExtension::facilities::cam::print_indented(std::cout, *cam, "  ", 1);
    }
}
