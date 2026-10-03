#include "config.h"
#include "uart/communication.h"
#include "ui/app.h"

#include <Arduino.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>

TFT_eSPI tft;
FT6336U ft6336u(CTP_SDA_PIN, CTP_SCL_PIN, CTP_RST_PIN, CTP_INT_PIN);

Application* app;

void setup() {
    LittleFS.begin();
    tft.init();
    ft6336u.begin();

    initCommunication();

    static Application application(tft);
    app = &application;
}

void loop() {
    app->update(ft6336u);
    delay(25);
}