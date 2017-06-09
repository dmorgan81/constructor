#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "weather.h"
#include "weather-time-layer.h"

typedef struct {
    char buf[32];
    FctxRectLayer *rect_layer;
    FctxTextLayer *text_layer;
    EventHandle settings_event_handle;
    EventHandle weather_event_handle;
#ifdef PBL_COLOR
    EventHandle tick_timer_event_handle;
#endif
} Data;

#ifdef PBL_COLOR
static void tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    GenericWeatherInfo *info = weather_peek();
    time_t now = mktime(tick_time);
    uint16_t interval = atoi(enamel_get_WEATHER_INTERVAL()) * SECONDS_PER_MINUTE;
    if (now - info->timestamp > interval) {
        fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
            .fill_color = enamel_get_WEATHER_TIME_STALE_COLOR,
            .alignment = enamel_get_WEATHER_TIME_ALIGNMENT,
            .rotation = enamel_get_WEATHER_TIME_ROTATION,
            .font_size = enamel_get_WEATHER_TIME_FONT_SIZE,
            .offset_x = enamel_get_WEATHER_TIME_X,
            .offset_y = enamel_get_WEATHER_TIME_Y
        });
    } else {
        fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
            .fill_color = enamel_get_WEATHER_TIME_COLOR,
            .alignment = enamel_get_WEATHER_TIME_ALIGNMENT,
            .rotation = enamel_get_WEATHER_TIME_ROTATION,
            .font_size = enamel_get_WEATHER_TIME_FONT_SIZE,
            .offset_x = enamel_get_WEATHER_TIME_X,
            .offset_y = enamel_get_WEATHER_TIME_Y
        });
    }
    layer_mark_dirty(this);
}
#endif

static void weather_handler(GenericWeatherInfo *info, GenericWeatherStatus status, void *this) {
    log_func();
    if (status == GenericWeatherStatusAvailable) {
        Data *data = fctx_layer_get_data(this);
        struct tm *tick_time = localtime(&info->timestamp);
        char s[8];
        if (enamel_get_WEATHER_TIME_LEADING_ZERO()) {
            strftime(s, sizeof(s), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
        } else {
            strftime(s, sizeof(s), clock_is_24h_style() ? "%k:%M" : "%l:%M", tick_time);
        }
        snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_WEATHER_TIME_PREFIX(), s, enamel_get_WEATHER_TIME_SUFFIX());
        layer_mark_dirty(this);

#ifdef PBL_COLOR
        time_t now = time(NULL);
        tick_handler(localtime(&now), MINUTE_UNIT, this);
#endif
    }
}

static void settings_handler(void *this) {
    log_func();
    weather_handler(weather_peek(), weather_status_peek(), this);
}

WeatherTimeLayer *weather_time_layer_create(void) {
    log_func();
    WeatherTimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

    data->rect_layer = fctx_rect_layer_create();
    fctx_rect_layer_set_handles(data->rect_layer, (FctxRectLayerHandles) {
        .border_color = enamel_get_WEATHER_TIME_RECT_BORDER_COLOR,
        .border_width = enamel_get_WEATHER_TIME_RECT_BORDER_WIDTH,
        .fill_color = enamel_get_WEATHER_TIME_RECT_FILL_COLOR,
        .alignment = enamel_get_WEATHER_TIME_ALIGNMENT,
        .rotation = enamel_get_WEATHER_TIME_ROTATION,
        .size_w = enamel_get_WEATHER_TIME_RECT_SIZE_W,
        .size_h = enamel_get_WEATHER_TIME_RECT_SIZE_H,
        .offset_x = enamel_get_WEATHER_TIME_X,
        .offset_y = enamel_get_WEATHER_TIME_Y
    });
    fctx_layer_add_child(this, data->rect_layer);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
        .fill_color = enamel_get_WEATHER_TIME_COLOR,
        .alignment = enamel_get_WEATHER_TIME_ALIGNMENT,
        .rotation = enamel_get_WEATHER_TIME_ROTATION,
        .font_size = enamel_get_WEATHER_TIME_FONT_SIZE,
        .offset_x = enamel_get_WEATHER_TIME_X,
        .offset_y = enamel_get_WEATHER_TIME_Y
    });
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);
    fctx_layer_add_child(this, data->text_layer);

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
    Data *data = fctx_layer_get_data(this);
#ifdef PBL_COLOR
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
#endif
    events_weather_unsubscribe(data->weather_event_handle);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(data->text_layer);
    fctx_rect_layer_destroy(data->rect_layer);
    fctx_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
