#pragma once

#include "ui/util.h"
#include "widget.h"

#include <vector>

class Container : public Widget {
private:
    uint32_t bg_color;
    uint32_t bgbg_color;

    int32_t r;

    // DirtyArea dirty_area;

    std::vector<Widget*> children;

public:
    int w;
    int h;

    Container(TFT_eSPI& tft, int x, int y, int w, int h, int32_t r, uint32_t bg_color,
              uint32_t bggb_color)
        : w(w), h(h), bg_color(bg_color), bgbg_color(bgbg_color), r(r), Widget(tft, x, y) {}

    void draw() override {
        tft.fillSmoothRoundRect(getX(), getY(), w, h, r, bg_color, bgbg_color);

        this->dirty = false;
    };

    void update(TouchPointType& touch) override {
        bool self_dirty = isDirty();
        if (self_dirty) {
            draw();
        }

        for (auto ch : children) {
            if (self_dirty)
                ch->invalidate();
            ch->update(touch);
        }
    };

    /// Adds a new child
    void add_children(Widget* child) {
        // child->disableSelfRedraw();
        child->setRelativeCords(getX(), getY());
        children.push_back(child);
    }
};