#pragma once

#include "ui/util.h"
#include "widget.h"

#include <FS.h>
#include <LittleFS.h>
#include <vector>

class Image : public Widget {
private:
    int w;
    int h;

public:
    fs::File file;
    bool loaded = false;
    Image(TFT_eSPI& tft, int x, int y, int w, int h) : w(w), h(h), Widget(tft, x, y) {}

    void loadImage(String path) {
        file = LittleFS.open(path, "r");
        loaded = (bool)file;
    }

    ~Image() {
        if (file)
            file.close();
    }

    void draw() override {
        if (!loaded)
            return;

        file.seek(0);

        tft.setSwapBytes(true);
        tft.startWrite();
        tft.setAddrWindow(getX(), getY(), w, h);

        uint16_t buffer[4096 / sizeof(uint16_t)];
        size_t pixelsLeft = (size_t)w * h;

        while (pixelsLeft > 0) {
            size_t pixels = min(pixelsLeft, sizeof(buffer) / sizeof(buffer[0]));
            size_t bytes = pixels * sizeof(uint16_t);
            size_t read = file.read((uint8_t*)buffer, bytes);

            if (read != bytes) {
                tft.endWrite();
                this->dirty = false;

                return;
            }

            tft.pushPixels(buffer, pixels);
            pixelsLeft -= pixels;
        }

        tft.endWrite();
        tft.setSwapBytes(false);
    };

    void update(TouchPointType& touch) override {
        redraw();
        this->dirty = false;
    };
};