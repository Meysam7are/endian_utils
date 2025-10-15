/*
MIT License

Copyright (c) 2021-2024 Meysam Zare

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MZ_ENDIAN_READ_BUFFER_HEADER_FILE
#define MZ_ENDIAN_READ_BUFFER_HEADER_FILE
#pragma once

/**
 * @file EndianReadBuffer.h
 * @brief Provides a non-templated read buffer with stream endianness conversion
 *
 * This header defines the ReadBuffer class, which is a specialization of
 * BasicReadBuffer that uses the default stream endianness. It provides a simpler
 * interface for the common case of reading data with automatic endianness conversion
 * from the predefined stream format.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include "EndianBasicBuffers.h"


namespace mz {
    namespace endian {

        /**
         * @class ReadBuffer
         * @brief A non-templated read buffer that uses the default stream endianness
         *
         * This class is a specialization of BasicReadBuffer that automatically uses
         * the predefined stream endianness (typically little-endian). It provides the
         * same functionality as BasicReadBuffer but without requiring the endianness
         * to be explicitly specified.
         */
        class ReadBuffer : public BasicReadBuffer<mz::endian::stream_endian> {
        public:
            /**
             * @brief Constructs a read buffer with specified begin and end pointers
             * @param begin Pointer to the beginning of the buffer
             * @param end Pointer one-past-the-end of the buffer
             */
            explicit constexpr ReadBuffer(const_pointer begin, const_pointer end) noexcept
                : BasicReadBuffer<mz::endian::stream_endian>(begin, end) {}

            /**
             * @brief Constructs a read buffer with specified begin pointer and size
             * @param begin Pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr ReadBuffer(const_pointer begin, size_t size) noexcept
                : BasicReadBuffer<mz::endian::stream_endian>(begin, size) {}

            /**
             * @brief Constructs a read buffer from a void pointer and size
             * @param begin Void pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr ReadBuffer(const void* begin, size_t size) noexcept
                : BasicReadBuffer<mz::endian::stream_endian>(begin, size) {}

            // All functionality is inherited from BasicReadBuffer<mz::endian::stream_endian>
            // No need for additional overrides
        };

    } // namespace endian

    /**
     * @name Global Helper Functions
     * @brief These functions provide a convenient API for working with ReadBuffer
     * @{
     */

     /**
      * @brief Reads a value from the front of a ReadBuffer without checking boundaries
      * @tparam T Type of the value to read (must satisfy SwapType and not be const)
      * @param buffer ReadBuffer to read from
      * @param value Reference to store the read value
      */
    template <endian::SwapType T> requires (!std::is_const_v<T>)
    inline void unsafePopFront(endian::ReadBuffer& buffer, T& value) noexcept {
        buffer.unsafePopFront(value);
    }

    /**
     * @brief Reads a span of values from the front of a ReadBuffer without checking boundaries
     * @tparam T Type of the values to read (must satisfy SwapType and not be const)
     * @tparam N Size of the span
     * @param buffer ReadBuffer to read from
     * @param span Span to store the read values
     */
    template <endian::SwapType T, size_t N> requires (!std::is_const_v<T>)
    inline void unsafePopFront(endian::ReadBuffer& buffer, std::span<T, N> span) noexcept {
        buffer.unsafePopFront(span);
    }

    /**
     * @brief Reads a value from the back of a ReadBuffer without checking boundaries
     * @tparam T Type of the value to read (must satisfy SwapType and not be const)
     * @param buffer ReadBuffer to read from
     * @param value Reference to store the read value
     */
    template <endian::SwapType T> requires (!std::is_const_v<T>)
    inline void unsafePopBack(endian::ReadBuffer& buffer, T& value) noexcept {
        buffer.unsafePopBack(value);
    }

    /**
     * @brief Reads a span of values from the back of a ReadBuffer without checking boundaries
     * @tparam T Type of the values to read (must satisfy SwapType and not be const)
     * @tparam N Size of the span
     * @param buffer ReadBuffer to read from
     * @param span Span to store the read values
     */
    template <endian::SwapType T, size_t N> requires (!std::is_const_v<T>)
    inline void unsafePopBack(endian::ReadBuffer& buffer, std::span<T, N> span) noexcept {
        buffer.unsafePopBack(span);
    }

    /**
     * @brief Safely reads a value from the front of a ReadBuffer
     * @tparam T Type of the value to read (must satisfy SwapType and not be const)
     * @param buffer ReadBuffer to read from
     * @param value Reference to store the read value
     * @return true if the operation failed (not enough data), false on success
     */
    template <endian::SwapType T> requires (!std::is_const_v<T>)
    inline bool popFront(endian::ReadBuffer& buffer, T& value) noexcept {
        return buffer.popFront(value);
    }

    /**
     * @brief Safely reads a value from the back of a ReadBuffer
     * @tparam T Type of the value to read (must satisfy SwapType and not be const)
     * @param buffer ReadBuffer to read from
     * @param value Reference to store the read value
     * @return true if the operation failed (not enough data), false on success
     */
    template <endian::SwapType T> requires (!std::is_const_v<T>)
    inline bool popBack(endian::ReadBuffer& buffer, T& value) noexcept {
        return buffer.popBack(value);
    }

    /**
     * @brief Safely reads a span of values from the front of a ReadBuffer
     * @tparam T Type of the values to read (must satisfy SwapType and not be const)
     * @tparam N Size of the span
     * @param buffer ReadBuffer to read from
     * @param span Span to store the read values
     * @return true if the operation failed (not enough data), false on success
     */
    template <endian::SwapType T, size_t N> requires (!std::is_const_v<T>)
    inline bool popFront(endian::ReadBuffer& buffer, std::span<T, N> span) noexcept {
        return buffer.popFront(span);
    }

    /**
     * @brief Safely reads a span of values from the back of a ReadBuffer
     * @tparam T Type of the values to read (must satisfy SwapType and not be const)
     * @tparam N Size of the span
     * @param buffer ReadBuffer to read from
     * @param span Span to store the read values
     * @return true if the operation failed (not enough data), false on success
     */
    template <endian::SwapType T, size_t N> requires (!std::is_const_v<T>)
    inline bool popBack(endian::ReadBuffer& buffer, std::span<T, N> span) noexcept {
        return buffer.popBack(span);
    }

    /**
     * @brief Safely reads a string from the front of a ReadBuffer
     * @param buffer ReadBuffer to read from
     * @param str String to store the read data
     * @return true if the operation failed (not enough data or validation error), false on success
     */
    inline bool popFront(endian::ReadBuffer& buffer, std::string& str) noexcept {
        return buffer.popFront(str);
    }

    /**
     * @brief Safely reads a wide string from the front of a ReadBuffer
     * @param buffer ReadBuffer to read from
     * @param wstr Wide string to store the read data
     * @return true if the operation failed (not enough data or validation error), false on success
     */
    inline bool popFront(endian::ReadBuffer& buffer, std::wstring& wstr) noexcept {
        return buffer.popFront(wstr);
    }

    /**
     * @brief Safely reads a string from the back of a ReadBuffer
     * @param buffer ReadBuffer to read from
     * @param str String to store the read data
     * @return true if the operation failed (not enough data or validation error), false on success
     */
    inline bool popBack(endian::ReadBuffer& buffer, std::string& str) noexcept {
        return buffer.popBack(str);
    }

    /**
     * @brief Safely reads a wide string from the back of a ReadBuffer
     * @param buffer ReadBuffer to read from
     * @param wstr Wide string to store the read data
     * @return true if the operation failed (not enough data or validation error), false on success
     */
    inline bool popBack(endian::ReadBuffer& buffer, std::wstring& wstr) noexcept {
        return buffer.popBack(wstr);
    }
    /** @} */

} // namespace mz

#endif // MZ_ENDIAN_READ_BUFFER_HEADER_FILE
