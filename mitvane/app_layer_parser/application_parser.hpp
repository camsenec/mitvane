/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/application.hpp>
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


#ifndef APPLICATION_PARSER_HPP
#define APPLICATION_PARSER_HPP

#include <vanetza/btp/data_interface.hpp>
#include <vanetza/btp/data_indication.hpp>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/router.hpp>

class ApplicationParser
{
public:
    using DataConfirm = vanetza::geonet::DataConfirm;
    using DataIndication = vanetza::btp::DataIndication;
    using DataRequest = vanetza::btp::DataRequestGeoNetParams;
    using DownPacketPtr = vanetza::geonet::Router::DownPacketPtr;
    using PortType = vanetza::btp::port_type;
    using PromiscuousHook = vanetza::btp::PortDispatcher::PromiscuousHook;
    using UpPacketPtr = vanetza::geonet::Router::UpPacketPtr;

    ApplicationParser() = default;
    ApplicationParser(const ApplicationParser&) = delete;
    ApplicationParser& operator=(const ApplicationParser&) = delete;
    virtual ~ApplicationParser() = default;
    
    virtual PortType port() = 0;
    virtual void parse(UpPacketPtr) = 0;
    

private:
    friend class RouterContext;
    vanetza::geonet::GbcDataRequest request_gbc(const DataRequest&);
    vanetza::geonet::ShbDataRequest request_shb(const DataRequest&);
    vanetza::geonet::Router* router_;
};

#endif /* APPLICATION_HPP */

