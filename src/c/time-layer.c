#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "time-layer.h"

typedef struct {
    char buf[8];
    FFont *font;
    EventHandle tick_timer_event_handle;
    EventHandle settings_event_handle;
} Data;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    if (enamel_get_TIME_LEADING_ZERO()) {
        strftime(data->buf, sizeof(data->buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    } else {
        strftime(data->buf, sizeof(data->buf), clock_is_24h_style() ? "%k:%M" : "%l:%M", tick_time);
    }
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    time_t now = time(NULL);
    tick_handler(localtime(&now), MINUTE_UNIT, this);
}

static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FPoint offset = FPointI(enamel_get_TIME_X(), enamel_get_TIME_Y());

    fctx_set_rotation(fctx, DEG_TO_TRIGANGLE(-enamel_get_TIME_ROTATION()));
    fctx_set_offset(fctx, offset);
    fctx_set_text_em_height(fctx, data->font, enamel_get_TIME_FONT_SIZE());
    fctx_set_fill_color(fctx, enamel_get_TIME_COLOR());
    fctx_begin_fill(fctx);
    fctx_draw_string(fctx, data->buf, data->font, atoi(enamel_get_TIME_ALIGNMENT()), FTextAnchorMiddle);
    fctx_end_fill(fctx);
}

TimeLayer *time_layer_create(void) {
    log_func();
    TimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->font = fonts_get(RESOURCE_ID_LECO_FFONT);

    time_t now = time(NULL);
    tick_handler(localtime(&now), MINUTE_UNIT, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(MINUTE_UNIT, tick_handler, this);

    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void time_layer_destroy(TimeLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    fctx_layer_destroy(this);
}
