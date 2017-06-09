#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-fctx/fctx.h>
#include <pebble-generic-weather/pebble-generic-weather.h>
#include "fonts.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "weather.h"
#include "condition-layer.h"

typedef struct {
    char buf[32];
    FctxRectLayer *rect_layer;
    FctxTextLayer *text_layer;
    EventHandle settings_event_handle;
    EventHandle weather_event_handle;
} Data;

static void weather_handler(GenericWeatherInfo *info, GenericWeatherStatus status, void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    char s[8];
    if (status == GenericWeatherStatusAvailable) {
        switch (info->condition) {
             case GenericWeatherConditionClearSky:
                snprintf(s, sizeof(s), "Clear");
                break;
            case GenericWeatherConditionFewClouds:
            case GenericWeatherConditionScatteredClouds:
            case GenericWeatherConditionBrokenClouds:
                snprintf(s, sizeof(s), "Clouds");
                break;
            case GenericWeatherConditionShowerRain:
            case GenericWeatherConditionRain:
                snprintf(s, sizeof(s), "Rain");
                break;
            case GenericWeatherConditionThunderstorm:
                snprintf(s, sizeof(s), "Storm");
                break;
            case GenericWeatherConditionSnow:
                snprintf(s, sizeof(s), "Snow");
                break;
            case GenericWeatherConditionMist:
                snprintf(s, sizeof(s), "Mist");
                break;
            default:
                snprintf(s, sizeof(s), "Unknown");
                break;
        }
    } else if (status != GenericWeatherStatusPending) {
        snprintf(s, sizeof(s), "ERROR");
    } else {
        snprintf(s, sizeof(s), "Pending");
    }
    snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_CONDITION_PREFIX(), s, enamel_get_CONDITION_SUFFIX());
    layer_mark_dirty(this);
}

static void settings_handler(void *this) {
    log_func();
    weather_handler(weather_peek(), weather_status_peek(), this);
}

ConditionLayer *condition_layer_create(void) {
    log_func();
    ConditionLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

    data->rect_layer = fctx_rect_layer_create();
    fctx_rect_layer_set_handles(data->rect_layer, (FctxRectLayerHandles) {
        .border_color = enamel_get_CONDITION_RECT_BORDER_COLOR,
        .border_width = enamel_get_CONDITION_RECT_BORDER_WIDTH,
        .fill_color = enamel_get_CONDITION_RECT_FILL_COLOR,
        .alignment = enamel_get_CONDITION_ALIGNMENT,
        .rotation = enamel_get_CONDITION_ROTATION,
        .size_w = enamel_get_CONDITION_RECT_SIZE_W,
        .size_h = enamel_get_CONDITION_RECT_SIZE_H,
        .offset_x = enamel_get_CONDITION_X,
        .offset_y = enamel_get_CONDITION_Y
    });
    fctx_layer_add_child(this, data->rect_layer);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
        .fill_color = enamel_get_CONDITION_COLOR,
        .alignment = enamel_get_CONDITION_ALIGNMENT,
        .rotation = enamel_get_CONDITION_ROTATION,
        .font_size = enamel_get_CONDITION_FONT_SIZE,
        .offset_x = enamel_get_CONDITION_X,
        .offset_y = enamel_get_CONDITION_Y
    });
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);
    fctx_layer_add_child(this, data->text_layer);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    weather_handler(weather_peek(), weather_status_peek(), this);
    data->weather_event_handle = events_weather_subscribe(weather_handler, this);

    return this;
}

void condition_layer_destroy(ConditionLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    events_weather_unsubscribe(data->weather_event_handle);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    fctx_text_layer_destroy(data->text_layer);
    fctx_rect_layer_destroy(data->rect_layer);
    fctx_layer_destroy(this);
}
#endif // PBL_PLATFORM_APLITE
