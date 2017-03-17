#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-fctx/fctx.h>
#include <pebble-generic-weather/pebble-generic-weather.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "weather.h"
#include "condition-layer.h"

typedef struct {
    char buf[8];
    EventHandle settings_event_handle;
    EventHandle weather_event_handle;
} Data;

static void weather_handler(GenericWeatherInfo *info, GenericWeatherStatus status, void *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    if (status == GenericWeatherStatusAvailable) {
        switch (info->condition) {
             case GenericWeatherConditionClearSky:
                snprintf(data->buf, sizeof(data->buf), "Clear");
                break;
            case GenericWeatherConditionFewClouds:
            case GenericWeatherConditionScatteredClouds:
            case GenericWeatherConditionBrokenClouds:
                snprintf(data->buf, sizeof(data->buf), "Clouds");
                break;
            case GenericWeatherConditionShowerRain:
            case GenericWeatherConditionRain:
                snprintf(data->buf, sizeof(data->buf), "Rain");
                break;
            case GenericWeatherConditionThunderstorm:
                snprintf(data->buf, sizeof(data->buf), "Storm");
                break;
            case GenericWeatherConditionSnow:
                snprintf(data->buf, sizeof(data->buf), "Snow");
                break;
            case GenericWeatherConditionMist:
                snprintf(data->buf, sizeof(data->buf), "Mist");
                break;
            default:
                snprintf(data->buf, sizeof(data->buf), "Unknown");
                break;
        }
    } else if (status != GenericWeatherStatusPending) {
        snprintf(data->buf, sizeof(data->buf), "Unknown");
    }
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_CONDITION_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_CONDITION_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_CONDITION_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_CONDITION_X(), enamel_get_CONDITION_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_CONDITION_ROTATION()));
}

ConditionLayer *condition_layer_create(void) {
    log_func();
    ConditionLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
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

void condition_layer_destroy(ConditionLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    events_weather_unsubscribe(data->weather_event_handle);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
