/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/gps_position_provider.hpp>
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


#ifndef GPS_POSITION_PROVIDER_HPP_GYN3GVQA
#define GPS_POSITION_PROVIDER_HPP_GYN3GVQA

#include "positioning.hpp"
#include <vanetza/common/clock.hpp>
#include <vanetza/common/position_provider.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <string>
#include <gps.h>

class GpsPositionProvider : public vanetza::PositionProvider
{
public:
    class GpsPositioningException : public PositioningException
    {
    protected:
        GpsPositioningException(int);
        friend class GpsPositionProvider;
    };

    GpsPositionProvider(boost::asio::io_service& io);
    GpsPositionProvider(boost::asio::io_service& io, const std::string& hostname, const std::string& port);
    ~GpsPositionProvider();

    const vanetza::PositionFix& position_fix() override;
    void fetch_position_fix();

private:
    void schedule_timer();
    void on_timer(const boost::system::error_code& ec);

    boost::asio::steady_timer timer_;
    gps_data_t gps_data;
    vanetza::PositionFix fetched_position_fix;
};

namespace gpsd
{

constexpr const char* default_port = DEFAULT_GPSD_PORT;
constexpr const char* shared_memory = GPSD_SHARED_MEMORY;

} // namespace gpsd

#endif /* GPS_POSITION_PROVIDER_HPP_GYN3GVQA */
