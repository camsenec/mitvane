/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/cohda_link.hpp>
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


#ifndef COHDA_LINK_HPP_IXOCQ5RH
#define COHDA_LINK_HPP_IXOCQ5RH

#include "raw_socket_link.hpp"

class CohdaLink : public RawSocketLink
{
public:
    using RawSocketLink::RawSocketLink;

    void request(const vanetza::access::DataRequest&, std::unique_ptr<vanetza::ChunkPacket>) override;

protected:
    boost::optional<vanetza::EthernetHeader> parse_ethernet_header(vanetza::CohesivePacket&) const override;
};

#endif /* COHDA_LINK_HPP_IXOCQ5RH */

