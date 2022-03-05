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


#ifndef GEONET_DATA_HPP
#define GEONET_DATA_HPP

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/its_aid.hpp>
#include <vanetza/geonet/destination_variant.hpp>
#include <vanetza/geonet/interface.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/security/decap_confirm.hpp>
#include <boost/optional.hpp>


namespace mitvane 
{

enum class SecurityInfo {
    NoSecurity,
    Signed,
};

struct GeonetData
{   
    unsigned int protocol_version;
    vanetza::geonet::TransportType header_type;
    vanetza::geonet::DestinationVariant destination;
    vanetza::geonet::ShortPositionVector source_position;
    SecurityInfo security_info;
    boost::optional<vanetza::security::HashedId8> certificate_id;
    boost::optional<vanetza::security::PayloadType> payload_type;
    boost::optional<uint8_t> secure_protocol_version;
    boost::optional<vanetza::security::SignerInfoType> signer_info_type;
    boost::optional<unsigned int> chain_size;
    vanetza::geonet::Lifetime remaining_packet_lifetime;
    unsigned remaining_hop_limit;
};

} // namespace mitvane

#endif /* GEONET_DATA_HPP */