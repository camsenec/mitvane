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


#include "vanetza-extension/asn1/denm.hpp"
#include "vanetza-extension/facilities/denm_functions.hpp"
#include <iostream>

namespace vanetzaExtension
{
namespace facilities
{
namespace denm
{

void print_indented(std::ostream& os, const asn1::Denm& message, const std::string& indent, unsigned level)
{
    auto prefix = [&](const char* field) -> std::ostream& {
        for (unsigned i = 0; i < level; ++i) {
            os << indent;
        }
        os << field << ": ";
        return os;
    };

    const ItsPduHeader_t& header = message->header;
    prefix("ITS PDU Header") << "\n";
    ++level;
    prefix("Protocol Version") << header.protocolVersion << "\n";
    prefix("Message ID") << header.messageID << "\n";
    prefix("Station ID") << header.stationID << "\n";
    --level;

    const DecentralizedEnvironmentalNotificationMessage_t& denm = message->denm;
    prefix("DecentralizedEnvironmentalNotification") << "\n";
    ++level;
    prefix("Management Container") << "\n";
    ++level;
    prefix("Action ID") << "\n";
    ++level;
    prefix("Originating Station ID") << denm.management.actionID.originatingStationID << "\n";
    prefix("Sequence Number") << denm.management.actionID.sequenceNumber << "\n";
    --level;
    long detectionTime, referenceTime;
    asn_INTEGER2long(&denm.management.detectionTime, &detectionTime);
    prefix("Detection Time") << detectionTime << "\n";
    asn_INTEGER2long(&denm.management.referenceTime, &referenceTime);
    prefix("Detection Time") << referenceTime << "\n";

    prefix("Event Position") << "\n";
    ++level;
    prefix("Altitude") << denm.management.eventPosition.altitude.altitudeValue << "\n";
    prefix("Altitude Confidence") << denm.management.eventPosition.altitude.altitudeConfidence << "\n";
    prefix("Longitude") << denm.management.eventPosition.longitude << "\n";
    prefix("Latitude") << denm.management.eventPosition.latitude << "\n";
    prefix("Semi Major Orientation") << denm.management.eventPosition.positionConfidenceEllipse.semiMajorOrientation << "\n";
    prefix("Semi Major Confidence") << denm.management.eventPosition.positionConfidenceEllipse.semiMajorConfidence << "\n";
    prefix("Semi Minor Confidence") << denm.management.eventPosition.positionConfidenceEllipse.semiMinorConfidence << "\n";
    --level;
    --level;
}

} // namespace denm
} // namespace facilities
} // namespace vanetzaExtension
