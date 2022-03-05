/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/facilities/cam_functions.cpp>
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

/* 
 * State Changes
 * - Include file paths are changed.
 * - Several functions are removed and moved to functions.cpp.
 * 
*/


#include "vanetza-extension/asn1/cam.hpp"
#include "vanetza-extension/facilities/cam_functions.hpp"
#include <boost/algorithm/clamp.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <algorithm>
#include <limits>
#undef min

namespace vanetzaExtension
{
namespace facilities
{
namespace cam
{

using vanetza::units::Angle;

static const auto microdegree = vanetza::units::degree * vanetza::units::si::micro;


bool similar_heading(const Heading& a, const Heading& b, Angle limit)
{
    // HeadingValues are tenth of degree (900 equals 90 degree east)
    static_assert(HeadingValue_wgs84East == 900, "HeadingValue interpretation fails");

    bool result = false;
    if (is_available(a) && is_available(b)) {
        using vanetza::units::degree;
        const Angle angle_a { a.headingValue / 10.0 * degree };
        const Angle angle_b { b.headingValue / 10.0 * degree };
        result = similar_heading(angle_a, angle_b, limit);
    }

    return result;
}

bool similar_heading(const Heading& a, Angle b, Angle limit)
{
    bool result = false;
    if (is_available(a)) {
        using vanetza::units::degree;
        result = similar_heading(Angle { a.headingValue / 10.0 * degree}, b, limit);
    }
    return result;
}

bool similar_heading(Angle a, Angle b, Angle limit)
{
    using namespace boost::units;
    using boost::math::double_constants::pi;

    static const Angle full_circle = 2.0 * pi * si::radian;
    const Angle abs_diff = fmod(abs(a - b), full_circle);
    return abs_diff <= limit || abs_diff >= full_circle - limit;
}

bool is_available(const Heading& hd)
{
    return hd.headingValue != HeadingValue_unavailable;
}

bool check_service_specific_permissions(const asn1::Cam& cam, vanetza::security::CamPermissions ssp)
{
    using vanetza::security::CamPermission;
    using vanetza::security::CamPermissions;

    CamPermissions required_permissions;
    const CamParameters_t& params = cam->cam.camParameters;

    if (params.highFrequencyContainer.present == HighFrequencyContainer_PR_rsuContainerHighFrequency) {
        const RSUContainerHighFrequency_t& rsu = params.highFrequencyContainer.choice.rsuContainerHighFrequency;
        if (rsu.protectedCommunicationZonesRSU) {
            required_permissions.add(CamPermission::CEN_DSRC_Tolling_Zone);
        }
    }

    if (const SpecialVehicleContainer_t* special = params.specialVehicleContainer) {
        const EmergencyContainer_t* emergency = nullptr;
        const SafetyCarContainer_t* safety = nullptr;
        const RoadWorksContainerBasic_t* roadworks = nullptr;

        switch (special->present) {
            case SpecialVehicleContainer_PR_publicTransportContainer:
                required_permissions.add(CamPermission::Public_Transport);
                break;
            case SpecialVehicleContainer_PR_specialTransportContainer:
                required_permissions.add(CamPermission::Special_Transport);
                break;
            case SpecialVehicleContainer_PR_dangerousGoodsContainer:
                required_permissions.add(CamPermission::Dangerous_Goods);
                break;
            case SpecialVehicleContainer_PR_roadWorksContainerBasic:
                required_permissions.add(CamPermission::Roadwork);
                roadworks = &special->choice.roadWorksContainerBasic;
                break;
            case SpecialVehicleContainer_PR_rescueContainer:
                required_permissions.add(CamPermission::Rescue);
                break;
            case SpecialVehicleContainer_PR_emergencyContainer:
                required_permissions.add(CamPermission::Emergency);
                emergency = &special->choice.emergencyContainer;
                break;
            case SpecialVehicleContainer_PR_safetyCarContainer:
                required_permissions.add(CamPermission::Safety_Car);
                safety = &special->choice.safetyCarContainer;
                break;
            case SpecialVehicleContainer_PR_NOTHING:
            default:
                break;
        }

        if (emergency && emergency->emergencyPriority && emergency->emergencyPriority->size == 1) {
            // testing bit strings from asn1c is such a mess...
            assert(emergency->emergencyPriority->buf);
            uint8_t bits = *emergency->emergencyPriority->buf;
            if (bits & (1 << (7 - EmergencyPriority_requestForRightOfWay))) {
                required_permissions.add(CamPermission::Request_For_Right_Of_Way);
            }
            if (bits & (1 << (7 - EmergencyPriority_requestForFreeCrossingAtATrafficLight))) {
                required_permissions.add(CamPermission::Request_For_Free_Crossing_At_Traffic_Light);
            }
        }

        if (roadworks && roadworks->closedLanes) {
            required_permissions.add(CamPermission::Closed_Lanes);
        }

        if (safety && safety->trafficRule) {
            switch (*safety->trafficRule) {
                case TrafficRule_noPassing:
                    required_permissions.add(CamPermission::No_Passing);
                    break;
                case TrafficRule_noPassingForTrucks:
                    required_permissions.add(CamPermission::No_Passing_For_Trucks);
                    break;
                default:
                    break;
            }
        }

        if (safety && safety->speedLimit) {
            required_permissions.add(CamPermission::Speed_Limit);
        }
    }

    return ssp.has(required_permissions);
}

void print_indented(std::ostream& os, const asn1::Cam& message, const std::string& indent, unsigned level)
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

