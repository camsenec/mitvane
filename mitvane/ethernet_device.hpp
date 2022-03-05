/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/ethernet_device.hpp>
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


#ifndef ETHERNET_DEVICE_HPP_NEVC5DAY
#define ETHERNET_DEVICE_HPP_NEVC5DAY

#include <vanetza/net/mac_address.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <string>

class EthernetDevice
{
public:
    using protocol = boost::asio::generic::raw_protocol;

    EthernetDevice(const char* devname);
    EthernetDevice(const EthernetDevice&) = delete;
    EthernetDevice& operator=(const EthernetDevice&) = delete;
    ~EthernetDevice();

    protocol::endpoint endpoint(int family) const;
    vanetza::MacAddress address() const;

private:
    int index() const;

    int local_socket_;
    std::string interface_name_;
};

#endif /* ETHERNET_DEVICE_HPP_NEVC5DAY */

