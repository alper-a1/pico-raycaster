

#ifndef FP_MATH_H
#define FP_MATH_H

#include "fixed_point.hpp"

#include <array>
#include <cmath>
#include <numbers>


/**
 * @brief Compile-time namespace for sine calculation helpers
 */
namespace {

    consteval double cxprPow(double base, int exp) {
        double res = 1.0;
        for (int i = 0; i < exp; ++i) res *= base;
        return res;
    }


    consteval double cxprFactorial(int n) {
        double res = 1.0;
        for (int i = 2; i <= n; ++i) res *= i;
        return res;
    }

    /// @brief Calculate sine using Taylor series approximation
    consteval double cxprTaylorSin(double x) {
        double sum = x;
        double sign = -1.0;
        
        // 10 terms is massive overkill for Q15.16 precision, 
        // but since it's compile-time, it costs nothing.
        for (int i = 3; i < 20; i += 2) {
            sum += sign * (cxprPow(x, i) / cxprFactorial(i));
            sign *= -1.0;
        }
        return sum;
    }

    inline constexpr uint8_t SIN_LUT_SIZE = 91; // [0,90)

    /**
     * @brief Generate sine lookup table for angles 0 to 90 degrees
     * @return Array of Fixed15_16 sine values
     * @note compile-time evaluation
     */
    consteval std::array<Fixed15_16, SIN_LUT_SIZE> generateSinTable() {
        std::array<Fixed15_16, SIN_LUT_SIZE> table{};

        for (size_t d = 0; d <= 90; d++) {
            double rad = d * (std::numbers::pi / 180.0);

            table[d] = Fixed15_16(static_cast<float>(cxprTaylorSin(rad)));
        }

        return table;
    }

} // consteval namespace

static constexpr auto FP_SIN_TABLE = generateSinTable();

/**
 * @brief Lookup sine value from LUT
 * @param a Angle in degrees
 * @return Sine of angle in Fixed15_16 format
 */
[[nodiscard]] constexpr Fixed15_16 sinfp(int16_t a) {
    // Normalize angle to [0, 360)
    a %= 360;
    if (a < 0) a += 360;

    uint8_t index;
    bool negative = false;

    if (a >= 0 && a <= 90) {
        index = static_cast<uint8_t>(a);

    } else if (a > 90 && a <= 180) {
        index = static_cast<uint8_t>(180 - a);

    } else if (a > 180 && a <= 270) {
        index = static_cast<uint8_t>(a - 180);
        negative = true;
    } else { // a > 270 and <= 360
        index = static_cast<uint8_t>(360 - a);
        negative = true;
    }

    Fixed15_16 result = FP_SIN_TABLE[index];

    return negative ? -result : result;
}

[[nodiscard]] constexpr Fixed15_16 cosfp(int16_t a) {
    return sinfp(90 - a);
}

[[nodiscard]] constexpr Fixed15_16 sinfp(Fixed15_16 angleDeg) { return sinfp(angleDeg.toInt()); }
[[nodiscard]] constexpr Fixed15_16 cosfp(Fixed15_16 angleDeg) { return cosfp(angleDeg.toInt()); }

/// @brief Floor function for Fixed15_16
[[nodiscard]] constexpr Fixed15_16 floor(Fixed15_16 val) noexcept {
    return Fixed15_16::fromRaw(val.toRaw() & 0xFFFF0000);
}

/// @brief Fractional part extraction for Fixed15_16, preserved sign
[[nodiscard]] constexpr Fixed15_16 fractional(Fixed15_16 val) noexcept{
    return val - floor(val);
}

/**
 * @brief Absolute value for Fixed15_16
 * @note This is to prevent Fixed15_16::fromRaw(abs((value_).toRaw()));
 */
[[nodiscard]] constexpr Fixed15_16 abs(Fixed15_16 val) noexcept {
    if (val < 0) {
        return Fixed15_16::fromRaw(-val.toRaw());
    } else {
        return val;
    }
}


#endif // FP_MATH_H