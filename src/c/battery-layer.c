#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "battery-layer.h"

typedef struct {
    char buf[PBL_IF_LOW_MEM_ELSE(8, 32)];
#ifndef PBL_PLATFORM_APLITE
    FctxRectLayer *rect_layer;
#endif
    FctxTextLayer *text_layer;
    EventHandle battery_state_event_handle;
    EventHandle settings_event_handle;
} Data;

static void battery_state_handler(BatteryChargeState state, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
#ifdef PBL_PLATFORM_APLITE
    snprintf(data->buf, sizeof(data->buf), "%d%%", state.charge_percent);
#else
    snprintf(data->buf, sizeof(data->buf), "%s%d%s", enamel_get_BATTERY_PREFIX(), state.charge_percent, enamel_get_BATTERY_SUFFIX());
#endif
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    battery_state_handler(battery_state_service_peek(), this);
}

BatteryLayer *battery_layer_create(void) {
    log_func();
    BatteryLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

#ifndef PBL_PLATFORM_APLITE
    data->rect_layer = fctx_rect_layer_create();
    fctx_rect_layer_set_handles(data->rect_layer, (FctxRectLayerHandles) {
        .border_color = enamel_get_BATTERY_RECT_BORDER_COLOR,
        .border_width = enamel_get_BATTERY_RECT_BORDER_WIDTH,
        .fill_color = enamel_get_BATTERY_RECT_FILL_COLOR,
        .alignment = enamel_get_BATTERY_ALIGNMENT,
        .rotation = enamel_get_BATTERY_ROTATION,
        .size_w = enamel_get_BATTERY_RECT_SIZE_W,
        .size_h = enamel_get_BATTERY_RECT_SIZE_H,
        .offset_x = enamel_get_BATTERY_X,
        .offset_y = enamel_get_BATTERY_Y
    });
    fctx_layer_add_child(this, data->rect_layer);
#endif

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
        .fill_color = enamel_get_BATTERY_COLOR,
        .alignment = enamel_get_BATTERY_ALIGNMENT,
        .rotation = enamel_get_BATTERY_ROTATION,
        .font_size = enamel_get_BATTERY_FONT_SIZE,
        .offset_x = enamel_get_BATTERY_X,
        .offset_y = enamel_get_BATTERY_Y
    });
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);
    fctx_layer_add_child(this, data->text_layer);

    battery_state_handler(battery_state_service_peek(), this);
    data->battery_state_event_handle = events_battery_state_service_subscribe_context(battery_state_handler, this);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void battery_layer_destroy(BatteryLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_battery_state_service_unsubscribe(data->battery_state_event_handle);
    fctx_text_layer_destroy(data->text_layer);
#ifndef PBL_PLATFORM_APLITE
    fctx_rect_layer_destroy(data->rect_layer);
#endif
    fctx_layer_destroy(this);
}
