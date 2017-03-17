#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "battery-layer.h"

typedef struct {
    char buf[8];
    EventHandle battery_state_event_handle;
    EventHandle settings_event_handle;
} Data;

static void battery_state_handler(BatteryChargeState state, void *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    snprintf(data->buf, sizeof(data->buf), "%d%%", state.charge_percent);
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_BATTERY_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_BATTERY_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_BATTERY_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_BATTERY_X(), enamel_get_BATTERY_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_BATTERY_ROTATION()));
}

BatteryLayer *battery_layer_create(void) {
    log_func();
    BatteryLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
    fctx_text_layer_set_anchor(this, FTextAnchorMiddle);
    fctx_text_layer_set_font(this, fonts_get(RESOURCE_ID_LECO_FFONT));

    Data *data = fctx_text_layer_get_data(this);
    fctx_text_layer_set_text(this, data->buf);

    battery_state_handler(battery_state_service_peek(), this);
    data->battery_state_event_handle = events_battery_state_service_subscribe_context(battery_state_handler, this);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void battery_layer_destroy(BatteryLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_battery_state_service_unsubscribe(data->battery_state_event_handle);
    fctx_text_layer_destroy(this);
}
