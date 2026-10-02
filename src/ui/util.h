#pragma once
#include <Arduino.h>

/// @brief Darken TFT color
uint16_t darken(uint16_t color, uint8_t amount) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;

    r = r * (255 - amount) / 255;
    g = g * (255 - amount) / 255;
    b = b * (255 - amount) / 255;

    return (r << 11) | (g << 5) | b;
}
