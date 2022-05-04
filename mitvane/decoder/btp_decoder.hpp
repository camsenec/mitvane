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


#ifndef BTP_DECODER_HPP
#define BTP_DECODER_HPP

#include "mitvane/detector/detection_context.hpp"
#include "mitvane/detector/btp_data.hpp"
#include "mitvane/classifier/classifier.hpp"
#include <vanetza/geonet/data_indication.hpp>
#include <vanetza/btp/header.hpp>
#include <vanetza/btp/data_interface.hpp>
#include <unordered_map>

namespace mitvane {

class BtpDecoder
{
    public:
        BtpDecoder(DetectionContext&);

        void set_application_layer_parser(vanetza::btp::port_type, vanetza::btp::IndicationInterface*);
        void decode(const vanetza::geonet::DataIndication&, std::unique_ptr<vanetza::UpPacket>);

    private:
        DetectionContext& m_detection_context;
};

} // namespace mitvane

#endif // BTP_DECODER_HPP