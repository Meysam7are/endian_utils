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

#ifndef MZ_ENDIAN_CONCEPTS_HEADER_FILE
#define MZ_ENDIAN_CONCEPTS_HEADER_FILE
#pragma once

/**
 * @file EndianConcepts.h
 * @brief Defines concepts and utilities for endian-aware data handling
 *
 * This header provides concepts, type traits, and helper functions that facilitate
 * endian-aware data manipulation. It includes utilities for determining endianness
 * mismatches, converting between endian formats, and performing endian-aware memory
 * operations with automatic byte swapping when needed.
 *
 * The library provides both C++20 concepts and C++14 compatible SFINAE alternatives
 * for maximum compatibility across different compiler versions.
 *
 * @author Meysam Zare
 * @date 2024-10-14
 */

#include <cstring>
#include <type_traits>
#include <string>
#include <concepts>
#include <bit>
#include <span>


// Include our endian conversion utilities
#include "EndianConversions.h"



namespace mz {
    namespace endian {

        /**
         * @namespace mz::endian
         * @brief Contains utilities for endianness conversion and handling
         */

         //-----------------------------------------------------------------------------
         // Endianness constants and detection
         //-----------------------------------------------------------------------------

         /**
          * @brief The endianness used for serialized data streams
          *
          * This defines the target endianness for serialization. All data will be
          * converted to this endianness when serializing, and from this endianness
          * when deserializing.
          */
        static constexpr auto stream_endian{ std::endian::little };

        /**
         * @brief The native endianness of the current platform
         *
         * Automatically detected at compile time using standard library facilities
         * or compiler-specific macros.
         */
        static constexpr auto native_endian{ std::endian::native };

        /**
         * @brief Flag indicating if the native endianness differs from stream endianness
         *
         * When true, byte swapping will be necessary for endian-sensitive operations.
         * This check is performed at compile time for maximum efficiency.
         */
        static constexpr bool endian_mismatch{ native_endian != stream_endian };

        //-----------------------------------------------------------------------------
        // Type traits and concepts
        //-----------------------------------------------------------------------------


    /**
     * @brief Concept for types that are trivially copyable
     *
     * These types can be safely copied byte-by-byte without special handling.
     */
        template <typename T>
        concept TrivialType = std::is_trivially_copyable_v<T>;

        /**
         * @brief Concept for non-const trivially copyable types
         *
         * These types can be safely modified after copying.
         */
        template <typename T>
        concept TrivialTypeNonConst = std::is_trivially_copyable_v<T> && !std::is_const_v<T>;

        /**
         * @brief Concept for integral types with unique object representations
         *
         * These types can be safely serialized and deserialized as raw bytes.
         */
        template <typename T>
        concept IntType = std::is_integral_v<T> && std::has_unique_object_representations_v<T>;

        /**
         * @brief Concept for enumeration types with unique object representations
         */
        template <typename T>
        concept EnumType = std::is_enum_v<T> && std::has_unique_object_representations_v<T>;

        /**
         * @brief Concept for enumeration types with standardized none/invalid values
         *
         * These enums have special values for representing null or invalid states,
         * which can be useful for validation and error checking.
         */
        template <typename T>
        concept SafeEnumType = std::is_enum_v<T> &&
            std::has_unique_object_representations_v<T> &&
            requires(T e) {
            e = T::none;
            e = T::invalid;
        };

        /**
         * @brief Concept for types that can undergo byte swapping
         *
         * These types are either integral or enum types with unique object representations,
         * which means they can be safely byte-swapped for endianness conversion.
         */
        template <typename T>
        concept SwapType = std::has_unique_object_representations_v<T> &&
            (std::is_enum_v<T> || std::is_integral_v<T>);

        /**
         * @brief Concept for non-const types that can undergo byte swapping
         */
        template <typename T>
        concept SwapTypeNonConst = (!std::is_const_v<T>) &&
            std::has_unique_object_representations_v<T> &&
            (std::is_enum_v<T> || std::is_integral_v<T>);

        //-----------------------------------------------------------------------------
        // Endian conversion functions
        //-----------------------------------------------------------------------------

        /**
         * @brief Converts a value to the stream endianness if necessary
         *
         * Automatically detects endianness mismatches and performs byte swapping
         * only when needed, making it efficient for both little and big-endian platforms.
         *
         * @tparam T A type that can be byte-swapped (integral or enum)
         * @param value The value to convert
         * @return The value converted to stream endianness
         */
        template <SwapType T>
        [[nodiscard]] inline T toStreamEndian(T value) noexcept {
            if constexpr (endian_mismatch) {
                value = byteSwap(value); // Use new standardized name
            }
            return value;
        }

