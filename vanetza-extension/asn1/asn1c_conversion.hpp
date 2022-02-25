/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/asn1/asn1c_conversion.hpp>
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
 * - Include file paths are changed.
*/


#ifndef ASN1C_CONVERSION_HPP_9E5QN6UC
#define ASN1C_CONVERSION_HPP_9E5QN6UC

#include "vanetza-extension/asn1/asn1c_wrapper.hpp"
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <memory>
#include <type_traits>

namespace vanetza
{
namespace convertible
{

template<class T>
struct byte_buffer_impl : public byte_buffer
{
public:
    using wrapper_type = T;
    static_assert(
        std::is_base_of<vanetzaExtension::asn1::asn1c_wrapper_common<typename T::asn1c_type>, T>::value,
        "Only asn1c_wrapper derivates are supported");

    byte_buffer_impl(wrapper_type&& t) :
        m_wrapper(new wrapper_type(std::move(t)))
    {
    }

    byte_buffer_impl(std::shared_ptr<const wrapper_type> other) :
        m_wrapper(other)
    {
    }

    void convert(ByteBuffer& buffer) const override
    {
        buffer = m_wrapper->encode();
    }

    std::unique_ptr<byte_buffer> duplicate() const override
    {
        return std::unique_ptr<byte_buffer> {
            new byte_buffer_impl { m_wrapper }
        };
    }

    std::size_t size() const override
    {
        return m_wrapper->size();
    }

    std::shared_ptr<const wrapper_type> wrapper() const
    {
        return m_wrapper;
    }

private:
    std::shared_ptr<const wrapper_type> m_wrapper;
};

} // namespace convertible
} // namespace vanetzaExtension

#endif /* ASN1C_CONVERSION_HPP_9E5QN6UC */

