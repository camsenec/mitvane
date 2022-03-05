/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/raw_socket_link.hpp>
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


#ifndef RAW_SOCKET_LINK_HPP_VUXH507U
#define RAW_SOCKET_LINK_HPP_VUXH507U

#include "link_layer.hpp"
#include <vanetza/access/interface.hpp>
#include <vanetza/net/ethernet_header.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <boost/optional/optional.hpp>
#include <array>
#include <functional>

class RawSocketLink : public LinkLayer
{
public:
    RawSocketLink(boost::asio::generic::raw_protocol::socket&&);
    void request(const vanetza::access::DataRequest&, std::unique_ptr<vanetza::ChunkPacket>) override;
    void indicate(IndicationCallback) override;

protected:
    std::size_t transmit(std::unique_ptr<vanetza::ChunkPacket>);
    virtual boost::optional<vanetza::EthernetHeader> parse_ethernet_header(vanetza::CohesivePacket&) const;

private:
    void do_receive();
    void on_read(const boost::system::error_code&, std::size_t);
    void pass_up(vanetza::CohesivePacket&&);

    static constexpr std::size_t layers_ = num_osi_layers(vanetza::OsiLayer::Physical, vanetza::OsiLayer::Application);

    boost::asio::generic::raw_protocol::socket socket_;
    std::array<vanetza::ByteBuffer, layers_> buffers_;
    IndicationCallback callback_;
    vanetza::ByteBuffer receive_buffer_;
    boost::asio::generic::raw_protocol::endpoint receive_endpoint_;
};

#endif /* RAW_SOCKET_LINK_HPP_VUXH507U */

