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


#ifndef DCC_PASSTHROUGH_HPP_GSDFESAE
#define DCC_PASSTHROUGH_HPP_GSDFESAE

#include "time_trigger.hpp"
#include <vanetza/access/interface.hpp>
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/net/cohesive_packet.hpp>

class DccPassthrough : public vanetza::dcc::RequestInterface
{
public:
    DccPassthrough(vanetza::access::Interface&, TimeTrigger& trigger);

    void request(const vanetza::dcc::DataRequest& request, std::unique_ptr<vanetza::ChunkPacket> packet) override;

    void allow_packet_flow(bool allow);
    bool allow_packet_flow();

private:
    vanetza::access::Interface& access_;
    TimeTrigger& trigger_;
    bool allow_packet_flow_ = true;
};

#endif /* DCC_PASSTHROUGH_HPP_GSDFESAE */
