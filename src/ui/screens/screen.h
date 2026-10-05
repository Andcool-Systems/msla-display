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

    virtual void update(TouchPointType& t) = 0;

    /// @brief Calls on every uart packet
    virtual void onUART(PacketReader& pr) = 0;

    /// @brief CInvalidate screen contents
    virtual void invalidate() = 0;

protected:
    TFT_eSPI& tft;
};