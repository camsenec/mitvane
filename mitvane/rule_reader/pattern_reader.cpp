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


#include "pattern_reader.hpp"
#include "signature.hpp"
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include <iostream>

namespace mitvane
{

PatternReaderStatusCode PatternReader::read_geonet_pattern(YAML::Node rules, Signature &sig)
{
    for (auto it = geonet_keywords.begin(); it < geonet_keywords.end(); ++it)
    {
        std::string key = *it;
        if (rules[key].IsDefined()) 
        {
            if (key == "nh") 
            {
                const std::string nh = rules[key].as<std::string>();
                if (nh != "common" && nh != "secure") 
                {
                    std::cerr << "nh must be common or secure" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                sig.patterns[key] = nh;
            }
            else if (key == "ht") 
            {
                const std::string ht = rules[key].as<std::string>();
                if (ht != "TSB" && ht != "GBC" && ht != "SHB") 
                {
                    std::cerr << "ht must be TSB, GBC or SHB" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                sig.patterns[key] = ht;
            } 
            else if (key == "mhl") 
            {
                const int mhl = rules[key].as<int>();
                sig.patterns[key] = mhl;
            } 
            else if (key == "destination_area") 
            {
                geonet_destination_area geo_dest_area;

                YAML::Node destination_area_node = rules[key];
                if (!destination_area_node.IsMap()) 
                {
                    std::cerr << "Bad destination area" << "\n";
                    return PatternReaderStatusCode::Error;
                }

                YAML::Node distance_a_node = destination_area_node["destination_a"];
                if (!distance_a_node.IsDefined()) 
                {
                    std::cerr << "Destination a is required for destination_area" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                if (!distance_a_node.IsScalar()) 
                {
                    std::cerr << "Bad destination a for destination_area" << "\n";
                    return PatternReaderStatusCode::Error;
                }

                YAML::Node distance_b_node = destination_area_node["destination_b"];
                if (!distance_b_node.IsDefined()) 
                {
                    std::cerr << "Destination a is required for destination_area" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                if (!distance_b_node.IsScalar()) 
                {
                    std::cerr << "Bad destination b for destination_area" << "\n";
                    return PatternReaderStatusCode::Error;
                }

                geo_dest_area.distance_a = distance_a_node.as<int>();
                geo_dest_area.distance_b = distance_b_node.as<int>();

                sig.patterns[key] = geo_dest_area;

            } 
            else if (key == "allowed_so_range") 
            {
                geonet_allowed_so_range geo_allowed_so_range;
                YAML::Node allowed_so_range_node = rules[key];
                if (!allowed_so_range_node.IsMap()) 
                {
                    std::cerr << "Bad allowed so range" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                
                YAML::Node shape_node = allowed_so_range_node["shape"];
                if (!shape_node.IsDefined()) 
                {
                    std::cerr << "Shape is required for allowed_so_range" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                if (!shape_node.IsScalar()) 
                {
                    std::cerr << "Bad shape for allowed_so_range" << "\n";
                    return PatternReaderStatusCode::Error;
                }

                YAML::Node distance_a_node = allowed_so_range_node["destination_a"];
                if (!distance_a_node.IsDefined()) 
                {
                    std::cerr << "Destination a is required for allowed_so_range" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                if (!distance_a_node.IsScalar()) 
                {
                    std::cerr << "Bad destination a for allowed_so_range" << "\n";
                    return PatternReaderStatusCode::Error;
                }

                YAML::Node distance_b_node = allowed_so_range_node["destination_b"];
                if (!distance_b_node.IsDefined()) 
                {
                    std::cerr << "Destination b is required for allowed_so_range" << "\n";
                    return PatternReaderStatusCode::Error;
                }
                if (!distance_b_node.IsScalar()) 
                {
                    std::cerr << "Bad destination b for allowed_so_range" << "\n";
                    return PatternReaderStatusCode::Error;
                }

                geo_allowed_so_range.distance_a = distance_a_node.as<int>();
                geo_allowed_so_range.distance_b = distance_b_node.as<int>();

                sig.patterns[key] = geo_allowed_so_range;
                
            } 
            else 
            {
                std::cerr << "Keyword: " << key << " is not supported" << "\n";
            }
        }
    }

    return PatternReaderStatusCode::Success;
 
}

PatternReaderStatusCode PatternReader::read_facility_pattern(YAML::Node rules, Signature &sig)
{
    for (auto it = geonet_keywords.begin(); it < geonet_keywords.end(); ++it)
    {
        std::string key = *it;
        if (rules[key].IsDefined()) 
        {
            if (key == "allowed_ids") {
                std::vector<uint32_t> allowed_ids;
                YAML::Node allowed_ids_node = rules[key];

                if (!allowed_ids_node.IsSequence()) 
                {
                    std::cerr << "Bad allowed ids" << "\n";
                    return PatternReaderStatusCode::Error;
                }

                for (int i = 0; i < (int)allowed_ids_node.size(); ++i) 
                {
                    int id = allowed_ids_node[i].as<int>();
                    allowed_ids.emplace_back(id);
                }

                sig.patterns[key] = allowed_ids;
            } 
            else 
            {
                std::cerr << "Keyword: " << key << " is not supported" << "\n";
            }
        }
    }
    
    return PatternReaderStatusCode::Success;
}



} // namespace mitvane

