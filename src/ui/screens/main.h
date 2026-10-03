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

bool isPrintState(uint8_t s) {
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

    Container bottom_card =
        Container(tft, 5, TFT_WIDTH / 2 + CARD_GAP, TFT_HEIGHT - (CARD_MARGIN * 2),
                  TFT_WIDTH / 2 - (CARD_GAP + CARD_MARGIN), 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    Label lbl = Label(tft, "Label", 5, 5, TFT_WHITE, TFT_CARD_COLOR, 2);
    Button btn = Button(tft, "Btn", 10, 10, 50, 50, TFT_RED, TFT_WHITE, 2);

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
    Main(TFT_eSPI& tft) : Screen(tft, "main") {
        stack_image.loadImage("/stack.rgb565");
        remaining_image.loadImage("/clock.rgb565");
        logo.loadImage("/logo.rgb565");

        btn.setPressedCallback([this]() { UARTSend(40); });

        image_cont.add_children(&logo);

        // iamge_cont.add_children(&btn);
        bottom_card.add_children(&lbl);

        top_right_cont.add_children(&printer_status);
        top_right_cont.add_children(&global_progress);

        top_right_cont.add_children(&stack_image);
        top_right_cont.add_children(&layers);

        top_right_cont.add_children(&remaining_image);
        top_right_cont.add_children(&remaining);
    }

    void update(FT6336U& touch, SetScreenCallback setScreen) override {
        TouchPointType t = getTouch(touch);
        unsigned long mill = millis();

        if (mill - status_request_last_time > 5000 || status_request_last_time == 0) {
            status_request_last_time = mill;
            UARTSend(10); // STATUS REQUEST
        }

        if (mill - remaining_time_last_time > ETA_UPDATE_SECONDS * 1000) {
            remaining_time_last_time = mill;
            if (estimated_finish_time >= ETA_UPDATE_SECONDS)
                estimated_finish_time -= ETA_UPDATE_SECONDS;
            else
                estimated_finish_time = 0;

            remaining.setText(String("ETA: ") +
                              String(format_duration(estimated_finish_time).c_str()));
        }

        while (true) {
            PacketReader pr;
            if (!readPacket(pr))
                break;
            handlePacket(&pr);
        }

        image_cont.update(t);
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

            /// PRINTING PREVIEW
            case 21:
                handlePreviewResponse(pr);
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
                layers.setText("Layer " + String(current_layer) + "/" + String(total_ir));

                global_progress.setProgress((current_ir / (float)total_ir) * 100.f);

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
                global_progress.setProgress(100);
                break;

            default:
                break;
        }

        if (isPrintState(last_state_t) && !isPrintState(state_t)) {
            image_cont.invalidate();
        }

        if (!isPrintState(last_state_t) && isPrintState(state_t)) {
            startRequestingImage();
        }

        last_state_t = state_t;
    }

    void startRequestingImage() {
        preview_loaded = false;
        preview_loading_offset = 0;
        image_cont.invalidate();
        requestPreview();
    }

    /// @brief Create preview loading request
    void requestPreview() {
        if (preview_loaded)
            return;

        PacketWriter pw;
        pw.write((uint8_t)20);
        pw.write((uint16_t)(image_cont.h));
        pw.write((uint16_t)((image_cont.h * preview_loading_lines) * 2));
        pw.write((uint16_t)(preview_loading_offset * 2));
        UARTSend(pw);
    }

    /// @brief  Handle and display preview response
    void handlePreviewResponse(PacketReader* pr) {
        uint8_t status = 0;
        if (pr->readUInt8(status) && status != 0) {
            return;
        }

        uint16_t len = 0;
        uint16_t pixels[image_cont.h * preview_loading_lines];

        if (pr->readUInt16(len) && pr->readExact(pixels, len)) {
            lbl.setText(String(pixels[0]));
            uint16_t y = preview_loading_offset / image_cont.h;

            tft.pushImage(image_cont.getX(), image_cont.getY() + y, image_cont.h,
                          preview_loading_lines, reinterpret_cast<uint16_t*>(pixels));

            if (y >= image_cont.h) {
                preview_loaded = true;
                return;
            }

            preview_loading_offset += image_cont.h * preview_loading_lines;
            requestPreview();
        }
    }
};