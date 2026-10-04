#pragma once

#include <FT6336U.h>
#include <TFT_eSPI.h>

class Widget {
public:
    uint16_t x = 0;
    uint16_t y = 0;

    Widget(TFT_eSPI& tft, uint16_t x, uint16_t y) : tft(tft), x(x), y(y) {}
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

    uint16_t getX() {
        return rel_x + x;
    }

    uint16_t getY() {
        return rel_y + y;
    }

    void setRelativeCords(uint16_t x, uint16_t y) {
        rel_x = x;
        rel_y = y;
    }

protected:
    bool self_redraw = true;
    bool dirty = true;
    TFT_eSPI& tft;

    uint16_t rel_x = 0;
    uint16_t rel_y = 0;
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

class ContainerAbstract : public Widget {
protected:
    std::vector<Widget*> children;

    /// @brief Updates a children of container
    void updateChildren(TouchPointType& touch) {
        for (auto ch : children) {
            if (isDirty())
                ch->invalidate();
            ch->update(touch);
        }
    }

public:
    ContainerAbstract(TFT_eSPI& tft, uint16_t x, uint16_t y) : Widget(tft, x, y) {};

    /// Adds a new child
    void add_children(Widget* child) {
        // child->disableSelfRedraw();
        child->setRelativeCords(getX(), getY());
        children.push_back(child);
    }
};