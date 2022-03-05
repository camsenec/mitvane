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


#ifndef BTP_DATA_HPP
#define BTP_DATA_HPP

#include <vanetza/geonet/interface.hpp>
#include <vanetza/btp/header.hpp>

namespace mitvane 
{

struct BtpData
{   
    vanetza::geonet::UpperProtocol btp_type;
    boost::optional<vanetza::btp::port_type> source_port;
    vanetza::btp::port_type destination_port;
    boost::optional<decltype(vanetza::btp::HeaderB::destination_port_info)> destination_port_info;
};

} // namespace mitvane

#endif /* BTP_DATA_HPP */