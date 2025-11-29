#include "st7735.hpp"


namespace {
    constexpr uint8_t SWRESET    = 0x01;
    constexpr uint8_t RDDID      = 0x04;
    constexpr uint8_t RDDST      = 0x09;
    constexpr uint8_t SLPIN      = 0x10;
    constexpr uint8_t SLPOUT     = 0x11;
    constexpr uint8_t PTLON      = 0x12;
    constexpr uint8_t NORON      = 0x13;
    constexpr uint8_t INVOFF     = 0x20;
    constexpr uint8_t INVON      = 0x21;
    constexpr uint8_t DISPOFF    = 0x28;
    constexpr uint8_t DISPON     = 0x29;
    constexpr uint8_t CASET      = 0x2A;
    constexpr uint8_t RASET      = 0x2B;
    constexpr uint8_t RAMWR      = 0x2C;
    constexpr uint8_t RAMRD      = 0x2E;
    constexpr uint8_t PTLAR      = 0x30;
    constexpr uint8_t VSCRDEF    = 0x33;
    constexpr uint8_t COLMOD     = 0x3A;
    constexpr uint8_t MADCTL     = 0x36;
    constexpr uint8_t MADCTL_MY  = 0x80;
    constexpr uint8_t MADCTL_MX  = 0x40;
    constexpr uint8_t MADCTL_MV  = 0x20;
    constexpr uint8_t MADCTL_ML  = 0x10;
    constexpr uint8_t MADCTL_RGB = 0x00;
    constexpr uint8_t VSCRSADD   = 0x37;
    constexpr uint8_t FRMCTR1    = 0xB1;
    constexpr uint8_t FRMCTR2    = 0xB2;
    constexpr uint8_t FRMCTR3    = 0xB3;
    constexpr uint8_t INVCTR     = 0xB4;
    constexpr uint8_t DISSET5    = 0xB6;
    constexpr uint8_t PWCTR1     = 0xC0;
    constexpr uint8_t PWCTR2     = 0xC1;
    constexpr uint8_t PWCTR3     = 0xC2;
    constexpr uint8_t PWCTR4     = 0xC3;
    constexpr uint8_t PWCTR5     = 0xC4;
    constexpr uint8_t VMCTR1     = 0xC5;
    constexpr uint8_t RDID1      = 0xDA;
    constexpr uint8_t RDID2      = 0xDB;
    constexpr uint8_t RDID3      = 0xDC;
    constexpr uint8_t RDID4      = 0xDD;
    constexpr uint8_t PWCTR6     = 0xFC;
    constexpr uint8_t GMCTRP1    = 0xE0;
    constexpr uint8_t GMCTRN1    = 0xE1;
}

void ST7735::Bcmd() {
    // Initialization commands for GENERIC_TAB
    writeCommand(SWRESET);
    sleep_ms(50);
    writeCommand(SLPOUT);
    sleep_ms(250);
    sleep_ms(250);
    writeCommand(COLMOD);
    writeByte(0x05);
    sleep_ms(10);
    writeCommand(FRMCTR1);
    writeByte(0x00);
    writeByte(0x06);
    writeByte(0x03);
    sleep_ms(10);
    writeCommand(MADCTL);
    writeByte(0x08);
    writeCommand(DISSET5);
    writeByte(0x15);
    writeByte(0x02);
    writeCommand(INVCTR);
    writeByte(0x00);
    writeCommand(PWCTR1);
    writeByte(0x02);
    writeByte(0x70);
    sleep_ms(10);
    writeCommand(PWCTR2);
    writeByte(0x05);
    writeCommand(PWCTR3);
    writeByte(0x01);
    writeByte(0x02);
    writeCommand(VMCTR1);
    writeByte(0x3C);
    writeByte(0x38);
    sleep_ms(10);
    writeCommand(PWCTR6);
    writeByte(0x11);
    writeByte(0x15);
    writeCommand(GMCTRP1);
    writeByte(0x09); writeByte(0x16); writeByte(0x09); writeByte(0x20);
    writeByte(0x21); writeByte(0x1B); writeByte(0x13); writeByte(0x19);
    writeByte(0x17); writeByte(0x15); writeByte(0x1E); writeByte(0x2B);
    writeByte(0x04); writeByte(0x05); writeByte(0x02); writeByte(0x0E);
    writeCommand(GMCTRN1);
    writeByte(0x0B); writeByte(0x14); writeByte(0x08); writeByte(0x1E);
    writeByte(0x22); writeByte(0x1D); writeByte(0x18); writeByte(0x1E);
    writeByte(0x1B); writeByte(0x1A); writeByte(0x24); writeByte(0x2B);
    writeByte(0x06); writeByte(0x06); writeByte(0x02); writeByte(0x0F);
    sleep_ms(10);
    writeCommand(CASET);
    writeByte(0x00); writeByte(0x02); writeByte(0x08); writeByte(0x81);
    writeCommand(RASET);
    writeByte(0x00); writeByte(0x01); writeByte(0x08); writeByte(0xA0);
    writeCommand(NORON);
    sleep_ms(10);
    writeCommand(DISPON);
    sleep_ms(250);
    sleep_ms(250);
}

