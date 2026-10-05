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
    Initial(TFT_eSPI& tft, Application& app);

    void update(TouchPointType& t) override;
    void onUART(PacketReader& pr) override;
    void invalidate() override;
};