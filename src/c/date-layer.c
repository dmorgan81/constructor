#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "date-layer.h"

typedef struct {
    char buf[48];
    FFont *font;
    EventHandle tick_timer_event_handle;
    EventHandle settings_event_handle;
} Data;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    strftime(data->buf, sizeof(data->buf), enamel_get_DATE_FORMAT(), tick_time);
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
    FRect bounds = fctx_layer_get_bounds(this);
    FPoint center = FPoint(bounds.size.w / 2, bounds.size.h / 2);
    FPoint offset = FPoint(INT_TO_FIXED(enamel_get_DATE_X()), INT_TO_FIXED(enamel_get_DATE_Y()));

    fctx_set_offset(fctx, fpoint_add(center, offset));
    fctx_set_text_em_height(fctx, data->font, enamel_get_DATE_FONT_SIZE());
    fctx_set_fill_color(fctx, enamel_get_DATE_COLOR());
    fctx_begin_fill(fctx);
    fctx_draw_string(fctx, data->buf, data->font, atoi(enamel_get_DATE_ALIGNMENT()), FTextAnchorMiddle);
    fctx_end_fill(fctx);
}

DateLayer *date_layer_create(void) {
    log_func();
    DateLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->font = fonts_get(RESOURCE_ID_LECO_FFONT);

    time_t now = time(NULL);
    tick_handler(localtime(&now), DAY_UNIT, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(DAY_UNIT, tick_handler, this);

    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void date_layer_destroy(DateLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    fctx_layer_destroy(this);
}
