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


#ifndef SIGNATURE_HPP
#define SIGNATURE_HPP

#include <boost/variant/variant.hpp>
#include <vector>
#include <map>

namespace mitvane
{

enum class Action 
{
    Drop,
    Alert,
    NotSupported_Action
};

enum class Protocol 
{
    GeoNetworking,
    BTP,
    Facility,
    CA,
    DEN,
    MAP,
    SPAT,
    NotSupported_Protocol
};

enum class Allowed_So_Range_Shape
{
    Circle
};

struct MetaData 
{
    std::string msg; uint32_t sid;
    uint16_t rev;

};

const std::vector<std::string> geonet_keywords = {"nh", "ht", "mhl", "destination_area", "allowed_so_range"};
const std::vector<std::string> facility_keywords = {"allowed_ids"};

struct geonet_destination_area 
{
    int distance_a;
    int distance_b;
};

struct geonet_allowed_so_range 
{
    Allowed_So_Range_Shape shape;
    int distance_a;
    int distance_b;
};

typedef std::vector<uint32_t> facility_ids_type;

typedef boost::variant<
    int, 
    std::string, 
    geonet_destination_area, 
    geonet_allowed_so_range,
    facility_ids_type
    > pattern_type;

struct Signature 
{
    public:
        Action action;
        Protocol protocol; 
        MetaData meta;
        std::map<std::string, pattern_type> patterns;

};


} // namespace mitvane

#endif // SIGNATURE_HPP