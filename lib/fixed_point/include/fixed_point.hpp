/**
 * @file fixed_point.h
 * @brief High Performace Fixed-point (Q15.16 format) arithmetic library.
 * Uses 1 sign bit, 15 integer bits, and 16 fractional bits.
 * Provides arithmetic and bitwise operations, float casting/conversion
 * C++17 compliant
 * 
 * @author Alper Alpcan
 * @date 2025-11-28
 */
   
#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H
   
#include <cstdint>
#include <compare>

/**
 * @class Fixed15_16
 * @brief A class representing fixed-point numbers in Q15.16 format.
 * Uses 1 sign bit, 15 integer bits, and 16 fractional bits. 
 * Resolution: ~0.0000152 (1/65536)
 * Range:      [-32768.0, +32767.99998]
 */
class Fixed15_16 {

    private:
        int32_t value_; 
        static constexpr uint8_t FRAC_SHIFT = 16;

        
        
        /// @brief tag dispatcher for compiler
        struct RawTag {}; 

        /// @brief Construct from raw (pre scaled) value
        constexpr explicit Fixed15_16(const int32_t raw, RawTag) noexcept : value_(raw) {} 

        /**
         * @brief Convert a float to fixed-point
         * @note 0.5 rounding implemented
         */
        static constexpr int32_t roundToFixed(float x) noexcept {
            float scaled = x * ONE;
            if (scaled >= 0.0) {
                return static_cast<int32_t>(scaled + 0.5);
            } else {
                return static_cast<int32_t>(scaled - 0.5);
            }
        }

    public:
        /// @brief representation of '1' in fixed-point
        static constexpr int32_t ONE = ((int32_t)1 << FRAC_SHIFT); 

        // =====================================================================
        // Constructors 
        // =====================================================================

        constexpr Fixed15_16() noexcept : value_(0) {};
        
        constexpr Fixed15_16(const Fixed15_16&) noexcept = default;
        constexpr Fixed15_16& operator=(const Fixed15_16&) noexcept = default;

        /**
         * @brief Float constructor
         * @note Not clamped on overflow
         */
        constexpr explicit Fixed15_16(const float dVal) noexcept : value_(static_cast<int32_t>(roundToFixed(dVal))) {} 
        
        /**
         * @brief Integer constructor
         * @note Not clamped on overflow, keep below INT16_MAX
         */
        constexpr explicit Fixed15_16(int iVal) noexcept : value_(static_cast<int32_t>(iVal) << FRAC_SHIFT) {} 
        
        /// @brief Convert a int32_t raw bit pattern to Fixed15_16 representation
        [[nodiscard]] static constexpr Fixed15_16 fromRaw(const int32_t raw) noexcept {
            return Fixed15_16(raw, RawTag{});
        }

        // =============================================================
        // Conversion Methods 
        // =============================================================

        [[nodiscard]] constexpr int16_t toInt() const noexcept {
            return static_cast<int16_t>(value_ >> FRAC_SHIFT);
        }

        [[nodiscard]] constexpr int32_t toRaw() const noexcept {
            return value_;
        }

        /// @note floats may be slow on embedded hardware without FPU
        [[nodiscard]] constexpr float toFloat() const noexcept {
            return static_cast<float>(value_) / ONE;
        }

        // =============================================================
        // Operator Overloads (Comparisons)
        // =============================================================
        
        // fixed-fixed
        friend constexpr auto operator<=>(const Fixed15_16 lhs, const Fixed15_16 rhs) noexcept = default;
        friend constexpr bool operator==(const Fixed15_16 lhs, const Fixed15_16 rhs) noexcept = default;

        // fixed-int16_t
        friend constexpr auto operator<=>(const Fixed15_16 lhs, const int16_t rhs) noexcept {
            return lhs.value_ <=> (static_cast<int32_t>(rhs) << FRAC_SHIFT);
        }
        
        friend constexpr bool operator==(const Fixed15_16 lhs, const int16_t rhs) noexcept {
            return lhs.value_ == (static_cast<int32_t>(rhs) << FRAC_SHIFT);
        }

        // =============================================================
        // Operator Overloads (Compound Assignment)
        // =============================================================


        constexpr Fixed15_16& operator+=(const Fixed15_16 other) noexcept {
            value_ += other.value_;
            return *this;
        }

        constexpr Fixed15_16& operator-=(const Fixed15_16 other) noexcept {
            value_ -= other.value_;
            return *this;
        }
        
        constexpr Fixed15_16& operator*=(const Fixed15_16 other) noexcept {
            value_ = (static_cast<int64_t>(value_) * other.value_) >> FRAC_SHIFT;
            return *this;
        }

        /// @warning does not check for division by zero
        constexpr Fixed15_16& operator/=(const Fixed15_16 other) noexcept {
            value_ = ((static_cast<int64_t>(value_) << FRAC_SHIFT) / other.value_);
            return *this;
        }

        // =============================================================
        // Operator Overloads (Unary)
        // =============================================================

