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

#ifndef RULE_READER_HPP
#define RULE_READER_HPP

#include "signature.hpp"
#include <yaml-cpp/yaml.h>
#include <iostream>


namespace mitvane 
{

enum class MetaReaderStatusCode 
{
    Success,
    Undefined_Msg,
    Bad_Msg,
    Undefined_Sid,
    Bad_Sid,
    Undefined_Rev,
    Bad_Rev
};

enum class RuleReaderStatusCode 
{
    Success,
    Broken_Rule_Structure,
    Undefined_Action,
    Bad_Action,
    Undefined_Protocol,
    Bad_Protocol,
    Undefined_MetaData,
    Bad_MetaData,
    Bad_Pattern
};

class RuleReader 
{
    public:
        RuleReader(std::string rule_filepath);
        RuleReaderStatusCode read(std::map<Protocol, std::vector<Signature>> &signatures);
    private:
        Action read_action(YAML::Node action_node);
        Protocol read_protocol(YAML::Node protocol_node);
        MetaReaderStatusCode read_metadata(YAML::Node meta_node, MetaData &meta);

        std::string m_rule_filepath;
        
};

} // namespace mitvane

#endif // RULE_READER_HPP