        /**
         * @brief Converts a value to little-endian format if necessary
         *
         * @tparam T A type that can be byte-swapped (integral or enum)
         * @param value The value to convert
         * @return The value converted to little-endian format
         */
        template <SwapType T>
        [[nodiscard]] inline T toLittleEndian(T value) noexcept {
            if constexpr (native_endian != std::endian::little) {
                value = byteSwap(value);
            }
            return value;
        }

        /**
         * @brief Converts a value to big-endian (network) format if necessary
         *
         * @tparam T A type that can be byte-swapped (integral or enum)
         * @param value The value to convert
         * @return The value converted to big-endian format
         */
        template <SwapType T>
        [[nodiscard]] inline T toBigEndian(T value) noexcept {
            if constexpr (native_endian != std::endian::big) {
                value = byteSwap(value);
            }
            return value;
        }

        // Legacy aliases for backward compatibility
        template <SwapType T>
        [[nodiscard]] inline T as_endian(T value) noexcept {
            return toStreamEndian(value);
        }

        template <SwapType T>
        [[nodiscard]] inline T as_little(T value) noexcept {
            return toLittleEndian(value);
        }

        template <SwapType T>
        [[nodiscard]] inline T as_big(T value) noexcept {
            return toBigEndian(value);
        }

        /**
         * @brief Deleted overload to prevent accidental use with unsupported types
         */
        template <typename T>
        void copy(void* dst, T src) noexcept = delete;

        //-----------------------------------------------------------------------------
        // Enum validation utilities
        //-----------------------------------------------------------------------------

        /**
         * @brief Validates if an enum value is within the valid range
         *
         * For safe enums (with none and invalid markers), checks if the value
         * falls within the valid range (greater than none, less than invalid).
         *
         * @tparam S An enumeration type with none and invalid values
         * @param value The enum value to validate
         * @return true if the value is valid, false otherwise
         */
        template <SafeEnumType S>
        [[nodiscard]] constexpr bool isValid(S value) noexcept {
            using UnderlyingType = std::underlying_type_t<S>;
            return static_cast<UnderlyingType>(value) > static_cast<UnderlyingType>(S::none) &&
                static_cast<UnderlyingType>(value) < static_cast<UnderlyingType>(S::invalid);
        }

        /**
         * @brief Checks if an enum value is invalid
         *
         * @tparam S An enumeration type with none and invalid values
         * @param value The enum value to check
         * @return true if the value is invalid, false if valid
         */
        template <SafeEnumType S>
        [[nodiscard]] constexpr bool isInvalid(S value) noexcept {
            return !isValid(value);
        }

        //-----------------------------------------------------------------------------
        // Endian-aware memory operations (basic copy variants with explicit endianness)
        //-----------------------------------------------------------------------------

        /**
         * @brief Copies a value to memory with specific endianness handling
         *
         * @tparam Encoding Target endianness for the copy operation
         * @tparam T A type that can be byte-swapped
         * @param destination Destination memory address
         * @param value Value to copy
         */
        template <std::endian Encoding, SwapType T>
        inline void basicCopy(void* destination, T value) noexcept {
            // Copy the value first
            std::memcpy(destination, &value, sizeof(T));

            // Swap bytes if necessary based on specified encoding
            if constexpr (native_endian != Encoding) {
                T* typedPtr = static_cast<T*>(destination);
                typedPtr[0] = byteSwap(typedPtr[0]);
            }
        }

        /**
         * @brief Copies a span of values to memory with specific endianness handling
         *
         * Optimized to avoid byte swapping for single-byte types.
         *
         * @tparam Encoding Target endianness for the copy operation
         * @tparam T A type that can be byte-swapped
         * @tparam N Size of the span (can be dynamic_extent)
         * @param destination Destination memory address
         * @param source Span of source values
         */
        template <std::endian Encoding, SwapType T, size_t N>
        inline void basicCopy(void* destination, const std::span<T, N>& source) noexcept {
            using PtrType = typename std::remove_const<typename std::remove_const<T>::type*>::type;

            // Copy the entire span in one efficient operation
            std::memcpy(destination, source.data(), sizeof(T) * source.size());

            // Swap bytes if necessary based on specified encoding
            // Skip for single-byte types as byte swapping isn't needed
            if constexpr (native_endian != Encoding && sizeof(T) > 1) {
                PtrType typedPtr = static_cast<PtrType>(destination);
                for (size_t i = 0; i < source.size(); ++i) {
                    typedPtr[i] = byteSwap(typedPtr[i]);
                }
            }
        }

