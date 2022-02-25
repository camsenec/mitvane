/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/dcc_passthrough.cpp>
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


#include "dcc_passthrough.hpp"
#include "time_trigger.hpp"
#include <vanetza/access/data_request.hpp>
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/dcc/mapping.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <iostream>

using namespace vanetza;

DccPassthrough::DccPassthrough(access::Interface& access, TimeTrigger& trigger) :
        access_(access), trigger_(trigger) {}


void DccPassthrough::request(const dcc::DataRequest& request, std::unique_ptr<ChunkPacket> packet)
{
    if (!allow_packet_flow_) {
        std::cout << "ignored request because packet flow is suppressed\n";
        return;
    }

    trigger_.schedule();

    access::DataRequest acc_req;
    acc_req.ether_type = request.ether_type;
    acc_req.source_addr = request.source;
    acc_req.destination_addr = request.destination;
    acc_req.access_category = dcc::map_profile_onto_ac(request.dcc_profile);
    access_.request(acc_req, std::move(packet));
}

void DccPassthrough::allow_packet_flow(bool allow)
{
    allow_packet_flow_ = allow;
}

bool DccPassthrough::allow_packet_flow()
{
    return allow_packet_flow_;
}