void ST7735::Rcmd1() {
    writeCommand(SWRESET);
    sleep_ms(150);
    writeCommand(SLPOUT);
    sleep_ms(250);
    sleep_ms(250);
    writeCommand(FRMCTR1);
    writeByte(0x01);
    writeByte(0x2C);
    writeByte(0x2D);
    writeCommand(FRMCTR2);
    writeByte(0x01);
    writeByte(0x2C);
    writeByte(0x2D);
    writeCommand(FRMCTR3);
    writeByte(0x01); writeByte(0x2C); writeByte(0x2D);
    writeByte(0x01); writeByte(0x2C); writeByte(0x2D);
    writeCommand(INVCTR);
    writeByte(0x07);
    writeCommand(PWCTR1);
    writeByte(0xA2);
    writeByte(0x02);
    writeByte(0x84);
    writeCommand(PWCTR2);
    writeByte(0xC5);
    writeCommand(PWCTR3);
    writeByte(0x0A);
    writeByte(0x00);
    writeCommand(PWCTR4);
    writeByte(0x8A);
    writeByte(0x2A);
    writeCommand(PWCTR5);
    writeByte(0x8A);
    writeByte(0xEE);
    writeCommand(VMCTR1);
    writeByte(0x0E);
    writeCommand(INVOFF);
    writeCommand(MADCTL);
    writeByte(0xC8);
    writeCommand(COLMOD);
    writeByte(0x05); 
}

void ST7735::Rcmd2red() {
    writeCommand(CASET);
    writeByte(0x00); writeByte(0x00);
    writeByte(0x00); writeByte(0x7F);
    writeCommand(RASET);
    writeByte(0x00); writeByte(0x00);
    writeByte(0x00); writeByte(0x9F);
}

void ST7735::Rcmd2green() {
    writeCommand(CASET);
    writeByte(0x00); writeByte(0x02);
    writeByte(0x00); writeByte(0x7F + 0x02);
    writeCommand(RASET);
    writeByte(0x00); writeByte(0x01);
    writeByte(0x00); writeByte(0x9F + 0x01);
}

void ST7735::Rcmd3() {
    writeCommand(GMCTRP1);
    writeByte(0x02); writeByte(0x1C); writeByte(0x07); writeByte(0x12);
    writeByte(0x37); writeByte(0x32); writeByte(0x29); writeByte(0x2D);
    writeByte(0x29); writeByte(0x25); writeByte(0x2B); writeByte(0x39);
    writeByte(0x00); writeByte(0x01); writeByte(0x03); writeByte(0x10);
    writeCommand(GMCTRN1);
    writeByte(0x03); writeByte(0x1D); writeByte(0x07); writeByte(0x06);
    writeByte(0x2E); writeByte(0x2C); writeByte(0x29); writeByte(0x2D);
    writeByte(0x2E); writeByte(0x2E); writeByte(0x37); writeByte(0x3F);
    writeByte(0x00); writeByte(0x00); writeByte(0x02); writeByte(0x10);
    writeCommand(NORON);
    sleep_ms(10);
    writeCommand(DISPON);
    sleep_ms(100);
}

/**
 * @brief Write a command to the display
 * @note MUST be called between select() and deselect()
 */
void ST7735::writeCommand(uint8_t cmd) {
    gpio_put(dc_pin_, 0); // Command mode
    spi_write_blocking(spi_, &cmd, 1);
}

/**
 * @brief Write a byte to the display
 * @note MUST be called between select() and deselect()
 */
void ST7735::writeByte(uint8_t data) {
    gpio_put(dc_pin_, 1); // Data mode
    spi_write_blocking(spi_, &data, 1);
}

/**
 * @brief Write a 16-bit word to the display
 * Sends in big-endian format for compatibility with ST7735
 * @note MUST be called between select() and deselect()
 */
void ST7735::writeWord(uint16_t data) {
    gpio_put(dc_pin_, 1); // Data mode
    uint8_t bytes[2];
    bytes[0] = static_cast<uint8_t>(data >> 8);
    bytes[1] = static_cast<uint8_t>(data & 0xFF);
    spi_write_blocking(spi_, bytes, 2);
}

void ST7735::writeByteBuffer(const uint8_t* buffer, size_t length) {
    gpio_put(dc_pin_, 1); // Data mode
    spi_write_blocking(spi_, buffer, length);
}

