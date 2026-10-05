#include "main.h"

Main::Main(TFT_eSPI& tft, Application& app)
    : Screen(tft, app, "main"), global_state_timer(5000), physical_state_timer(1500),
      remaining_update_timer(ETA_UPDATE_SECONDS * 1000) {
    stack_image.loadImage("/stack.rgb565");
    remaining_image.loadImage("/clock.rgb565");
    logo.loadImage("/logo.rgb565");
    home_image.loadImage("/home.rgb565");
    elapsed_image.loadImage("/hourglass.rgb565");

    z_pos_image.loadImage("/bp.rgb565");
    uv_image.loadImage("/uv.rgb565");

    preview.setRequestCallback([](uint16_t h, uint16_t request_bytes, uint16_t offset_bytes) {
        PacketWriter pw;
        pw.write((uint8_t)20);
        pw.write(h);
        pw.write(request_bytes);
        pw.write(offset_bytes);

        UARTSend(pw);
    });

    image_cont.add_children(&logo);
    image_cont.add_children(&preview);

    bottom_card.add_children(&lbl);

    top_right_cont.add_children(&printer_status);
    top_right_cont.add_children(&hr1);
    top_right_cont.add_children(&global_progress);

    top_right_cont.add_children(&stack_image);
    top_right_cont.add_children(&layers);

    top_right_cont.add_children(&remaining_image);
    top_right_cont.add_children(&remaining);

    top_right_cont.add_children(&elapsed_image);
    top_right_cont.add_children(&elapsed);

    home.setPressedCallback([this]() { UARTSend(56); });

    buttons_cont.add_children(&btn1);
    buttons_cont.add_children(&btn2);
    buttons_cont.add_children(&home);
    home.add_children(&home_image);

    left_bottom_card.add_children(&z_pos_image);
    left_bottom_card.add_children(&z_pos);
    left_bottom_card.add_children(&uv_image);
    left_bottom_card.add_children(&uv_state);
}

void Main::update(TouchPointType& t) {
    if (global_state_timer.timeout())
        UARTSend(10); // STATUS REQUEST

    if (physical_state_timer.timeout()) {
        UARTSend(12); // MECHANICAL STATUS REQUEST
    }

    switch (t.status) {
        case TouchStatusEnum::release:
            lbl.setText("release");
            break;

        case TouchStatusEnum::stream:
            lbl.setText("stream");
            break;

        case TouchStatusEnum::touch:
            lbl.setText("touch");
            break;
    }

    if (remaining_update_timer.timeout()) {
        if (estimated_finish_time >= ETA_UPDATE_SECONDS)
            estimated_finish_time -= ETA_UPDATE_SECONDS;
        else
            estimated_finish_time = 0;

        if (state_t == 1)
            total_elapsed += 3;

        remaining.setText(String("ETA: ") + String(format_duration(estimated_finish_time).c_str()));
        elapsed.setText(String("Elapsed: ") + String(format_duration(total_elapsed).c_str()));
    }

    image_cont.update(t);
    top_right_cont.update(t);
    bottom_card.update(t);
    buttons_cont.update(t);
    left_bottom_card.update(t);
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

        /// MECHANICAL STATUS RESPONSE
        case 13:
            updateMechanicalState(pr);
            break;

        /// PRINTING PREVIEW
        case 21:
            preview.onUART(pr);
            break;
    }
}

void Main::updateMechanicalState(PacketReader& pr) {
    if (pr.readUInt8(uv_state_data))
        uv_state.setText(uv_state_data ? "ON" : "OFF");

    if (pr.readFloat(stepper_pos))
        z_pos.setText("Z:" + String(stepper_pos, 2) + "mm");
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
        preview.reset();
        image_cont.invalidate();
    }

    if (!isPrintState(last_state_t) && isPrintState(state_t)) {
        preview.startLoading();
    }

    last_state_t = state_t;
}

void Main::invalidate() {
    tft.fillScreen(TFT_BG_COLOR);

    image_cont.invalidate();
    top_right_cont.invalidate();
    bottom_card.invalidate();
    buttons_cont.invalidate();
    left_bottom_card.invalidate();
}