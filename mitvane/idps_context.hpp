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


#ifndef IDPS_CONTEXT_HPP
#define IDPS_CONTEXT_HPP

#include "link_layer.hpp"
#include "raw_socket_link.hpp"
#include "mitvane/rule_reader/rule_reader.hpp"
#include "mitvane/app_layer_parser/application_parser.hpp"
#include <vanetza/common/position_provider.hpp>
#include <vanetza/btp/header.hpp>
#include <vanetza/geonet/mib.hpp>
#include <array>
#include <list>
#include <memory>


typedef std::map<mitvane::Protocol, std::vector<mitvane::Signature>> signatures_type;

class IdpsContext
{

public:
    IdpsContext(const vanetza::geonet::MIB&, vanetza::PositionProvider&, signatures_type&, RawSocketLink*);
    ~IdpsContext();
    void set_link_layer(LinkLayer*);
    void set_application_layer_parser(vanetza::btp::port_type port, ApplicationLayerParser* handler);
    void enable(ApplicationLayerParser*);
    void disable(ApplicationLayerParser*);

private:
    void run(vanetza::CohesivePacket&&, const vanetza::EthernetHeader&);
    void decode(vanetza::CohesivePacket&&, const vanetza::EthernetHeader&);
    void update_position_vector();
    void update_packet_flow(const vanetza::geonet::LongPositionVector&);

    std::unordered_map<vanetza::btp::port_type, ApplicationLayerParser*> m_app_layer_parsers;
    vanetza::geonet::MIB mib_;
    vanetza::PositionProvider& positioning_;
    std::list<ApplicationLayerParser*> app_parsers_;
    signatures_type& signatures_;
    RawSocketLink* destination_link_;
    
};


#endif /* IDPS_CONTEXT_HPP */