        /**
         * @brief Copies memory to a value with specific endianness handling
         *
         * @tparam Encoding Source endianness for the copy operation
         * @tparam T A non-const type that can be byte-swapped
         * @param destination Reference to the destination value
         * @param source Source memory address
         */
        template <std::endian Encoding, SwapTypeNonConst T>
        inline void basicCopy(T& destination, const void* source) noexcept {
            // Copy the value first
            std::memcpy(&destination, source, sizeof(T));

            // Swap bytes if necessary based on specified encoding
            if constexpr (native_endian != Encoding) {
                destination = byteSwap(destination);
            }
        }

        /**
         * @brief Copies memory to a span of values with specific endianness handling
         *
         * Optimized to avoid byte swapping for single-byte types.
         *
         * @tparam Encoding Source endianness for the copy operation
         * @tparam T A non-const type that can be byte-swapped
         * @tparam N Size of the span (can be dynamic_extent)
         * @param destination Span of destination values
         * @param source Source memory address
         */
        template <std::endian Encoding, SwapTypeNonConst T, size_t N>
        inline void basicCopy(std::span<T, N> destination, const void* source) noexcept {
            // Copy the entire span in one efficient operation
            std::memcpy(destination.data(), source, sizeof(T) * destination.size());

            // Swap bytes if necessary based on specified encoding
            // Skip for single-byte types as byte swapping isn't needed
            if constexpr (native_endian != Encoding && sizeof(T) > 1) {
                for (auto& item : destination) {
                    item = byteSwap(item);
                }
            }
        }

        //-----------------------------------------------------------------------------
        // Endian-aware memory operations (default variants using stream_endian)
        //-----------------------------------------------------------------------------

        /**
         * @brief Copies a value to memory with stream endianness handling
         *
         * @tparam T A type that can be byte-swapped
         * @param destination Destination memory address
         * @param value Value to copy
         */
        template <SwapType T>
        inline void copy(void* destination, T value) noexcept {
            // Copy the value first
            std::memcpy(destination, &value, sizeof(T));

            // Swap bytes if necessary based on stream endianness
            if constexpr (endian_mismatch) {
                T* typedPtr = static_cast<T*>(destination);
                typedPtr[0] = byteSwap(typedPtr[0]);
            }
        }

        /**
         * @brief Copies a span of values to memory with stream endianness handling
         *
         * @tparam T A type that can be byte-swapped
         * @tparam N Size of the span (can be dynamic_extent)
         * @param destination Destination memory address
         * @param source Span of source values
         */
        template <SwapType T, size_t N>
        inline void copy(void* destination, const std::span<T, N>& source) noexcept {
            using PtrType = typename std::remove_const<typename std::remove_const<T>::type*>::type;

            // Copy the entire span in one efficient operation
            std::memcpy(destination, source.data(), sizeof(T) * source.size());

            // Swap bytes if necessary based on stream endianness
            if constexpr (endian_mismatch && sizeof(T) > 1) {
                PtrType typedPtr = static_cast<PtrType>(destination);
                for (size_t i = 0; i < source.size(); ++i) {
                    typedPtr[i] = byteSwap(typedPtr[i]);
                }
            }
        }

        /**
         * @brief Copies memory to a value with stream endianness handling
         *
         * @tparam T A non-const type that can be byte-swapped
         * @param destination Reference to the destination value
         * @param source Source memory address
         */
        template <SwapTypeNonConst T>
        inline void copy(T& destination, const void* source) noexcept {
            // Copy the value first
            std::memcpy(&destination, source, sizeof(T));

            // Swap bytes if necessary based on stream endianness
            if constexpr (endian_mismatch) {
                destination = byteSwap(destination);
            }
        }

        /**
         * @brief Copies memory to a span of values with stream endianness handling
         *
         * @tparam T A non-const type that can be byte-swapped
         * @tparam N Size of the span (can be dynamic_extent)
         * @param destination Span of destination values
         * @param source Source memory address
         */
        template <SwapTypeNonConst T, size_t N>
        inline void copy(std::span<T, N> destination, const void* source) noexcept {
            // Copy the entire span in one efficient operation
            std::memcpy(destination.data(), source, sizeof(T) * destination.size());

            // Swap bytes if necessary based on stream endianness
            if constexpr (endian_mismatch && sizeof(T) > 1) {
                for (auto& item : destination) {
                    item = byteSwap(item);
                }
            }
        }

        //-----------------------------------------------------------------------------
        // Advanced type concepts and features
        //-----------------------------------------------------------------------------

        /**
         * @brief Concept for types that can be automatically serialized and deserialized
         *
         * These types support the necessary copy operations in both directions,
         * making them suitable for automatic endian-aware serialization.
         */

        template <typename T>
        concept Streamable = requires(void* ptr, T & value, const T & constValue) {
            copy(value, ptr);
            copy(ptr, constValue);
        };


    } // namespace endian
} // namespace mz



#endif // MZ_ENDIAN_CONCEPTS_HEADER_FILE
