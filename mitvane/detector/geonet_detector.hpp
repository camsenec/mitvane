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


#ifndef GEONET_DETECTOR_HPP
#define GEONET_DETECTOR_HPP

#include "mitvane/idps_context.hpp"
#include "detector.hpp"
#include "geonet_data.hpp"

namespace mitvane 
{

enum class TransformStatusCode {
    Success,
    Error
};

class GeonetDetector : public Detector
{    
    
    public:
        GeonetDetector (GeonetData& data, vanetza::PositionProvider& positioning);
        DetectionReport detect(signatures_type &signatures) override;

    private:
        TransformStatusCode transform_nh(std::string nh_str, vanetza::geonet::NextHeaderBasic &nh);
        TransformStatusCode transform_ht(std::string ht_str, uint8_t &ht);
        GeonetData& m_data; 
        vanetza::PositionProvider& m_positioning;
};



} // namespace mitvane

#endif /* GEONET_DETECTOR_HPP */