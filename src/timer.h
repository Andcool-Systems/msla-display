#pragma once

class Timer {
public:
    Timer(int period);

    /// @brief  Resets timer
    void reset();

    /// @brief Checks if the timer is fired
    bool timeout();

private:
    unsigned long lt;
    int period;
};