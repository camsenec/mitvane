/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/cohda.hpp>
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


#ifndef COHDA_HPP_GBENHCVN
#define COHDA_HPP_GBENHCVN

#include <boost/optional/optional.hpp>
#include <vanetza/access/data_request.hpp>
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/clock.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <vanetza/net/ethernet_header.hpp>

namespace vanetza
{

/**
 * Add Physical and Link layer headers understood by Cohda V2X API
 * \param req access layer request parameters
 * \param packet packet to be transmitted
 */
void insert_cohda_tx_header(const access::DataRequest& req, std::unique_ptr<ChunkPacket>& packet);

/**
 * Remove packet headers by Cohda V2X API and build Ethernet header from them
 * \return equivalent EthernetHeader if successfully received
 */
boost::optional<EthernetHeader> strip_cohda_rx_header(CohesivePacket&);

} // namespace vanetza

#endif /* COHDA_HPP_GBENHCVN */

