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

#ifndef MZ_ENDIAN_BYTE_ARRAY_HEADER_FILE
#define MZ_ENDIAN_BYTE_ARRAY_HEADER_FILE
#pragma once

/**
 * @file EndianByteArray.h
 * @brief Provides a fixed-size byte array with endian-aware operations
 *
 * This header defines the ByteArray template class, which wraps a fixed-size array
 * of bytes with additional functionality for endianness handling, string conversion,
 * and hash generation. It provides a memory-efficient alternative to std::vector
 * for fixed-size byte sequences.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include <cstdint>
#include <string>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <span>
#include "EndianConcepts.h"



namespace mz {
    namespace endian {

        /**
         * @class ByteArray
         * @brief A fixed-size array of bytes with endian-aware operations
         *
         * This class provides a fixed-size array of bytes with additional
         * functionality for endianness handling, string conversion, and hash generation.
         * It is designed for performance-critical applications where the size is known
         * at compile time.
         *
         * @tparam N Size of the byte array (must be greater than 0)
         */
        template <size_t N>
            requires (N > 0)
        class alignas(8) ByteArray
        {
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
              * @brief Default constructor
              *
              * Creates a zero-initialized byte array.
              */
            explicit constexpr ByteArray() noexcept = default;

            /**
             * @brief Constructs a byte array from a string
             * @param str String to copy into the byte array
             *
             * The string is copied into the array, and if it's shorter than the array size,
             * the remaining bytes are filled using a hash-based algorithm.
             */
            explicit ByteArray(const std::string& str) noexcept {
                fill(str);
            }

            /**
             * @brief Constructs a byte array from a wide string
             * @param wstr Wide string to copy into the byte array with endian conversion
             *
             * The wide string is copied into the array with endian conversion if needed.
             * If it's shorter than the array size, the remaining bytes are filled using
             * a hash-based algorithm.
             */
            explicit ByteArray(const std::wstring& wstr) noexcept {
                fill(wstr);
            }
            /** @} */

            /**
             * @name Assignment Operators
             * @{
             */

             /**
              * @brief Assigns a string to the byte array
              * @param str String to copy into the byte array
              * @return Reference to this byte array
              */
            ByteArray& operator=(const std::string& str) noexcept {
                fill(str);
                return *this;
            }

            /**
             * @brief Assigns a wide string to the byte array
             * @param wstr Wide string to copy into the byte array with endian conversion
             * @return Reference to this byte array
             */
            ByteArray& operator=(const std::wstring& wstr) noexcept {
                fill(wstr);
                return *this;
            }
            /** @} */

            /**
             * @name Accessors
             * @{
             */

             /**
              * @brief Gets a pointer to the byte array data
              * @return Pointer to the first byte of the array
              */
            [[nodiscard]] constexpr pointer data() noexcept {
                return m_bytes;
            }

            /**
             * @brief Gets a const pointer to the byte array data
             * @return Const pointer to the first byte of the array
             */
            [[nodiscard]] constexpr const_pointer data() const noexcept {
                return m_bytes;
            }

            /**
             * @brief Gets the size of the byte array
             * @return Size of the array in bytes
             */
            [[nodiscard]] constexpr size_t size() const noexcept {
                return N;
            }

            /**
             * @brief Creates a span view of the byte array
             * @return A span referencing the byte array
             */
            [[nodiscard]] constexpr std::span<value_type, N> span() noexcept {
                return std::span<value_type, N>{m_bytes};
            }

            /**
             * @brief Creates a const span view of the byte array
             * @return A const span referencing the byte array
             */
            [[nodiscard]] constexpr std::span<const value_type, N> span() const noexcept {
                return std::span<const value_type, N>{m_bytes};
            }
            /** @} */

            /**
             * @name Hash Generation
             * @{
             */

             /**
              * @brief Generates a 64-bit FNV-1a hash of the entire byte array
              * @return 64-bit hash value
              *
              * This method computes the FNV-1a hash of the entire byte array.
              * The FNV-1a hash is a non-cryptographic hash function that is fast
              * and has good distribution properties.
              */
            [[nodiscard]] uint64_t generateHash() const noexcept {
                return computeHash(0, N);
            }
            /** @} */

            /**
             * @name Array Operations
             * @{
             */

             /**
              * @brief Clears the byte array (sets all bytes to zero)
              *
              * This method is optimized for performance using memset.
              */
            void clear() noexcept {
                std::memset(m_bytes, 0, N);
            }

            /**
             * @brief Fills the byte array with a sequence of values
             * @param initial Starting value for the sequence
             * @param step Step size for the sequence
             *
             * This method fills the array with the sequence:
             * [initial, initial+step, initial+2*step, ...]
             */
            void fillWithRange(int initial = 0, int step = 1) noexcept {
                for (size_t i = 0; i < N; ++i) {
                    m_bytes[i] = static_cast<uint8_t>((i * step) + initial);
                }
            }
            /** @} */

            /**
             * @name String Conversion
             * @{
             */

             /**
              * @brief Converts a specific number of bytes to a string
              * @param count Number of bytes to convert (capped at array size)
              * @return String containing the converted bytes
              *
              * This method converts up to 'count' bytes from the array to a string.
              * If 'count' is greater than the array size, it is capped at the array size.
              */
            [[nodiscard]] std::string toString(size_t count) const noexcept {
                // Cap count at array size
                count = std::min(count, N);

                // Create a string from the bytes
                return std::string{ reinterpret_cast<const char*>(data()), count };
            }

            /**
             * @brief Converts the byte array to a string up to the first null byte
             * @return String containing the converted bytes
             *
             * This method converts the bytes from the array to a string, stopping at
             * the first null byte or the end of the array.
             */
            [[nodiscard]] std::string toString() const noexcept {
                // Find the first null byte or the end of the array
                size_t nullPos = 0;
                while (nullPos < N && m_bytes[nullPos] != 0) {
                    ++nullPos;
                }

                // Convert to string
                return toString(nullPos);
            }

            /**
             * @brief Checks if the byte array is empty (all zeros)
             * @return true if all bytes are zero, false otherwise
             *
             * This method is optimized for performance by avoiding temporary arrays.
             */
            [[nodiscard]] bool empty() const noexcept {
                // Check if all bytes are zero
                for (size_t i = 0; i < N; ++i) {
                    if (m_bytes[i] != 0) {
                        return false;
                    }
                }
                return true;
            }
            /** @} */

            /**
             * @name Comparison Operators
             * @{
             */

             /**
              * @brief Less-than comparison operator
              * @param lhs First byte array to compare
              * @param rhs Second byte array to compare
              * @return true if lhs is lexicographically less than rhs
              *
              * This operator compares the arrays lexicographically.
              */
            friend bool operator<(const ByteArray& lhs, const ByteArray& rhs) noexcept {
                return std::memcmp(lhs.m_bytes, rhs.m_bytes, N) < 0;
            }

            /**
             * @brief Equality comparison operator
             * @param lhs First byte array to compare
             * @param rhs Second byte array to compare
             * @return true if the arrays have identical content
             *
             * This operator checks if the arrays have identical content.
             */
            friend bool operator==(const ByteArray& lhs, const ByteArray& rhs) noexcept {
                return std::memcmp(lhs.m_bytes, rhs.m_bytes, N) == 0;
            }

            /**
             * @brief Inequality comparison operator
             * @param lhs First byte array to compare
             * @param rhs Second byte array to compare
             * @return true if the arrays have different content
             */
            friend bool operator!=(const ByteArray& lhs, const ByteArray& rhs) noexcept {
                return !(lhs == rhs);
            }

            /**
             * @brief Greater-than comparison operator
             * @param lhs First byte array to compare
             * @param rhs Second byte array to compare
             * @return true if lhs is lexicographically greater than rhs
             */
            friend bool operator>(const ByteArray& lhs, const ByteArray& rhs) noexcept {
                return rhs < lhs;
            }

            /**
             * @brief Less-than-or-equal comparison operator
             * @param lhs First byte array to compare
             * @param rhs Second byte array to compare
             * @return true if lhs is lexicographically less than or equal to rhs
             */
            friend bool operator<=(const ByteArray& lhs, const ByteArray& rhs) noexcept {
                return !(lhs > rhs);
            }

            /**
             * @brief Greater-than-or-equal comparison operator
             * @param lhs First byte array to compare
             * @param rhs Second byte array to compare
             * @return true if lhs is lexicographically greater than or equal to rhs
             */
            friend bool operator>=(const ByteArray& lhs, const ByteArray& rhs) noexcept {
                return !(lhs < rhs);
            }
            /** @} */

        protected:
            /**
             * @name Hash Constants
             * @{
             */

             /// FNV-1a prime for 64-bit hash
            static constexpr uint64_t HashPrime{ 1099511628211ULL };

            /// FNV-1a initial value for 64-bit hash
            static constexpr uint64_t HashInit{ 14695981039346656037ULL };
            /** @} */

            /**
             * @name Hash Computation
             * @{
             */

             /**
              * @brief Computes the next hash value in the FNV-1a algorithm
              * @param currentHash Current hash value
              * @param nextByte Next byte to incorporate into the hash
              * @return Updated hash value
              *
              * This method implements one step of the FNV-1a hash algorithm:
              * 1. XOR the current hash with the next byte
              * 2. Multiply by the FNV prime
              */
            [[nodiscard]] constexpr static uint64_t computeNextHash(uint64_t currentHash, uint64_t nextByte) noexcept {
                currentHash ^= nextByte;
                currentHash *= HashPrime;
                return currentHash;
            }

            /**
             * @brief Computes the FNV-1a hash of a portion of the byte array
             * @param initialHash Starting hash value
             * @param maxIndex Maximum index to include in the hash (exclusive)
             * @return Computed hash value
             *
             * This method efficiently computes the FNV-1a hash of bytes from
             * index 0 up to but not including maxIndex. For larger arrays,
             * it uses loop unrolling to improve performance.
             */
            [[nodiscard]] uint64_t computeHash(uint64_t initialHash, uint64_t maxIndex) const noexcept {
                uint64_t hash = initialHash;
                uint64_t index = 0;

                // For small arrays, use simple loop
                if (maxIndex <= 8) {
                    while (index < maxIndex) {
                        hash = computeNextHash(hash, m_bytes[index++]);
                    }
                    return hash;
                }

                // For larger arrays, use loop unrolling for better performance
                // Process 8 bytes at a time for most of the array
                const uint64_t bulkEnd = maxIndex & ~0x7ULL;
                while (index < bulkEnd) {
                    hash = computeNextHash(hash, m_bytes[index++]);
                    hash = computeNextHash(hash, m_bytes[index++]);
                    hash = computeNextHash(hash, m_bytes[index++]);
                    hash = computeNextHash(hash, m_bytes[index++]);
                    hash = computeNextHash(hash, m_bytes[index++]);
                    hash = computeNextHash(hash, m_bytes[index++]);
                    hash = computeNextHash(hash, m_bytes[index++]);
                    hash = computeNextHash(hash, m_bytes[index++]);
                }

                // Process remaining bytes
                while (index < maxIndex) {
                    hash = computeNextHash(hash, m_bytes[index++]);
                }

                return hash;
            }
            /** @} */

            /**
             * @name String Filling Methods
             * @{
             */

             /**
              * @brief Fills the beginning of the byte array with a string
              * @param str String to copy
              * @return Number of bytes filled
              *
              * This method copies the string into the byte array, skipping null characters.
              * It returns the number of bytes filled, which may be less than the string length
              * if the array is smaller or if the string contains null characters.
              */
            [[nodiscard]] size_t fillHead(const std::string& str) noexcept {
                size_t index = 0;

                // Process characters in blocks for better performance when possible
                if (str.size() <= N) {
                    for (char c : str) {
                        if (c) {
                            m_bytes[index] = static_cast<uint8_t>(c);
                            if (++index == N) break;
                        }
                    }
                }
                else {
                    // If string is larger than the array, only process what fits
                    for (size_t i = 0; i < N; ++i) {
                        char c = str[i];
                        if (c) {
                            m_bytes[index] = static_cast<uint8_t>(c);
                            if (++index == N) break;
                        }
                    }
                }

                return index;
            }

            /**
             * @brief Fills the beginning of the byte array with a wide string
             * @param wstr Wide string to copy with endian conversion
             * @return Number of bytes filled
             *
             * This method copies the wide string into the byte array with endian conversion
             * if needed. It handles each UTF-16 character as two bytes, skipping null bytes.
             */
            [[nodiscard]] size_t fillHead(const std::wstring& wstr) noexcept {
                size_t index = 0;

                for (wchar_t w : wstr) {
                    // Apply endian conversion if needed
                    if constexpr (std::endian::native != std::endian::little) {
                        w = swap_bytes(w);
                    }

                    // Process the lower byte
                    if (uint8_t x = static_cast<uint8_t>(w & 0xFF); x) {
                        m_bytes[index] = x;
                        if (++index == N) break;
                    }

                    // Process the upper byte
                    if (uint8_t x = static_cast<uint8_t>(w >> 8); x) {
                        m_bytes[index] = x;
                        if (++index == N) break;
                    }
                }

                return index;
            }

            /**
             * @brief Fills the remainder of the byte array with hash-derived values
             * @param filledBytes Number of bytes already filled
             * @return Final hash value used for filling
             *
             * This method fills the remainder of the array (after filledBytes)
             * with values derived from a hash of the already filled portion.
             * If filledBytes is 0, the entire array is cleared.
             */
            [[nodiscard]] uint64_t fillTail(uint64_t filledBytes) noexcept {
                // If nothing was filled, clear the array
                if (filledBytes == 0) {
                    std::memset(m_bytes, 0, N);
                    return 0;
                }

                // Compute hash of the filled portion
                uint64_t hashValue = computeHash(HashInit, filledBytes);

                // Add a null terminator if there's room
                if (filledBytes < N) {
                    m_bytes[filledBytes++] = 0;
                }

                // Fill the rest with hash-derived values
                while (filledBytes < N) {
                    // Update hash based on the bytes we've already written
                    // Using modulo ensures we use values from the start of the array
                    // when we've filled more than N bytes (which shouldn't happen in practice)
                    hashValue = computeNextHash(hashValue, m_bytes[filledBytes % N]);

                    // Write the lowest byte of the hash to the array
                    m_bytes[filledBytes++] = static_cast<uint8_t>(hashValue);
                }

                return hashValue;
            }

            /**
             * @brief Fills the byte array with a string and hash-derived padding
             * @param str String to copy
             * @return Final hash value used for padding
             *
             * This method clears the array, copies the string, and fills any
             * remaining space with hash-derived values.
             */
            [[nodiscard]] uint64_t fill(const std::string& str) noexcept {
                std::memset(m_bytes, 0, N);
                return fillTail(fillHead(str));
            }

            /**
             * @brief Fills the byte array with a wide string and hash-derived padding
             * @param wstr Wide string to copy
             * @return Final hash value used for padding
             *
             * This method clears the array, copies the wide string (with endian conversion
             * if needed), and fills any remaining space with hash-derived values.
             */
            [[nodiscard]] uint64_t fill(const std::wstring& wstr) noexcept {
                std::memset(m_bytes, 0, N);
                return fillTail(fillHead(wstr));
            }
            /** @} */

            // The byte array storage (kept as specified)
            uint8_t m_bytes[N]{ 0 };
        };

    } // namespace endian
} // namespace mz

// Clean up compatibility macros to avoid namespace pollution


#endif // MZ_ENDIAN_BYTE_ARRAY_HEADER_FILE
