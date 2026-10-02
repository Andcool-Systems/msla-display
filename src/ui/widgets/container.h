#pragma once

#include "ui/util.h"
#include "widget.h"

#include <vector>

class Container : public Widget {
private:
    int w;
    int h;

    uint32_t bg_color;
    uint32_t bgbg_color;

    int32_t r;

    // DirtyArea dirty_area;

    std::vector<Widget*> children;

public:
    Container(TFT_eSPI& tft, int x, int y, int w, int h, int32_t r, uint32_t bg_color,
              uint32_t bggb_color)
        : w(w), h(h), bg_color(bg_color), bgbg_color(bgbg_color), r(r), Widget(tft, x, y) {}

    void draw() override {
        tft.fillSmoothRoundRect(getX(), getY(), w, h, r, bg_color, bgbg_color);

        for (auto ch : children) {
            ch->draw();
        }

        this->dirty = false;
    };

    void update(TouchPointType& touch) override {
        bool dirty_children = isDirty();

        for (auto ch : children) {
            ch->update(touch);
            dirty_children = dirty_children || ch->isDirty();
        }

        if (dirty_children) {
            draw();
        }
    };

    /// Adds a new child
    void add_children(Widget* child) {
        child->disableSelfRedraw();
        child->setRelativeCords(getX(), getY());
        children.push_back(child);
    }
};