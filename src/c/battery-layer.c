#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "battery-layer.h"

typedef struct {
    char buf[8];
    FFont *font;
    EventHandle battery_state_event_handle;
    EventHandle settings_event_handle;
} Data;

static void battery_state_handler(BatteryChargeState state, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    snprintf(data->buf, sizeof(data->buf), "%d%%", state.charge_percent);
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    layer_mark_dirty(this);
}

static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FPoint offset = FPointI(enamel_get_BATTERY_X(), enamel_get_BATTERY_Y());

    fctx_set_rotation(fctx, DEG_TO_TRIGANGLE(-enamel_get_BATTERY_ROTATION()));
    fctx_set_offset(fctx, offset);
    fctx_set_text_em_height(fctx, data->font, enamel_get_BATTERY_FONT_SIZE());
    fctx_set_fill_color(fctx, enamel_get_BATTERY_COLOR());
    fctx_begin_fill(fctx);
    fctx_draw_string(fctx, data->buf, data->font, atoi(enamel_get_BATTERY_ALIGNMENT()), FTextAnchorMiddle);
    fctx_end_fill(fctx);
}

BatteryLayer *battery_layer_create() {
    log_func();
    BatteryLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->font = fonts_get(RESOURCE_ID_LECO_FFONT);

    battery_state_handler(battery_state_service_peek(), this);
    data->battery_state_event_handle = events_battery_state_service_subscribe_context(battery_state_handler, this);

    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void battery_layer_destroy(BatteryLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_battery_state_service_unsubscribe(data->battery_state_event_handle);
    fctx_layer_destroy(this);
}
