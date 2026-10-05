#include "timer.h"

#include <Arduino.h>

Timer::Timer(int t) {
    period = t;
}

void Timer::reset() {
    lt = millis();
}

bool Timer::timeout() {
    if (millis() - lt > period) {
        lt = millis();
        return true;
    }

    return false;
}