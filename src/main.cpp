#include "ui/screens/main.h"

#include "config.h"
#include "uart/communication.h"

#include <Arduino.h>
#include <FT6336U.h>
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

/*

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

uint32_t frame = 0;
uint32_t startTime;
uint32_t lastReport;

uint32_t minHeap = UINT32_MAX;
uint32_t maxHeap = 0;

const uint16_t colors[] = {TFT_BLACK,   TFT_WHITE,  TFT_RED,    TFT_GREEN, TFT_BLUE,  TFT_CYAN,
                           TFT_MAGENTA, TFT_YELLOW, TFT_ORANGE, TFT_PINK,  TFT_PURPLE};

constexpr size_t COLOR_COUNT = sizeof(colors) / sizeof(colors[0]);

// ------------------------------------------------------------
// FULL SCREEN FLOOD
// ------------------------------------------------------------

void testFlood() {
    tft.fillScreen(TFT_RED);
    tft.fillScreen(TFT_GREEN);
    tft.fillScreen(TFT_BLUE);
    tft.fillScreen(TFT_WHITE);
    tft.fillScreen(TFT_BLACK);
}

// ------------------------------------------------------------
// GRID
// ------------------------------------------------------------

void testGrid() {
    int w = tft.width();
    int h = tft.height();

    for (int y = 0; y < h; y += 8) {
        tft.drawFastHLine(0, y, w, colors[(frame + y) % COLOR_COUNT]);
    }

    for (int x = 0; x < w; x += 8) {
        tft.drawFastVLine(x, 0, h, colors[(frame + x + 3) % COLOR_COUNT]);
    }
}

// ------------------------------------------------------------
// RANDOM RECTANGLES
// ------------------------------------------------------------

void testRects() {
    int w = tft.width();
    int h = tft.height();

    for (int i = 0; i < 150; i++) {
        int x = random(0, w);
        int y = random(0, h);

        int rw = random(1, 150);
        int rh = random(1, 150);

        if (x + rw > w)
            rw = w - x;

        if (y + rh > h)
            rh = h - y;

        tft.fillRect(x, y, rw, rh, colors[random(COLOR_COUNT)]);
    }
}

// ------------------------------------------------------------
// RANDOM LINES
// ------------------------------------------------------------

void testLines() {
    int w = tft.width();
    int h = tft.height();

    for (int i = 0; i < 150; i++) {
        int x1 = random(w);
        int y1 = random(h);

        int x2 = random(w);
        int y2 = random(h);

        tft.drawLine(x1, y1, x2, y2, colors[random(COLOR_COUNT)]);
    }
}

// ------------------------------------------------------------
// RANDOM CIRCLES
// ------------------------------------------------------------

void testCircles() {
    int w = tft.width();
    int h = tft.height();

    for (int i = 0; i < 50; i++) {
        int x = random(w);
        int y = random(h);

        int r = random(2, 80);

        tft.fillCircle(x, y, r, colors[random(COLOR_COUNT)]);
    }
}

// ------------------------------------------------------------
// RANDOM OUTLINED RECTANGLES
// ------------------------------------------------------------

void testOutlinedRects() {
    int w = tft.width();
    int h = tft.height();

    for (int i = 0; i < 100; i++) {
        int x = random(w);
        int y = random(h);

        int rw = random(5, 150);
        int rh = random(5, 150);

        if (x + rw > w)
            rw = w - x;

        if (y + rh > h)
            rh = h - y;

        tft.drawRect(x, y, rw, rh, colors[random(COLOR_COUNT)]);
    }
}

// ------------------------------------------------------------
// TEXT
// ------------------------------------------------------------

void testText() {
    int w = tft.width();
    int h = tft.height();

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    for (int i = 0; i < 30; i++) {
        int x = random(0, max(1, w - 100));
        int y = random(0, max(1, h - 20));

        tft.setCursor(x, y);

        tft.printf("FRAME %lu", frame);
    }

    tft.setTextSize(3);

    tft.setCursor(10, 10);
    tft.printf("FRAME: %lu", frame);
}

// ------------------------------------------------------------
// FULL FRAME
// ------------------------------------------------------------

void stressFrame() {
    testFlood();

    testGrid();

    testRects();

    testLines();

    testCircles();

    testOutlinedRects();

    testText();
}

// ------------------------------------------------------------
// STATISTICS
// ------------------------------------------------------------

void printStats() {
    uint32_t now = millis();

    if (now - lastReport < 1000)
        return;

    lastReport = now;

    uint32_t heap = ESP.getFreeHeap();

    minHeap = min(minHeap, heap);
    maxHeap = max(maxHeap, heap);

    float seconds = (now - startTime) / 1000.0f;

    float fps = seconds > 0 ? frame / seconds : 0;

    Serial.println();
    Serial.println("======================================");

    Serial.printf("FRAME       : %lu\n", frame);

    Serial.printf("TIME        : %.1f sec\n", seconds);

    Serial.printf("FPS         : %.2f\n", fps);

    Serial.printf("HEAP        : %lu\n", heap);

    Serial.printf("MIN HEAP    : %lu\n", minHeap);

    Serial.printf("MAX HEAP    : %lu\n", maxHeap);

    Serial.printf("WIDTH       : %d\n", tft.width());

    Serial.printf("HEIGHT      : %d\n", tft.height());

    Serial.printf("UPTIME      : %lu sec\n", now / 1000);

    Serial.println("======================================");
}

// ------------------------------------------------------------
// SETUP
// ------------------------------------------------------------

void setup() {
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println();
    Serial.println("==========================================");
    Serial.println("       TFT EXTREME STRESS TEST");
    Serial.println("==========================================");

    Serial.printf("Initial heap: %lu\n", ESP.getFreeHeap());

    // Подсветка
    pinMode(21, OUTPUT);
    digitalWrite(21, HIGH);

    // TFT
    tft.init();

    tft.setRotation(1);

    // Максимально случайные данные
    randomSeed(esp_random());

    tft.fillScreen(TFT_BLACK);

    startTime = millis();
    lastReport = startTime;
}

// ------------------------------------------------------------
// LOOP
// ------------------------------------------------------------

void loop() {
    int w = tft.width();
    int h = tft.height();

    for (int i = 0; i < 500; i++) {
        int x = random(w);
        int y = random(h);

        int rw = random(1, 100);
        int rh = random(1, 100);

        if (x + rw > w)
            rw = w - x;

        if (y + rh > h)
            rh = h - y;

        tft.fillRect(x, y, rw, rh, colors[random(COLOR_COUNT)]);
    }

    for (int i = 0; i < 300; i++) {
        tft.drawLine(random(w), random(h), random(w), random(h), colors[random(COLOR_COUNT)]);
    }

    frame++;
    printStats();
}*/