/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/cohda_proxy/proxy.cpp>
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


#include "proxy_handler.hpp"
#include <vanetza/asio/cohda.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <mk2mac-api-types.h>
#include <iostream>

using namespace boost;

int main(int /* argc */, char** /* argv */)
{
    const tMK2Channel pwlan_channel = 180;
    const tMK2MCS pwlan_mcs = MK2MCS_R12QPSK;
    const unsigned udp_port = 8041;

    try {
        asio::io_service io_service;
        asio::ip::udp::endpoint udp_endpoint = { asio::ip::udp::v4(), udp_port };
        asio::ip::udp::socket udp_socket = { io_service, udp_endpoint };
        vanetza::asio::cohda::endpoint cohda_endpoint = { vanetza::asio::cohda::wave_raw() };
        vanetza::asio::cohda::socket cohda_socket = { io_service, cohda_endpoint };
        ProxyHandler handler(udp_socket, cohda_socket);
        handler.channel_number(pwlan_channel);
        handler.mcs(pwlan_mcs);
        handler.start_transfer();
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}

