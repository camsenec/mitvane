/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/facilities/cam_functions.cpp>
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
 * - Several functions are moved from the original cam_function.cpp at 
 *   <https://github.com/riebl/vanetza/blob/master/vanetza/facilities/cam_functions.cpp>
 * 
*/


#include "vanetza-extension/facilities/functions.hpp"
#include "vanetza-extension/asn1/its/ReferencePosition.h"
#include <vanetza/common/position_fix.hpp>
#include <vanetza/geonet/areas.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/systems/angle/degrees.hpp>

namespace vanetzaExtension
{
namespace facilities
{

static const auto microdegree = vanetza::units::degree * vanetza::units::si::micro;


vanetza::units::Length distance(const ReferencePosition_t& a, const ReferencePosition_t& b)
{
    using vanetza::geonet::GeodeticPosition;
    using vanetza::units::GeoAngle;

    auto length = vanetza::units::Length::from_value(std::numeric_limits<double>::quiet_NaN());
    if (is_available(a) && is_available(b)) {
        GeodeticPosition geo_a {
            GeoAngle { a.latitude / Latitude_oneMicrodegreeNorth * microdegree },
            GeoAngle { a.longitude / Longitude_oneMicrodegreeEast * microdegree }
        };
        GeodeticPosition geo_b {
            GeoAngle { b.latitude / Latitude_oneMicrodegreeNorth * microdegree },
            GeoAngle { b.longitude / Longitude_oneMicrodegreeEast * microdegree }
        };
        length = vanetza::geonet::distance(geo_a, geo_b);
    }
    return length;
}

vanetza::units::Length distance(const ReferencePosition_t& a, vanetza::units::GeoAngle lat, vanetza::units::GeoAngle lon)
{
    using vanetza::geonet::GeodeticPosition;
    using vanetza::units::GeoAngle;

    auto length = vanetza::units::Length::from_value(std::numeric_limits<double>::quiet_NaN());
    if (is_available(a)) {
        GeodeticPosition geo_a {
            GeoAngle { a.latitude / Latitude_oneMicrodegreeNorth * microdegree },
            GeoAngle { a.longitude / Longitude_oneMicrodegreeEast * microdegree }
        };
        GeodeticPosition geo_b { lat, lon };
        length = vanetza::geonet::distance(geo_a, geo_b);
    }
    return length;
}

bool is_available(const ReferencePosition& pos)
{
    return pos.latitude != Latitude_unavailable && pos.longitude != Longitude_unavailable;
}


template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v { q };
    return std::round(v.value());
}

void copy(const vanetza::PositionFix& position, ReferencePosition& reference_position) {
    reference_position.longitude = round(position.longitude, microdegree) * Longitude_oneMicrodegreeEast;
    reference_position.latitude = round(position.latitude, microdegree) * Latitude_oneMicrodegreeNorth;
    reference_position.positionConfidenceEllipse.semiMajorOrientation = HeadingValue_unavailable;
    reference_position.positionConfidenceEllipse.semiMajorConfidence = SemiAxisLength_unavailable;
    reference_position.positionConfidenceEllipse.semiMinorConfidence = SemiAxisLength_unavailable;
    if (position.altitude) {
        reference_position.altitude.altitudeValue = to_altitude_value(position.altitude->value());
        reference_position.altitude.altitudeConfidence = to_altitude_confidence(position.altitude->confidence());
    } else {
        reference_position.altitude.altitudeValue = AltitudeValue_unavailable;
        reference_position.altitude.altitudeConfidence = AltitudeConfidence_unavailable;
    }
}

AltitudeConfidence_t to_altitude_confidence(vanetza::units::Length confidence)
{
    const double alt_con = confidence / vanetza::units::si::meter;

    if (alt_con < 0 || std::isnan(alt_con)) {
        return AltitudeConfidence_unavailable;
    } else if (alt_con <= 0.01) {
        return AltitudeConfidence_alt_000_01;
    } else if (alt_con <= 0.02) {
        return AltitudeConfidence_alt_000_02;
    } else if (alt_con <= 0.05) {
        return AltitudeConfidence_alt_000_05;
    } else if (alt_con <= 0.1) {
        return AltitudeConfidence_alt_000_10;
    } else if (alt_con <= 0.2) {
        return AltitudeConfidence_alt_000_20;
    } else if (alt_con <= 0.5) {
        return AltitudeConfidence_alt_000_50;
    } else if (alt_con <= 1.0) {
        return AltitudeConfidence_alt_001_00;
    } else if (alt_con <= 2.0) {
        return AltitudeConfidence_alt_002_00;
    } else if (alt_con <= 5.0) {
        return AltitudeConfidence_alt_005_00;
    } else if (alt_con <= 10.0) {
        return AltitudeConfidence_alt_010_00;
    } else if (alt_con <= 20.0) {
        return AltitudeConfidence_alt_020_00;
    } else if (alt_con <= 50.0) {
        return AltitudeConfidence_alt_050_00;
    } else if (alt_con <= 100.0) {
        return AltitudeConfidence_alt_100_00;
    } else if (alt_con <= 200.0) {
        return AltitudeConfidence_alt_200_00;
    } else {
        return AltitudeConfidence_outOfRange;
    }
}

AltitudeValue_t to_altitude_value(vanetza::units::Length alt)
{
    using boost::units::isnan;

    if (!isnan(alt)) {
        alt = boost::algorithm::clamp(alt, -1000.0 * vanetza::units::si::meter, 8000.0 * vanetza::units::si::meter);
        return AltitudeValue_oneCentimeter * 100.0 * (alt / vanetza::units::si::meter);
    } else {
        return AltitudeValue_unavailable;
    }
}


} // namespace facilities
} // namespace vanetzaExtension