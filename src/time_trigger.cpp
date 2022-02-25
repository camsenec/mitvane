/* 
 * This file is copied
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/time_trigger.cpp>
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


#include "time_trigger.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <functional>

namespace asio = boost::asio;
namespace posix_time = boost::posix_time;
using namespace vanetza;

TimeTrigger::TimeTrigger(asio::io_service& io_service) :
    io_service_(io_service), timer_(io_service), runtime_(Clock::at(now()))
{
    std::cout << "Starting runtime at " << now() <<"\n";
    schedule();
}

posix_time::ptime TimeTrigger::now() const
{
    return posix_time::microsec_clock::universal_time();
}

void TimeTrigger::schedule()
{
    update_runtime();
    auto next = runtime_.next();
    if (next < Clock::time_point::max()) {
        timer_.expires_at(Clock::at(next));
        timer_.async_wait(std::bind(&TimeTrigger::on_timeout, this, std::placeholders::_1));
    } else {
        timer_.cancel();
    }
}

void TimeTrigger::on_timeout(const boost::system::error_code& ec)
{
    if (asio::error::operation_aborted != ec) {
        schedule();
    }
}

void TimeTrigger::update_runtime()
{
    auto current_time = now();
    runtime_.trigger(Clock::at(current_time));
}
