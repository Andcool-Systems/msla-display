#include "initial.h"

#include "ui/app.h"

Initial::Initial(TFT_eSPI& tft, Application& app) : Screen(tft, app, "initial") {
    logo.loadImage("/logo.rgb565");

    uint8_t s = tft.textsize;
    tft.setTextSize(loading.font_size);
    loading.x = (TFT_HEIGHT - tft.textWidth(loading.text)) / 2;
    tft.setTextSize(s);

    cont.add_children(&logo);
    cont.add_children(&loading);
}

void Initial::update(FT6336U& touch) {
    TouchPointType t = getTouch(touch);

    unsigned long mill = millis();
    if (mill - status_request_last_time > 5000 || status_request_last_time == 0) {
        status_request_last_time = mill;
        UARTSend(10); // STATUS REQUEST
    }

    cont.update(t);
}

void Initial::onUART(PacketReader& pr) {
    uint8_t id;
    // SUCCESS STATUS RESPONSE
    if (pr.readUInt8(id) && id == 11) {
        app.setScreen("main");
        return;
    }
}