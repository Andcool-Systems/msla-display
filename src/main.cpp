#include "ui/screens/main.h"

#include "config.h"
#include "uart/communication.h"

#include <Arduino.h>
#include <FT6336U.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>

TFT_eSPI tft;
FT6336U ft6336u(CTP_SDA_PIN, CTP_SCL_PIN, CTP_RST_PIN, CTP_INT_PIN);

void setup() {
    pinMode(TFT_BCKL, OUTPUT);

    tft.init();
    ft6336u.begin();

    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    initCommunication();

    digitalWrite(TFT_BCKL, HIGH);
}

Main main_screen = Main(tft);

void loop() {
    main_screen.update(ft6336u);

    delay(25);
}
