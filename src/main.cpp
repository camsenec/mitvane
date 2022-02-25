/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/tools/socktap/main.cpp>
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
 * - Options are added.
 * - DEN, MAP and SPAT applications are added.
*/


#include "ethernet_device.hpp"
#include "cam_application.hpp"
#include "denm_application.hpp"
#include "spatem_application.hpp"
#include "mapem_application.hpp"
#include "link_layer.hpp"
#include "positioning.hpp"
#include "router_context.hpp"
#include "security.hpp"
#include "time_trigger.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <iostream>

namespace asio = boost::asio;
namespace gn = vanetza::geonet;
namespace po = boost::program_options;
using namespace vanetza;

int main(int argc, const char** argv)
{
    po::options_description options("Allowed options");
    options.add_options()
        ("help", "Print out available options.")
        ("link-layer,l", po::value<std::string>()->default_value("ethernet"), "Link layer type")
        ("interface,i", po::value<std::string>()->default_value("lo"), "Network interface to use.")
        ("mac-address", po::value<std::string>(), "Override the network interface's MAC address.")
        ("require-gnss-fix", "Suppress transmissions while GNSS position fix is missing")
        ("gn-version", po::value<unsigned>()->default_value(1), "GeoNetworking protocol version to use.")
        ("cam-interval", po::value<unsigned>()->default_value(500), "CAM sending interval in milliseconds.")
        ("denm-interval", po::value<unsigned>()->default_value(500), "DENM sending interval in milliseconds.")
        ("spat-interval", po::value<unsigned>()->default_value(500), "SPATEM sending interval in milliseconds.")
        ("map-interval", po::value<unsigned>()->default_value(1000), "MAPEM sending interval in milliseconds.")
        ("print-rx-all", "Print all received messages")
        ("print-rx-cam", "Print received CAMs")
        ("print-rx-denm", "Print received DENMs")
        ("print-rx-spat", "Print received SPATEMs")
        ("print-rx-map", "Print received MAPEMs")
        ("print-tx-all", "Print all generated messages")
        ("print-tx-cam", "Print generated CAMs")
        ("print-tx-denm", "Print generated DENMs")
        ("print-tx-spat", "Print generated SPATEMs")
        ("print-tx-map", "Print generated MAPEMs")
        ("benchmark", "Enable benchmarking")
        ("applications,a", po::value<std::vector<std::string>>()->default_value({"ca", "den", "spat", "map"}, "ca den spat map")->multitoken(), "Run applications [ca,den,spat,map]")
        ("non-strict", "Set MIB parameter ItsGnSnDecapResultHandling to NON_STRICT")
    ;
    add_positioning_options(options);
    add_security_options(options);

    po::positional_options_description positional_options;
    positional_options.add("interface", 1);

    po::variables_map vm;

    try {
        po::store(
            po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .run(),
            vm
        );
        po::notify(vm);
    } catch (po::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << options << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return 1;
    }

    try {
        asio::io_service io_service;
        TimeTrigger trigger(io_service);

        const char* device_name = vm["interface"].as<std::string>().c_str();
        EthernetDevice device(device_name);
        vanetza::MacAddress mac_address = device.address();

        if (vm.count("mac-address")) {
            std::cout << "Using MAC address: " << vm["mac-address"].as<std::string>() << "." << std::endl;

            if (!parse_mac_address(vm["mac-address"].as<std::string>().c_str(), mac_address)) {
                std::cerr << "The specified MAC address is invalid." << std::endl;
                return 1;
            }
        }

        const std::string link_layer_name = vm["link-layer"].as<std::string>();
        auto link_layer =  create_link_layer(io_service, device, link_layer_name);
        if (!link_layer) {
            std::cerr << "No link layer '" << link_layer_name << "' found." << std::endl;
            return 1;
        }

        auto signal_handler = [&io_service](const boost::system::error_code& ec, int signal_number) {
            if (!ec) {
                std::cout << "Termination requested." << std::endl;
                io_service.stop();
            }
        };
        asio::signal_set signals(io_service, SIGINT, SIGTERM);
        signals.async_wait(signal_handler);

        // configure management information base
        // TODO: make more MIB options configurable by command line flags
        gn::MIB mib;
        mib.itsGnLocalGnAddr.mid(mac_address);
        mib.itsGnLocalGnAddr.is_manually_configured(true);
        mib.itsGnLocalAddrConfMethod = geonet::AddrConfMethod::Managed;
        mib.itsGnSecurity = false;
        if (vm.count("non-strict")) {
            mib.itsGnSnDecapResultHandling = vanetza::geonet::SecurityDecapHandling::Non_Strict;
        }
        mib.itsGnProtocolVersion = vm["gn-version"].as<unsigned>();

        if (mib.itsGnProtocolVersion != 0 && mib.itsGnProtocolVersion != 1) {
            throw std::runtime_error("Unsupported GeoNetworking version, only version 0 and 1 are supported.");
        }

        auto positioning = create_position_provider(io_service, vm, trigger.runtime());
        if (!positioning) {
            std::cerr << "Requested positioning method is not available\n";
            return 1;
        }

        auto security = create_security_entity(vm, trigger.runtime(), *positioning);
        if (security) {
            mib.itsGnSecurity = true;
        }

        RouterContext context(mib, trigger, *positioning, security.get());
        context.require_position_fix(vm.count("require-gnss-fix") > 0);
        context.set_link_layer(link_layer.get());

        std::map<std::string, std::unique_ptr<Application>> apps;
        for (const std::string& app_name : vm["applications"].as<std::vector<std::string>>()) {
            if (apps.find(app_name) != apps.end()) {
                std::cerr << "application '" << app_name << "' requested multiple times, skip\n";
                continue;
            }

            if (app_name == "ca") {
                std::unique_ptr<CamApplication> ca {
                    new CamApplication(*positioning, trigger.runtime())
                };
                ca->set_interval(std::chrono::milliseconds(vm["cam-interval"].as<unsigned>()));
                ca->print_received_message(vm.count("print-rx-cam") > 0 | vm.count("print-rx-all") > 0);
                ca->print_generated_message(vm.count("print-tx-cam") > 0 | vm.count("print-tx-all") > 0);
                apps.emplace(app_name, std::move(ca));
            } else if (app_name == "den") {
                std::unique_ptr<DenmApplication> den {
                    new DenmApplication(*positioning, trigger.runtime())
                };
                den->set_interval(std::chrono::milliseconds(vm["denm-interval"].as<unsigned>()));
                den->print_received_message(vm.count("print-rx-denm") > 0 | vm.count("print-rx-all") > 0);
                den->print_generated_message(vm.count("print-tx-denm") > 0 | vm.count("print-tx-all") > 0);
                apps.emplace(app_name, std::move(den));
            } else if (app_name == "spat") {
                std::unique_ptr<SpatemApplication> spatem {
                    new SpatemApplication(*positioning, trigger.runtime())
                };
                spatem->set_interval(std::chrono::milliseconds(vm["spat-interval"].as<unsigned>()));
                spatem->print_received_message(vm.count("print-rx-spat") > 0 | vm.count("print-rx-all") > 0);
                spatem->print_generated_message(vm.count("print-tx-spat") > 0 | vm.count("print-tx-all") > 0);
                apps.emplace(app_name, std::move(spatem));
            } else if (app_name == "map") { 
                std::unique_ptr<MapemApplication> mapem {
                    new MapemApplication(*positioning, trigger.runtime())
                };
                mapem->set_interval(std::chrono::milliseconds(vm["map-interval"].as<unsigned>()));
                mapem->print_received_message(vm.count("print-rx-map") > 0 | vm.count("print-rx-all") > 0);
                mapem->print_generated_message(vm.count("print-tx-map") > 0 | vm.count("print-tx-all") > 0);
                apps.emplace(app_name, std::move(mapem));
            } else {
                std::cerr << "skip unknown application '" << app_name << "'\n";
            }
        }

        if (apps.empty()) {
            std::cerr << "Warning: No applications are configured, only GN beacons will be exchanged\n";
        }

        for (const auto& app : apps) {
            std::cout << "Enable application '" << app.first << "'...\n";
            context.enable(app.second.get());
        }

        io_service.run();
    } catch (PositioningException& e) {
        std::cerr << "Exit because of positioning error: " << e.what() << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << "Exit: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
