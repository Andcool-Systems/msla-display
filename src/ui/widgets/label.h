#pragma once

#include "ui/util.h"
#include "widget.h"

class Label : public Widget {
private:
    uint32_t bg_color;
    uint32_t text_color;

    DirtyArea dirty_area;

public:
    String text;
    uint8_t font_size = 1;

    Label(TFT_eSPI& tft, String text, int x, int y, uint32_t text_color, uint32_t bg_color,
          uint8_t font_size)
        : text(text), bg_color(bg_color), text_color(text_color), font_size(font_size),
          Widget(tft, x, y) {}

    void draw() override {
        tft.setTextColor(this->text_color, this->bg_color);
        tft.setTextSize(this->font_size);

        tft.fillRect(dirty_area.x, dirty_area.y, dirty_area.w, dirty_area.h, this->bg_color);
        tft.drawString(this->text, getX(), getY());
    };

    void update(TouchPointType& touch) override {
        redraw();
        this->dirty = false;
    };

    /// @brief Set label text
    void setText(String str) {
        if (str == text)
            return;

        uint8_t s = tft.textsize;
        tft.setTextSize(this->font_size);
        int text_w = tft.textWidth(text);
        int font_h = tft.fontHeight();
        tft.setTextSize(s);

        this->dirty_area = DirtyArea(getX(), getY(), text_w, font_h);

        this->text = str;
        this->invalidate();
    };
};