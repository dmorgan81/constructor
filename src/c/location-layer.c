#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "weather.h"
#include "location-layer.h"

typedef struct {
    char buf[128];
    FctxRectLayer *rect_layer;
    FctxTextLayer *text_layer;
    EventHandle settings_event_handle;
    EventHandle weather_event_handle;
} Data;

static void weather_handler(GenericWeatherInfo *info, GenericWeatherStatus status, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    if (status == GenericWeatherStatusAvailable) {
        snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_LOCATION_PREFIX(), info->name, enamel_get_LOCATION_SUFFIX());
        layer_mark_dirty(this);
    } else if (status != GenericWeatherStatusPending) {
        data->buf[0] = '\0';
    }
}

static void settings_handler(void *this) {
    log_func();
    weather_handler(weather_peek(), weather_status_peek(), this);
}

LocationLayer *location_layer_create(void) {
    log_func();
    LocationLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

    data->rect_layer = fctx_rect_layer_create();
    fctx_rect_layer_set_handles(data->rect_layer, (FctxRectLayerHandles) {
        .border_color = enamel_get_LOCATION_RECT_BORDER_COLOR,
        .border_width = enamel_get_LOCATION_RECT_BORDER_WIDTH,
        .fill_color = enamel_get_LOCATION_RECT_FILL_COLOR,
        .alignment = enamel_get_LOCATION_ALIGNMENT,
        .rotation = enamel_get_LOCATION_ROTATION,
        .size_w = enamel_get_LOCATION_RECT_SIZE_W,
        .size_h = enamel_get_LOCATION_RECT_SIZE_H,
        .offset_x = enamel_get_LOCATION_X,
        .offset_y = enamel_get_LOCATION_Y
    });
    fctx_layer_add_child(this, data->rect_layer);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
        .fill_color = enamel_get_LOCATION_COLOR,
        .alignment = enamel_get_LOCATION_ALIGNMENT,
        .rotation = enamel_get_LOCATION_ROTATION,
        .font_size = enamel_get_LOCATION_FONT_SIZE,
        .offset_x = enamel_get_LOCATION_X,
        .offset_y = enamel_get_LOCATION_Y
    });
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);
    fctx_layer_add_child(this, data->text_layer);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);
    data->weather_event_handle = events_weather_subscribe(weather_handler, this);

    return this;
}

void location_layer_destroy(LocationLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    events_weather_unsubscribe(data->weather_event_handle);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(data->text_layer);
    fctx_rect_layer_destroy(data->rect_layer);
    fctx_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
