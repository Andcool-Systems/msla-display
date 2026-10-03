#pragma once

#include <FT6336U.h>
#include <TFT_eSPI.h>

class Widget {
public:
    int x = 0;
    int y = 0;

    Widget(TFT_eSPI& tft, int x, int y) : tft(tft), x(x), y(y) {}
    virtual void draw() = 0;
    virtual void update(TouchPointType& touch) = 0;

    void invalidate() {
        dirty = true;
    }

    bool isDirty() const {
        return dirty;
    }

    void disableSelfRedraw() {
        self_redraw = false;
    }

    /// @brief Redraws elements if self-redrawing not disabled
    void redraw() {
        if (!self_redraw)
            return;

        if (!isDirty())
            return;

        draw();
    }

    int getX() {
        return rel_x + x;
    }

    int getY() {
        return rel_y + y;
    }

    void setRelativeCords(int x, int y) {
        rel_x = x;
        rel_y = y;
    }

protected:
    bool self_redraw = true;
    bool dirty = true;
    TFT_eSPI& tft;

    int rel_x = 0;
    int rel_y = 0;
};

class DirtyArea {
public:
    uint16_t x = 0;
    uint16_t y = 0;

    uint16_t w = 0;
    uint16_t h = 0;

    DirtyArea() {}
    DirtyArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h) : x(x), y{y}, w(w), h(h) {}
};