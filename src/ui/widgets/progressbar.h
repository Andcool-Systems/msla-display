#pragma once

#include "ui/util.h"
#include "widget.h"

class ProgressBar : public Widget {
private:
    int w;
    int h;

    int new_w;
    String text;
    String last_text;

    uint8_t font_size = 1;

    uint32_t text_color;
    uint32_t active_color;
    uint32_t bg_color;
    uint32_t parent_color;

    float progress;

    int progress_width;
    int last_progress_width;

    DirtyArea dirty_area;

public:
    ProgressBar(TFT_eSPI& tft, int x, int y, int w, int h, uint32_t text_color,
                uint32_t active_color, uint32_t bg_color, uint32_t parent_color, uint8_t font_size)

        : w(w), h(h), bg_color(bg_color), text_color(text_color), active_color(active_color),
          font_size(font_size), parent_color(parent_color), Widget(tft, x, y) {}

    void draw() override {
        tft.setTextColor(this->text_color, this->parent_color);
        tft.setTextSize(this->font_size);

        int text_h = tft.fontHeight();

        int x = getX();
        int y = getY();

        tft.fillSmoothRoundRect(x, y, new_w, h, h / 2, bg_color, parent_color);

        if (progress_width > h) {
            tft.fillSmoothRoundRect(x, y, progress_width, h, h / 2, active_color, parent_color);
        } else {
            int r = h / 2;
            tft.fillSmoothCircle(x + r, y + r, r, active_color, bg_color);
        }

        tft.drawString(this->text, x + new_w + 10, y + ((h / 2) - (text_h / 2)));

        this->dirty = false;
    };

    void update(TouchPointType& touch) override {
        text = String(progress, 1) + "%";
        int text_w = tft.textWidth(text);
        new_w = w - (text_w + 10);

        last_progress_width = progress_width;
        progress_width = new_w * (progress / 100.0);
        if (last_progress_width != progress_width || last_text != text) {
            invalidate();
        }

        last_text = text;
        redraw();
    };

    // Set progress bar progress
    void setProgress(float p) {
        progress = min(max(0.f, p), 100.f);
    }
};