#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "time-layer.h"

typedef struct {
    char buf[8];
    EventHandle tick_timer_event_handle;
    EventHandle settings_event_handle;
} Data;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    if (enamel_get_TIME_LEADING_ZERO()) {
        strftime(data->buf, sizeof(data->buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    } else {
        strftime(data->buf, sizeof(data->buf), clock_is_24h_style() ? "%k:%M" : "%l:%M", tick_time);
    }
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_TIME_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_TIME_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_TIME_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_TIME_X(), enamel_get_TIME_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_TIME_ROTATION()));

    time_t now = time(NULL);
    tick_handler(localtime(&now), MINUTE_UNIT, this);
}

TimeLayer *time_layer_create(void) {
    log_func();
    TimeLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
    fctx_text_layer_set_anchor(this, FTextAnchorMiddle);
    fctx_text_layer_set_font(this, fonts_get(RESOURCE_ID_LECO_FFONT));

    Data *data = fctx_text_layer_get_data(this);
    fctx_text_layer_set_text(this, data->buf);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(MINUTE_UNIT, tick_handler, this);

    return this;
}

void time_layer_destroy(TimeLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    fctx_text_layer_destroy(this);
}
