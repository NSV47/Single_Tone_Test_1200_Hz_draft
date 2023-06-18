#pragma once

#include <furi.h>
#include <gui/gui.h>

struct SingleToneTest1200HzDraftApp{
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    
    const GpioPin* output_pin;

    bool output_value;
};

typedef struct SingleToneTest1200HzDraftApp SingleToneTest1200HzDraftApp;
