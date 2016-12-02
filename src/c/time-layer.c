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
} Data;

static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FRect bounds = fctx_layer_get_bounds(this);
    FPoint center = FPoint(bounds.size.w / 2, bounds.size.h / 2);

    fctx_set_offset(fctx, center);
    fctx_set_text_em_height(fctx, data->font, 40);
    fctx_set_fill_color(fctx, GColorWhite);
    fctx_begin_fill(fctx);
    fctx_draw_string(fctx, data->buf, data->font, GTextAlignmentCenter, FTextAnchorMiddle);
    fctx_end_fill(fctx);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    strftime(data->buf, sizeof(data->buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    layer_mark_dirty(this);
}

TimeLayer *time_layer_create() {
    log_func();
    TimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->font = fonts_get(RESOURCE_ID_LECO_FFONT);

    time_t now = time(NULL);
    tick_handler(localtime(&now), MINUTE_UNIT, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(MINUTE_UNIT, tick_handler, this);

    return this;
}

void time_layer_destroy(TimeLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    fctx_layer_destroy(this);
}