/**
 * @brief rectangle for blitting pixels
 * @param x0 Top left x coordinate
 * @param y0 Top left y coordinate
 * @param x1 Bottom right x coordinate
 * @param y1 Bottom right y coordinate
 * @note MUST be called between select() and deselect()
 */
void ST7735::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    writeCommand(CASET);
    writeByte(0);
    writeByte(x0 + x_start_);
    writeByte(0);
    writeByte(x1 + x_start_);
    writeCommand(RASET);
    writeByte(0);
    writeByte(y0 + y_start_);
    writeByte(0);
    writeByte(y1 + y_start_);
    writeCommand(RAMWR); // Write to RAM    
}

/**
 * @brief Set the rotation of the display
 * @param m Rotation value (0-3)
 * map: 
 * 0 - 0 degrees
 * 1 - 90 degrees (vertical)
 * 2 - 180 degrees (horizontal flipped)
 * 3 - 270 degrees (vertical flipped)
 */
void ST7735::setRotation(uint8_t m) {
    // m can be 0-3
    uint8_t madctl = 0;

    rotation_ = m % 4;

    switch (rotation_) {
    case 0:
        madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB;
        tft_height_ = 160;
        tft_width_ = 128;
        x_start_ = col_start_;
        y_start_ = row_start_;
        break;
    case 1:
        madctl = MADCTL_MY | MADCTL_MV | MADCTL_RGB;
        tft_width_ = 160;
        tft_height_ = 128;
        y_start_ = col_start_;
        x_start_ = row_start_;
        break;
    case 2:
        madctl = MADCTL_RGB;
        tft_height_ = 160;
        tft_width_ = 128;
        x_start_ = col_start_;
        y_start_ = row_start_;
        break;
    case 3:
        madctl = MADCTL_MX | MADCTL_MV | MADCTL_RGB;
        tft_width_ = 160;
        tft_height_ = 128;
        y_start_ = col_start_;
        x_start_ = row_start_;
        break;
    }
    select();
    writeCommand(MADCTL);
    writeByte(madctl);
    deselect();
}

void ST7735::normalDisplay() {
    select();
    writeCommand(NORON);
    deselect();
}

void ST7735::invertDisplay(bool i) {
    select();
    if (i) {
        writeCommand(INVON);
    } else {
        writeCommand(INVOFF);
    }
    deselect();
}

/**
 * @brief Constructor for ST7735 class
 * @param rotation Initial rotation (0-3) - see setRotation for details
 * @param spi SPI instance
 * @param sck_pin SPI clock pin
 * @param mosi_pin SPI MOSI (TX) pin
 * @param cs_pin Chip select pin
 * @param dc_pin Data/command pin
 * @param rst_pin Reset pin
 * @param bl_pin Backlight pin
 */
ST7735::ST7735(uint8_t rotation, spi_inst_t* spi, uint8_t sck_pin, uint8_t mosi_pin, uint8_t cs_pin, uint8_t dc_pin, uint8_t rst_pin, uint8_t bl_pin) {
    spi_ = spi;
    sck_pin_ = sck_pin;
    mosi_pin_ = mosi_pin;
    cs_pin_ = cs_pin;
    dc_pin_ = dc_pin;
    rst_pin_ = rst_pin;
    bl_pin_ = bl_pin;
    rotation_ = rotation;
} 

/**
 * @brief Initialize the display
 * @param type TFT type (tab color)
 */
void ST7735::initialize(TFT_Type type) {
    spi_init(spi_, 50 * 1000 * 1000); // 50 MHz
    
    gpio_set_function(sck_pin_,   GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin_,   GPIO_FUNC_SPI);
    

    gpio_init(cs_pin_);
    gpio_set_dir(cs_pin_, GPIO_OUT);
    deselect();

    gpio_init(dc_pin_);
    gpio_set_dir(dc_pin_, GPIO_OUT);
    gpio_put(dc_pin_, 1);

    gpio_init(rst_pin_);
    gpio_set_dir(rst_pin_, GPIO_OUT);

    gpio_init(bl_pin_);
    gpio_set_dir(bl_pin_, GPIO_OUT);
    gpio_put(bl_pin_, 1); // turn on backlight -- this can also just be left on 3.3v

    // Reset the display
    gpio_put(rst_pin_, 0);
    sleep_ms(10);
    gpio_put(rst_pin_, 1);
    sleep_ms(20);

    // Initialization sequence
    gpio_put(dc_pin_, 0); // Command mode
    select();
    switch (type) {
        case TFT_Type::GREEN_TAB:
            // Initialization for GREEN_TAB
            Rcmd1();
            Rcmd2green();
            Rcmd3();
            col_start_ = 2;
            row_start_ = 1;
            break;

        case TFT_Type::RED_TAB:
            // Initialization for RED_TAB
            Rcmd1();    
            Rcmd2red();
            Rcmd3();
            break;

        case TFT_Type::BLACK_TAB:
            // Initialization for BLACK_TAB    
            Rcmd1();
            Rcmd2red();
            Rcmd3();
            writeCommand(MADCTL);
            writeByte(0xC0);
            break;

        case TFT_Type::GENERIC_TAB:
            // Initialization for GENERIC_TAB
            Bcmd();
            break;
    }
    deselect();

    setRotation(rotation_);
}

