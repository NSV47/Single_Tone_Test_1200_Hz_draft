#include "single_tone_test_1200_hz_draft_app.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

#define _1200   1

static void single_tone_test_1200_hz_draft_app_input_callback(InputEvent* input_event, void* ctx){
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void single_tone_test_1200_hz_draft_app_draw_callback(Canvas* canvas, void* ctx){

    furi_assert(ctx);
    canvas_clear(canvas);
    
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 14, 8, "This is an APRS Hello World");
    
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 55, 32, "RUN on PA6");

    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 127, 40, AlignRight, AlignTop, "to finish, \n click back");
}

SingleToneTest1200HzDraftApp* single_tone_test_1200_hz_draft_app_alloc(){
    SingleToneTest1200HzDraftApp* app = malloc(sizeof(SingleToneTest1200HzDraftApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, single_tone_test_1200_hz_draft_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, single_tone_test_1200_hz_draft_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->output_pin = &gpio_ext_pa6;

    furi_hal_gpio_init(app->output_pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    return app;
}

void single_tone_test_1200_hz_draft_app_free(SingleToneTest1200HzDraftApp* app){
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);
}
//----------------------------------------------------------------------
const float baud_adj = 0.97;
const float adj_1200 = 1.0 * baud_adj;
unsigned int tc1200 = (unsigned int)(0.5 * adj_1200 * 1000000.0 / 1200.0);

void set_nada_1200(SingleToneTest1200HzDraftApp* app)
{
    uint32_t value = 5000;
    taskENTER_CRITICAL();
        while(value--){
            app->output_value = true;
            furi_hal_gpio_write(app->output_pin, app->output_value);
            furi_delay_us(tc1200);
            app->output_value = false;
            furi_hal_gpio_write(app->output_pin, app->output_value);
            furi_delay_us(tc1200);
        }
    taskEXIT_CRITICAL();
}

void set_nada(SingleToneTest1200HzDraftApp* app, bool nada)
{
    if(nada)
        set_nada_1200(app);
}

//----------------------------------------------------------------------
int32_t single_tone_test_1200_hz_draft_app(void *p){
    UNUSED(p);
    SingleToneTest1200HzDraftApp* app = single_tone_test_1200_hz_draft_app_alloc();

    InputEvent event;

    set_nada(app, _1200);

    while(1){

        if(furi_message_queue_get(app->event_queue, &event, 1000) == FuriStatusOk) {
            if(event.type == InputTypePress) {
                if(event.key == InputKeyBack) 
                    break;
                //else if(event.key == InputKeyOk){
                    //notification_message(app->notifications, &example_sound_sequence);  
            }
        }
    }

    single_tone_test_1200_hz_draft_app_free(app);
    return 0;
}