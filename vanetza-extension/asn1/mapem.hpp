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


#ifndef MAPEM_HPP_XGC8NRDI
#define MAPEM_HPP_XGC8NRDI

#include "vanetza-extension/asn1/asn1c_wrapper.hpp"
#include "vanetza-extension/asn1/its/MAPEM.h"

namespace vanetzaExtension
{
namespace asn1
{

class Mapem : public asn1c_per_wrapper<MAPEM_t>
{
public:
    using wrapper = asn1c_per_wrapper<MAPEM_t>;
    Mapem() : wrapper(asn_DEF_MAPEM) {}
};

} // namespace asn1
} // namespace vanetzaExtension

#endif /* MAPEM_HPP_XGC8NRDI */

