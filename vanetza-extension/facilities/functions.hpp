/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/facilities/cam_functions.hpp>
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
 * - Several functions are moved from the original cam_function.hpp at 
 *   <https://github.com/riebl/vanetza/blob/master/vanetza/facilities/cam_functions.cpp>
 * 
*/


#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "vanetza/asn1/its/ReferencePosition.h"
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

/**
 * Calculate distance between positions
 * \param a one position
 * \param b another position
 * \return distance between given positions (or NaN if some position is unavailable)
 */
vanetza::units::Length distance(const ReferencePosition_t& a, const ReferencePosition_t& b);
vanetza::units::Length distance(const ReferencePosition_t& a, vanetza::units::GeoAngle lat, vanetza::units::GeoAngle lon);

/**
 * Check if ASN.1 data element indicates unavailable value
 * \return true if value is available
 */
bool is_available(const ReferencePosition_t&);

/**
 * Round value
 * \param q rounded value
 * \param u unit to round 
 * \return rounded value
 */
template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u);

/**
 * Copy position information into a ReferencePosition structure from CDD
 */
void copy(const vanetza::PositionFix&, ReferencePosition&);

/**
 * Convert altitude to AltitudeValue from CDD
 */
AltitudeValue_t to_altitude_value(vanetza::units::Length);

/**
 * Convert altitude confidence to AltitudeConfidence from CDD
 */
AltitudeConfidence_t to_altitude_confidence(vanetza::units::Length);

}; //namespace facilities
}; //namespace vanetzaExtension

#endif /* FUNCTIONS_HPP */