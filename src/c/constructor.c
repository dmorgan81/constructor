#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-connection-vibes/connection-vibes.h>
#include <pebble-hourly-vibes/hourly-vibes.h>
#include "fonts.h"
#include "weather.h"
#include "fctx-layer.h"
#include "time-layer.h"
#include "date-layer.h"
#include "battery-layer.h"
#include "connection-layer.h"
#ifndef PBL_PLATFORM_APLITE
#include "quiet-time-layer.h"
#include "temperature-layer.h"
#include "condition-layer.h"
#include "weather-time-layer.h"
#include "location-layer.h"
#endif
#ifdef PBL_HEALTH
#include "step-layer.h"
#include "distance-layer.h"
#endif
#ifdef PBL_PLATFORM_DIORITE
#include "heart-rate-layer.h"
#endif

static Window *s_window;

static FctxLayer *s_root_layer;
static TimeLayer *s_time_layer;
static DateLayer *s_date_layer;
static BatteryLayer *s_battery_layer;
static ConnectionLayer *s_connection_layer;
#ifndef PBL_PLATFORM_APLITE
static QuietTimeLayer *s_quiet_time_layer;
static TemperatureLayer *s_temperature_layer;
static ConditionLayer *s_condition_layer;
static WeatherTimeLayer *s_weather_time_layer;
static LocationLayer *s_location_layer;
#endif
#ifdef PBL_HEALTH
static StepLayer *s_step_layer;
static DistanceLayer *s_distance_layer;
#endif
#ifdef PBL_PLATFORM_DIORITE
static HeartRateLayer *s_heart_rate_layer;
#endif

static EventHandle s_settings_event_handle;

static void settings_handler(void *context) {
    log_func();
    light_enable_interaction();
    window_set_background_color(s_window, enamel_get_BACKGROUND_COLOR());
    connection_vibes_set_state(atoi(enamel_get_CONNECTION_VIBE()));
    hourly_vibes_set_enabled(enamel_get_HOURLY_VIBE());

    if (enamel_get_DATE_ENABLED() && !s_date_layer) {
        s_date_layer = date_layer_create();
        fctx_layer_add_child(s_root_layer, s_date_layer);
    } else if (!enamel_get_DATE_ENABLED() && s_date_layer) {
        fctx_layer_remove_child(s_root_layer, s_date_layer);
        date_layer_destroy(s_date_layer);
        s_date_layer = NULL;
    }

    if (enamel_get_BATTERY_ENABLED() && !s_battery_layer) {
        s_battery_layer = battery_layer_create();
        fctx_layer_add_child(s_root_layer, s_battery_layer);
    } else if (!enamel_get_BATTERY_ENABLED() && s_battery_layer) {
        fctx_layer_remove_child(s_root_layer, s_battery_layer);
        battery_layer_destroy(s_battery_layer);
        s_battery_layer = NULL;
    }

    if (enamel_get_CONNECTION_ENABLED() && !s_connection_layer) {
        s_connection_layer = connection_layer_create();
        fctx_layer_add_child(s_root_layer, s_connection_layer);
    } else if (!enamel_get_CONNECTION_ENABLED() && s_connection_layer) {
        fctx_layer_remove_child(s_root_layer, s_connection_layer);
        connection_layer_destroy(s_connection_layer);
        s_connection_layer = NULL;
    }

#ifndef PBL_PLATFORM_APLITE
    if (enamel_get_QUIET_TIME_ENABLED() && !s_quiet_time_layer) {
        s_quiet_time_layer = quiet_time_layer_create();
        fctx_layer_add_child(s_root_layer, s_quiet_time_layer);
    } else if (!enamel_get_QUIET_TIME_ENABLED() && s_quiet_time_layer) {
        fctx_layer_remove_child(s_root_layer, s_quiet_time_layer);
        quiet_time_layer_destroy(s_quiet_time_layer);
        s_quiet_time_layer = NULL;
    }

    if (enamel_get_TEMPERATURE_ENABLED() && !s_temperature_layer) {
        s_temperature_layer = temperature_layer_create();
        fctx_layer_add_child(s_root_layer, s_temperature_layer);
    } else if (!enamel_get_TEMPERATURE_ENABLED() && s_temperature_layer) {
        fctx_layer_remove_child(s_root_layer, s_temperature_layer);
        temperature_layer_destroy(s_temperature_layer);
        s_temperature_layer = NULL;
    }

     if (enamel_get_CONDITION_ENABLED() && !s_condition_layer) {
        s_condition_layer = condition_layer_create();
        fctx_layer_add_child(s_root_layer, s_condition_layer);
    } else if (!enamel_get_CONDITION_ENABLED() && s_condition_layer) {
        fctx_layer_remove_child(s_root_layer, s_condition_layer);
        condition_layer_destroy(s_condition_layer);
        s_condition_layer = NULL;
    }

    if (enamel_get_WEATHER_TIME_ENABLED() && !s_weather_time_layer) {
        s_weather_time_layer = weather_time_layer_create();
        fctx_layer_add_child(s_root_layer, s_weather_time_layer);
    } else if (!enamel_get_WEATHER_TIME_ENABLED() && s_weather_time_layer) {
        fctx_layer_remove_child(s_root_layer, s_weather_time_layer);
        weather_time_layer_destroy(s_weather_time_layer);
        s_weather_time_layer = NULL;
    }

    if (enamel_get_LOCATION_ENABLED() && !s_location_layer) {
        s_location_layer = location_layer_create();
        fctx_layer_add_child(s_root_layer, s_location_layer);
    } else if (!enamel_get_LOCATION_ENABLED() && s_location_layer) {
        fctx_layer_remove_child(s_root_layer, s_location_layer);
        location_layer_destroy(s_location_layer);
        s_location_layer = NULL;
    }
#endif

#ifdef PBL_HEALTH
    if (enamel_get_STEPS_ENABLED() && !s_step_layer) {
        s_step_layer = step_layer_create();
        fctx_layer_add_child(s_root_layer, s_step_layer);
    } else if (!enamel_get_STEPS_ENABLED() && s_step_layer) {
        fctx_layer_remove_child(s_root_layer, s_step_layer);
        step_layer_destroy(s_step_layer);
        s_step_layer = NULL;
    }

    if (enamel_get_DISTANCE_ENABLED() && !s_distance_layer) {
        s_distance_layer = distance_layer_create();
        fctx_layer_add_child(s_root_layer, s_distance_layer);
    } else if (!enamel_get_DISTANCE_ENABLED() && s_distance_layer) {
        fctx_layer_remove_child(s_root_layer, s_distance_layer);
        distance_layer_destroy(s_distance_layer);
        s_distance_layer = NULL;
    }

#ifdef PBL_PLATFORM_DIORITE
    if (enamel_get_HEART_RATE_ENABLED() && !s_heart_rate_layer) {
        s_heart_rate_layer = heart_rate_layer_create();
        fctx_layer_add_child(s_root_layer, s_heart_rate_layer);
    } else if (!enamel_get_HEART_RATE_ENABLED() && s_heart_rate_layer) {
        fctx_layer_remove_child(s_root_layer, s_heart_rate_layer);
        heart_rate_layer_destroy(s_heart_rate_layer);
        s_heart_rate_layer = NULL;
    }

    connection_vibes_enable_health(s_step_layer != NULL || s_distance_layer != NULL || s_heart_rate_layer != NULL);
    hourly_vibes_enable_health(s_step_layer != NULL || s_distance_layer != NULL || s_heart_rate_layer != NULL);
#else
    connection_vibes_enable_health(s_step_layer != NULL || s_distance_layer != NULL);
    hourly_vibes_enable_health(s_step_layer != NULL || s_distance_layer != NULL);
#endif // PBL_PLATFORM_DIORITE
#endif // PBL_HEATH

    // Ensure time layer is last layer so it draw "above" every other layer
    fctx_layer_remove_child(s_root_layer, s_time_layer);
    fctx_layer_add_child(s_root_layer, s_time_layer);
}

