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
#ifdef PBL_COLOR
    EventHandle tick_timer_event_handle;
#endif
} Data;

#ifdef PBL_COLOR
static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    GenericWeatherInfo *info = weather_peek();
    time_t now = mktime(tick_time);
    uint16_t interval = atoi(enamel_get_WEATHER_INTERVAL()) * SECONDS_PER_MINUTE;
    if (now - info->timestamp > interval) {
        fctx_text_layer_set_fill_color(this, enamel_get_WEATHER_TIME_STALE_COLOR());
    } else {
        fctx_text_layer_set_fill_color(this, enamel_get_WEATHER_TIME_COLOR());
    }
}
#endif

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

#ifdef PBL_COLOR
        time_t now = time(NULL);
        tick_handler(localtime(&now), MINUTE_UNIT, this);
#endif
    }
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_WEATHER_TIME_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_WEATHER_TIME_FONT_SIZE());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_WEATHER_TIME_X(), enamel_get_WEATHER_TIME_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_WEATHER_TIME_ROTATION()));
#ifndef PBL_COLOR
    fctx_text_layer_set_fill_color(this, enamel_get_WEATHER_TIME_COLOR());
#endif

    weather_handler(weather_peek(), GenericWeatherStatusAvailable, this);

#ifdef PBL_COLOR
    time_t now = time(NULL);
    tick_handler(localtime(&now), MINUTE_UNIT, this);
#endif
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
    data->weather_event_handle = events_weather_subscribe(weather_handler, this);
#ifdef PBL_COLOR
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(MINUTE_UNIT, tick_handler, this);
#endif

    return this;
}

void weather_time_layer_destroy(WeatherTimeLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
#ifdef PBL_COLOR
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
#endif
    events_weather_unsubscribe(data->weather_event_handle);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
