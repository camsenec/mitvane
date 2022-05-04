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

#include "logging.hpp"
#include "message_handler.hpp"
#include "mitvane/rule_reader/signature.hpp"

namespace mitvane
{

MessageHandler::MessageHandler(){
    mitvane::Logging::init_log();
}


std::string action_to_string(Action& action){
    if (action == Action::Alert) {
        return "alert";
    } else if (action == Action::Drop) {
        return "drop";
    } else {
        return "unknown action";
    }
}

std::string protocol_to_string(Protocol& protocol){
    if (protocol == Protocol::GeoNetworking) {
        return "GeoNetworking";
    } else if (protocol == Protocol::BTP) {
        return "BTP";
    } else if (protocol == Protocol::Facility) {
        return "Facility";
    } else if (protocol == Protocol::CA) {
        return "CA";
    } else if (protocol == Protocol::DEN) {
        return "DEN";
    } else if (protocol == Protocol::SPAT) {
        return "SPAT";
    } else if (protocol == Protocol::MAP) {
        return "MAP";
    } else {
        return "unknown protocol";
    }
}

std::string sid_rev_to_string(MetaData& meta) {
    return "[" + std::to_string(meta.sid) + ":" + std::to_string(meta.rev) + "] ";
}


std::string format_log(Signature& signature) {
    return sid_rev_to_string(signature.meta) + 
        signature.meta.msg + 
        + " " + action_to_string(signature.action) + " " + 
        "{" + protocol_to_string(signature.protocol) + "}"; 
}

HandleReport MessageHandler::handle(std::vector<Signature>& sigs) {
    HandleReport rep = HandleReport::Allow;
    for (auto sig = sigs.begin(); sig != sigs.end(); ++sig) {
        if (sig->action == Action::Alert) {
            BOOST_LOG(lg) << format_log(*sig);
        } else if (sig->action == Action::Drop) {
            BOOST_LOG(lg) << format_log(*sig);
            rep = HandleReport::Drop;
        } else {
            BOOST_LOG(lg) << "Unknown action. No log output.";
            rep = HandleReport::Drop;
        }
    }
    return rep;
}

} // namespace mitvane
