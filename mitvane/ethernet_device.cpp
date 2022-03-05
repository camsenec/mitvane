/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/ethernet_device.cpp>
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


#include "ethernet_device.hpp"
#include <algorithm>
#include <cstring>
#include <system_error>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>

static void initialize(ifreq& request, const char* interface_name)
{
    std::memset(&request, 0, sizeof(ifreq));
    std::strncpy(request.ifr_name, interface_name, IF_NAMESIZE);
    request.ifr_name[IF_NAMESIZE - 1] = '\0';
}

EthernetDevice::EthernetDevice(const char* devname) :
    local_socket_(::socket(AF_LOCAL, SOCK_DGRAM, 0)),
    interface_name_(devname)
{
    if (!local_socket_) {
        throw std::system_error(errno, std::system_category());
    }
}

EthernetDevice::~EthernetDevice()
{
    if (local_socket_ >= 0)
        ::close(local_socket_);
}

EthernetDevice::protocol::endpoint EthernetDevice::endpoint(int family) const
{
    sockaddr_ll socket_address = {0};
    socket_address.sll_family = family;
    socket_address.sll_protocol = htons(ETH_P_ALL);
    socket_address.sll_ifindex = index();
    return protocol::endpoint(&socket_address, sizeof(sockaddr_ll));
}

int EthernetDevice::index() const
{
    ifreq data;
    initialize(data, interface_name_.c_str());
    ::ioctl(local_socket_, SIOCGIFINDEX, &data);
    return data.ifr_ifindex;
}

vanetza::MacAddress EthernetDevice::address() const
{
    ifreq data;
    initialize(data, interface_name_.c_str());
    ::ioctl(local_socket_, SIOCGIFHWADDR, &data);

    vanetza::MacAddress addr;
    std::copy_n(data.ifr_hwaddr.sa_data, addr.octets.size(), addr.octets.data());
    return addr;
}
