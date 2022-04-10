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
#include "mitvane/app_layer_parser/application_parser.hpp"
#include "mitvane/decoder/geonet_decoder.hpp"
#include "mitvane/classifier/classifier.hpp"
#include "mitvane/detector/detection_context.hpp"
#include <vanetza/common/byte_order.hpp>
#include <iostream>


IdpsContext::IdpsContext(const vanetza::geonet::MIB& mib, vanetza::PositionProvider& positioning, signatures_type& signatures) :
    mib_(mib), positioning_(positioning), signatures_(signatures) {}

IdpsContext::~IdpsContext()
{
    for (auto* app : app_parsers_) {
        disable(app);
    }
}

void IdpsContext::set_link_layer(LinkLayer* link_layer)
{
    namespace sph = std::placeholders;

    if (link_layer) {
        link_layer->indicate(std::bind(&IdpsContext::run, this, sph::_1, sph::_2));
    }
}

void IdpsContext::set_application_layer_parser(vanetza::btp::port_type port, ApplicationParser* handler)
{
    m_app_layer_parsers[port] = handler;
}

void IdpsContext::run(vanetza::CohesivePacket&& packet, const vanetza::EthernetHeader& hdr)
{
    if (hdr.source != mib_.itsGnLocalGnAddr.mid() && hdr.type == vanetza::access::ethertype::GeoNetworking) {
        std::cout << "received packet from " << hdr.source << " (" << packet.size() << " bytes)\n";
        mitvane::DetectionContext detection_context;

        // 1. Decode
        std::unique_ptr<vanetza::PacketVariant> up_dec { new vanetza::PacketVariant(packet) };
        mitvane::GeonetDecoder decoder(mib_, detection_context);
        decoder.decode(std::move(up_dec), hdr.source, hdr.destination);
        
        // 2. Classify and get handler
        mitvane::Classifier classifier(detection_context);
        ApplicationParser* handler = classifier.classify(m_app_layer_parsers);

        // 3. Apply application layer parser
        handler->parse(std::unique_ptr<vanetza::PacketVariant>(std::move(detection_context.payload)));

        // 4. Detect using detection_context and signatures
    }

}

void IdpsContext::enable(ApplicationParser* app)
{
    if (app->port() != vanetza::btp::port_type(0)) {
        set_application_layer_parser(app->port(), app);
    }
}

void IdpsContext::disable(ApplicationParser* app)
{
    if (app->port() != vanetza::btp::port_type(0)) {
        set_application_layer_parser(app->port(), nullptr);
    }
}


