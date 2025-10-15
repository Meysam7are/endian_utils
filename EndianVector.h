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

#ifndef MZ_ENDIAN_VECTOR_HEADER_FILE
#define MZ_ENDIAN_VECTOR_HEADER_FILE
#pragma once

/**
 * @file EndianVector.h
 * @brief Provides a non-templated vector for endian-aware binary serialization
 *
 * This header defines the Vector class, which is a specialization of
 * BasicVector that uses the default stream endianness. It provides a simpler
 * interface for the common case of serializing data with automatic endianness
 * conversion to the predefined stream format.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

#include "EndianBasicVector.h"
#include "EndianReadBuffer.h"
#include "EndianWriteBuffer.h"

namespace mz {
    namespace endian {

        /**
         * @class Vector
         * @brief A non-templated dynamic vector that uses the default stream endianness
         *
         * This class is a specialization of BasicVector that automatically uses
         * the predefined stream endianness (typically little-endian). It provides the
         * same functionality as BasicVector but without requiring the endianness
         * to be explicitly specified.
         *
         * Vector allows for reading and writing primitive values, spans, and strings
         * with automatic endianness conversion when needed based on the platform's
         * native endianness.
         */
        class Vector : public BasicVector<stream_endian> {
        public:
            /**
             * @name Constructors and Assignment
             * @{
             */

             /**
              * @brief Default constructor
              *
              * Creates an empty vector with zero size.
              */
            explicit constexpr Vector() noexcept : BasicVector<stream_endian>() {}

            /**
             * @brief Constructs a vector with a specific initial size
             * @param size Initial size in bytes
             *
             * Creates a vector with the specified size and capacity.
             */
            explicit constexpr Vector(size_t size) noexcept : BasicVector<stream_endian>(size) {}

            /**
             * @brief Copy constructor
             * @param other Vector to copy from
             *
             * Creates a deep copy of the source vector.
             */
            Vector(const Vector& other) noexcept : BasicVector<stream_endian>(other) {}

            /**
             * @brief Move constructor
             * @param other Vector to move from
             *
             * Moves the resources from the source vector, leaving it empty.
             */
            Vector(Vector&& other) noexcept : BasicVector<stream_endian>(std::move(other)) {}

            /**
             * @brief Copy assignment operator
             * @param other Vector to copy from
             * @return Reference to this vector
             *
             * Makes this vector a deep copy of the source vector.
             */
            Vector& operator=(const Vector& other) noexcept {
                BasicVector<stream_endian>::operator=(other);
                return *this;
            }

            /**
             * @brief Move assignment operator
             * @param other Vector to move from
             * @return Reference to this vector
             *
             * Moves the resources from the source vector, leaving it empty.
             */
            Vector& operator=(Vector&& other) noexcept {
                BasicVector<stream_endian>::operator=(std::move(other));
                return *this;
            }
            /** @} */

            /**
             * @name Vector-specific Methods
             * @{
             */

             /**
              * @brief Creates a read buffer from the end of the vector and shrinks the vector
              * @param size Number of bytes to remove from the vector for the buffer
              * @return ReadBuffer referencing the removed portion
              *
              * This method creates a ReadBuffer that references the data at the end of the
              * vector, then shrinks the vector by the specified size. This allows for
              * reading data from the vector without copying it.
              *
              * @warning The returned ReadBuffer is only valid until the Vector is modified,
              *          as it references memory owned by the Vector.
              */
            ReadBuffer popBackBuffer(size_t Len) noexcept {
                Len = shrinkBy(Len);
                return ReadBuffer(data() + size(), Len);
            }

            /**
             * @brief Checks if the vector is in an error state
             * @return 0 if no error, 1 if the internal size exceeds capacity
             *
             * This method provides a simple error check to determine if the vector
             * is in an inconsistent state where the logical size exceeds the
             * allocated capacity.
             */
            [[nodiscard]] constexpr int error() const noexcept {
                // Check if there's insufficient capacity for the current size
                return BasicVector<stream_endian>::size() < size() ? 1 : 0;
            }
            /** @} */

            /**
             * @name Equality Operators
             * @{
             */

             /**
              * @brief Equality comparison operator
              * @param lhs First vector to compare
              * @param rhs Second vector to compare
              * @return true if the vectors have identical content, false otherwise
              *
              * Two vectors are considered equal if they have the same size and
              * their content is byte-for-byte identical.
              */
            friend bool operator==(const Vector& lhs, const Vector& rhs) noexcept {
                return lhs.size() == rhs.size()
                    && std::memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
            }

            /**
             * @brief Inequality comparison operator
             * @param lhs First vector to compare
             * @param rhs Second vector to compare
             * @return true if the vectors differ in size or content, false if they're identical
             */
            friend bool operator!=(const Vector& lhs, const Vector& rhs) noexcept {
                return !(lhs == rhs);
            }
            /** @} */

            // All other functionality is inherited from BasicVector<stream_endian>
        };

    } // namespace endian
} // namespace mz

#endif // MZ_ENDIAN_VECTOR_HEADER_FILE
