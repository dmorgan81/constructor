#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-connection-vibes/connection-vibes.h>
#include <pebble-hourly-vibes/hourly-vibes.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "time-layer.h"

static Window *s_window;

static FctxLayer *s_root_layer;
static TimeLayer *s_time_layer;

static EventHandle s_settings_event_handle;

static void settings_handler(void *context) {
    log_func();
    window_set_background_color(s_window, enamel_get_BACKGROUND_COLOR());
    connection_vibes_set_state(atoi(enamel_get_CONNECTION_VIBE()));
    hourly_vibes_set_enabled(enamel_get_HOURLY_VIBE());
}

static void window_load(Window *window) {
    log_func();
    s_root_layer = fctx_layer_get_root_layer(window);

    s_time_layer = time_layer_create();
    fctx_layer_add_child(s_root_layer, s_time_layer);

    settings_handler(NULL);
    s_settings_event_handle = enamel_settings_received_subscribe(settings_handler, NULL);
}

static void window_unload(Window *window) {
    log_func();
    enamel_settings_received_unsubscribe(s_settings_event_handle);

    time_layer_destroy(s_time_layer);
    fctx_layer_destroy(s_root_layer);
}

static void init(void) {
    log_func();
    fonts_init();
    enamel_init();
    connection_vibes_init();
    hourly_vibes_init();
    uint32_t const pattern[] = { 100 };
    hourly_vibes_set_pattern((VibePattern) {
        .durations = pattern,
        .num_segments = 1
    });

#ifdef PBL_HEALTH
    connection_vibes_enable_health(true);
    hourly_vibes_enable_health(true);
#endif

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
    enamel_deinit();
    fonts_deinit();
}

int main(void) {
    log_func();
    init();
    app_event_loop();
    deinit();
}
