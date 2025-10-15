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

#ifndef MZ_ENDIAN_BASIC_BUFFERS_HEADER_FILE
#define MZ_ENDIAN_BASIC_BUFFERS_HEADER_FILE
#pragma once

/**
 * @file EndianBasicBuffers.h
 * @brief Provides templated buffer classes for endian-aware reading and writing
 *
 * This header defines two primary buffer classes for binary data manipulation
 * with automatic endianness conversion:
 * - BasicWriteBuffer: For writing data to a memory buffer
 * - BasicReadBuffer: For reading data from a memory buffer
 *
 * Both classes support primitive types, spans, and strings with automatic
 * endianness conversion based on the specified target endianness.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include <cstdint>
#include <type_traits>
#include <string>
#include <concepts>
#include <span>
#include <bit>


#include "EndianConversions.h"
#include "EndianConcepts.h"

namespace mz {
    namespace endian {

        /**
         * @namespace mz::endian
         * @brief Contains utilities for endianness-aware data manipulation
         */

         /**
          * @class BasicWriteBuffer
          * @brief A memory buffer for writing data with automatic endianness conversion
          *
          * This class provides methods to write primitive values, spans, and strings
          * to a memory buffer with automatic conversion to the target endianness.
          * Both safe and unsafe operations are provided, where safe operations check
          * buffer boundaries before writing.
          *
          * @tparam Encoding Target endianness for the written data
          */
        template <std::endian Encoding>
        class BasicWriteBuffer {
        public:
            /**
             * @name Type Definitions
             * @{
             */
            using value_type = uint8_t;        ///< The underlying byte type
            using pointer = uint8_t*;          ///< Pointer to byte type
            using const_pointer = const uint8_t*; ///< Const pointer to byte type
            /** @} */

            /**
             * @name Constructors
             * @{
             */

             /**
              * @brief Constructs a buffer with specified begin and end pointers
              * @param begin Pointer to the beginning of the buffer
              * @param end Pointer one-past-the-end of the buffer
              */
            explicit constexpr BasicWriteBuffer(pointer begin, pointer end) noexcept
                : m_begin{ begin }, m_end{ end } {
            }

            /**
             * @brief Constructs a buffer with specified begin pointer and size
             * @param begin Pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr BasicWriteBuffer(pointer begin, size_t size) noexcept
                : BasicWriteBuffer{ begin, begin + size } {
            }

            /**
             * @brief Constructs a buffer from a void pointer and size
             * @param begin Void pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr BasicWriteBuffer(void* begin, size_t size) noexcept
                : BasicWriteBuffer{ static_cast<pointer>(begin), size } {
            }
            /** @} */

            /**
             * @name Buffer Accessors
             * @{
             */

             /**
              * @brief Gets the current end pointer of the buffer
              * @return Pointer to the end of the buffer
              */
            [[nodiscard]] constexpr pointer end() noexcept { return m_end; }

            /**
             * @brief Gets the current data pointer (write position)
             * @return Pointer to the current write position
             */
            [[nodiscard]] constexpr pointer data() noexcept { return m_begin; }

            /**
             * @brief Gets the current begin pointer (write position)
             * @return Pointer to the current write position
             */
            [[nodiscard]] constexpr pointer begin() noexcept { return m_begin; }

            /**
             * @brief Gets the current end pointer of the buffer (const version)
             * @return Const pointer to the end of the buffer
             */
            [[nodiscard]] constexpr const_pointer end() const noexcept { return m_end; }

            /**
             * @brief Gets the current data pointer (const version)
             * @return Const pointer to the current write position
             */
            [[nodiscard]] constexpr const_pointer data() const noexcept { return m_begin; }

            /**
             * @brief Gets the current begin pointer (const version)
             * @return Const pointer to the current write position
             */
            [[nodiscard]] constexpr const_pointer begin() const noexcept { return m_begin; }

            /**
             * @brief Checks if the buffer is empty
             * @return true if the buffer has no remaining space
             */
            [[nodiscard]] constexpr bool empty() const noexcept { return m_begin == m_end; }

            /**
             * @brief Checks if the buffer is in an error state
             * @return true if the buffer pointers are invalid
             */
            [[nodiscard]] constexpr bool error() const noexcept { return m_end < m_begin || !m_begin; }

            /**
             * @brief Gets the remaining size of the buffer
             * @return Number of bytes available for writing
             */
            [[nodiscard]] constexpr size_t size() const noexcept { return m_end - m_begin; }
            /** @} */

            /**
             * @name Buffer Operations
             * @{
             */

             /**
              * @brief Skips a specified number of bytes in the buffer
              * @param bytes Number of bytes to skip
              *
              * This function advances the write position by the specified number of bytes.
              * If skipping would go beyond the end of the buffer, the write position is set
              * to the end of the buffer.
              */
            constexpr void skip(size_t bytes) noexcept {
                auto ptr = m_begin + bytes;
                m_begin = (ptr <= m_end) ? ptr : m_end;
            }
            /** @} */

            /**
             * @name Deleted Generic Overloads
             * @{
             */

             /**
              * @brief Deleted overload to prevent usage with unsupported types
              */
            template <typename T>
            void pushBack(T) = delete;

            /**
             * @brief Deleted overload to prevent usage with unsupported types
             */
            template <typename T>
            void unsafePushBack(T) = delete;
            /** @} */

            /**
             * @name Unsafe Write Operations
             * @brief Operations that write data without checking buffer boundaries
             * @{
             */

             /**
              * @brief Writes a value to the buffer without checking boundaries
              * @tparam T Type of the value to write (must satisfy SwapType)
              * @param value Value to write
              *
              * Writes a value to the buffer with automatic endianness conversion.
              * This function does not check if there's enough space in the buffer.
              */
            template <SwapType T>
            void unsafePushBack(T value) noexcept {
                // Copy the value with endian conversion if needed
                basicCopy<Encoding>(m_begin, value);
                m_begin += sizeof(T);
            }

            /**
             * @brief Writes a span of values to the buffer without checking boundaries
             * @tparam T Type of the values in the span (must satisfy SwapType)
             * @tparam N Size of the span
             * @param span Span of values to write
             *
             * Writes multiple values to the buffer with automatic endianness conversion.
             * This function does not check if there's enough space in the buffer.
             */
            template <SwapType T, size_t N>
            void unsafePushBack(const std::span<T, N>& span) noexcept {
                // Copy the span with endian conversion if needed
                basicCopy<Encoding>(m_begin, span);
                m_begin += sizeof(T) * span.size();
            }

            /**
             * @brief Calculates the serialized size of a string
             * @param str String to calculate size for
             * @return Total size in bytes needed to store the string
             *
             * The serialization format includes:
             * - 4 bytes for the size prefix
             * - N bytes for the string content
             * - 4 bytes for the size suffix (used for validation)
             */
            [[nodiscard]] static constexpr size_t calculateSerializedSize(const std::string& str) noexcept {
                return str.size() + 8; // size prefix (4) + content + size suffix (4)
            }

            /**
             * @brief Writes a string to the buffer without checking boundaries
             * @param str String to write
             *
             * Writes a string to the buffer using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             * This function does not check if there's enough space in the buffer.
             */
            void unsafePushBack(const std::string& str) noexcept {
                const uint32_t size = static_cast<uint32_t>(str.size());
                unsafePushBack(size); // Write size prefix
                unsafePushBack(std::span{ str }); // Write content
                unsafePushBack(size); // Write size suffix for validation
            }

            /**
             * @brief Calculates the serialized size of a wide string
             * @param wstr Wide string to calculate size for
             * @return Total size in bytes needed to store the wide string
             *
             * The serialization format includes:
             * - 4 bytes for the size prefix
             * - N*2 bytes for the wide string content
             * - 4 bytes for the size suffix (used for validation)
             */
            [[nodiscard]] static constexpr size_t calculateSerializedSize(const std::wstring& wstr) noexcept {
                return wstr.size() * 2 + 8; // size prefix (4) + content (char16_t) + size suffix (4)
            }

            /**
             * @brief Writes a wide string to the buffer without checking boundaries
             * @param wstr Wide string to write
             *
             * Writes a wide string to the buffer using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             * This function does not check if there's enough space in the buffer.
             */
            void unsafePushBack(const std::wstring& wstr) noexcept {
                const uint32_t size = static_cast<uint32_t>(wstr.size());
                unsafePushBack(size); // Write size prefix
                unsafePushBack(std::span{ wstr }); // Write content
                unsafePushBack(size); // Write size suffix for validation
            }
            /** @} */

            /**
             * @name Safe Write Operations
             * @brief Operations that check buffer boundaries before writing
             * @{
             */

             /**
              * @brief Safely writes a value to the buffer
              * @tparam T Type of the value to write (must satisfy SwapType)
              * @param value Value to write
              * @return true if the operation failed (not enough space), false on success
              *
              * Writes a value to the buffer with automatic endianness conversion,
              * but only if there's enough space available.
              */
            template <SwapType T>
            [[nodiscard]] bool pushBack(T value) noexcept {
                if (m_begin + sizeof(T) <= m_end) {
                    unsafePushBack(value);
                    return false; // Success (no error)
                }
                return true; // Error (buffer full)
            }

            /**
             * @brief Safely writes a span of values to the buffer
             * @tparam T Type of the values in the span (must satisfy SwapType)
             * @tparam N Size of the span
             * @param span Span of values to write
             * @return true if the operation failed (not enough space), false on success
             *
             * Writes multiple values to the buffer with automatic endianness conversion,
             * but only if there's enough space available for the entire span.
             */
            template <SwapType T, size_t N>
            [[nodiscard]] bool pushBack(const std::span<T, N>& span) noexcept {
                if (m_begin + sizeof(T) * span.size() <= m_end) {
                    unsafePushBack(span);
                    return false; // Success (no error)
                }
                return true; // Error (buffer full)
            }

            /**
             * @brief Safely writes a string to the buffer
             * @param str String to write
             * @return true if the operation failed (not enough space), false on success
             *
             * Writes a string to the buffer using the format:
             * [size][content][size]
             * but only if there's enough space available for the entire serialized string.
             */
            [[nodiscard]] bool pushBack(const std::string& str) noexcept {
                const size_t requiredSize = calculateSerializedSize(str);
                if (m_begin + requiredSize <= m_end) {
                    unsafePushBack(str);
                    return false; // Success (no error)
                }
                return true; // Error (buffer full)
            }

            /**
             * @brief Safely writes a wide string to the buffer
             * @param wstr Wide string to write
             * @return true if the operation failed (not enough space), false on success
             *
             * Writes a wide string to the buffer using the format:
             * [size][content][size]
             * but only if there's enough space available for the entire serialized string.
             */
            [[nodiscard]] bool pushBack(const std::wstring& wstr) noexcept {
                const size_t requiredSize = calculateSerializedSize(wstr);
                if (m_begin + requiredSize <= m_end) {
                    unsafePushBack(wstr);
                    return false; // Success (no error)
                }
                return true; // Error (buffer full)
            }

            /**
             * @brief Writes as many values from a span as will fit in the buffer
             * @tparam T Type of the values in the span (must satisfy SwapType)
             * @tparam N Size of the span
             * @param span Span of values to write
             * @return Number of elements successfully written
             *
             * This function writes as many values from the span as will fit in the
             * remaining buffer space, and returns the number of elements written.
             */
            template <SwapType T, size_t N>
            [[nodiscard]] size_t pushBackSome(const std::span<T, N>& span) noexcept {
                // Calculate how many elements can fit in the remaining buffer
                const size_t maxElements = (m_end - m_begin) / sizeof(T);
                const size_t elementsToWrite = (maxElements < span.size()) ? maxElements : span.size();

                // Write as many elements as will fit
                if (elementsToWrite > 0) {
                    unsafePushBack(std::span<const T>(span.data(), elementsToWrite));
                }

                return elementsToWrite;
            }
            /** @} */

        protected:
            pointer m_begin{ nullptr }; ///< Current write position
            pointer m_end{ nullptr };   ///< End of buffer
        };

        /**
         * @class BasicReadBuffer
         * @brief A memory buffer for reading data with automatic endianness conversion
         *
         * This class provides methods to read primitive values, spans, and strings
         * from a memory buffer with automatic conversion from the target endianness.
         * Both safe and unsafe operations are provided, where safe operations check
         * buffer boundaries before reading.
         *
         * @tparam Encoding Source endianness of the data
         */
        template <std::endian Encoding>
        class BasicReadBuffer {
        public:
            /**
             * @name Type Definitions
             * @{
             */
            using value_type = uint8_t;        ///< The underlying byte type
            using pointer = uint8_t*;          ///< Pointer to byte type
            using const_pointer = const uint8_t*; ///< Const pointer to byte type
            /** @} */

            /**
             * @name Constructors
             * @{
             */

             /**
              * @brief Constructs a read buffer with specified begin and end pointers
              * @param begin Pointer to the beginning of the buffer
              * @param end Pointer one-past-the-end of the buffer
              */
            explicit constexpr BasicReadBuffer(const_pointer begin, const_pointer end) noexcept
                : m_begin{ begin }, m_end{ end } {
            }

            /**
             * @brief Constructs a read buffer with specified begin pointer and size
             * @param begin Pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr BasicReadBuffer(const_pointer begin, size_t size) noexcept
                : BasicReadBuffer{ begin, begin + size } {
            }

            /**
             * @brief Constructs a read buffer from a void pointer and size
             * @param begin Void pointer to the beginning of the buffer
             * @param size Size of the buffer in bytes
             */
            explicit constexpr BasicReadBuffer(const void* begin, size_t size) noexcept
                : BasicReadBuffer{ static_cast<const_pointer>(begin), size } {
            }
            /** @} */

            /**
             * @name Buffer Accessors
             * @{
             */

             /**
              * @brief Gets the current end pointer of the buffer
              * @return Const pointer to the end of the buffer
              */
            [[nodiscard]] constexpr const_pointer end() const noexcept { return m_end; }

            /**
             * @brief Gets the current data pointer (read position)
             * @return Const pointer to the current read position
             */
            [[nodiscard]] constexpr const_pointer data() const noexcept { return m_begin; }

            /**
             * @brief Gets the current begin pointer (read position)
             * @return Const pointer to the current read position
             */
            [[nodiscard]] constexpr const_pointer begin() const noexcept { return m_begin; }

            /**
             * @brief Checks if the buffer is empty
             * @return true if there's no more data to read
             */
            [[nodiscard]] constexpr bool empty() const noexcept { return m_begin == m_end; }

            /**
             * @brief Checks if the buffer is in an error state
             * @return true if the buffer pointers are invalid
             */
            [[nodiscard]] constexpr bool error() const noexcept { return m_end < m_begin || !m_begin; }

            /**
             * @brief Gets the remaining size of the buffer
             * @return Number of bytes available for reading
             */
            [[nodiscard]] constexpr size_t size() const noexcept { return m_end - m_begin; }
            /** @} */

            /**
             * @name Buffer Operations
             * @{
             */

             /**
              * @brief Skips a specified number of bytes from the front
              * @param bytes Number of bytes to skip
              *
              * This function advances the read position by the specified number of bytes.
              * If skipping would go beyond the end of the buffer, the read position is set
              * to the end of the buffer.
              */
            constexpr void skipFront(size_t bytes) noexcept {
                auto ptr = m_begin + bytes;
                m_begin = (ptr <= m_end) ? ptr : m_end;
            }

            /**
             * @brief Skips a specified number of bytes from the back
             * @param bytes Number of bytes to skip
             *
             * This function moves the end position backward by the specified number of bytes.
             * If skipping would go before the beginning of the buffer, the end position is set
             * to the beginning of the buffer.
             */
            constexpr void skipBack(size_t bytes) noexcept {
                auto ptr = m_end - bytes;
                m_end = (ptr >= m_begin) ? ptr : m_begin;
            }
            /** @} */

            /**
             * @name Unsafe Read Operations (Front)
             * @brief Operations that read data from the front without checking boundaries
             * @{
             */

             /**
              * @brief Reads a value from the front without checking boundaries
              * @tparam T Type of the value to read (must satisfy SwapTypeNonConst)
              * @param value Reference to store the read value
              *
              * Reads a value from the front of the buffer with automatic endianness conversion.
              * This function does not check if there's enough data in the buffer.
              */
            template <SwapTypeNonConst T>
            void unsafePopFront(T& value) noexcept {
                // Copy the value with endian conversion if needed
                basicCopy<Encoding>(value, m_begin);
                m_begin += sizeof(T);
            }

            /**
             * @brief Reads a span of values from the front without checking boundaries
             * @tparam T Type of the values in the span (must satisfy SwapTypeNonConst)
             * @tparam N Size of the span
             * @param span Span to store the read values
             *
             * Reads multiple values from the front of the buffer with automatic endianness conversion.
             * This function does not check if there's enough data in the buffer.
             */
            template <SwapTypeNonConst T, size_t N>
            void unsafePopFront(std::span<T, N> span) noexcept {
                // Copy the span with endian conversion if needed
                basicCopy<Encoding>(span, m_begin);
                m_begin += sizeof(T) * span.size();
            }

            /**
             * @brief Reads a value from the front without checking boundaries
             * @tparam T Type of the value to read (must satisfy SwapType)
             * @return The read value
             *
             * Reads a value from the front of the buffer with automatic endianness conversion.
             * This function does not check if there's enough data in the buffer.
             */
            template <SwapType T>
            [[nodiscard]] T unsafePopFront() noexcept {
                T value{};
                unsafePopFront(value);
                return value;
            }
            /** @} */

            /**
             * @name Unsafe Read Operations (Back)
             * @brief Operations that read data from the back without checking boundaries
             * @{
             */

             /**
              * @brief Reads a value from the back without checking boundaries
              * @tparam T Type of the value to read (must satisfy SwapTypeNonConst)
              * @param value Reference to store the read value
              *
              * Reads a value from the back of the buffer with automatic endianness conversion.
              * This function does not check if there's enough data in the buffer.
              */
            template <SwapTypeNonConst T>
            void unsafePopBack(T& value) noexcept {
                m_end -= sizeof(T);
                // Copy the value with endian conversion if needed
                basicCopy<Encoding>(value, m_end);
            }

            /**
             * @brief Reads a span of values from the back without checking boundaries
             * @tparam T Type of the values in the span (must satisfy SwapTypeNonConst)
             * @tparam N Size of the span
             * @param span Span to store the read values
             *
             * Reads multiple values from the back of the buffer with automatic endianness conversion.
             * This function does not check if there's enough data in the buffer.
             */
            template <SwapTypeNonConst T, size_t N>
            void unsafePopBack(std::span<T, N> span) noexcept {
                m_end -= sizeof(T) * span.size();
                // Copy the span with endian conversion if needed
                basicCopy<Encoding>(span, m_end);
            }

            /**
             * @brief Reads a value from the back without checking boundaries
             * @tparam T Type of the value to read (must satisfy SwapType)
             * @return The read value
             *
             * Reads a value from the back of the buffer with automatic endianness conversion.
             * This function does not check if there's enough data in the buffer.
             */
            template <SwapType T>
            [[nodiscard]] T unsafePopBack() noexcept {
                T value{};
                unsafePopBack(value); // Fixed bug: was calling unsafePopFront
                return value;
            }
            /** @} */

            /**
             * @name Safe Read Operations (Front)
             * @brief Operations that check buffer boundaries before reading from the front
             * @{
             */

             /**
              * @brief Safely reads a value from the front
              * @tparam T Type of the value to read (must satisfy SwapTypeNonConst)
              * @param value Reference to store the read value
              * @return true if the operation failed (not enough data), false on success
              *
              * Reads a value from the front of the buffer with automatic endianness conversion,
              * but only if there's enough data available.
              */
            template <SwapTypeNonConst T>
            [[nodiscard]] bool popFront(T& value) noexcept {
                if (m_begin + sizeof(T) <= m_end) {
                    unsafePopFront(value);
                    return false; // Success (no error)
                }
                return true; // Error (buffer underflow)
            }

            /**
             * @brief Safely reads a span of values from the front
             * @tparam T Type of the values in the span (must satisfy SwapTypeNonConst)
             * @tparam N Size of the span
             * @param span Span to store the read values
             * @return true if the operation failed (not enough data), false on success
             *
             * Reads multiple values from the front of the buffer with automatic endianness conversion,
             * but only if there's enough data available for the entire span.
             */
            template <SwapTypeNonConst T, size_t N>
            [[nodiscard]] bool popFront(std::span<T, N> span) noexcept {
                if (m_begin + sizeof(T) * span.size() <= m_end) {
                    unsafePopFront(span);
                    return false; // Success (no error)
                }
                return true; // Error (buffer underflow)
            }

            /**
             * @brief Safely reads a string from the front
             * @param str String to store the read data
             * @return true if the operation failed (not enough data or validation error), false on success
             *
             * Reads a string from the front of the buffer using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             * The size suffix is checked to validate the string length.
             */
            [[nodiscard]] bool popFront(std::string& str) noexcept {
                // Save the current position in case of failure
                auto oldBegin = m_begin;

                // Check if we have enough space for at least the size prefix and suffix (8 bytes)
                if (m_begin + 8 <= m_end) {
                    // Read size prefix
                    uint32_t size = 0;
                    unsafePopFront(size);

                    // Check if we have enough space for the content and size suffix
                    if (m_begin + size + 4 <= m_end) {
                        // Resize string to hold the content
                        str.resize(size);

                        // Read content
                        if (!str.empty()) {
                            unsafePopFront(std::span{ &str[0], size });
                        }

                        // Read size suffix for validation
                        uint32_t sizeCheck = 0;
                        unsafePopFront(sizeCheck);

                        // Validate the size suffix
                        if (size == sizeCheck) {
                            return false; // Success (no error)
                        }
                    }
                }

                // If we get here, something went wrong
                str.clear();
                m_begin = oldBegin; // Restore the original position
                return true; // Error
            }

            /**
             * @brief Safely reads a wide string from the front
             * @param wstr Wide string to store the read data
             * @return true if the operation failed (not enough data or validation error), false on success
             *
             * Reads a wide string from the front of the buffer using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             * The size suffix is checked to validate the string length.
             */
            [[nodiscard]] bool popFront(std::wstring& wstr) noexcept {
                // Save the current position in case of failure
                auto oldBegin = m_begin;

                // Check if we have enough space for at least the size prefix and suffix (8 bytes)
                if (m_begin + 8 <= m_end) {
                    // Read size prefix
                    uint32_t size = 0;
                    unsafePopFront(size);

                    // Check if we have enough space for the content and size suffix
                    // Note: wchar_t is 2 bytes on Windows, might be 4 on other platforms
                    if (m_begin + (size * sizeof(wchar_t)) + 4 <= m_end) {
                        // Resize string to hold the content
                        wstr.resize(size);

                        // Read content
                        if (!wstr.empty()) {
                            unsafePopFront(std::span{ &wstr[0], size });
                        }

                        // Read size suffix for validation
                        uint32_t sizeCheck = 0;
                        unsafePopFront(sizeCheck);

                        // Validate the size suffix
                        if (size == sizeCheck) {
                            return false; // Success (no error)
                        }
                    }
                }

                // If we get here, something went wrong
                wstr.clear();
                m_begin = oldBegin; // Restore the original position
                return true; // Error
            }
            /** @} */

            /**
             * @name Safe Read Operations (Back)
             * @brief Operations that check buffer boundaries before reading from the back
             * @{
             */

             /**
              * @brief Safely reads a value from the back
              * @tparam T Type of the value to read (must satisfy SwapTypeNonConst)
              * @param value Reference to store the read value
              * @return true if the operation failed (not enough data), false on success
              *
              * Reads a value from the back of the buffer with automatic endianness conversion,
              * but only if there's enough data available.
              */
            template <SwapTypeNonConst T>
            [[nodiscard]] bool popBack(T& value) noexcept {
                if (m_begin + sizeof(T) <= m_end) {
                    unsafePopBack(value);
                    return false; // Success (no error)
                }
                return true; // Error (buffer underflow)
            }

            /**
             * @brief Safely reads a span of values from the back
             * @tparam T Type of the values in the span (must satisfy SwapTypeNonConst)
             * @tparam N Size of the span
             * @param span Span to store the read values
             * @return true if the operation failed (not enough data), false on success
             *
             * Reads multiple values from the back of the buffer with automatic endianness conversion,
             * but only if there's enough data available for the entire span.
             */
            template <SwapTypeNonConst T, size_t N>
            [[nodiscard]] bool popBack(std::span<T, N> span) noexcept {
                if (m_begin + sizeof(T) * span.size() <= m_end) {
                    unsafePopBack(span);
                    return false; // Success (no error)
                }
                return true; // Error (buffer underflow)
            }

            /**
             * @brief Safely reads a string from the back
             * @param str String to store the read data
             * @return true if the operation failed (not enough data or validation error), false on success
             *
             * Reads a string from the back of the buffer using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             * The size suffix is checked to validate the string length.
             */
            [[nodiscard]] bool popBack(std::string& str) noexcept {
                // Save the current end position in case of failure
                auto oldEnd = m_end;

                // Check if we have enough space for at least the size prefix and suffix (8 bytes)
                if (m_begin + 8 <= m_end) {
                    // Read size suffix (which is at the end when reading from back)
                    uint32_t size = 0;
                    unsafePopBack(size);

                    // Check if we have enough space for the content and size prefix
                    if (m_begin + size + 4 <= m_end) {
                        // Resize string to hold the content
                        str.resize(size);

                        // Read content
                        if (!str.empty()) {
                            unsafePopBack(std::span{ &str[0], size });
                        }

                        // Read size prefix for validation
                        uint32_t sizeCheck = 0;
                        unsafePopBack(sizeCheck);

                        // Validate the size prefix
                        if (size == sizeCheck) {
                            return false; // Success (no error)
                        }
                    }
                }

                // If we get here, something went wrong
                str.clear();
                m_end = oldEnd; // Restore the original position
                return true; // Error
            }

            /**
             * @brief Safely reads a wide string from the back
             * @param wstr Wide string to store the read data
             * @return true if the operation failed (not enough data or validation error), false on success
             *
             * Reads a wide string from the back of the buffer using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             * The size suffix is checked to validate the string length.
             */
            [[nodiscard]] bool popBack(std::wstring& wstr) noexcept {
                // Save the current end position in case of failure
                auto oldEnd = m_end;

                // Check if we have enough space for at least the size prefix and suffix (8 bytes)
                if (m_begin + 8 <= m_end) {
                    // Read size suffix (which is at the end when reading from back)
                    uint32_t size = 0;
                    unsafePopBack(size);

                    // Check if we have enough space for the content and size prefix
                    // Note: wchar_t is 2 bytes on Windows, might be 4 on other platforms
                    if (m_begin + (size * sizeof(wchar_t)) + 4 <= m_end) {
                        // Resize string to hold the content
                        wstr.resize(size);

                        // Read content
                        if (!wstr.empty()) {
                            unsafePopBack(std::span{ &wstr[0], size });
                        }

                        // Read size prefix for validation
                        uint32_t sizeCheck = 0;
                        unsafePopBack(sizeCheck);

                        // Validate the size prefix
                        if (size == sizeCheck) {
                            return false; // Success (no error)
                        }
                    }
                }

                // If we get here, something went wrong
                wstr.clear();
                m_end = oldEnd; // Restore the original position
                return true; // Error
            }
            /** @} */

        private:
            const_pointer m_begin{ nullptr }; ///< Current read position
            const_pointer m_end{ nullptr };   ///< End of buffer
        };

    } // namespace endian
} // namespace mz

#endif // MZ_ENDIAN_BASIC_BUFFERS_HEADER_FILE
