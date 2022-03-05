/* 
 * This file is modified
 * by Tomoya Tanaka <deepsky2221@gmail.com>
 * from <https://github.com/riebl/vanetza/blob/master/vanetza/asn1/asn1c_wrapper.hpp>
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


#ifndef ASN1C_WRAPPER_HPP
#define ASN1C_WRAPPER_HPP

#include "vanetza-extension/asn1/support/asn_system.h"
#include "vanetza-extension/asn1/support/constr_TYPE.h"
#include <vanetza/common/byte_buffer.hpp>
#include <cstddef>
#include <string>
#include <utility>

namespace vanetzaExtension
{
namespace asn1
{

void* allocate(std::size_t);
void free(asn_TYPE_descriptor_t&, void*);
void* copy(asn_TYPE_descriptor_t&, const void*);
bool validate(asn_TYPE_descriptor_t&, const void*);
bool validate(asn_TYPE_descriptor_t&, const void*, std::string&);
std::size_t size_per(asn_TYPE_descriptor_t&, const void*);
std::size_t size_oer(asn_TYPE_descriptor_t&, const void*);
vanetza::ByteBuffer encode_per(asn_TYPE_descriptor_t&, const void*);
bool decode_per(asn_TYPE_descriptor_t&, void**, const vanetza::ByteBuffer&);
bool decode_per(asn_TYPE_descriptor_t&, void**, const void* buffer, std::size_t size);
vanetza::ByteBuffer encode_oer(asn_TYPE_descriptor_t&, const void*);
bool decode_oer(asn_TYPE_descriptor_t&, void**, const vanetza::ByteBuffer&);
bool decode_oer(asn_TYPE_descriptor_t&, void**, const void* buffer, std::size_t size);

template<class T>
T* allocate()
{
    return static_cast<T*>(allocate(sizeof(T)));
}

template<class T>
class asn1c_wrapper_common
{

public:
    typedef T asn1c_type;

    asn1c_wrapper_common(asn_TYPE_descriptor_t& desc) :
        m_struct(vanetzaExtension::asn1::allocate<asn1c_type>()), m_type(desc) {}
    ~asn1c_wrapper_common() { vanetzaExtension::asn1::free(m_type, m_struct); }

    // copy semantics
    asn1c_wrapper_common(const asn1c_wrapper_common& other) :
        m_struct(static_cast<asn1c_type*>(copy(other.m_type, other.m_struct))), m_type(other.m_type) {}
    asn1c_wrapper_common& operator=(const asn1c_wrapper_common& other)
    {
        asn1c_wrapper_common tmp = other;
        swap(tmp);
        return *this;
    }

    // move semantics
    asn1c_wrapper_common(asn1c_wrapper_common&& other) noexcept :
        m_struct(nullptr), m_type(other.m_type) { swap(other); }
    asn1c_wrapper_common& operator=(asn1c_wrapper_common&& other) noexcept
    {
        swap(other);
        return *this;
    }

    // dereferencing
    asn1c_type& operator*() { return *m_struct; }
    asn1c_type* operator->() { return m_struct; }
    const asn1c_type& operator*() const { return *m_struct; }
    const asn1c_type* operator->() const { return m_struct; }

    /**
     * Check ASN.1 constraints
     * \param error (optional) copy of error message
     * \return true if valid
     */
    bool validate() const
    {
        return vanetzaExtension::asn1::validate(m_type, m_struct);
    }

    /**
     * Check ASN.1 constraints
     * \param error Error message if any constraint failed
     * \return true if valid
     */
    bool validate(std::string& error) const
    {
        return vanetzaExtension::asn1::validate(m_type, m_struct, error);
    }

    /**
     * Swap ASN.1 wrapper content
     * \param other wrapper
     */
    void swap(asn1c_wrapper_common& other) noexcept
    {
        std::swap(m_struct, other.m_struct);
        std::swap(m_type, other.m_type);
    }

protected:
    asn1c_type* m_struct;
    asn_TYPE_descriptor_t& m_type;
};

template<typename T>
void swap(asn1c_wrapper_common<T>& lhs, asn1c_wrapper_common<T>& rhs)
{
    lhs.swap(rhs);
}


template<class T>
class asn1c_per_wrapper : public asn1c_wrapper_common<T>
{
public:
    using base = asn1c_wrapper_common<T>;
    using base::base;

    /**
     * Encode ASN.1 struct into byte buffer
     * \return byte buffer containing serialized ASN.1 struct
     */
    vanetza::ByteBuffer encode() const
    {
        return vanetzaExtension::asn1::encode_per(base::m_type, base::m_struct);
    }

    /**
     * Try to decode ASN.1 struct from byte buffer
     * \param buffer input data
     * \return true if decoding has been successful
     */
    bool decode(const vanetza::ByteBuffer& buffer)
    {
        return vanetzaExtension::asn1::decode_per(base::m_type, (void**)&(base::m_struct), buffer);
    }

    bool decode(vanetza::ByteBuffer::const_iterator begin, vanetza::ByteBuffer::const_iterator end)
    {
        return vanetzaExtension::asn1::decode_per(base::m_type, (void**)&(base::m_struct), &(*begin), std::distance(begin, end));
    }

    bool decode(const void* buffer, std::size_t len)
    {
        return vanetzaExtension::asn1::decode_per(base::m_type, (void**)&(base::m_struct), buffer, len);
    }

    /**
     * Get size of encoded ASN.1 struct
     * \return size in bytes
     */
    std::size_t size() const
    {
        return vanetzaExtension::asn1::size_per(base::m_type, base::m_struct);
    }
};

template<class T>
using asn1c_wrapper = asn1c_per_wrapper<T>;


template<class T>
class asn1c_oer_wrapper : public asn1c_wrapper_common<T>
{
public:
    using base = asn1c_wrapper_common<T>;
    using base::base;

    /**
     * Encode ASN.1 struct into byte buffer
     * \return byte buffer containing serialized ASN.1 struct
     */
    vanetza::ByteBuffer encode() const
    {
        return vanetzaExtension::asn1::encode_oer(base::m_type, base::m_struct);
    }

    /**
     * Try to decode ASN.1 struct from byte buffer
     * \param buffer input data
     * \deprecated use decode_per instead
     * \return true if decoding has been successful
     */
    bool decode(const vanetza::ByteBuffer& buffer)
    {
        return vanetzaExtension::asn1::decode_oer(base::m_type, (void**)&(base::m_struct), buffer);
    }

    bool decode(vanetza::ByteBuffer::const_iterator begin, vanetza::ByteBuffer::const_iterator end)
    {
        return vanetzaExtension::asn1::decode_oer(base::m_type, (void**)&(base::m_struct), &(*begin), std::distance(begin, end));
    }

    bool decode(const void* buffer, std::size_t len)
    {
        return vanetzaExtension::asn1::decode_oer(base::m_type, (void**)&(base::m_struct), buffer, len);
    }

    /**
     * Get size of encoded ASN.1 struct
     * \return size in bytes
     */
    std::size_t size() const
    {
        return vanetzaExtension::asn1::size_oer(base::m_type, base::m_struct);
    }
};

} // namespace asn1
} // namespace vanetzaExtension

#endif /* ASN1C_WRAPPER_HPP */

