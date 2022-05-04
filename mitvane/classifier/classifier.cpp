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


#include "classifier.hpp"
#include <cassert>


using namespace vanetza;
using namespace vanetza::btp;


namespace mitvane 
{

Classifier::Classifier(DetectionContext& detection_context) : 
    m_detection_context(detection_context){}

ApplicationLayerParser* Classifier::classify(port_map& app_layer_parsers)
{
    ApplicationLayerParser* handler = nullptr;

    handler = app_layer_parsers[m_detection_context.btp_data->destination_port];

    return handler;

}
} // namespace mitvane