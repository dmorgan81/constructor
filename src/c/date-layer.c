#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "date-layer.h"

typedef struct {
    char buf[PBL_IF_LOW_MEM_ELSE(48, 128)];
#ifndef PBL_PLATFORM_APLITE
    FctxRectLayer *rect_layer;
#endif
    FctxTextLayer *text_layer;
    EventHandle tick_timer_event_handle;
    EventHandle settings_event_handle;
} Data;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
#ifdef PBL_PLATFORM_APLITE
    strftime(data->buf, sizeof(data->buf), enamel_get_DATE_FORMAT(), tick_time);
#else
    char s[48];
    strftime(s, sizeof(s), enamel_get_DATE_FORMAT(), tick_time);
    snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_DATE_PREFIX(), s, enamel_get_DATE_SUFFIX());
#endif
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FPoint offset = FPointI(enamel_get_DATE_X(), enamel_get_DATE_Y());
    uint32_t rotation = DEG_TO_TRIGANGLE(enamel_get_DATE_ROTATION());
    GTextAlignment alignment = atoi(enamel_get_DATE_ALIGNMENT());

    fctx_text_layer_set_alignment(data->text_layer, alignment);
    fctx_text_layer_set_em_height(data->text_layer, enamel_get_DATE_FONT_SIZE());
    fctx_text_layer_set_fill_color(data->text_layer, enamel_get_DATE_COLOR());
    fctx_text_layer_set_offset(data->text_layer, offset);
    fctx_text_layer_set_rotation(data->text_layer, rotation);

    time_t now = time(NULL);
    tick_handler(localtime(&now), DAY_UNIT, this);
}

DateLayer *date_layer_create(void) {
    log_func();
    DateLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

#ifndef PBL_PLATFORM_APLITE
    data->rect_layer = fctx_rect_layer_create();
    fctx_rect_layer_set_handles(data->rect_layer, (FctxRectLayerHandles) {
        .border_color = enamel_get_DATE_RECT_BORDER_COLOR,
        .border_width = enamel_get_DATE_RECT_BORDER_WIDTH,
        .fill_color = enamel_get_DATE_RECT_FILL_COLOR,
        .alignment = enamel_get_DATE_ALIGNMENT,
        .rotation = enamel_get_DATE_ROTATION,
        .size_w = enamel_get_DATE_RECT_SIZE_W,
        .size_h = enamel_get_DATE_RECT_SIZE_H,
        .offset_x = enamel_get_DATE_X,
        .offset_y = enamel_get_DATE_Y
    });
    fctx_layer_add_child(this, data->rect_layer);
#endif

    data->text_layer = fctx_text_layer_create();
    fctx_layer_add_child(this, data->text_layer);
    fctx_text_layer_set_anchor(data->text_layer, FTextAnchorMiddle);
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(DAY_UNIT, tick_handler, this);

    return this;
}

void date_layer_destroy(DateLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    fctx_text_layer_destroy(data->text_layer);
#ifndef PBL_PLATFORM_APLITE
    fctx_rect_layer_destroy(data->rect_layer);
#endif
    fctx_layer_destroy(this);
}
