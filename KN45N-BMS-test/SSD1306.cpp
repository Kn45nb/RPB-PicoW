#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

#define SSD1306_I2C_ADDRESS 0x3C
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

class SSD1306 {
public:
    SSD1306(i2c_inst_t* i2c_instance, uint8_t i2c_address = SSD1306_I2C_ADDRESS);

    void begin();
    void clearDisplay();
    void display();
    void drawPixel(uint8_t x, uint8_t y, bool color);
    void drawChar(uint8_t x, uint8_t y, char c, bool color);
    void printText(uint8_t x, uint8_t y, const char* text, bool color);

private:
    i2c_inst_t* i2c_instance;
    uint8_t i2c_address;
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

    void sendCommand(uint8_t command);
    void sendData(uint8_t data);
    void initialize();
};

#endif // SSD1306_H
