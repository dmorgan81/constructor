#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "weather.h"
#include "location-layer.h"

typedef struct {
    char buf[65];
    EventHandle settings_event_handle;
    EventHandle weather_event_handle;
} Data;

static void weather_handler(GenericWeatherInfo *info, GenericWeatherStatus status, void *this) {
    log_func();
    if (status == GenericWeatherStatusAvailable) {
        Data *data = fctx_text_layer_get_data(this);
        logi("location: %s", info->name);
        snprintf(data->buf, sizeof(data->buf), "%s", info->name);
        layer_mark_dirty(this);
    }
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_LOCATION_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_LOCATION_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_LOCATION_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_LOCATION_X(), enamel_get_LOCATION_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_LOCATION_ROTATION()));


    weather_handler(weather_peek(), GenericWeatherStatusAvailable, this);
}

LocationLayer *location_layer_create(void) {
    log_func();
    LocationLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
    fctx_text_layer_set_anchor(this, FTextAnchorMiddle);
    fctx_text_layer_set_font(this, fonts_get(RESOURCE_ID_LECO_FFONT));

    Data *data = fctx_text_layer_get_data(this);
    fctx_text_layer_set_text(this, data->buf);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);
    data->weather_event_handle = events_weather_subscribe(weather_handler, this);

    return this;
}

void location_layer_destroy(LocationLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    events_weather_unsubscribe(data->weather_event_handle);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
