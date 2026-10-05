#pragma once

#include "ui/util.h"
#include "widget.h"

class Hr : public Widget {
private:
    int w;
    int thickness;

    uint32_t bg_color;
    uint32_t parent_color;

public:
    Hr(TFT_eSPI& tft, int x, int y, int w, int thickness, uint32_t bg_color, uint32_t parent_color)
        : bg_color(bg_color), parent_color(parent_color), w(w), thickness(thickness),
          Widget(tft, x, y) {}

    void draw() override {
        tft.fillSmoothRoundRect(getX(), getY(), w, thickness, thickness / 2, bg_color,
                                parent_color);

        this->dirty = false;
    };

    void update(TouchPointType& touch) override {
        redraw();
    };
};