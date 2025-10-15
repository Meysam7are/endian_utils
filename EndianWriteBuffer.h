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

#ifndef MZ_ENDIAN_WRITE_BUFFER_HEADER_FILE
#define MZ_ENDIAN_WRITE_BUFFER_HEADER_FILE
#pragma once

/**
 * @file EndianWriteBuffer.h
 * @brief Provides a non-templated write buffer with stream endianness conversion
 *
 * This header defines the WriteBuffer class, which is a specialization of
 * BasicWriteBuffer that uses the default stream endianness. It provides a simpler
 * interface for the common case of writing data with automatic endianness conversion
 * to the predefined stream format.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include "EndianBasicBuffers.h"

namespace mz {
    namespace endian {

        /**
         * @class WriteBuffer
         * @brief A non-templated write buffer that uses the default stream endianness
         *
         * This class is a specialization of BasicWriteBuffer that automatically uses
         * the predefined stream endianness (typically little-endian). It provides the
         * same functionality as BasicWriteBuffer but without requiring the endianness
         * to be explicitly specified.
         */
        class WriteBuffer : public BasicWriteBuffer<stream_endian> {
        public:
            /**
             * @name Constructors
             * @{
             */

             /**
              * @brief Constructs a buffer with specified begin and end pointers
              * @param begin Pointer to the beginning of the buffer
              * @param end Pointer one-past-the-end of the buffer
              */
            explicit constexpr WriteBuffer(pointer begin, pointer end) noexcept
                : BasicWriteBuffer<stream_endian>(begin, end) {
            }

            /**
             * @brief Constructs a buffer with specified begin pointer and size
             * @param begin Pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr WriteBuffer(pointer begin, size_t size) noexcept
                : BasicWriteBuffer<stream_endian>(begin, size) {
            }

            /**
             * @brief Constructs a buffer from a void pointer and size
             * @param begin Void pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr WriteBuffer(void* begin, size_t size) noexcept
                : BasicWriteBuffer<stream_endian>(begin, size) {
            }
            /** @} */

            /**
             * @name Stream Endianness Aliases
             * @brief These methods maintain compatibility with the original API
             * @{
             */

             /**
              * @brief Alias for calculateSerializedSize to maintain compatibility
              * @param str String to calculate size for
              * @return Size required for the serialized string
              */
            [[nodiscard]] static constexpr size_t size_of(const std::string& str) noexcept {
                return calculateSerializedSize(str);
            }

            /**
             * @brief Alias for calculateSerializedSize to maintain compatibility
             * @param wstr Wide string to calculate size for
             * @return Size required for the serialized wide string
             */
            [[nodiscard]] static constexpr size_t size_of(const std::wstring& wstr) noexcept {
                return calculateSerializedSize(wstr);
            }

            // Methods like unsafePushBack, pushBack are inherited from BasicWriteBuffer
            // and already have the required functionality
        };

    } // namespace endian

    /**
     * @name Global Helper Functions
     * @brief These functions provide a convenient API for working with WriteBuffer
     * @{
     */

     /**
      * @brief Writes a value to a WriteBuffer without checking boundaries
      * @tparam T Type of the value to write (must satisfy SwapType)
      * @param buffer WriteBuffer to write to
      * @param value Value to write
      */
    template <endian::SwapType T>
    inline void unsafePushBack(endian::WriteBuffer& buffer, T value) noexcept {
        buffer.unsafePushBack(value);
    }

    /**
     * @brief Writes a span of values to a WriteBuffer without checking boundaries
     * @tparam T Type of the values to write (must satisfy SwapType)
     * @tparam N Size of the span
     * @param buffer WriteBuffer to write to
     * @param span Span of values to write
     */
    template <endian::SwapType T, size_t N>
    inline void unsafePushBack(endian::WriteBuffer& buffer, const std::span<T, N>& span) noexcept {
        buffer.unsafePushBack(span);
    }

    /**
     * @brief Writes a string to a WriteBuffer without checking boundaries
     * @param buffer WriteBuffer to write to
     * @param str String to write
     */
    inline void unsafePushBack(endian::WriteBuffer& buffer, const std::string& str) noexcept {
        buffer.unsafePushBack(str);
    }

    /**
     * @brief Writes a wide string to a WriteBuffer without checking boundaries
     * @param buffer WriteBuffer to write to
     * @param wstr Wide string to write
     */
    inline void unsafePushBack(endian::WriteBuffer& buffer, const std::wstring& wstr) noexcept {
        buffer.unsafePushBack(wstr);
    }

    /**
     * @brief Safely writes a value to a WriteBuffer
     * @tparam T Type of the value to write (must satisfy SwapType)
     * @param buffer WriteBuffer to write to
     * @param value Value to write
     * @return true if the operation failed (not enough space), false on success
     */
    template <endian::SwapType T>
    inline bool pushBack(endian::WriteBuffer& buffer, T value) noexcept {
        return buffer.pushBack(value);
    }

    /**
     * @brief Safely writes a span of values to a WriteBuffer
     * @tparam T Type of the values to write (must satisfy SwapType)
     * @tparam N Size of the span
     * @param buffer WriteBuffer to write to
     * @param span Span of values to write
     * @return true if the operation failed (not enough space), false on success
     */
    template <endian::SwapType T, size_t N>
    inline bool pushBack(endian::WriteBuffer& buffer, const std::span<T, N>& span) noexcept {
        return buffer.pushBack(span);
    }

    /**
     * @brief Safely writes a string to a WriteBuffer
     * @param buffer WriteBuffer to write to
     * @param str String to write
     * @return true if the operation failed (not enough space), false on success
     */
    inline bool pushBack(endian::WriteBuffer& buffer, const std::string& str) noexcept {
        return buffer.pushBack(str);
    }

    /**
     * @brief Safely writes a wide string to a WriteBuffer
     * @param buffer WriteBuffer to write to
     * @param wstr Wide string to write
     * @return true if the operation failed (not enough space), false on success
     */
    inline bool pushBack(endian::WriteBuffer& buffer, const std::wstring& wstr) noexcept {
        return buffer.pushBack(wstr);
    }
    /** @} */

} // namespace mz

#endif // MZ_ENDIAN_WRITE_BUFFER_HEADER_FILE
