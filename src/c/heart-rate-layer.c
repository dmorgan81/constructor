#ifdef PBL_PLATFORM_DIORITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "heart-rate-layer.h"

typedef struct {
    char buf[8];
    EventHandle health_event_handle;
    EventHandle settings_event_handle;
} Data;

static void health_handler(HealthEventType event, void *this) {
    log_func();
    if (event == HealthEventSignificantUpdate) {
        health_handler(HealthEventHeartRateUpdate, this);
    } else if (event == HealthEventHeartRateUpdate) {
        Data *data = fctx_text_layer_get_data(this);
        time_t start = time_start_of_today();
        time_t end = time(NULL);
        HealthServiceAccessibilityMask mask = health_service_metric_accessible(HealthMetricHeartRateBPM, start, end);
        if (mask & HealthServiceAccessibilityMaskAvailable) {
            HealthValue value = health_service_peek_current_value(HealthMetricHeartRateBPM);
            snprintf(data->buf, sizeof(data->buf), "%ld", value);
        } else {
            data->buf[0] = '\0';
        }
        layer_mark_dirty(this);
    }
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_HEART_RATE_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_HEART_RATE_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_HEART_RATE_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_HEART_RATE_X(), enamel_get_HEART_RATE_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_HEART_RATE_ROTATION()));
}

HeartRateLayer *heart_rate_layer_create(void) {
    log_func();
    HeartRateLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
    fctx_text_layer_set_anchor(this, FTextAnchorMiddle);
    fctx_text_layer_set_font(this, fonts_get(RESOURCE_ID_LECO_FFONT));

    Data *data = fctx_text_layer_get_data(this);
    fctx_text_layer_set_text(this, data->buf);

    health_handler(HealthEventSignificantUpdate, this);
    data->health_event_handle = events_health_service_events_subscribe(health_handler, this);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void heart_rate_layer_destroy(HeartRateLayer *this) {
    log_func();
    Data *data = fctx_text_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_health_service_events_unsubscribe(data->health_event_handle);
    fctx_text_layer_destroy(this);
}
#endif // PBL_PLATFORM_DIORITE
