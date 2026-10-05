#pragma once
#include <Arduino.h>

/// @brief Darken TFT color
inline uint16_t darken(uint16_t color, uint8_t amount) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;

    r = r * (255 - amount) / 255;
    g = g * (255 - amount) / 255;
    b = b * (255 - amount) / 255;

    return (r << 11) | (g << 5) | b;
}

/// @brief Formats duration
inline std::string format_duration(uint32_t total_seconds) {
    uint32_t days = total_seconds / (3600 * 24);
    uint32_t hours = (total_seconds % (3600 * 24)) / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds = total_seconds % 60;

    if (days > 0)
        return std::to_string(days) + "d " + std::to_string(hours) + "h " +
               std::to_string(minutes) + "m " + std::to_string(seconds) + "s";

    if (hours > 0)
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m " +
               std::to_string(seconds) + "s";

    if (minutes > 0)
        return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";

    return std::to_string(seconds) + "s";
}

inline TouchPointType getTouch(FT6336U& touch) {
    FT6336U_TouchPointType tp = touch.scan();
    TouchPointType t = tp.tp[0];
    t.x = tp.tp[0].y;
    t.y = TFT_WIDTH - tp.tp[0].x;

    return t;
}