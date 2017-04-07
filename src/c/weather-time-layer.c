#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "weather.h"
#include "weather-time-layer.h"

typedef struct {
    char buf[8];
    EventHandle settings_event_handle;
    EventHandle weather_event_handle;
} Data;

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_WEATHER_TIME_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_WEATHER_TIME_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_WEATHER_TIME_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_WEATHER_TIME_X(), enamel_get_WEATHER_TIME_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_WEATHER_TIME_ROTATION()));
}

static void weather_handler(GenericWeatherInfo *info, GenericWeatherStatus status, void *this) {
    log_func();
    if (status == GenericWeatherStatusAvailable) {
        Data *data = fctx_text_layer_get_data(this);
        struct tm *tick_time = localtime(&info->timestamp);
        if (enamel_get_WEATHER_TIME_LEADING_ZERO()) {
            strftime(data->buf, sizeof(data->buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
        } else {
            strftime(data->buf, sizeof(data->buf), clock_is_24h_style() ? "%k:%M" : "%l:%M", tick_time);
        }
        layer_mark_dirty(this);
    }
}

WeatherTimeLayer *weather_time_layer_create(void) {
    log_func();
    WeatherTimeLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
    fctx_text_layer_set_anchor(this, FTextAnchorMiddle);
    fctx_text_layer_set_font(this, fonts_get(RESOURCE_ID_LECO_FFONT));

    Data *data = fctx_text_layer_get_data(this);
    fctx_text_layer_set_text(this, data->buf);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    weather_handler(weather_peek(), GenericWeatherStatusAvailable, this);
    data->weather_event_handle = events_weather_subscribe(weather_handler, this);

    return this;
}

void weather_time_layer_destroy(WeatherTimeLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    events_weather_unsubscribe(data->weather_event_handle);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
