#pragma once

#include "uart/packet.h"
#include "ui/util.h"
#include "widget.h"

#include <vector>

class RemoteImage : public Widget {
private:
    int w;
    int h;

    int lines_per_packet;
    int loading_offset;

    bool fully_loaded = false;

    std::function<void(uint16_t h, uint16_t request_bytes, uint16_t offset_bytes)>
        request_next_callback = nullptr;

    void request() {
        if (request_next_callback)
            request_next_callback(h, (h * lines_per_packet) * 2, loading_offset * 2);
    }

public:
    RemoteImage(TFT_eSPI& tft, int x, int y, int w, int h, int lines_per_packet)
        : w(w), h(h), lines_per_packet(lines_per_packet), Widget(tft, x, y) {
        dirty = false;
    }

    void draw() override {};

    void update(TouchPointType& touch) override {
        if (isDirty()) {
            reset();
            startLoading();
        }

        redraw();
        this->dirty = false;
    };

    /// @brief On new remote packet
    void onUART(PacketReader pr) {
        uint8_t status = 0;
        if (pr.readUInt8(status) && status != 0) {
            return;
        }

        uint16_t len = 0;
        uint16_t temp[w * lines_per_packet];

        if (pr.readUInt16(len) && pr.readExact(temp, len)) {
            uint16_t y = loading_offset / h;

            tft.pushImage(getX(), getY() + y, w, lines_per_packet, temp);

            if (y >= h) {
                fully_loaded = true;
                dirty = false;
                return;
            }

            loading_offset += h * lines_per_packet;

            request();
        }
    }

    /// @brief Resets and invalidate image
    void reset() {
        fully_loaded = false;
    }

    /// @brief Sets a data request callback
    void setRequestCallback(
        std::function<void(uint16_t h, uint16_t request_bytes, uint16_t offset_bytes)> c) {
        request_next_callback = c;
    }

    /// @brief Start image loading
    void startLoading() {
        request();
    }
};