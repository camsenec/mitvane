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


#include "mitvane/idps_context.hpp"
#include "mitvane/rule_reader/rule_reader.hpp"
#include "detector.hpp"
#include "geonet_data.hpp"
#include "geonet_detector.hpp"
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/security/region.hpp>
#include <boost/units/cmath.hpp>
#include <GeographicLib/Geodesic.hpp>

namespace mitvane 
{

using namespace vanetza::geonet;

GeonetDetector::GeonetDetector(GeonetData &data, vanetza::PositionProvider& positioning) : 
    m_data(data), m_positioning(positioning){}

bool is_within_circle(const vanetza::geonet::Area& inner, const vanetza::geonet::Area& outer)
{
    const auto& geod = GeographicLib::Geodesic::WGS84();
    double center_dist = 0.0;
    const vanetza::units::GeoAngle inner_lat = inner.position.latitude;
    const vanetza::units::GeoAngle inner_long = inner.position.longitude;
    const vanetza::units::GeoAngle outer_lat = outer.position.latitude;
    const vanetza::units::GeoAngle outer_long = outer.position.longitude;
    
    Circle range = boost::get<Circle>(inner.shape);
    Circle dest_area = boost::get<Circle>(outer.shape);
    geod.Inverse(inner_lat / vanetza::units::degree, inner_long / vanetza::units::degree,
            outer_lat / vanetza::units::degree, outer_long / vanetza::units::degree, center_dist);
    return center_dist + range.r / vanetza::units::si::meter <= dest_area.r / vanetza::units::si::meter;
}

TransformStatusCode GeonetDetector::transform_nh(std::string nh_str, NextHeaderBasic &nh)
{
    if (nh_str == "common") {
        nh = NextHeaderBasic::Common;
        return TransformStatusCode::Success;
    } else if (nh_str == "secure") {
        nh = NextHeaderBasic::Secured;
        return TransformStatusCode::Success;
    } else {
        return TransformStatusCode::Error;
    }
}

TransformStatusCode GeonetDetector::transform_ht(std::string ht_str, uint8_t &ht){
    if (ht_str == "TSB") {
        ht = static_cast<uint8_t>(HeaderType::TSB_Multi_Hop);
        return TransformStatusCode::Success;
    } else if (ht_str == "SHB") {
        ht = static_cast<uint8_t>(HeaderType::TSB_Single_Hop);
        return TransformStatusCode::Success;
    } else if (ht_str == "GBC") {
        ht = static_cast<uint8_t>(HeaderType::GeoAnycast_Circle) | 
            static_cast<uint8_t>(HeaderType::GeoAnycast_Rect) | 
            static_cast<uint8_t>(HeaderType::GeoAnycast_Elip);
        return TransformStatusCode::Success;
    } else {
        return TransformStatusCode::Error;
    }
}

DetectionReport GeonetDetector::detect(signatures_type &signatures)
{
    std::vector<mitvane::Signature> geonet_signatures = signatures[Protocol::GeoNetworking];
    TransformStatusCode ret;

    for (auto sig_it = geonet_signatures.begin(); sig_it != geonet_signatures.end(); ++sig_it) {
        mitvane::Signature sig = *sig_it;
        
        if (sig.patterns.count("nh")) {
            NextHeaderBasic nh;
            ret = transform_nh(boost::get<std::string>(sig.patterns["nh"]), nh);
            if (ret != TransformStatusCode::Success) {
                std::cerr << "[sid: " << sig.meta.sid << "] " << "Detection skipped: Invalid next header" << "\n";
                continue;
            }
            if (nh != m_data.next_header) {
                return DetectionReport{sig, DetectionReport::DetectionResult::Detected};
            }
        }
        if (sig.patterns.count("ht")){
            uint8_t ht;
            ret = transform_ht(boost::get<std::string>(sig.patterns["ht"]), ht);
            if (ret != TransformStatusCode::Success) {
                std::cerr << "[sid: " << sig.meta.sid << "] " << "Detection skipped: Invalid header type" << "\n";
                continue;
            }
            if (!(ht & static_cast<uint8_t>(m_data.header_type))) {
                return DetectionReport{sig, DetectionReport::DetectionResult::Detected};
            }
        }
        if (sig.patterns.count("mhl")){
            if(boost::get<int>(sig.patterns["mhl"]) <= m_data.maximum_hop_limit) {
                return DetectionReport{sig, DetectionReport::DetectionResult::Detected};
            }
        }

        if (sig.patterns.count("allowed_so_range")) {
            geonet_allowed_so_range allowed_so_range = boost::get<geonet_allowed_so_range>(sig.patterns["allowed_so_range"]);
            vanetza::units::Length limit_a 
                = boost::get<geonet_destination_area>(sig.patterns["allowed_so_range"]).distance_a * boost::units::si::meters;
            vanetza::units::Length limit_b
                = boost::get<geonet_destination_area>(sig.patterns["allowed_so_range"]).distance_b * boost::units::si::meters;
            if (allowed_so_range.shape == Allowed_So_Range_Shape::Circle) {
                vanetza::security::TwoDLocation range_center 
                    = vanetza::security::TwoDLocation(m_positioning.position_fix().latitude, m_positioning.position_fix().longitude);
                vanetza::security::CircularRegion allowed_range_circle = vanetza::security::CircularRegion(range_center, limit_a);
                
                vanetza::geonet::geo_angle_i32t source_latitude = m_data.source_position.latitude;
                vanetza::geonet::geo_angle_i32t source_longitude = m_data.source_position.longitude;
                vanetza::security::TwoDLocation source_position 
                    = vanetza::security::TwoDLocation(source_latitude, source_longitude);

                if (!vanetza::security::is_within(source_position, allowed_range_circle)) {
                    return DetectionReport{sig, DetectionReport::DetectionResult::Detected};
                }

            } else {
                std::cout << "[sid: " << sig.meta.sid << "]" << "Detection Skipped: " <<
                    "Shapes except for circle as allowed_so_range's shape is currently not supported" << "\n";
                continue;
            }
        }

          /* Deprecated
        if (sig.patterns.count("destination_area") && m_data.destination) {
            DestinationVariant dest = *m_data.destination;
            vanetza::units::Length limit_a 
                = boost::get<geonet_destination_area>(sig.patterns["destination_area"]).distance_a * boost::units::si::meters;
            vanetza::units::Length limit_b
                = boost::get<geonet_destination_area>(sig.patterns["destination_area"]).distance_b * boost::units::si::meters;
            
            if(dest.type() == typeid(Area)){
                Area dest_area = boost::get<Area>(dest);
                if (dest_area.shape.type() == typeid(Circle)) {
                    Circle dest_area_circle = boost::get<Circle>(dest_area.shape);
                    if (limit_a < dest_area_circle.r){
                        //message_handler.handle(action, msg)
                    }
                } else if (dest_area.shape.type() == typeid(Rectangle)) {
                    Rectangle dest_area_rect = boost::get<Rectangle>(dest_area.shape);
                    if (limit_a < dest_area_rect.a || limit_b < dest_area_rect.b){
                        //message_handler.handle(sig)
                    }
                } else if (dest_area.shape.type() == typeid(Ellipse)) {
                    Ellipse dest_area_ellipse = boost::get<Ellipse>(dest_area.shape);
                    if (limit_a < dest_area_ellipse.a || limit_b < dest_area_ellipse.b){
                        //message_handler.handle(action, msg)
                    }
                } else {
                    std::cout << "[sid: " << sig.meta.sid << "]" << "Detection skipped: Invalid destination shape" << "\n";
                    continue;
                }

            } else {
                std::cout << "[sid: " << sig.meta.sid << "]" << "Detection Skipped: area is not included in the destination field" << "\n";
                continue;
            }
        }
        */
    }
    return DetectionReport{boost::optional<Signature>(), DetectionReport::DetectionResult::Detected};
}



} // namespace mitvane
