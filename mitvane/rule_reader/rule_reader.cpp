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


#include "rule_reader.hpp"
#include "signature.hpp"
#include "pattern_reader.hpp"
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <iostream>


namespace mitvane 
{

RuleReader::RuleReader(std::string rule_filepath) :
    m_rule_filepath(rule_filepath){}

Action RuleReader::read_action(YAML::Node action_node) 
{
    std::string action = action_node.as<std::string>();
    transform(action.begin(), action.end(), action.begin(), ::tolower);
    if (action == "alert") {
        return Action::Alert;
    } else if (action == "drop") {
        return Action::Drop;
    } else {
        return Action::NotSupported_Action;
    }
}

Protocol RuleReader::read_protocol(YAML::Node protocol_node) 
{
    std::string protocol = protocol_node.as<std::string>();
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::tolower);

    if (protocol == "geonetworking" || protocol == "geonet") {
        return Protocol::GeoNetworking;
    } else if (protocol == "btp") {
        return Protocol::BTP;
    } else if (protocol == "facility") {
        return Protocol::Facility;
    } else if (protocol == "ca" || protocol == "cam") {
        return Protocol::CA;
    } else if (protocol == "den" || protocol == "denm") {
        return Protocol::DEN;
    } else if (protocol == "spat" || protocol == "spatem" || protocol == "tlm") {
        return Protocol::SPAT;
    } else if (protocol == "map" || protocol == "mapem" || protocol == "rlt") {
        return Protocol::MAP;
    } else {
        return Protocol::NotSupported_Protocol;
    }
}

MetaReaderStatusCode RuleReader::read_metadata(YAML::Node meta_node, MetaData &meta) 
{
    // read msg
    YAML::Node msg_node = meta_node["msg"];

    if (!msg_node.IsDefined()) {
        return MetaReaderStatusCode::Undefined_Msg;
    }
    if (!msg_node.IsScalar()) {
        return MetaReaderStatusCode::Bad_Msg;
    }

    meta.msg = msg_node.as<std::string>();
    
    // read id
    YAML::Node sid_node = meta_node["sid"];

    if (!sid_node.IsDefined()) {
        return MetaReaderStatusCode::Undefined_Sid;
    }
    if (!sid_node.IsScalar()) {
        return MetaReaderStatusCode::Bad_Sid;
    }

    meta.sid = sid_node.as<uint32_t>();

    // read rev
    YAML::Node rev_node = meta_node["rev"];

    if (!rev_node.IsDefined()) {
        return MetaReaderStatusCode::Undefined_Sid;
    }
    if (!rev_node.IsScalar()) {
        return MetaReaderStatusCode::Bad_Sid;
    }

    meta.rev = rev_node.as<uint16_t>();

    return MetaReaderStatusCode::Success;
}

RuleReaderStatusCode RuleReader::read(std::map<Protocol, std::vector<Signature>> &signatures) 
{
    YAML::Node ruleset = YAML::LoadFile(m_rule_filepath);
    if (!ruleset.IsMap()) {
        return RuleReaderStatusCode::Broken_Rule_Structure;
    }

    YAML::Node rules = ruleset["rules"];
    if (!rules.IsSequence()) {
        return RuleReaderStatusCode::Broken_Rule_Structure;
    }
  
    for(int i = 0; i < (int)rules.size(); ++i) {
        Signature sig;

        // read action
        YAML::Node action_node = rules[i]["action"];
        if (!action_node.IsDefined()) {
            return RuleReaderStatusCode::Undefined_Action;
        }
        if (!action_node.IsScalar()) {
            return RuleReaderStatusCode::Bad_Action;
        }

        Action action = read_action(action_node);
        if (action == Action::NotSupported_Action) {
            return RuleReaderStatusCode::Bad_Action;
        }
        sig.action = action;
        
        // read protocol
        YAML::Node protocol_node = rules[i]["protocol"];
        if (!protocol_node.IsDefined()) {
            return RuleReaderStatusCode::Undefined_Protocol;
        }
        
        if (!protocol_node.IsScalar()) {
            return RuleReaderStatusCode::Bad_Protocol;
        }

        Protocol protocol = read_protocol(protocol_node);
        if (protocol == Protocol::NotSupported_Protocol) {
            return RuleReaderStatusCode::Bad_Protocol;
        }
        sig.protocol = protocol;

        // read metadata
        YAML::Node meta_node = rules[i]["meta"];
        if (!meta_node.IsDefined()) {
            return RuleReaderStatusCode::Undefined_MetaData;
        }
        
        if (!meta_node.IsMap()) {
            return RuleReaderStatusCode::Bad_MetaData;
        }

        MetaData meta;
        MetaReaderStatusCode ret = read_metadata(meta_node, meta);
        if (ret != MetaReaderStatusCode::Success) {
            return RuleReaderStatusCode::Bad_MetaData;
        }
        

        PatternReader pattern_reader = PatternReader();
        PatternReaderStatusCode pattern_st;
        switch (protocol) {
            case Protocol::GeoNetworking:
            {
                pattern_st = pattern_reader.read_geonet_pattern(rules, sig); 
                if (pattern_st != PatternReaderStatusCode::Success){
                    return RuleReaderStatusCode::Bad_Pattern;
                }
                if(signatures.count(Protocol::GeoNetworking) == 0) {
                    signatures[Protocol::GeoNetworking] = std::vector<Signature>{sig};
                } else {
                    signatures[Protocol::GeoNetworking].emplace_back(sig);
                }
                break;
            }
            case Protocol::BTP:
            {
                break;
            }
            case Protocol::Facility:
            {
                pattern_st = pattern_reader.read_facility_pattern(rules, sig);
                if (pattern_st != PatternReaderStatusCode::Success){
                    return RuleReaderStatusCode::Bad_Pattern;
                }
                if(signatures.count(Protocol::Facility) == 0) {
                    signatures[Protocol::Facility] = std::vector<Signature>{sig};
                } else {
                    signatures[Protocol::Facility].emplace_back(sig);
                }
                break;
            }
            case Protocol::CA:
            {
                break;
            }
            case Protocol::DEN:
            {
                break;
            }
            case Protocol::MAP:
            {
                break;
            }
            case Protocol::SPAT:
            {
                break;
            }
            default:
                break;
        }
    }
     return RuleReaderStatusCode::Success;
}

} // namespace mitvane