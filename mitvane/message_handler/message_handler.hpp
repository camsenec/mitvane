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

#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include "logging.hpp"
#include "mitvane/rule_reader/signature.hpp"

namespace mitvane
{

enum class HandleReport {
    Allow,
    Drop
};

class MessageHandler
{   
    public:
        MessageHandler() = default;
        HandleReport handle(std::vector<Signature>& sigs);
    private:
        boost::log::sources::logger lg;
};

} // namespace mitvane

#endif //MESSAGE_HANDLER_HPP