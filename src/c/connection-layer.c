#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-layer.h"
#include "fctx-text-layer.h"
#include "connection-layer.h"

typedef struct {
    FctxTextLayer *text_layer;
    EventHandle connection_event_handle;
    EventHandle settings_event_handle;
} Data;

static void connection_handler(bool connected, void *this) {
    log_func();
    layer_set_hidden(this, !connected);
}

static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    if (enamel_get_CONNECTION_QT_HIDDEN() && quiet_time_is_active()) return;
    Data *data = fctx_layer_get_data(this);
    fctx_text_layer_draw(data->text_layer, fctx);
}

static void settings_handler(void *this) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    fctx_text_layer_set_alignment(data->text_layer, atoi(enamel_get_CONNECTION_ALIGNMENT()));
    fctx_text_layer_set_em_height(data->text_layer, enamel_get_CONNECTION_FONT_SIZE());
    fctx_text_layer_set_fill_color(data->text_layer, enamel_get_CONNECTION_COLOR());
    fctx_text_layer_set_offset(data->text_layer, FPointI(enamel_get_CONNECTION_X(), enamel_get_CONNECTION_Y()));
    fctx_text_layer_set_rotation(data->text_layer, DEG_TO_TRIGANGLE(enamel_get_CONNECTION_ROTATION()));
}

ConnectionLayer *connection_layer_create(void) {
    log_func();
    ConnectionLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);

    data->text_layer = fctx_text_layer_create();
    fctx_text_layer_set_text(data->text_layer, "BT");
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
    fctx_layer_destroy(this);
}
