/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/udp_link.hpp>
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


#ifndef UDP_LINK_HPP_A16QFBX3
#define UDP_LINK_HPP_A16QFBX3

#include "link_layer.hpp"
#include <vanetza/common/byte_buffer.hpp>
#include <boost/asio/ip/udp.hpp>
#include <array>

class UdpLink : public LinkLayer
{
public:
    UdpLink(boost::asio::io_service&, const boost::asio::ip::udp::endpoint&);

    void indicate(IndicationCallback) override;
    void request(const vanetza::access::DataRequest&, std::unique_ptr<vanetza::ChunkPacket>) override;

private:
    void do_receive();

    static constexpr std::size_t layers_ = num_osi_layers(vanetza::OsiLayer::Link, vanetza::OsiLayer::Application);

    boost::asio::ip::udp::endpoint multicast_endpoint_;
    boost::asio::ip::udp::socket tx_socket_;
    boost::asio::ip::udp::socket rx_socket_;
    std::array<vanetza::ByteBuffer, layers_> tx_buffers_;
    vanetza::ByteBuffer rx_buffer_;
    boost::asio::ip::udp::endpoint rx_endpoint_;
    IndicationCallback callback_;
};

#endif /* UDP_LINK_HPP_A16QFBX3 */