        constexpr Fixed15_16 operator-() const noexcept {
            return Fixed15_16::fromRaw(-value_);
        }

        // =============================================================
        // Operator Overloads (Bitwise Assignment)
        // =============================================================


        constexpr Fixed15_16& operator<<=(const uint8_t shift) noexcept {
            value_ <<= shift;
            return *this;
        }

        constexpr Fixed15_16& operator>>=(const uint8_t shift) noexcept {
            value_ >>= shift;
            return *this;
        }

        constexpr Fixed15_16& operator&=(const int32_t mask) noexcept {
            value_ &= mask;
            return *this;
        }

        constexpr Fixed15_16& operator|=(const int32_t mask) noexcept {
            value_ |= mask;
            return *this;
        }

        constexpr Fixed15_16& operator^=(const int32_t mask) noexcept {
            value_ ^= mask;
            return *this;
        }
};


// =============================================================
// Arithmetic Operators
// =============================================================

[[nodiscard]] constexpr Fixed15_16 operator+(const Fixed15_16 a, const Fixed15_16 b) noexcept { return Fixed15_16(a) += b; }
[[nodiscard]] constexpr Fixed15_16 operator-(const Fixed15_16 a, const Fixed15_16 b) noexcept { return Fixed15_16(a) -= b; }
[[nodiscard]] constexpr Fixed15_16 operator*(const Fixed15_16 a, const Fixed15_16 b) noexcept { return Fixed15_16(a) *= b; }
[[nodiscard]] constexpr Fixed15_16 operator/(const Fixed15_16 a, const Fixed15_16 b) noexcept { return Fixed15_16(a) /= b; }

// =============================================================
// Mixed-Arithmetic Operators (Fixed15_16 with int16_t)
// =============================================================


[[nodiscard]] constexpr Fixed15_16 operator+(const Fixed15_16 a, const int16_t b) noexcept { return a + Fixed15_16(b); }
[[nodiscard]] constexpr Fixed15_16 operator+(const int16_t a, const Fixed15_16 b) noexcept { return Fixed15_16(a) + b; }
[[nodiscard]] constexpr Fixed15_16 operator-(const Fixed15_16 a, const int16_t b) noexcept { return a - Fixed15_16(b); }
[[nodiscard]] constexpr Fixed15_16 operator-(const int16_t a, const Fixed15_16 b) noexcept { return Fixed15_16(a) - b; }
[[nodiscard]] constexpr Fixed15_16 operator/(const Fixed15_16 a, const int16_t b) noexcept { return a / Fixed15_16(b); }
[[nodiscard]] constexpr Fixed15_16 operator/(const int16_t a, const Fixed15_16 b) noexcept { return Fixed15_16(a) / b; }
[[nodiscard]] constexpr Fixed15_16 operator*(const Fixed15_16 a, const int16_t b) noexcept { return Fixed15_16::fromRaw(a.toRaw() * b); } // minor optimisation for multiplication, we can skip the casting of b into a Fixed Point, saving cycles of multiplication&division by Fixed15_16 ONE as it cancels out
[[nodiscard]] constexpr Fixed15_16 operator*(const int16_t a, const Fixed15_16 b) noexcept { return Fixed15_16::fromRaw(a * b.toRaw()); } // potential to overflow if a or b are large enough

// =============================================================
// Bitwise Operators
// =============================================================


[[nodiscard]] constexpr Fixed15_16 operator<<(const Fixed15_16 a, const uint8_t shift) noexcept { return Fixed15_16::fromRaw(a.toRaw() << shift); }
[[nodiscard]] constexpr Fixed15_16 operator>>(const Fixed15_16 a, const uint8_t shift) noexcept { return Fixed15_16::fromRaw(a.toRaw() >> shift); }
[[nodiscard]] constexpr Fixed15_16 operator&(const Fixed15_16 a, const int32_t mask) noexcept { return Fixed15_16::fromRaw(a.toRaw() & mask); } // other is int32_t, cast fixed to raw to mask as fp-fp
[[nodiscard]] constexpr Fixed15_16 operator^(const Fixed15_16 a, const int32_t mask) noexcept { return Fixed15_16::fromRaw(a.toRaw() ^ mask); }
[[nodiscard]] constexpr Fixed15_16 operator|(const Fixed15_16 a, const int32_t mask) noexcept { return Fixed15_16::fromRaw(a.toRaw() | mask); }

// =============================================================
// UDLs for Fixed15_16 literals
// =============================================================

/// @note val must be below INT16_MAX to avoid silent wrapping/overflow
[[nodiscard]] consteval Fixed15_16 operator"" _fp(unsigned long long val) noexcept {
    return Fixed15_16(static_cast<int16_t>(val));
}

[[nodiscard]] consteval Fixed15_16 operator"" _fp(long double val) noexcept {
    return Fixed15_16(static_cast<float>(val));
}
 
#endif // FIXEDPOINT_H