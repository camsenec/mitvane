/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/asn1/denm.hpp>
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


#ifndef DENM_HPP_XGC8NRDI
#define DENM_HPP_XGC8NRDI

#include "vanetza-extension/asn1/asn1c_wrapper.hpp"
#include "vanetza-extension/asn1/its/DENM.h"

namespace vanetzaExtension
{
namespace asn1
{

class Denm : public asn1c_per_wrapper<DENM_t>
{
public:
    using wrapper = asn1c_per_wrapper<DENM_t>;
    Denm() : wrapper(asn_DEF_DENM) {}
};

} // namespace asn1
} // namespace vanetzaExtension

#endif /* DENM_HPP_XGC8NRDI */

