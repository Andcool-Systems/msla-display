#pragma once

#include "ui/util.h"
#include "widget.h"

class Button : public ContainerAbstract {
private:
    int width;
    int height;

    String text;

    uint32_t fg_color;
    uint32_t text_color;
    uint8_t font_size;

    uint32_t parent_color;
    int r;

    bool pressed = false;

    std::function<void()> fn_pressed = nullptr;
    std::function<void()> fn_released = nullptr;

public:
    Button(TFT_eSPI& tft, String text, int x, int y, int w, int h, int r, uint32_t fg_color,
           uint32_t text_color, uint32_t parent_color, uint8_t font_size)
        : text(text), width(w), height(h), fg_color(fg_color), text_color(text_color),
          font_size(font_size), parent_color(parent_color), r(r), ContainerAbstract(tft, x, y) {}

    void draw() override {
        tft.setTextColor(this->text_color);
        tft.setTextSize(this->font_size);

        int w = tft.textWidth(this->text);
        int h = tft.fontHeight();

        int x = getX();
        int y = getY();

        tft.fillSmoothRoundRect(x, y, width, height, r, pressed ? darken(fg_color, 30) : fg_color,
                                parent_color);

        tft.drawString(this->text, x + (width / 2 - w / 2), y + (height / 2 - h / 2));
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

        updateChildren(touch);
        this->dirty = false;
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