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


#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include "mitvane/rule_reader/signature.hpp"
#include "mitvane/idps_context.hpp"
#include "boost/optional/optional.hpp"

namespace mitvane 
{

struct DetectionReport 
{
    enum class DetectionResult
    {
        Detected,
        Not_Detected
    };
    DetectionReport() = default;
    DetectionReport(boost::optional<Signature> sig, DetectionResult result) : 
        sig{sig}, result{result} {}

    boost::optional<Signature> sig;
    DetectionResult result;
};

class Detector
{   
    public:
        virtual DetectionReport detect(signatures_type &signatures) = 0;
};



} // namespace mitvane

#endif /* DETECTOR_HPP */