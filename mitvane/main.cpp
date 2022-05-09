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


#include "ethernet_device.hpp"
#include "idps_context.hpp"
#include "link_layer.hpp"
#include "positioning.hpp"
#include "mitvane/app_layer_parser/cam_application_parser.hpp"
#include "mitvane/app_layer_parser/denm_application_parser.hpp"
#include "mitvane/app_layer_parser/spatem_application_parser.hpp"
#include "mitvane/app_layer_parser/mapem_application_parser.hpp"
#include "mitvane/rule_reader/rule_reader.hpp"
#include <vanetza/common/manual_runtime.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <memory>


namespace asio = boost::asio;
namespace gn = vanetza::geonet;
namespace po = boost::program_options;
namespace posix_time = boost::posix_time;
using namespace vanetza;

int main(int argc, const char** argv)
{
    po::options_description options("Allowed options");
    options.add_options()
        ("help", "Print out available options.")
        ("link-layer,l", po::value<std::string>()->default_value("ethernet"), "Link layer type")
        ("interface-src,s", po::value<std::string>()->default_value("lo"), "Network interface for source.")
        ("interface-dest,d", po::value<std::string>()->default_value("lo"), "Network interface for destination.")
        ("gn-version", po::value<unsigned>()->default_value(1), "GeoNetworking protocol version to use.")
        ("rule-file", po::value<std::string>()->default_value("../example_rule/rules.yaml"), "Path to a rule file.")
        ("print-rx-all", "Print all received messages")
        ("print-rx-cam", "Print received CAMs")
        ("print-rx-denm", "Print received DENMs")
        ("print-rx-spat", "Print received SPATEMs")
        ("print-rx-map", "Print received MAPEMs")
        ("benchmark", "Enable benchmarking")
        ("app-parsers,a", po::value<std::vector<std::string>>()->default_value({"ca", "den", "spat", "map"}, "ca den spat map")->multitoken(), "Run applications [ca,den,spat,map]")
    ;
    add_positioning_options(options);

    po::positional_options_description positional_options;
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
        vanetza::ManualRuntime runtime(Clock::at(posix_time::microsec_clock::universal_time()));
        
        const std::string link_layer_name = vm["link-layer"].as<std::string>();
        
        // Create source link 
        const char* src_device_name = vm["interface-src"].as<std::string>().c_str();
        EthernetDevice src_device(src_device_name);
        
        auto source_link_layer =  create_link_layer(io_service, src_device, link_layer_name);
        if (!source_link_layer) {
            std::cerr << "No link layer for source'" << link_layer_name << "' found." << std::endl;
            return 1;
        }

        // Create destination link 
        const char* dest_device_name = vm["interface-dest"].as<std::string>().c_str();
        EthernetDevice dest_device(dest_device_name);
        
        boost::asio::generic::raw_protocol raw_protocol(AF_PACKET, vanetza::access::ethertype::GeoNetworking.net());
        boost::asio::generic::raw_protocol::socket raw_socket(io_service, raw_protocol);
        raw_socket.bind(dest_device.endpoint(AF_PACKET));
        std::unique_ptr<RawSocketLink> dest_link_layer { new RawSocketLink{std::move(raw_socket)} };

        auto signal_handler = [&io_service](const boost::system::error_code& ec, int signal_number) {
            if (!ec) {
                std::cout << "Termination requested." << std::endl;
                io_service.stop();
            }
        };
        asio::signal_set signals(io_service, SIGINT, SIGTERM);
        signals.async_wait(signal_handler);

        std::string rule_filename = vm["rule-file"].as<std::string>();
        mitvane::RuleReader rule_reader = mitvane::RuleReader(rule_filename);
        std::map<mitvane::Protocol, std::vector<mitvane::Signature>> signatures;
        mitvane::RuleReaderStatusCode rule_reader_st = rule_reader.read(signatures);
        if (rule_reader_st != mitvane::RuleReaderStatusCode::Success) {
            std::cerr << "Failed to read rule file (Error Code: " << static_cast<int>(rule_reader_st) << ")" << "\n";
        }

        gn::MIB mib;
        mib.itsGnLocalGnAddr.is_manually_configured(true);
        mib.itsGnLocalAddrConfMethod = geonet::AddrConfMethod::Managed;
        mib.itsGnSecurity = false;
        mib.itsGnProtocolVersion = vm["gn-version"].as<unsigned>();

        if (mib.itsGnProtocolVersion != 0 && mib.itsGnProtocolVersion != 1) {
            throw std::runtime_error("Unsupported GeoNetworking version, only version 0 and 1 are supported.");
        }

        auto positioning = create_position_provider(io_service, vm, runtime);
        if (!positioning) {
            std::cerr << "Requested positioning method is not available\n";
            return 1;
        }

        IdpsContext context(mib, *positioning, signatures, dest_link_layer.get());
        context.set_link_layer(source_link_layer.get());

        std::map<std::string, std::unique_ptr<ApplicationLayerParser>> app_parsers;
        for (const std::string& app_name : vm["app-parsers"].as<std::vector<std::string>>()) {
            if (app_parsers.find(app_name) != app_parsers.end()) {
                std::cerr << "application '" << app_name << "' requested multiple times, skip\n";
                continue;
            }

            if (app_name == "ca") {
                std::unique_ptr<CamApplicationParser> cam_parser { new CamApplicationParser() };
                cam_parser->print_received_message((vm.count("print-rx-denm") > 0) | (vm.count("print-rx-all") > 0));
                app_parsers.emplace(app_name, std::move(cam_parser));
            } else if (app_name == "den") {
                std::unique_ptr<DenmApplicationParser> denm_parser { new DenmApplicationParser() };
                denm_parser->print_received_message((vm.count("print-rx-denm") > 0) | (vm.count("print-rx-all") > 0));
                app_parsers.emplace(app_name, std::move(denm_parser));
            } else if (app_name == "spat") {
                std::unique_ptr<SpatemApplicationParser> spatem_parser { new SpatemApplicationParser() };
                spatem_parser->print_received_message((vm.count("print-rx-spat") > 0) | (vm.count("print-rx-all") > 0));
                app_parsers.emplace(app_name, std::move(spatem_parser));
            } else if (app_name == "map") { 
                std::unique_ptr<MapemApplicationParser> mapem_parser { new MapemApplicationParser() };
                mapem_parser->print_received_message((vm.count("print-rx-map") > 0) | (vm.count("print-rx-all") > 0));
                app_parsers.emplace(app_name, std::move(mapem_parser));
            } else {
                std::cerr << "skip unknown application '" << app_name << "'\n";
            }
        }

        if (app_parsers.empty()) {
            std::cerr << "Warning: No applications parsers are configured.\n";
        }

        for (const auto& app : app_parsers) {
            std::cout << "Enable application parser '" << app.first << "'...\n";
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
