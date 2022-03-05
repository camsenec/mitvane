/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/asn1/packet_visitor.hpp>
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

/* 
 * State Changes
 * - Include file paths are changed.
*/


#ifndef PACKET_VISITOR_HPP
#define PACKET_VISITOR_HPP

#include "vanetza-extension/asn1/asn1c_conversion.hpp"
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <vanetza/net/packet_variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <memory>

namespace vanetzaExtension
{
namespace asn1
{

/**
 * PacketVisitor is a helper to extract the ASN.1 wrapper object from a packet's application layer.
 * This is a generalised version of the implementation found in Artery before.
 *
 * ChunkPacket's application layer is simply casted for the sake of speed whenever possible.
 * When casting fails, this helper can fall back to deserialization, see allow_chunk_deserialization.
 * Deserialization is the only option for CohesivePacket's application layer.
 *
 * \param T is the asn1c_wrapper<> type, e.g. Cam or Denm
 */
template<typename T>
class PacketVisitor : public boost::static_visitor<std::shared_ptr<const T>>
{
    public:
        std::shared_ptr<const T> operator()(const vanetza::ChunkPacket& packet)
        {
            using byte_buffer_impl = vanetza::convertible::byte_buffer_impl<T>;
            auto impl = dynamic_cast<const byte_buffer_impl*>(packet[vanetza::OsiLayer::Application].ptr());
            if (impl) {
                m_wrapper = impl->wrapper();
            } else if (m_deserialize_chunk) {
                deserialize(create_byte_view(packet, m_start_layer, vanetza::OsiLayer::Application));
            } else {
                m_wrapper.reset();
            }
            return m_wrapper;
        }

        std::shared_ptr<const T> operator()(const vanetza::CohesivePacket& packet)
        {
            deserialize(create_byte_view(packet, m_start_layer, vanetza::OsiLayer::Application));
            return m_wrapper;
        }

        std::shared_ptr<const T> get_shared_wrapper() const { return m_wrapper; }

        /**
         * Allow deserialization attempt of ChunkPacket when casting failed.
         * \param flag true allows deserialization
         */
        void allow_chunk_deserialization(bool flag)
        {
            m_deserialize_chunk = flag;
        }

        /**
         * Set OSI layer where deserialization shall start, it always stops at end of OsiLayer::Application.
         * By default, PacketVisitors starts at OsiLayer::Session for compatibility with BTP parser functions.
         * \param start OSI layer
         */
        void start_deserialization_at(vanetza::OsiLayer start)
        {
            m_start_layer = start;
        }

    private:
        void deserialize(const vanetza::byte_view_range& range)
        {
            auto tmp = std::make_shared<T>();
            bool decoded = tmp->decode(range.begin(), range.end());
            m_wrapper = decoded ? tmp : nullptr;
        }

        std::shared_ptr<const T> m_wrapper;
        bool m_deserialize_chunk = true;
        vanetza::OsiLayer m_start_layer = vanetza::OsiLayer::Session;
};

} // namespace asn1
} // namespace vanetzaExtension

#endif /* PACKET_VISITOR_HPP */

