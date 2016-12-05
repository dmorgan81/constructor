#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include "fonts.h"
#include "fctx-text-layer.h"
#include "connection-layer.h"

typedef struct {
    EventHandle connection_event_handle;
    EventHandle settings_event_handle;
} Data;

static void connection_handler(bool connected, void *this) {
    log_func();
    layer_set_hidden(this, !connected);
}

static void settings_handler(void *this) {
    log_func();
    fctx_text_layer_set_alignment(this, atoi(enamel_get_CONNECTION_ALIGNMENT()));
    fctx_text_layer_set_em_height(this, enamel_get_CONNECTION_FONT_SIZE());
    fctx_text_layer_set_fill_color(this, enamel_get_CONNECTION_COLOR());
    fctx_text_layer_set_offset(this, FPointI(enamel_get_CONNECTION_X(), enamel_get_CONNECTION_Y()));
    fctx_text_layer_set_rotation(this, DEG_TO_TRIGANGLE(enamel_get_CONNECTION_ROTATION()));
}

ConnectionLayer *connection_layer_create(void) {
    log_func();
    ConnectionLayer *this = fctx_text_layer_create_with_data(sizeof(Data));
    fctx_text_layer_set_anchor(this, FTextAnchorMiddle);

    Data *data = fctx_text_layer_get_data(this);
    fctx_text_layer_set_text(this, "BT");
    fctx_text_layer_set_font(this, fonts_get(RESOURCE_ID_LECO_FFONT));

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
    Data *data = fctx_text_layer_get_data(this);
    enamel_settings_received_unsubscribe(data->settings_event_handle);
    events_connection_service_unsubscribe(data->connection_event_handle);
    fctx_text_layer_destroy(this);
}
