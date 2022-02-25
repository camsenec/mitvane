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
 * - Several function definitions are removed and moved to functions.hpp.
 * 
*/


#ifndef CAM_FUNCTIONS_HPP_PUFKBEM8
#define CAM_FUNCTIONS_HPP_PUFKBEM8

#include "vanetza-extension/asn1/its/Heading.h"
#include <vanetza/security/cam_ssp.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>


// forward declaration of asn1c generated struct
struct BasicVehicleContainerLowFrequency;

namespace vanetzaExtension
{

// forward declaration of CAM message wrapper
namespace asn1 { class Cam; }

namespace facilities
{

namespace cam
{

class PathHistory;

/**
 * Copy PathHistory into BasicVehicleContainerLowFrequency's pathHistory element
 * \param Facilities' path history object (source)
 * \param ASN.1 CAM container (destination)
 */
void copy(const PathHistory&, BasicVehicleContainerLowFrequency&);

/**
 * Check if difference of two given heading values is within a limit
 * \param a one heading
 * \param b another heading
 * \param limit maximum difference (positive)
 * \return true if similar enough
 */
bool similar_heading(const Heading& a, const Heading& b, vanetza::units::Angle limit);
bool similar_heading(const Heading& a, vanetza::units::Angle b, vanetza::units::Angle limit);
bool similar_heading(vanetza::units::Angle a, vanetza::units::Angle b, vanetza::units::Angle limit);

/**
 * Check if ASN.1 data element indicates unavailable value
 * \return true if value is available
 */
bool is_available(const Heading&);

/**
 * Check if a CAM contains only allowed data elements
 * \param cam CA message
 * \param ssp CA service specific permissions
 * \return true if no forbidden data elements are included
 */
bool check_service_specific_permissions(const asn1::Cam& cam, vanetza::security::CamPermissions ssp);

/**
 * Print CAM content with indentation of nested fields
 * \param os output stream
 * \param cam CA message
 * \param indent indentation marker, by default one tab per level
 * \param start initial level of indentation
 *
 * This function is an idea of Erik de Britto e Silva (erikbritto@github)
 * from University of Antwerp - erik.debrittoesilva@uantwerpen.be
 */
void print_indented(std::ostream& os, const asn1::Cam& cam, const std::string& indent = "\t", unsigned start = 0);

} // namespace cam
} // namespace facilities
} // namespace vanetzaExtension

#endif /* CAM_FUNCTIONS_HPP_PUFKBEM8 */
