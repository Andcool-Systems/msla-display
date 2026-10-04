#pragma once

#include "screen.h"
#include "uart/uart.h"

#include <ui/widgets/button.h>
#include <ui/widgets/container.h>
#include <ui/widgets/image.h>
#include <ui/widgets/label.h>
#include <ui/widgets/progressbar.h>

#define CARD_MARGIN 5
#define CARD_GAP 10
#define CARD_PADDING 10

#define ETA_UPDATE_SECONDS 3

inline bool isPrintState(uint8_t s) {
    return s == 1 || s == 2;
}

class Main : public Screen {
private:
    unsigned long status_request_last_time = 0;
    unsigned long remaining_time_last_time = 0;

    bool preview_loaded = false;
    uint8_t preview_loading_lines = 2;
    uint16_t preview_loading_offset = 0;

    /// STATE
    uint8_t last_state_t = 0;
    uint8_t state_t = 0;
    uint16_t current_layer = 0;
    uint16_t total_layers = 0;

    uint32_t current_ir = 0;
    uint32_t total_ir = 0;

    uint32_t current_ir_duration = 0;
    uint32_t est_print_time = 0;
    uint32_t estimated_finish_time = 0;
    uint32_t total_elapsed = 0;
    /// !STATE

    Container image_cont = Container(tft, CARD_MARGIN, CARD_MARGIN, TFT_WIDTH / 2, TFT_WIDTH / 2, 5,
                                     TFT_CARD_COLOR, TFT_BG_COLOR);

    Image logo = Image(tft, ((TFT_WIDTH / 2) - 120) / 2, ((TFT_WIDTH / 2) - 120) / 2, 120, 120);

    Container top_right_cont = Container(tft, TFT_WIDTH / 2 + CARD_GAP, CARD_MARGIN,
                                         TFT_HEIGHT - (TFT_WIDTH / 2 + (CARD_GAP + CARD_MARGIN)),
                                         TFT_WIDTH / 2, 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    int buttons_size = (TFT_WIDTH / 2 - (CARD_GAP + CARD_MARGIN) - (CARD_MARGIN * 2)) / 3;

    Container bottom_card =
        Container(tft, 5, TFT_WIDTH / 2 + CARD_GAP,
                  TFT_HEIGHT - (CARD_MARGIN * 2) - buttons_size - CARD_MARGIN,
                  TFT_WIDTH / 2 - (CARD_GAP + CARD_MARGIN), 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    Container buttons_cont = Container(
        tft, TFT_HEIGHT - (CARD_MARGIN + buttons_size), TFT_WIDTH / 2 + CARD_GAP, buttons_size,
        TFT_WIDTH / 2 - (CARD_GAP + CARD_MARGIN), 5, TFT_BG_COLOR, TFT_BG_COLOR);

    Button btn1 = Button(tft, "1", 0, 0, buttons_size, buttons_size, 5, TFT_GREEN, TFT_WHITE,
                         TFT_BG_COLOR, 2);

    Button btn2 = Button(tft, "2", 0, buttons_size + CARD_MARGIN, buttons_size, buttons_size, 5,
                         TFT_GREEN, TFT_WHITE, TFT_BG_COLOR, 2);

    Image home_image = Image(tft, (buttons_size - 24) / 2, (buttons_size - 24) / 2, 24, 24);

    Button home = Button(tft, "", 0, buttons_size * 2 + CARD_MARGIN * 2, buttons_size, buttons_size,
                         5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 2);

    Label lbl = Label(tft, "Label", 5, 5, TFT_WHITE, TFT_CARD_COLOR, 2);
    // Button btn = Button(tft, "Btn", 10, 10, 50, 50, TFT_RED, TFT_WHITE, 2);

    Label printer_status =
        Label(tft, "Loading...", CARD_PADDING, CARD_PADDING, TFT_WHITE, TFT_CARD_COLOR, 2);

    ProgressBar global_progress =
        ProgressBar(tft, CARD_GAP, CARD_GAP + 25,
                    TFT_HEIGHT - (TFT_WIDTH / 2 + (CARD_GAP + CARD_MARGIN) + CARD_GAP * 2), 15,
                    TFT_WHITE, TFT_ACT_COLOR, TFT_EL_COLOR, TFT_CARD_COLOR, 2);

    /// layers
    Image stack_image = Image(tft, 5, CARD_GAP + 50, 24, 24);

    Label layers =
        Label(tft, "Layer -/-", CARD_PADDING + 26, CARD_GAP + 55, TFT_WHITE, TFT_CARD_COLOR, 2);

    /// remaining
    Image remaining_image = Image(tft, 5, CARD_GAP + 75, 24, 24);

    Label remaining =
        Label(tft, "0:0:0", CARD_PADDING + 26, CARD_GAP + 80, TFT_WHITE, TFT_CARD_COLOR, 2);

public:
    Main(TFT_eSPI& tft, Application& app);

    void update(FT6336U& touch) override;

    void onUART(PacketReader& pr) override;

    void updateState(PacketReader& pr);

    void startRequestingImage();

    /// @brief Create preview loading request
    void requestPreview();

    /// @brief  Handle and display preview response
    void handlePreviewResponse(PacketReader& pr);
};