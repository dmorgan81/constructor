#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "date-layer.h"

typedef struct {
    char buf[48];
    EventHandle tick_timer_event_handle;
    EventHandle settings_event_handle;
} Data;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    strftime(data->buf, sizeof(data->buf), enamel_get_DATE_FORMAT(), tick_time);
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_DATE_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_DATE_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_DATE_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_DATE_X(), enamel_get_DATE_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_DATE_ROTATION()));

    time_t now = time(NULL);
    tick_handler(localtime(&now), DAY_UNIT, this);
}

DateLayer *date_layer_create(void) {
    log_func();
    DateLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
    fctx_text_layer_set_anchor(this, FTextAnchorMiddle);
    fctx_text_layer_set_font(this, fonts_get(RESOURCE_ID_LECO_FFONT));

    Data *data = fctx_text_layer_get_data(this);
    fctx_text_layer_set_text(this, data->buf);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(DAY_UNIT, tick_handler, this);

    return this;
}

void date_layer_destroy(DateLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    fctx_text_layer_destroy(this);
}
