#ifdef PBL_HEALTH
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "step-layer.h"

typedef struct {
    char buf[32];
    FctxRectLayer *rect_layer;
    FctxTextLayer *text_layer;
    EventHandle health_event_handle;
    EventHandle settings_event_handle;
} Data;

static void health_handler(HealthEventType event, void *this) {
    log_func();
    if (event == HealthEventSignificantUpdate) {
        health_handler(HealthEventMovementUpdate, this);
    } else if (event == HealthEventMovementUpdate) {
        Data *data = fctx_layer_get_data(this);
        time_t start = time_start_of_today();
        time_t end = time(NULL);
        char s[8];
        HealthServiceAccessibilityMask mask = health_service_metric_accessible(HealthMetricStepCount, start, end);
        if (mask & HealthServiceAccessibilityMaskAvailable) {
            HealthValue sum = health_service_sum_today(HealthMetricStepCount);
            snprintf(s, sizeof(s), "%ld", sum);
        } else {
            s[0] = '\0';
        }
        snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_STEPS_PREFIX(), s, enamel_get_STEPS_SUFFIX());
        layer_mark_dirty(this);
    }
}

static void settings_handler(void *this) {
    log_func();
    health_handler(HealthEventSignificantUpdate, this);
}

StepLayer *step_layer_create(void) {
    log_func();
    StepLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

    data->rect_layer = fctx_rect_layer_create();
    fctx_rect_layer_set_handles(data->rect_layer, (FctxRectLayerHandles) {
        .border_color = enamel_get_STEPS_RECT_BORDER_COLOR,
        .border_width = enamel_get_STEPS_RECT_BORDER_WIDTH,
        .fill_color = enamel_get_STEPS_RECT_FILL_COLOR,
        .alignment = enamel_get_STEPS_ALIGNMENT,
        .rotation = enamel_get_STEPS_ROTATION,
        .size_w = enamel_get_STEPS_RECT_SIZE_W,
        .size_h = enamel_get_STEPS_RECT_SIZE_H,
        .offset_x = enamel_get_STEPS_X,
        .offset_y = enamel_get_STEPS_Y
    });
    fctx_layer_add_child(this, data->rect_layer);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_handles(data->text_layer, (FctxTextLayerHandles) {
        .fill_color = enamel_get_STEPS_COLOR,
        .alignment = enamel_get_STEPS_ALIGNMENT,
        .rotation = enamel_get_STEPS_ROTATION,
        .font_size = enamel_get_STEPS_FONT_SIZE,
        .offset_x = enamel_get_STEPS_X,
        .offset_y = enamel_get_STEPS_Y
    });
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_text(data->text_layer, data->buf);
    fctx_layer_add_child(this, data->text_layer);

    health_handler(HealthEventSignificantUpdate, this);
    data->health_event_handle = events_health_service_events_subscribe(health_handler, this);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    return this;
}

void step_layer_destroy(StepLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_health_service_events_unsubscribe(data->health_event_handle);
    fctx_text_layer_destroy(data->text_layer);
    fctx_rect_layer_destroy(data->rect_layer);
    fctx_layer_destroy(this);
}
#endif // PBL_HEALTH
