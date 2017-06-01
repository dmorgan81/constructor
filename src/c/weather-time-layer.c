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
        fctx_text_layer_set_fill_color(data->text_layer, enamel_get_WEATHER_TIME_STALE_COLOR());
    } else {
        fctx_text_layer_set_fill_color(data->text_layer, enamel_get_WEATHER_TIME_COLOR());
    }
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
    Data *data = fctx_layer_get_data(this);
    FPoint offset = FPointI(enamel_get_WEATHER_TIME_X(), enamel_get_WEATHER_TIME_Y());
    uint32_t rotation = DEG_TO_TRIGANGLE(enamel_get_WEATHER_TIME_ROTATION());
    GTextAlignment alignment = atoi(enamel_get_WEATHER_TIME_ALIGNMENT());

    fctx_rect_layer_set_fill_color(data->rect_layer, enamel_get_WEATHER_TIME_RECT_FILL_COLOR());
    FSize size = FSizeI(enamel_get_WEATHER_TIME_RECT_SIZE_W(), enamel_get_WEATHER_TIME_RECT_SIZE_H());
    fctx_rect_layer_set_size(data->rect_layer, size);
    fctx_rect_layer_set_offset(data->rect_layer, offset);
    fctx_rect_layer_set_rotation(data->rect_layer, rotation);
    fctx_rect_layer_set_border_color(data->rect_layer, enamel_get_WEATHER_TIME_RECT_BORDER_COLOR());
    fctx_rect_layer_set_border_width(data->rect_layer, enamel_get_WEATHER_TIME_RECT_BORDER_WIDTH());
    fctx_rect_layer_set_alignment(data->rect_layer, alignment);

    fctx_text_layer_set_alignment(data->text_layer, atoi(enamel_get_WEATHER_TIME_ALIGNMENT()));
    fctx_text_layer_set_em_height(data->text_layer, enamel_get_WEATHER_TIME_FONT_SIZE());
    fctx_text_layer_set_offset(data->text_layer, FPointI(enamel_get_WEATHER_TIME_X(), enamel_get_WEATHER_TIME_Y()));
    fctx_text_layer_set_rotation(data->text_layer, DEG_TO_TRIGANGLE(enamel_get_WEATHER_TIME_ROTATION()));
#ifndef PBL_COLOR
    fctx_text_layer_set_fill_color(data->text_layer, enamel_get_WEATHER_TIME_COLOR());
#endif

    weather_handler(weather_peek(), weather_status_peek(), this);

#ifdef PBL_COLOR
    time_t now = time(NULL);
    tick_handler(localtime(&now), MINUTE_UNIT, this);
#endif
}

WeatherTimeLayer *weather_time_layer_create(void) {
    log_func();
    WeatherTimeLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

    data->rect_layer = fctx_rect_layer_create();
    fctx_layer_add_child(this, data->rect_layer);

    data->text_layer = fctx_text_layer_create();
    fctx_layer_add_child(this, data->text_layer);
    fctx_text_layer_set_anchor(data->text_layer, FTextAnchorMiddle);
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);

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
