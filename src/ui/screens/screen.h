#pragma once
#include "config.h"
#include "ui/widgets/widget.h"

#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <vector>

class Screen {
public:
    Screen(TFT_eSPI& tft) : tft(tft) {}

    virtual void update(FT6336U& touch) = 0;

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