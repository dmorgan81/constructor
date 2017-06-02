#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "time-layer.h"

typedef struct {
    char buf[PBL_IF_LOW_MEM_ELSE(16, 48)];
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
#ifndef PBL_PLATFORM_APLITE
    char s[16];
#endif
    char format[16];
    if (enamel_get_TIME_LEADING_ZERO()) {
        snprintf(format, sizeof(format), clock_is_24h_style() ? "%%H:%%M%s" : "%%I:%%M%s", enamel_get_TIME_ENABLE_SECONDS() ? ":%S" : "");
    } else {
        snprintf(format, sizeof(format), clock_is_24h_style() ? "%%k:%%M%s" : "%%l:%%M%s", enamel_get_TIME_ENABLE_SECONDS() ? ":%S" : "");
    }
    strftime(PBL_IF_LOW_MEM_ELSE(data->buf, s), sizeof(PBL_IF_LOW_MEM_ELSE(data->buf, s)), format, tick_time);
#ifndef PBL_PLATFORM_APLITE
    snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_TIME_PREFIX(), s, enamel_get_TIME_SUFFIX());
#endif
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FPoint offset = FPointI(enamel_get_TIME_X(), enamel_get_TIME_Y());
    uint32_t rotation = DEG_TO_TRIGANGLE(enamel_get_TIME_ROTATION());
    GTextAlignment alignment = atoi(enamel_get_TIME_ALIGNMENT());

#ifndef PBL_PLATFORM_APLITE
    fctx_rect_layer_set_fill_color(data->rect_layer, enamel_get_TIME_RECT_FILL_COLOR());
    FSize size = FSizeI(enamel_get_TIME_RECT_SIZE_W(), enamel_get_TIME_RECT_SIZE_H());
    fctx_rect_layer_set_size(data->rect_layer, size);
    fctx_rect_layer_set_offset(data->rect_layer, offset);
    fctx_rect_layer_set_rotation(data->rect_layer, rotation);
    fctx_rect_layer_set_border_color(data->rect_layer, enamel_get_TIME_RECT_BORDER_COLOR());
    fctx_rect_layer_set_border_width(data->rect_layer, enamel_get_TIME_RECT_BORDER_WIDTH());
    fctx_rect_layer_set_alignment(data->rect_layer, alignment);
#endif

    fctx_text_layer_set_alignment(data->text_layer, alignment);
    fctx_text_layer_set_em_height(data->text_layer, enamel_get_TIME_FONT_SIZE());
    fctx_text_layer_set_fill_color(data->text_layer, enamel_get_TIME_COLOR());
    fctx_text_layer_set_offset(data->text_layer, offset);
    fctx_text_layer_set_rotation(data->text_layer, rotation);

    TimeUnits time_unit = enamel_get_TIME_ENABLE_SECONDS() ? SECOND_UNIT : MINUTE_UNIT;
    time_t now = time(NULL);
    tick_handler(localtime(&now), time_unit, this);
    if (data->tick_timer_event_handle) events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(time_unit, tick_handler, this);
}

TimeLayer *time_layer_create(void) {
    log_func();
    TimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

#ifndef PBL_PLATFORM_APLITE
    data->rect_layer = fctx_rect_layer_create();
    fctx_layer_add_child(this, data->rect_layer);
#endif

    data->text_layer = fctx_text_layer_create();
    fctx_layer_add_child(this, data->text_layer);
    fctx_text_layer_set_anchor(data->text_layer, FTextAnchorMiddle);
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void time_layer_destroy(TimeLayer *this) {
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
