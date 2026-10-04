#pragma once

#include "ui/util.h"
#include "widget.h"

#include <vector>

class Container : public ContainerAbstract {
private:
    uint32_t bg_color;
    uint32_t bgbg_color;

    int32_t r;

public:
    int w;
    int h;

    Container(TFT_eSPI& tft, int x, int y, int w, int h, int32_t r, uint32_t bg_color,
              uint32_t bggb_color)
        : w(w), h(h), bg_color(bg_color), bgbg_color(bgbg_color), r(r),
          ContainerAbstract(tft, x, y) {}

    void draw() override {
        tft.fillSmoothRoundRect(getX(), getY(), w, h, r, bg_color, bgbg_color);
    };

    void update(TouchPointType& touch) override {
        if (isDirty()) {
            draw();
        }

        updateChildren(touch);
        this->dirty = false;
    };
};