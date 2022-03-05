/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/geonet/router.hpp>
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
 * - Several functions which are not necessary for decoding are removed.
 * - Data store for detection module in IDS/IPS are added.
 * - Function names are changed from indicate_[basic|common|extended|secured]
 *   to decode_[basic|common|extended|secured]
*/


#ifndef DECODER_HPP
#define DECODER_HPP

#include "mitvane/detector/detection_context.hpp"
#include "mitvane/detector/geonet_data.hpp"
#include "mitvane/decoder/btp_decoder.hpp"



namespace mitvane
{

class GeonetDecoder
{   
    typedef std::unique_ptr<vanetza::UpPacket> UpPacketPtr;
        
    public:
        GeonetDecoder(const vanetza::geonet::MIB&, DetectionContext&);

        void decode(UpPacketPtr, const vanetza::MacAddress& sender, const vanetza::MacAddress& destination);

    private:
        
        void decode_basic(vanetza::geonet::IndicationContextBasic&);
        void decode_common(vanetza::geonet::IndicationContext&, const vanetza::geonet::BasicHeader&);
        void decode_extended(vanetza::geonet::IndicationContext&, const vanetza::geonet::CommonHeader&);
        void decode_secured(vanetza::geonet::IndicationContextBasic&, const vanetza::geonet::BasicHeader&);

        void pass_up(const vanetza::geonet::DataIndication&, UpPacketPtr);

        const vanetza::geonet::MIB& m_mib;
        DetectionContext &m_detection_context;
        
};

} // namespace mitvane

#endif //DECODER_HPP