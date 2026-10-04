#pragma once

#include "screens/initial.h"
#include "screens/main.h"
#include "screens/screen.h"

#include <FT6336U.h>
#include <TFT_eSPI.h>

class Application {
public:
    Application(TFT_eSPI& tft) : tft(tft), main_screen(tft, *this), initial_screen(tft, *this) {
        tft.setRotation(1);
        tft.fillScreen(TFT_BLACK);

        ledcSetup(0, 20000, 8);
        ledcAttachPin(TFT_BCKL, 0);

        screens.push_back(&main_screen);
        screens.push_back(&initial_screen);

        setScreen("initial");
    }

    void update(FT6336U& touch) {
        screens[current_screen]->update(touch);
    }

    void onUART(PacketReader pr) {
        screens[current_screen]->onUART(pr);
    }

    void setScreen(String name) {
        int i = 0;
        for (auto sc : screens) {
            if (sc->screen_name == name) {
                current_screen = i;

                tft.fillScreen(TFT_BG_COLOR);
                break;
            }
            i++;
        }
    }

    /// @brief Set backlight power
    void setBacklight(uint8_t pwm) {
        ledcWrite(0, pwm);
    }

private:
    TFT_eSPI& tft;

    Initial initial_screen;
    Main main_screen;

    std::vector<Screen*> screens;
    size_t current_screen = 0;
};