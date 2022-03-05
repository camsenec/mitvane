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


#include "mapem_application_parser.hpp"
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


void MapemApplicationParser::print_received_message(bool flag)
{
    print_rx_msg_ = flag;
}

MapemApplicationParser::PortType MapemApplicationParser::port()
{
    return vanetza::btp::ports::TOPO;
}

void MapemApplicationParser::parse(UpPacketPtr packet)
{
    vanetzaExtension::asn1::PacketVisitor<vanetzaExtension::asn1::Mapem> visitor;
    
    std::shared_ptr<const vanetzaExtension::asn1::Mapem> map = boost::apply_visitor(visitor, *packet);

    std::cout << "MAPEM application received a packet with " << (map ? "decodable" : "broken") << " content" << std::endl;
    if (map && print_rx_msg_) {
        std::cout << "Received MAPEM contains\n";
        vanetzaExtension::facilities::mapem::print_indented(std::cout, *map, "  ", 1);
    }
}