static void window_load(Window *window) {
    log_func();
    s_root_layer = fctx_layer_get_root_layer(window);

    s_time_layer = time_layer_create();
    fctx_layer_add_child(s_root_layer, s_time_layer);

    settings_handler(NULL);
    s_settings_event_handle = enamel_settings_received_subscribe(settings_handler, NULL);

    logd("used heap: %d", heap_bytes_used());
    logd("free heap: %d", heap_bytes_free());
}

static void window_unload(Window *window) {
    log_func();
    enamel_settings_received_unsubscribe(s_settings_event_handle);

#ifdef PBL_PLATFORM_DIORITE
    if (s_heart_rate_layer) heart_rate_layer_destroy(s_heart_rate_layer);
#endif // PBL_PLATFORM_DIORITE
#ifdef PBL_HEALTH
    if (s_distance_layer) distance_layer_destroy(s_distance_layer);
    if (s_step_layer) step_layer_destroy(s_step_layer);
#endif // PBL_HEALTH
#ifndef PBL_PLATFORM_APLITE
    if (s_quiet_time_layer) quiet_time_layer_destroy(s_quiet_time_layer);
    if (s_temperature_layer) temperature_layer_destroy(s_temperature_layer);
    if (s_condition_layer) condition_layer_destroy(s_condition_layer);
    if (s_weather_time_layer) weather_time_layer_destroy(s_weather_time_layer);
    if (s_location_layer) location_layer_destroy(s_location_layer);
#endif // PBL_PLATFORM_APLITE
    if (s_connection_layer) connection_layer_destroy(s_connection_layer);
    if (s_battery_layer) battery_layer_destroy(s_battery_layer);
    if (s_date_layer) date_layer_destroy(s_date_layer);
    time_layer_destroy(s_time_layer);
    fctx_layer_destroy(s_root_layer);
}

static void init(void) {
    log_func();
    fonts_init();
    enamel_init();
#ifndef PBL_PLATFORM_APLITE
    weather_init();
#endif
    connection_vibes_init();
    hourly_vibes_init();
    uint32_t const pattern[] = { 100 };
    hourly_vibes_set_pattern((VibePattern) {
        .durations = pattern,
        .num_segments = 1
    });

    events_app_message_open();

    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });
    window_stack_push(s_window, true);
}

static void deinit(void) {
    log_func();
    window_destroy(s_window);

    hourly_vibes_deinit();
    connection_vibes_deinit();
#ifndef PBL_PLATFORM_APLITE
    weather_deinit();
#endif
    fonts_deinit();
    enamel_deinit();
}

int main(void) {
    log_func();
    init();
    app_event_loop();
    deinit();
}
