#pragma once

#include "screen.h"
#include "uart/uart.h"

#include <ui/widgets/container.h>
#include <ui/widgets/image.h>
#include <ui/widgets/label.h>

class Initial : public Screen {
private:
    unsigned long status_request_last_time = 0;

    Container cont =
        Container(tft, 5, 5, TFT_HEIGHT - 10, TFT_WIDTH - 10, 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    Image logo = Image(tft, (TFT_HEIGHT - 120) / 2, ((TFT_WIDTH / 2) - 120) / 2 + 25, 120, 120);
    Label loading = Label(tft, "Loading...", 0, TFT_WIDTH / 2 + 25, TFT_WHITE, TFT_CARD_COLOR, 3);

public:
    Initial(TFT_eSPI& tft) : Screen(tft, "initial") {
        logo.loadImage("/logo.rgb565");

        uint8_t s = tft.textsize;
        tft.setTextSize(loading.font_size);
        loading.x = (TFT_HEIGHT - tft.textWidth(loading.text)) / 2;
        tft.setTextSize(s);

        cont.add_children(&logo);
        cont.add_children(&loading);
    }

    void update(FT6336U& touch, SetScreenCallback setScreen) override {
        TouchPointType t = getTouch(touch);

        unsigned long mill = millis();
        if (mill - status_request_last_time > 5000 || status_request_last_time == 0) {
            status_request_last_time = mill;
            UARTSend(10); // STATUS REQUEST
        }

        while (true) {
            PacketReader pr;
            if (!readPacket(pr))
                break;

            uint8_t id;
            // SUCCESS STATUS RESPONSE
            if (pr.readUInt8(id) && id == 11) {
                setScreen("main");
                return;
            }
        }

        cont.update(t);
    }
};