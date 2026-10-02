#pragma once

#include "ui/util.h"
#include "widget.h"

class Label : public Widget {
private:
    uint8_t font_size = 1;

    String text;

    uint32_t bg_color;
    uint32_t text_color;

    DirtyArea dirty_area;

public:
    Label(TFT_eSPI& tft, String text, int x, int y, uint32_t text_color, uint32_t bg_color,
          uint8_t font_size)
        : text(text), bg_color(bg_color), text_color(text_color), font_size(font_size),
          Widget(tft, x, y) {}

    void draw() override {
        tft.setTextColor(this->text_color, this->bg_color);
        tft.setTextSize(this->font_size);

        tft.fillRect(dirty_area.x, dirty_area.y, dirty_area.w, dirty_area.h, this->bg_color);
        tft.drawString(this->text, getX(), getY());

        this->dirty = false;
    };

    void update(TouchPointType& touch) override {
        redraw();
    };

    /// @brief Set label text
    void setText(String str) {
        if (str == text)
            return;

        this->dirty_area = DirtyArea(getX(), getY(), tft.textWidth(this->text), tft.fontHeight());

        this->text = str;
        this->invalidate();
    };
};