    const CoopAwareness_t& cam = message->cam;
    prefix("CoopAwarensess") << "\n";
    ++level;
    prefix("Generation Delta Time") << cam.generationDeltaTime << "\n";

    prefix("Basic Container") << "\n";
    ++level;
    const BasicContainer_t& basic = cam.camParameters.basicContainer;
    prefix("Station Type") << basic.stationType << "\n";
    prefix("Reference Position") << "\n";
    ++level;
    prefix("Longitude") << basic.referencePosition.longitude << "\n";
    prefix("Latitude") << basic.referencePosition.latitude << "\n";
    prefix("Semi Major Orientation") << basic.referencePosition.positionConfidenceEllipse.semiMajorOrientation << "\n";
    prefix("Semi Major Confidence") << basic.referencePosition.positionConfidenceEllipse.semiMajorConfidence << "\n";
    prefix("Semi Minor Confidence") << basic.referencePosition.positionConfidenceEllipse.semiMinorConfidence << "\n";
    prefix("Altitude [Confidence]") << basic.referencePosition.altitude.altitudeValue
        << " [" << basic.referencePosition.altitude.altitudeConfidence << "]\n";
    --level;
    --level;

    if (cam.camParameters.highFrequencyContainer.present == HighFrequencyContainer_PR_basicVehicleContainerHighFrequency) {
        prefix("High Frequency Container [Basic Vehicle]") << "\n";
        ++level;
        const BasicVehicleContainerHighFrequency& bvc =
            cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency;
        prefix("Heading [Confidence]") << bvc.heading.headingValue
            << " [" << bvc.heading.headingConfidence << "]\n";
        prefix("Speed [Confidence]") << bvc.speed.speedValue
            << " [" << bvc.speed.speedConfidence << "]\n";
        prefix("Drive Direction") << bvc.driveDirection << "\n";
        prefix("Longitudinal Acceleration") << bvc.longitudinalAcceleration.longitudinalAccelerationValue << "\n";
        prefix("Vehicle Length [Confidence Indication]") << bvc.vehicleLength.vehicleLengthValue
            << " [" << bvc.vehicleLength.vehicleLengthConfidenceIndication << "]\n";
        prefix("Vehicle Width") << bvc.vehicleWidth << "\n";
        prefix("Curvature [Confidence]") << bvc.curvature.curvatureValue
            << " [" << bvc.curvature.curvatureConfidence << "]\n";
        prefix("Curvature Calculation Mode") << bvc.curvatureCalculationMode << "\n";
        prefix("Yaw Rate [Confidence]") << bvc.yawRate.yawRateValue
            << " [" << bvc.yawRate.yawRateConfidence << "]\n";
        --level;
    } else {
        prefix("High Frequency Container") << cam.camParameters.highFrequencyContainer.present << "\n";
    }

    prefix("Low Frequency Container") << (cam.camParameters.lowFrequencyContainer ? "yes" : "no") << "\n";
    // TODO: print basic vehicle low frequency container in detail
    // TODO: print special vehicle container
    --level;
}

} // namespace cam
} // namespace facilities
} // namespace vanetzaExtension
