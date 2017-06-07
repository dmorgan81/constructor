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
    FPoint offset = FPointI(enamel_get_QUIET_TIME_X(), enamel_get_QUIET_TIME_Y());
    uint32_t rotation = DEG_TO_TRIGANGLE(enamel_get_QUIET_TIME_ROTATION());
    GTextAlignment alignment = atoi(enamel_get_QUIET_TIME_ALIGNMENT());

    fctx_rect_layer_set_fill_color(data->rect_layer, enamel_get_QUIET_TIME_RECT_FILL_COLOR());
    FSize size = FSizeI(enamel_get_QUIET_TIME_RECT_SIZE_W(), enamel_get_QUIET_TIME_RECT_SIZE_H());
    fctx_rect_layer_set_size(data->rect_layer, size);
    fctx_rect_layer_set_offset(data->rect_layer, offset);
    fctx_rect_layer_set_rotation(data->rect_layer, rotation);
    fctx_rect_layer_set_border_color(data->rect_layer, enamel_get_QUIET_TIME_RECT_BORDER_COLOR());
    fctx_rect_layer_set_border_width(data->rect_layer, enamel_get_QUIET_TIME_RECT_BORDER_WIDTH());
    fctx_rect_layer_set_alignment(data->rect_layer, alignment);

    fctx_text_layer_set_alignment(data->text_layer, alignment);
    fctx_text_layer_set_em_height(data->text_layer, enamel_get_QUIET_TIME_FONT_SIZE());
    fctx_text_layer_set_fill_color(data->text_layer, enamel_get_QUIET_TIME_COLOR());
    fctx_text_layer_set_offset(data->text_layer, offset);
    fctx_text_layer_set_rotation(data->text_layer, rotation);

    static char s[32];
    snprintf(s, sizeof(s), "%s%s%s", enamel_get_QUIET_TIME_PREFIX(), "QT", enamel_get_QUIET_TIME_SUFFIX());
    fctx_text_layer_set_text(data->text_layer, s);
}

QuietTimeLayer *quiet_time_layer_create(void) {
    log_func();
    QuietTimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->rect_layer = fctx_rect_layer_create();
    fctx_layer_add_child(this, data->rect_layer);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_anchor(data->text_layer, FTextAnchorMiddle);

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
