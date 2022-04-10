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


#ifndef YAML_UTIL_HPP
#define YAML_UTIL_HPP

#include <yaml-cpp/yaml.h>
#include <boost/variant/variant.hpp>
#include <map>
#include <iostream>

namespace mitvane
{


class YamlUtil 
{
    public:
        inline static void printType(YAML::Node node)
        {
            switch (node.Type()) {
                case YAML::NodeType::Null:
                std::cout << "Null" << std::endl;
                break;
                case YAML::NodeType::Scalar:
                std::cout << "Scalar" << std::endl;
                break;
                case YAML::NodeType::Sequence: 
                std::cout << "Sequence" << std::endl;
                break;
                case YAML::NodeType::Map:
                std::cout << "map" << std::endl;
                break;
                case YAML::NodeType::Undefined:
                std::cout << "Undefined" << std::endl;
                break;
            }
        }
};


} // namespace mitvane

#endif // YAML_UTIL_HPP