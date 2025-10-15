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

#ifndef MZ_ENDIAN_CONVERSIONS_HEADER_FILE
#define MZ_ENDIAN_CONVERSIONS_HEADER_FILE
#pragma once

/**
 * @file EndianConversions.h
 * @brief Cross-platform endianness conversion utilities
 *
 * This header provides platform-independent functions for byte-swapping
 * integral values and enumeration types. It automatically selects the
 * most efficient implementation based on the target platform.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <concepts>


// Platform-specific byte swap intrinsics
// Each section uses the most efficient intrinsics available for the platform
#if defined(_MSC_VER) // Microsoft Visual C++
#include <stdlib.h>
#define MZ_BSWAP16(x) _byteswap_ushort(x)
#define MZ_BSWAP32(x) _byteswap_ulong(x)
#define MZ_BSWAP64(x) _byteswap_uint64(x)
#elif defined(__APPLE__) // Apple macOS/iOS
    // macOS/iOS-specific byte order functions
#include <libkern/OSByteOrder.h>
#define MZ_BSWAP16(x) OSSwapInt16(x)
#define MZ_BSWAP32(x) OSSwapInt32(x)
#define MZ_BSWAP64(x) OSSwapInt64(x)
#elif defined(__GNUC__) || defined(__clang__) // GCC or Clang
    // GCC and Clang both support these built-ins
#define MZ_BSWAP16(x) __builtin_bswap16(x)
#define MZ_BSWAP32(x) __builtin_bswap32(x)
#define MZ_BSWAP64(x) __builtin_bswap64(x)
#elif defined(__sun) || defined(sun) // Solaris
#include <sys/byteorder.h>
#define MZ_BSWAP16(x) BSWAP_16(x)
#define MZ_BSWAP32(x) BSWAP_32(x)
#define MZ_BSWAP64(x) BSWAP_64(x)
#elif defined(__FreeBSD__) // FreeBSD
#include <sys/endian.h>
#define MZ_BSWAP16(x) bswap16(x)
#define MZ_BSWAP32(x) bswap32(x)
#define MZ_BSWAP64(x) bswap64(x)
#elif defined(__OpenBSD__) // OpenBSD
#include <sys/types.h>
#define MZ_BSWAP16(x) swap16(x)
#define MZ_BSWAP32(x) swap32(x)
#define MZ_BSWAP64(x) swap64(x)
#elif defined(__NetBSD__) // NetBSD
#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define MZ_BSWAP16(x) bswap16(x)
#define MZ_BSWAP32(x) bswap32(x)
#define MZ_BSWAP64(x) bswap64(x)
#else
#define MZ_BSWAP16(x) __builtin_bswap16(x)
#define MZ_BSWAP32(x) __builtin_bswap32(x)
#define MZ_BSWAP64(x) __builtin_bswap64(x)
#endif
#else // Fallback for other platforms
    // Generic implementation using shifts and masks if no intrinsics are available
#define MZ_BSWAP16(x) \
        ((uint16_t)((((uint16_t)(x) & 0xFF00U) >> 8) | \
                   (((uint16_t)(x) & 0x00FFU) << 8)))

#define MZ_BSWAP32(x) \
        ((uint32_t)((((uint32_t)(x) & 0xFF000000U) >> 24) | \
                   (((uint32_t)(x) & 0x00FF0000U) >> 8)  | \
                   (((uint32_t)(x) & 0x0000FF00U) << 8)  | \
                   (((uint32_t)(x) & 0x000000FFU) << 24)))

#define MZ_BSWAP64(x) \
        ((uint64_t)((((uint64_t)(x) & 0xFF00000000000000ULL) >> 56) | \
                   (((uint64_t)(x) & 0x00FF000000000000ULL) >> 40) | \
                   (((uint64_t)(x) & 0x0000FF0000000000ULL) >> 24) | \
                   (((uint64_t)(x) & 0x000000FF00000000ULL) >> 8)  | \
                   (((uint64_t)(x) & 0x00000000FF000000ULL) << 8)  | \
                   (((uint64_t)(x) & 0x0000000000FF0000ULL) << 24) | \
                   (((uint64_t)(x) & 0x000000000000FF00ULL) << 40) | \
                   (((uint64_t)(x) & 0x00000000000000FFULL) << 56)))
#endif

namespace mz {
    namespace endian {

        /**
         * @namespace mz::endian
         * @brief Contains utilities for endianness conversion and handling
         */

         // Forward declarations for template constraints

    /**
     * @brief Concept for integral types that can be byte-swapped
     */
        template <typename T>
        concept SwappableIntegral = std::integral<T>;

        /**
         * @brief Concept for enumeration types that can be byte-swapped
         */
        template <typename T>
        concept SwappableEnum = std::is_enum_v<T>;


        /**
         * @brief Performs byte-swapping on integral types
         *
         * This implementation is highly optimized using platform-specific intrinsics
         * where available. For single-byte types, no swapping is necessary.
         *
         * @tparam T The integral type to be swapped
         * @param value The value to byte-swap
         * @return The byte-swapped value
         */
        template <SwappableIntegral T>
        [[nodiscard]] constexpr T byteSwap(T value) noexcept {
            if constexpr (sizeof(T) == 1) {
                // 8-bit values don't need to be byte-swapped
                return value;
            }
            else if constexpr (sizeof(T) == 2) {
                // 16-bit byte swap
                return static_cast<T>(MZ_BSWAP16(static_cast<uint16_t>(value)));
            }
            else if constexpr (sizeof(T) == 4) {
                // 32-bit byte swap
                return static_cast<T>(MZ_BSWAP32(static_cast<uint32_t>(value)));
            }
            else if constexpr (sizeof(T) == 8) {
                // 64-bit byte swap
                return static_cast<T>(MZ_BSWAP64(static_cast<uint64_t>(value)));
            }
            else {
                // Shouldn't reach here, but handle any other sizes with a static_assert
                static_assert(sizeof(T) <= 8, "Byte swapping is only supported for types up to 8 bytes");
                return value;
            }
        }

        /**
         * @brief Performs byte-swapping on enumeration types
         *
         * Converts the enum to its underlying type, performs the byte swap,
         * and then converts back to the enum type.
         *
         * @tparam E The enumeration type to be swapped
         * @param value The enumeration value to byte-swap
         * @return The byte-swapped enumeration value
         */
        template <SwappableEnum E>
        [[nodiscard]] constexpr E byteSwap(E value) noexcept {
            using UnderlyingType = std::underlying_type_t<E>;
            if constexpr (sizeof(E) == 1) {
                // 8-bit values don't need to be byte-swapped
                return value;
            }
            else {
                // Convert to underlying type, swap bytes, convert back to enum
                return static_cast<E>(byteSwap(static_cast<UnderlyingType>(value)));
            }
        }

        // Legacy alias for backward compatibility
        template <typename T>
        [[nodiscard]] constexpr T swap_bytes(T value) noexcept {
            return byteSwap(value);
        }

    } // namespace endian
} // namespace mz

// Undefine the macros to avoid polluting the global namespace
#undef MZ_BSWAP16
#undef MZ_BSWAP32
#undef MZ_BSWAP64


#endif // MZ_ENDIAN_CONVERSIONS_HEADER_FILE
