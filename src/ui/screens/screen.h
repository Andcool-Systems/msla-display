#pragma once
#include "config.h"
#include "uart/packet.h"
#include "ui/widgets/widget.h"

#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <vector>

class Application;

class Screen {
public:
    Application& app;

    using SetScreenCallback = std::function<void(String)>;

    String screen_name;
    Screen(TFT_eSPI& tft, Application& app, String screen_name)
        : tft(tft), screen_name(screen_name), app(app) {}

    virtual void update(FT6336U& touch) = 0;

    /// @brief Calls on every uart packet
    virtual void onUART(PacketReader& pr) = 0;

protected:
    TFT_eSPI& tft;

    /// @brief Get current touch
    TouchPointType getTouch(FT6336U& touch) {
        FT6336U_TouchPointType tp = touch.scan();
        TouchPointType t = tp.tp[0];
        t.x = tp.tp[0].y;
        t.y = TFT_WIDTH - tp.tp[0].x;

        return t;
    }
};