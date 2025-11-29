/**
 * @file st7735.hpp
 * @brief Driver for ST7735 TFT LCD display controller
 * C++20 compliant
 * @author Alper Alpcan
 * 
 * Primarily based on:
 * https://github.com/bablokb/pico-st7735/tree/main - Pico ST7735 driver by bablokb
 * 
 */

#ifndef ST7735_H
#define ST7735_H

#include <cstdint>

#include "pico/stdlib.h"
#include "hardware/spi.h"

/**
 * @class ST7735
 * @brief Driver for ST7735 TFT LCD display controller
 */
class ST7735 {
    private:
        spi_inst_t* spi_;
        uint8_t sck_pin_;
        uint8_t mosi_pin_;
        uint8_t cs_pin_;
        uint8_t dc_pin_;
        uint8_t rst_pin_;
        uint8_t bl_pin_;

        uint8_t tft_width_, tft_height_;

        uint8_t row_start_, col_start_, x_start_, y_start_;
        uint8_t rotation_;

        /// @brief Set the chip select low
        inline void select() { gpio_put(cs_pin_, 0); }

        /// @brief Set the chip select high 
        inline void deselect() { gpio_put(cs_pin_, 1); }

        void writeCommand(uint8_t cmd);
        void writeByte(uint8_t data);
        void writeWord(uint16_t data);
        void writeByteBuffer(const uint8_t* buffer, size_t length); 
        void writeWordBuffer(const uint16_t* buffer, size_t length); // TODO

        void pushBlock(uint16_t color, uint32_t len);

        void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

        // internal initialization commands
        
        void Bcmd();
        void Rcmd1();
        void Rcmd2red();
        void Rcmd2green();
        void Rcmd3();
    
    public:
        /// @brief TFT types (corresponding to tab colors on the protective film)
        enum class TFT_Type {
            GREEN_TAB,
            RED_TAB,
            BLACK_TAB,
            GENERIC_TAB
        };

        ST7735(uint8_t rotation, spi_inst_t* spi, uint8_t sck_pin, uint8_t mosi_pin, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t bl_pin);

        void initialize(TFT_Type type);

        void setRotation(uint8_t m);
        void invertDisplay(bool i);
        void normalDisplay();

        // =====================================================================
        // Drawing Functions (automatic write transaction management) 
        // =====================================================================

        void drawPixel(uint8_t x, uint8_t y, uint16_t color);
        void drawFillScreen(uint16_t color);
        void drawFastVLine(uint8_t x, uint8_t y, uint8_t height, uint16_t color);

        void drawRaySolidColumn(uint8_t x, uint8_t wallStart, uint8_t wallHeight, uint16_t color);
        void drawRayColumnn(uint8_t x, const uint16_t* colors, size_t len);
};

#endif
