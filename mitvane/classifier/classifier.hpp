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

#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP

#include "mitvane/detector/detection_context.hpp"
#include "mitvane/app_layer_parser/application_parser.hpp"
#include <vanetza/btp/header.hpp>
#include <unordered_map>


namespace mitvane {


class Classifier {
    public:
        typedef std::unordered_map<vanetza::btp::port_type, ApplicationParser*> port_map;

        Classifier(DetectionContext&);
        ApplicationParser* classify(port_map&);

    private:
        DetectionContext& m_detection_context;
};

} // namespace mitvane

#endif // CLASSIFIER_HPP