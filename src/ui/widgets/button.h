#pragma once

#include "ui/util.h"
#include "widget.h"

class Button : public Widget {
private:
    int width;
    int height;

    String text;

    uint32_t fg_color;
    uint32_t text_color;
    uint8_t font_size;

    bool pressed = false;

    std::function<void()> fn_pressed = nullptr;
    std::function<void()> fn_released = nullptr;

public:
    Button(TFT_eSPI& tft, String text, int x, int y, int w, int h, uint32_t fg_color,
           uint32_t text_color, uint8_t font_size)
        : text(text), width(w), height(h), fg_color(fg_color), text_color(text_color),
          font_size(font_size), Widget(tft, x, y) {}

    void draw() override {
        tft.setTextColor(this->text_color);
        tft.setTextSize(this->font_size);

        int w = tft.textWidth(this->text);
        int h = tft.fontHeight();

        int x = getX();
        int y = getY();

        // tft.fillRect(x, y, _real_width, _real_height, TFT_BLACK);
        tft.fillRect(x, y, width, height, pressed ? darken(fg_color, 30) : fg_color);

        tft.drawString(this->text, x + (width / 2 - w / 2), y + (height / 2 - h / 2));

        this->dirty = false;
    };

    void update(TouchPointType& touch) override {
        int x = getX();
        int y = getY();

        bool inside = touch.x >= x && touch.x <= x + width && touch.y >= y && touch.y <= y + height;

        if ((touch.status == TouchStatusEnum::touch || touch.status == TouchStatusEnum::stream) &&
            inside) {
            if (!pressed) {
                if (fn_pressed)
                    fn_pressed();
                pressed = true;
                invalidate();
            }

        } else if (touch.status == TouchStatusEnum::release || !inside) {
            if (pressed) {
                if (fn_released)
                    fn_released();
                pressed = false;
                invalidate();
            }
        }

        redraw();
    };

    /// @brief Set button pressed callback
    void setPressedCallback(std::function<void()> fn) {
        this->fn_pressed = fn;
    };

    /// @brief Set button released callback
    void setReleasedCallback(std::function<void()> fn) {
        this->fn_released = fn;
    };

    /// @brief Set button text
    void setText(String str) {
        this->text = str;
        this->invalidate();
    };
};