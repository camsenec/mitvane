/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/positioning.cpp>
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


#include "positioning.hpp"
#include <vanetza/common/stored_position_provider.hpp>

#ifdef SOCKTAP_WITH_GPSD
#   include "gps_position_provider.hpp"
#endif

using namespace vanetza;
namespace po = boost::program_options;

std::unique_ptr<vanetza::PositionProvider>
create_position_provider(boost::asio::io_service& io_service, const po::variables_map& vm, const Runtime& runtime)
{
    std::unique_ptr<vanetza::PositionProvider> positioning;

    if (vm["positioning"].as<std::string>() == "gpsd") {
#ifdef SOCKTAP_WITH_GPSD
        positioning.reset(new GpsPositionProvider {
            io_service, vm["gpsd-host"].as<std::string>(), vm["gpsd-port"].as<std::string>()
        });
#endif
    } else if (vm["positioning"].as<std::string>() == "static") {
        std::unique_ptr<StoredPositionProvider> stored { new StoredPositionProvider() };
        PositionFix fix;
        fix.timestamp = runtime.now();
        fix.latitude = vm["latitude"].as<double>() * units::degree;
        fix.longitude = vm["longitude"].as<double>() * units::degree;
        fix.confidence.semi_major = vm["pos_confidence"].as<double>() * units::si::meter;
        fix.confidence.semi_minor = fix.confidence.semi_major;
        stored->position_fix(fix);
        positioning = std::move(stored);
    }

    return positioning;
}

void add_positioning_options(po::options_description& options)
{
#ifdef SOCKTAP_WITH_GPSD
    const char* default_positioning = "gpsd";
#else
    const char* default_positioning = "static";
#endif

    options.add_options()
        ("positioning,p", po::value<std::string>()->default_value(default_positioning), "Select positioning provider")
#ifdef SOCKTAP_WITH_GPSD
        ("gpsd-host", po::value<std::string>()->default_value(gpsd::shared_memory), "gpsd's server hostname")
        ("gpsd-port", po::value<std::string>()->default_value(gpsd::default_port), "gpsd's listening port")
#endif
        ("latitude", po::value<double>()->default_value(48.7668616), "Latitude of static position")
        ("longitude", po::value<double>()->default_value(11.432068), "Longitude of static position")
        ("pos_confidence", po::value<double>()->default_value(5.0), "95% circular confidence of static position")
    ;
}
