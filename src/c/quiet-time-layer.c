#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "fctx-text-layer.h"
#include "quiet-time-layer.h"

typedef struct {
    FctxTextLayer *text_layer;
    EventHandle settings_event_handle;
} Data;

static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    logd("%d", quiet_time_is_active());
    if (quiet_time_is_active()) {
        Data *data = fctx_layer_get_data(this);
        fctx_text_layer_draw(data->text_layer, fctx);
    }
}

static void settings_handler(void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    fctx_text_layer_set_alignment(data->text_layer, atoi(enamel_get_QUIET_TIME_ALIGNMENT()));
    fctx_text_layer_set_em_height(data->text_layer, enamel_get_QUIET_TIME_FONT_SIZE());
    fctx_text_layer_set_fill_color(data->text_layer, enamel_get_QUIET_TIME_COLOR());
    fctx_text_layer_set_offset(data->text_layer, FPointI(enamel_get_QUIET_TIME_X(), enamel_get_QUIET_TIME_Y()));
    fctx_text_layer_set_rotation(data->text_layer, DEG_TO_TRIGANGLE(enamel_get_QUIET_TIME_ROTATION()));
    layer_mark_dirty(this);
}

QuietTimeLayer *quiet_time_layer_create() {
    log_func();
    QuietTimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_text(data->text_layer, "QT");
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void quiet_time_layer_destroy(QuietTimeLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(data->text_layer);
    fctx_layer_destroy(this);
}
#endif
