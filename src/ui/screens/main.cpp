#include "main.h"

Main::Main(TFT_eSPI& tft, Application& app) : Screen(tft, app, "main") {
    stack_image.loadImage("/stack.rgb565");
    remaining_image.loadImage("/clock.rgb565");
    logo.loadImage("/logo.rgb565");
    home_image.loadImage("/home.rgb565");

    image_cont.add_children(&logo);

    bottom_card.add_children(&lbl);

    top_right_cont.add_children(&printer_status);
    top_right_cont.add_children(&global_progress);

    top_right_cont.add_children(&stack_image);
    top_right_cont.add_children(&layers);

    top_right_cont.add_children(&remaining_image);
    top_right_cont.add_children(&remaining);

    home.setPressedCallback([this]() { UARTSend(56); });

    buttons_cont.add_children(&btn1);
    buttons_cont.add_children(&btn2);
    buttons_cont.add_children(&home);
    home.add_children(&home_image);
}

void Main::update(FT6336U& touch) {
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

        remaining.setText(String("ETA: ") + String(format_duration(estimated_finish_time).c_str()));
    }

    image_cont.update(t);
    top_right_cont.update(t);
    bottom_card.update(t);
    buttons_cont.update(t);
}

void Main::onUART(PacketReader& pr) {
    uint8_t id;
    if (!pr.readUInt8(id))
        return;

    switch (id) {
        /// GLOBAL STATUS RESPONSE
        case 11:
            if (pr.readUInt8(state_t))
                updateState(pr);
            break;

        /// PRINTING PREVIEW
        case 21:
            handlePreviewResponse(pr);
            break;
    }
}

void Main::updateState(PacketReader& pr) {
    switch (state_t) {
        case 0:
            printer_status.setText("Printer is idle");
            break;

        case 1:
            pr.readUInt16(current_layer);
            pr.readUInt16(total_layers);

            pr.readUInt32(current_ir);
            pr.readUInt32(total_ir);

            pr.readUInt32(current_ir_duration);
            pr.readUInt32(est_print_time);
            pr.readUInt32(estimated_finish_time);
            pr.readUInt32(total_elapsed);

            printer_status.setText("Printing...");
            layers.setText("Layer " + String(current_layer) + "/" + String(total_layers));

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

void Main::startRequestingImage() {
    preview_loaded = false;
    preview_loading_offset = 0;
    image_cont.invalidate();
    requestPreview();
}

/// @brief Create preview loading request
void Main::requestPreview() {
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
void Main::handlePreviewResponse(PacketReader& pr) {
    uint8_t status = 0;
    if (pr.readUInt8(status) && status != 0) {
        return;
    }

    uint16_t len = 0;
    uint16_t pixels[image_cont.h * preview_loading_lines];

    if (pr.readUInt16(len) && pr.readExact(pixels, len)) {
        lbl.setText(String(pixels[0]));
        uint16_t y = preview_loading_offset / image_cont.h;

        tft.pushImage(image_cont.getX(), image_cont.getY() + y, image_cont.h, preview_loading_lines,
                      reinterpret_cast<uint16_t*>(pixels));

        if (y >= image_cont.h) {
            preview_loaded = true;
            return;
        }

        preview_loading_offset += image_cont.h * preview_loading_lines;
        requestPreview();
    }
}