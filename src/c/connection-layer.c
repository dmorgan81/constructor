#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "fctx-rect-layer.h"
#include "fctx-text-layer.h"
#include "connection-layer.h"

typedef struct {
#ifndef PBL_PLATFORM_APLITE
    char buf[32];
    FctxRectLayer *rect_layer;
#endif
    FctxTextLayer *text_layer;
    EventHandle connection_event_handle;
    EventHandle settings_event_handle;
} Data;

static void connection_handler(bool connected, void *this) {
    log_func();
    layer_set_hidden(this, !connected);
}

#ifndef PBL_PLATFORM_APLITE
static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    if (enamel_get_CONNECTION_QT_HIDDEN() && quiet_time_is_active()) return;
    Data *data = fctx_layer_get_data(this);
    fctx_layer_update_proc(data->rect_layer, fctx);
    fctx_layer_update_proc(data->text_layer, fctx);
}
#endif

static void settings_handler(void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FPoint offset = FPointI(enamel_get_CONNECTION_X(), enamel_get_CONNECTION_Y());
    uint32_t rotation = DEG_TO_TRIGANGLE(enamel_get_CONNECTION_ROTATION());
    GTextAlignment alignment = atoi(enamel_get_CONNECTION_ALIGNMENT());

#ifndef PBL_PLATFORM_APLITE
    fctx_rect_layer_set_fill_color(data->rect_layer, enamel_get_CONNECTION_RECT_FILL_COLOR());
    FSize size = FSizeI(enamel_get_CONNECTION_RECT_SIZE_W(), enamel_get_CONNECTION_RECT_SIZE_H());
    fctx_rect_layer_set_size(data->rect_layer, size);
    fctx_rect_layer_set_offset(data->rect_layer, offset);
    fctx_rect_layer_set_rotation(data->rect_layer, rotation);
    fctx_rect_layer_set_border_color(data->rect_layer, enamel_get_CONNECTION_RECT_BORDER_COLOR());
    fctx_rect_layer_set_border_width(data->rect_layer, enamel_get_CONNECTION_RECT_BORDER_WIDTH());
    fctx_rect_layer_set_alignment(data->rect_layer, alignment);

    snprintf(data->buf, sizeof(data->buf), "%s%s%s", enamel_get_CONNECTION_PREFIX(), "BT", enamel_get_CONNECTION_SUFFIX());
#endif

    fctx_text_layer_set_alignment(data->text_layer, alignment);
    fctx_text_layer_set_em_height(data->text_layer, enamel_get_CONNECTION_FONT_SIZE());
    fctx_text_layer_set_fill_color(data->text_layer, enamel_get_CONNECTION_COLOR());
    fctx_text_layer_set_offset(data->text_layer, offset);
    fctx_text_layer_set_rotation(data->text_layer, rotation);
}

ConnectionLayer *connection_layer_create(void) {
    log_func();
    ConnectionLayer *this = fctx_layer_create_with_data(sizeof(Data));
    Data *data = fctx_layer_get_data(this);

#ifndef PBL_PLATFORM_APLITE
    fctx_layer_set_update_proc(this, update_proc);

    data->rect_layer = fctx_rect_layer_create();
#endif

    data->text_layer = fctx_text_layer_create();
#ifdef PBL_PLATFORM_APLITE
    fctx_text_layer_set_text(data->text_layer, "BT");
    fctx_layer_add_child(this, data->text_layer);
#else
    fctx_text_layer_set_text(data->text_layer, data->buf);
#endif
    fctx_text_layer_set_font(data->text_layer, fonts_get(RESOURCE_ID_LECO_FFONT));
    fctx_text_layer_set_anchor(data->text_layer, FTextAnchorMiddle);

    settings_handler(this);
    data->settings_event_handle = enamel_settings_received_subscribe(settings_handler, this);

    connection_handler(connection_service_peek_pebble_app_connection(), this);
    data->connection_event_handle = events_connection_service_subscribe_context((EventConnectionHandlers) {
        .pebble_app_connection_handler = connection_handler
    }, this);

    return this;
}

void connection_layer_destroy(ConnectionLayer *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_connection_service_unsubscribe(data->connection_event_handle);
    fctx_text_layer_destroy(data->text_layer);
#ifndef PBL_PLATFORM_APLITE
    fctx_rect_layer_destroy(data->rect_layer);
#endif
    fctx_layer_destroy(this);
}
