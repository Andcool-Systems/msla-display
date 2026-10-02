#pragma once

#include "screen.h"
#include "uart/uart.h"

#include <ui/widgets/button.h>
#include <ui/widgets/container.h>
#include <ui/widgets/label.h>
#include <ui/widgets/progressbar.h>

const int card_margin = 5;
const int card_gap = 10;
const int card_padding = 10;

class Main : public Screen {
private:
    unsigned long status_request_last_time = 0;
    int x = 0;

    /// STATE
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

    Container iamge_cont = Container(tft, card_margin, card_margin, TFT_WIDTH / 2, TFT_WIDTH / 2, 5,
                                     TFT_CARD_COLOR, TFT_BG_COLOR);

    Container top_right_cont = Container(tft, TFT_WIDTH / 2 + card_gap, card_margin,
                                         TFT_HEIGHT - (TFT_WIDTH / 2 + (card_gap + card_margin)),
                                         TFT_WIDTH / 2, 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    Container bottom_card =
        Container(tft, 5, TFT_WIDTH / 2 + card_gap, TFT_HEIGHT - (card_margin * 2),
                  TFT_WIDTH / 2 - (card_gap + card_margin), 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    ProgressBar global_progress =
        ProgressBar(tft, card_gap, card_gap + 20,
                    TFT_HEIGHT - (TFT_WIDTH / 2 + (card_gap + card_margin) + card_gap * 2), 15,
                    TFT_WHITE, TFT_ACT_COLOR, TFT_EL_COLOR, TFT_CARD_COLOR, 2);

    Label lbl = Label(tft, "Label", 5, 60, TFT_WHITE, TFT_CARD_COLOR, 2);
    Button btn = Button(tft, "Btn", 10, 10, 50, 50, TFT_RED, TFT_WHITE, 2);

    Label printer_status =
        Label(tft, "Loading...", card_padding, card_padding, TFT_WHITE, TFT_CARD_COLOR, 2);

public:
    Main(TFT_eSPI& tft) : Screen(tft) {
        btn.setPressedCallback([this]() { UARTSend(40); });

        iamge_cont.add_children(&btn);
        iamge_cont.add_children(&lbl);

        top_right_cont.add_children(&printer_status);
        top_right_cont.add_children(&global_progress);
    }

    void update(FT6336U& touch) override {
        TouchPointType t = getTouch(touch);
        unsigned long mill = millis();

        if (mill - status_request_last_time > 5000) {
            status_request_last_time = mill;
            UARTSend(10); // STATUS REQUEST

            x++;
            lbl.setText(String(x));
        }

        PacketReader pr;
        while (readPacket(pr)) {
            handlePacket(&pr);
        }

        iamge_cont.update(t);
        top_right_cont.update(t);
        bottom_card.update(t);
    }

    void handlePacket(PacketReader* pr) {
        uint8_t id;
        if (!pr->readUInt8(id))
            return;

        switch (id) {
            /// GLOBAL STATUS RESPONSE
            case 11:
                if (pr->readUInt8(state_t))
                    updateState(pr);
                break;
        }
    }

    void updateState(PacketReader* pr) {
        switch (state_t) {
            case 0:
                printer_status.setText("Printer is idle");
                break;

            case 1:
                pr->readUInt16(current_layer);
                pr->readUInt16(total_layers);

                pr->readUInt32(current_ir);
                pr->readUInt32(total_ir);

                pr->readUInt32(current_ir_duration);
                pr->readUInt32(est_print_time);
                pr->readUInt32(estimated_finish_time);
                pr->readUInt32(total_elapsed);

                printer_status.setText("Printing...");
                global_progress.setProgress(current_ir / (float)total_ir);
                break;

            case 2:
                printer_status.setText("Print paused");
                break;

            case 3:
                printer_status.setText("Print failed </3");
                break;

            case 4:
                printer_status.setText("Printer is busy");
                break;

            case 5:
                printer_status.setText("Print aborted");
                break;

            case 6:
                printer_status.setText("Print finished <3");
                break;

            default:
                break;
        }
    }
};