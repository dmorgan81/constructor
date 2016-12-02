#include "common.h"
#include "fonts.h"
#include "fctx-layer.h"
#include "time-layer.h"

static Window *s_window;

static FctxLayer *s_root_layer;
static TimeLayer *s_time_layer;

static void window_load(Window *window) {
    log_func();
    window_set_background_color(window, GColorBlack);
    s_root_layer = fctx_layer_get_root_layer(window);

    s_time_layer = time_layer_create();
    fctx_layer_add_child(s_root_layer, s_time_layer);
}

static void window_unload(Window *window) {
    log_func();
    time_layer_destroy(s_time_layer);
    fctx_layer_destroy(s_root_layer);
}

static void init(void) {
    log_func();
    fonts_init();

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

    fonts_deinit();
}

int main(void) {
    log_func();
    init();
    app_event_loop();
    deinit();
}
