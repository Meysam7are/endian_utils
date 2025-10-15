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

#ifndef MZ_ENDIAN_BASIC_VECTOR_HEADER_FILE
#define MZ_ENDIAN_BASIC_VECTOR_HEADER_FILE
#pragma once

/**
 * @file EndianBasicVector.h
 * @brief Provides a dynamic vector for endian-aware binary serialization
 *
 * This header defines a class template that wraps std::vector<uint8_t> to provide
 * a dynamic buffer with automatic endianness conversion. It supports reading and
 * writing primitive types, spans, and strings with proper endianness handling.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include <cstdint>
#include <vector>
#include <cstring>  // For memcpy
#include <string>
#include <type_traits>
#include <concepts>
#include <span>
#include <bit>


#include "EndianBasicBuffers.h"
#include "EndianConcepts.h"

namespace mz {
    namespace endian {

        /**
         * @class BasicVector
         * @brief A dynamic vector with automatic endianness conversion
         *
         * This class wraps around std::vector<uint8_t> to provide a dynamically resizable
         * buffer with automatic endianness conversion for binary serialization. It supports
         * various operations for reading and writing primitive types, arrays, and strings.
         *
         * @tparam Encoding Target endianness for the stored data
         */
        template <std::endian Encoding>
        class BasicVector {
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
             * @name Basic Accessors
             * @{
             */

             /**
              * @brief Checks if the vector is empty
              * @return true if the vector contains no data
              */
            [[nodiscard]] constexpr bool empty() const noexcept {
                return m_size == 0;  // More explicit than !m_size
            }

            /**
             * @brief Gets the size of the vector in bytes
             * @return Size of the vector in bytes
             */
            [[nodiscard]] constexpr size_t size() const noexcept {
                return m_size;
            }

            /**
             * @brief Gets a pointer to the vector's data
             * @return Pointer to the beginning of the vector's data
             */
            [[nodiscard]] constexpr pointer data() noexcept {
                return m_data.data();
            }

            /**
             * @brief Gets a const pointer to the vector's data
             * @return Const pointer to the beginning of the vector's data
             */
            [[nodiscard]] constexpr const_pointer data() const noexcept {
                return m_data.data();
            }

            /**
             * @brief Gets the capacity of the vector in bytes
             * @return The current capacity of the vector
             *
             * This is the size of the underlying storage, which may be larger than
             * the logical size returned by size().
             */
            [[nodiscard]] constexpr size_t capacity() const noexcept {
                return m_data.capacity();
            }
            /** @} */

            /**
             * @name Constructors and Assignment
             * @{
             */

             /**
              * @brief Default constructor
              *
              * Creates an empty vector with zero size.
              */
            explicit constexpr BasicVector() noexcept
                : m_size{ 0 }, m_data{} {
            }

            /**
             * @brief Constructs a vector with a specific initial size
             * @param size Initial size in bytes
             *
             * Creates a vector with the specified size and capacity.
             */
            explicit constexpr BasicVector(size_t size) noexcept
                : m_size{ size }, m_data(size) {
            }

            /**
             * @brief Copy constructor
             * @param other Vector to copy from
             *
             * Creates a deep copy of the source vector.
             */
            BasicVector(const BasicVector& other) noexcept
                : m_size{ other.m_size }
            {
                m_data.resize(size());
                if (m_size > 0) {
                    std::memcpy(m_data.data(), other.m_data.data(), m_size);
                }
            }

            /**
             * @brief Move constructor
             * @param other Vector to move from
             *
             * Moves the resources from the source vector, leaving it empty.
             */
            BasicVector(BasicVector&& other) noexcept
                : m_size{ other.m_size }
                , m_data{ std::move(other.m_data) }
            {
                other.m_size = 0;
            }

            /**
             * @brief Copy assignment operator
             * @param other Vector to copy from
             * @return Reference to this vector
             *
             * Makes this vector a deep copy of the source vector.
             */
            BasicVector& operator=(const BasicVector& other) noexcept {
                if (this != &other) {
                    m_size = other.m_size;
                    m_data.resize(m_size);
                    if (m_size > 0) {
                        std::memcpy(m_data.data(), other.data(), m_size);
                    }
                }
                return *this;
            }

            /**
             * @brief Move assignment operator
             * @param other Vector to move from
             * @return Reference to this vector
             *
             * Moves the resources from the source vector, leaving it empty.
             */
            BasicVector& operator=(BasicVector&& other) noexcept {
                if (this != &other) {
                    m_size = other.m_size;
                    m_data = std::move(other.m_data);
                    other.m_size = 0;
                }
                return *this;
            }
            /** @} */

            /**
             * @name Memory Management
             * @{
             */

             /**
              * @brief Clears the vector's contents
              *
              * Resets the size to zero but keeps the allocated memory.
              * This is more efficient than resize(0) as it doesn't deallocate memory.
              */
            void clear() noexcept {
                m_size = 0;
            }

            /**
             * @brief Ensures the vector has enough capacity
             * @param newCapacity The minimum capacity to ensure
             *
             * Increases the capacity of the vector to at least the specified amount
             * if it's currently smaller.
             */
            void reserve(size_t newCapacity) noexcept {
                m_data.reserve(newCapacity);
                if (m_data.size() < newCapacity) {
                    m_data.resize(newCapacity);
                }
            }

            /**
             * @brief Resizes the vector
             * @param newSize The new size for the vector
             *
             * Changes the size of the vector to the specified value.
             * If the new size is larger, the additional space is default-initialized.
             */
            void resize(size_t newSize) noexcept {
                if (m_data.size() < newSize) {
                    m_data.resize(newSize);
                }
                m_size = newSize;
            }

            /**
             * @brief Expands the vector to its full capacity
             * @return The new size of the vector
             *
             * Increases the size of the vector to match its capacity.
             * This is useful for pre-allocating a buffer before writing to it.
             */
            [[nodiscard]] size_t expandToCapacity() noexcept {
                const size_t capacity = m_data.capacity();
                m_data.resize(capacity);
                m_size = capacity;
                return m_size;
            }

            /**
             * @brief Expands the vector by a specified amount
             * @param length Amount to expand by, in bytes
             *
             * Increases the size of the vector by the specified amount,
             * ensuring there's enough capacity first.
             */
            void expandBy(size_t length) noexcept {
                reserveExtra(length);
                m_size += length;
            }

            /**
             * @brief Reduces the size of the vector
             * @param length Amount to reduce by, in bytes
             * @return The actual amount reduced
             *
             * Decreases the size of the vector by the specified amount,
             * or to zero if the requested amount is greater than the current size.
             */
            [[nodiscard]] size_t shrinkBy(size_t length) noexcept {
                const size_t actualLength = (length < m_size) ? length : m_size;
                m_size -= actualLength;
                return actualLength;
            }
            /** @} */

            /**
             * @name Push Operations (Generic Delete)
             * @{
             */

             /**
              * @brief Deleted template to prevent usage with unsupported types
              *
              * This prevents instantiation of push operations with arbitrary types,
              * forcing the use of properly constrained overloads.
              */
            template <typename T>
            void pushBack(T) = delete;
            /** @} */

            /**
             * @name Push Operations (Endian-Aware)
             * @{
             */

             /**
              * @brief Appends a value to the vector without checking capacity
              * @tparam T Type of the value to append (must satisfy SwapType)
              * @param value Value to append
              *
              * Appends a value to the vector with automatic endianness conversion.
              * This function does not check if there's enough capacity in the vector.
              */
            template <SwapType T>
            void unsafePushBack(T value) noexcept {
                // Copy the value with endian conversion if needed
                basic_copy<Encoding>(data() + size(), value);
                m_size += sizeof(T);
            }

            /**
             * @brief Appends a value to the vector
             * @tparam T Type of the value to append (must satisfy SwapType)
             * @param value Value to append
             *
             * Appends a value to the vector with automatic endianness conversion,
             * ensuring there's enough capacity first.
             */
            template <SwapType T>
            void pushBack(T value) noexcept {
                reserveExtra(sizeof(T));
                unsafePushBack(value);
            }

            /**
             * @brief Appends a span of values to the vector
             * @tparam T Type of the values in the span (must satisfy SwapType)
             * @tparam N Size of the span
             * @param span Span of values to append
             *
             * Appends multiple values to the vector with automatic endianness conversion,
             * ensuring there's enough capacity first.
             */
            template <SwapType T, size_t N>
            void pushBack(const std::span<T, N>& span) noexcept {
                const size_t byteSize = sizeof(T) * span.size();
                reserveExtra(byteSize);
                basic_copy<Encoding>(data() + size(), span);
                m_size += byteSize;
            }

            /**
             * @brief Appends a string to the vector
             * @param str String to append
             *
             * Appends a string to the vector using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             */
            void pushBack(const std::string& str) noexcept {
                const size_t oldLength = size();
                // Calculate required size: 4 bytes for prefix + string length + 4 bytes for suffix
                const size_t byteSize = calculateStringSize(str);
                expandBy(byteSize);

                // Use a temporary BasicWriteBuffer to handle the string serialization
                BasicWriteBuffer<Encoding> buffer(data() + oldLength, byteSize);
                buffer.unsafePushBack(static_cast<uint32_t>(str.size()));
                if (!str.empty()) {
                    buffer.unsafePushBack(std::span<const char>(str.data(), str.size()));
                }
                buffer.unsafePushBack(static_cast<uint32_t>(str.size()));
            }

            /**
             * @brief Appends a wide string to the vector
             * @param wstr Wide string to append
             *
             * Appends a wide string to the vector using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             */
            void pushBack(const std::wstring& wstr) noexcept {
                const size_t oldLength = size();
                // Calculate required size: 4 bytes for prefix + string length * sizeof(wchar_t) + 4 bytes for suffix
                const size_t byteSize = calculateWideStringSize(wstr);
                expandBy(byteSize);

                // Use a temporary BasicWriteBuffer to handle the string serialization
                BasicWriteBuffer<Encoding> buffer(data() + oldLength, byteSize);
                buffer.unsafePushBack(static_cast<uint32_t>(wstr.size()));
                if (!wstr.empty()) {
                    buffer.unsafePushBack(std::span<const wchar_t>(wstr.data(), wstr.size()));
                }
                buffer.unsafePushBack(static_cast<uint32_t>(wstr.size()));
            }

            /**
             * @brief Appends a raw value to the vector without endianness conversion
             * @tparam T Type of the value to append (must satisfy TrivialType)
             * @param value Value to append
             *
             * Appends a value to the vector without any endianness conversion.
             * This is useful for values that don't need conversion, such as byte arrays.
             */
            template <TrivialType T>
            void pushBackRaw(const T& value) noexcept {
                const size_t oldLength = size();
                expandBy(sizeof(T));
                std::memcpy(data() + oldLength, &value, sizeof(T));
            }
            /** @} */

            /**
             * @name Pop Operations (Endian-Aware)
             * @{
             */

             /**
              * @brief Removes a value from the end of the vector without checking size
              * @tparam T Type of the value to retrieve (must satisfy SwapTypeNonConst)
              * @param value Reference to store the retrieved value
              *
              * Removes a value from the end of the vector with automatic endianness conversion.
              * This function does not check if there's enough data in the vector.
              */
            template <SwapTypeNonConst T>
            void unsafePopBack(T& value) noexcept {
                m_size -= sizeof(T);
                basic_copy<Encoding>(value, data() + size());
            }

            /**
             * @brief Safely removes a value from the end of the vector
             * @tparam T Type of the value to retrieve (must satisfy SwapTypeNonConst)
             * @param value Reference to store the retrieved value
             * @return true if the operation failed (not enough data), false on success
             *
             * Removes a value from the end of the vector with automatic endianness conversion,
             * but only if there's enough data available.
             */
            template <SwapTypeNonConst T>
            [[nodiscard]] bool popBack(T& value) noexcept {
                if (sizeof(T) <= m_size) {
                    unsafePopBack(value);
                    return false; // Success (no error)
                }
                return true; // Error (buffer underflow)
            }

            /**
             * @brief Safely removes a span of values from the end of the vector
             * @tparam T Type of the values in the span (must satisfy SwapTypeNonConst)
             * @tparam N Size of the span
             * @param span Span to store the retrieved values
             * @return true if the operation failed (not enough data), false on success
             *
             * Removes multiple values from the end of the vector with automatic endianness conversion,
             * but only if there's enough data available for the entire span.
             */
            template <SwapTypeNonConst T, size_t N>
            [[nodiscard]] bool popBack(std::span<T, N> span) noexcept {
                const size_t byteSize = sizeof(T) * span.size();
                if (byteSize <= m_size) {
                    m_size -= byteSize;
                    BasicReadBuffer<Encoding>{data() + size(), byteSize}.unsafePopFront(span);
                    return false; // Success (no error)
                }
                return true; // Error (buffer underflow)
            }

            /**
             * @brief Safely removes a string from the end of the vector
             * @param str String to store the retrieved data
             * @return true if the operation failed (not enough data or validation error), false on success
             *
             * Removes a string from the end of the vector using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             */
            [[nodiscard]] bool popBack(std::string& str) noexcept {
                // Create a read buffer for the entire vector
                auto buffer = BasicReadBuffer<Encoding>{ data(), size() };

                // Try to read the string from the back of the buffer
                if (!buffer.popBack(str)) {
                    // Update size on success
                    m_size = buffer.size();
                    return false; // Success (no error)
                }

                return true; // Error
            }

            /**
             * @brief Safely removes a wide string from the end of the vector
             * @param wstr Wide string to store the retrieved data
             * @return true if the operation failed (not enough data or validation error), false on success
             *
             * Removes a wide string from the end of the vector using the format:
             * [size][content][size]
             * where size is a uint32_t value representing the string length.
             */
            [[nodiscard]] bool popBack(std::wstring& wstr) noexcept {
                // Create a read buffer for the entire vector
                auto buffer = BasicReadBuffer<Encoding>{ data(), size() };

                // Try to read the wide string from the back of the buffer
                if (!buffer.popBack(wstr)) {
                    // Update size on success
                    m_size = buffer.size();
                    return false; // Success (no error)
                }

                return true; // Error
            }

            /**
             * @brief Safely removes a raw value from the end of the vector without endianness conversion
             * @tparam T Type of the value to retrieve (must satisfy TrivialTypeNonConst)
             * @param value Reference to store the retrieved value
             * @return true if the operation failed (not enough data), false on success
             *
             * Removes a value from the end of the vector without any endianness conversion,
             * but only if there's enough data available.
             */
            template <TrivialTypeNonConst T>
            [[nodiscard]] bool popBackRaw(T& value) noexcept {
                if (sizeof(T) <= m_size) {
                    m_size -= sizeof(T);
                    std::memcpy(&value, data() + size(), sizeof(T));
                    return false; // Success (no error)
                }
                return true; // Error (buffer underflow)
            }
            /** @} */

        private:
            size_t m_size{ 0 };               ///< Current logical size of the vector
            std::vector<uint8_t> m_data;    ///< Underlying storage

            /**
             * @brief Ensures the vector has enough capacity for additional data
             * @param extraSize Amount of extra capacity needed, in bytes
             *
             * This is an internal method used to ensure the vector can accommodate
             * additional data without reallocating during each append operation.
             */
            void reserveExtra(size_t extraSize) noexcept {
                const size_t requiredSize = m_size + extraSize;
                if (m_data.size() < requiredSize) {
                    // For larger allocations, use a growth factor to reduce reallocations
                    if (extraSize > 1024) {
                        const size_t newCapacity = requiredSize + (requiredSize / 2);
                        m_data.reserve(newCapacity);
                    }
                    m_data.resize(requiredSize);
                }
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
            [[nodiscard]] static constexpr size_t calculateStringSize(const std::string& str) noexcept {
                return 4 + str.size() + 4;  // prefix + content + suffix
            }

            /**
             * @brief Calculates the serialized size of a wide string
             * @param wstr Wide string to calculate size for
             * @return Total size in bytes needed to store the wide string
             *
             * The serialization format includes:
             * - 4 bytes for the size prefix
             * - N * sizeof(wchar_t) bytes for the string content
             * - 4 bytes for the size suffix (used for validation)
             */
            [[nodiscard]] static constexpr size_t calculateWideStringSize(const std::wstring& wstr) noexcept {
                return 4 + (wstr.size() * sizeof(wchar_t)) + 4;  // prefix + content + suffix
            }
        };

    } // namespace endian
} // namespace mz

// Clean up compatibility macros to avoid namespace pollution


#endif // MZ_ENDIAN_BASIC_VECTOR_HEADER_FILE