/**
 * @brief Draw a pixel at the specified coordinates with the given color
 * @param x X coordinate
 * @param y Y coordinate
 * @param color 16-bit color value
 */
void ST7735::drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if ((x >= tft_width_) || (y >= tft_height_)) return;

    select();
    
    setAddrWindow(x, y, x+1, y+1);
    writeWord(color);
    
    deselect();
}

/**
 * @brief Push a block of pixels with the same color
 * @param color 16-bit color value
 * @param len Number of pixels to write
 * @note MUST be called between select() and deselect() and after setAddrWindow()
 */
void ST7735::pushBlock(uint16_t color, uint32_t len) {
    if (len == 0) return;

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    constexpr size_t BUFFER_SIZE = 64;
    constexpr size_t PIXELS_IN_BUFFER = BUFFER_SIZE / 2;
    uint8_t buffer[BUFFER_SIZE];

    for (size_t i = 0; i < BUFFER_SIZE; i += 2) {
        buffer[i] = hi;
        buffer[i + 1] = lo;
    }

    while (len > 0) {
        // calculate how much of the buffer to send
        uint32_t pixels_to_send = (len > PIXELS_IN_BUFFER) ? PIXELS_IN_BUFFER : len;
        spi_write_blocking(spi_, buffer, pixels_to_send * 2);
        len -= pixels_to_send;
    }

}

/**
 * @brief Fill the entire screen with the specified color
 * @param color 16-bit color value
 */
void ST7735::drawFillScreen(uint16_t color) {
    select();
    
    setAddrWindow(0, 0, tft_width_ - 1, tft_height_ - 1);
    
    gpio_put(dc_pin_, 1); // Data mode
    
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    constexpr size_t BUFFER_SIZE = 64;
    uint8_t buffer[BUFFER_SIZE];

    for (size_t i = 0; i < BUFFER_SIZE; i += 2) {
        buffer[i] = hi;
        buffer[i + 1] = lo;
    }

    uint32_t total_pixels = tft_width_ * tft_height_;
    uint32_t pixels_written = 0;

    // assume total pixels is divisible by (BUFFER_SIZE / 2)
    while (pixels_written < total_pixels) {
        spi_write_blocking(spi_, buffer, BUFFER_SIZE);
        pixels_written += BUFFER_SIZE / 2;
    }
    
    deselect();
}

/**
 * @brief Draw a fast vertical line at the specified coordinates with the given height and color
 * @param x X coordinate
 * @param y Y coordinate
 * @param height Height of the line
 * @param color 16-bit color value
 */
void ST7735::drawFastVLine(uint8_t x, uint8_t y, uint8_t height, uint16_t color) {
    if ((x >= tft_width_) || (y >= tft_height_)) return;
    if ((y + height - 1) >= tft_height_) {
        height = tft_height_ - y;
    }

    select();
    setAddrWindow(x, y, x, y + height - 1);
    gpio_put(dc_pin_, 1); // Data mode

    pushBlock(color, height);

    deselect();
}

void ST7735::drawRaySolidColumn(uint8_t x, uint8_t wallStart, uint8_t wallHeight, uint16_t color) {
    if (x >= tft_width_) return;

    uint32_t floorHeight = tft_height_ - wallStart - wallHeight;
    if (floorHeight > tft_height_) {
        floorHeight = 0;
    }

    select();
    setAddrWindow(x, 0, x, tft_height_ - 1);
    gpio_put(dc_pin_, 1); // Data mode

    pushBlock(0x0000, wallStart); // Draw ceiling (black)
    pushBlock(color, wallHeight); // Draw wall
    pushBlock(0x0000, floorHeight); // Draw floor (black)

    deselect();
}

void ST7735::drawRayColumnn(uint8_t x, const uint16_t* colors, size_t len) {
    if (x >= tft_width_) return;
    if (len == 0) return;
    
    select();
    setAddrWindow(x, 0, x, tft_height_ - 1);
    gpio_put(dc_pin_, 1); // Data mode

    // send the color array directly
    spi_write_blocking(spi_, reinterpret_cast<const uint8_t*>(colors), len * 2);

    deselect();
}