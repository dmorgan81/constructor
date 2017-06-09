#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "quiet-time-layer.h"

typedef struct {
    char buf[32];
    FctxRectLayer *rect_layer;
    FctxTextLayer *text_layer;
    EventHandle settings_event_handle;
} Data;

static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    if (quiet_time_is_active()) {
        Data *data = fctx_layer_get_data(this);
        fctx_layer_update_proc(data->rect_layer, fctx);
        fctx_layer_update_proc(data->text_layer, fctx);
    }
}

static void settings_handler(void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_QUIET_TIME_PREFIX(), "QT", enamel_get_QUIET_TIME_SUFFIX());
    layer_mark_dirty(this);
}

QuietTimeLayer *quiet_time_layer_create(void) {
    log_func();
    QuietTimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->rect_layer = fctx_rect_layer_create();
    fctx_rect_layer_set_handles(data->rect_layer, (FctxRectLayerHandles) {
        .border_color = enamel_get_QUIET_TIME_RECT_BORDER_COLOR,
        .border_width = enamel_get_QUIET_TIME_RECT_BORDER_WIDTH,
        .fill_color = enamel_get_QUIET_TIME_RECT_FILL_COLOR,
        .alignment = enamel_get_QUIET_TIME_ALIGNMENT,
        .rotation = enamel_get_QUIET_TIME_ROTATION,
        .size_w = enamel_get_QUIET_TIME_RECT_SIZE_W,
        .size_h = enamel_get_QUIET_TIME_RECT_SIZE_H,
        .offset_x = enamel_get_QUIET_TIME_X,
        .offset_y = enamel_get_QUIET_TIME_Y
    });
    fctx_layer_add_child(this, data->rect_layer);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
        .fill_color = enamel_get_QUIET_TIME_COLOR,
        .alignment = enamel_get_QUIET_TIME_ALIGNMENT,
        .rotation = enamel_get_QUIET_TIME_ROTATION,
        .font_size = enamel_get_QUIET_TIME_FONT_SIZE,
        .offset_x = enamel_get_QUIET_TIME_X,
        .offset_y = enamel_get_QUIET_TIME_Y
    });
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void quiet_time_layer_destroy(QuietTimeLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(data->text_layer);
    fctx_rect_layer_destroy(data->rect_layer);
    fctx_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
