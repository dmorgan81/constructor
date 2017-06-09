#include "common.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fctx-text-layer.h"

typedef struct {
    FctxTextLayerHandles handles;
    FFont *font;
    char *text;
} Data;

static void update_proc(FctxLayer *this, FContext* fctx) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FctxTextLayerHandles handles = data->handles;

    int16_t font_size = handles.font_size();
    if (data->text && data->font && font_size > 0) {
        fctx_set_text_em_height(fctx, data->font, font_size);

        uint32_t rotation = DEG_TO_TRIGANGLE(handles.rotation());
        fctx_set_rotation(fctx, rotation);

        FPoint offset = FPointI(handles.offset_x(), handles.offset_y());
#ifdef PBL_QUICK_VIEW_ENABLED
        fctx_set_offset(fctx, fpoint_add(fctx_layer_get_offset(this), offset));
#else
        fctx_set_offset(fctx, offset);
#endif

        GColor fill_color = handles.fill_color();
        fctx_set_fill_color(fctx, fill_color);

        fctx_begin_fill(fctx);
        GTextAlignment alignment = atoi(handles.alignment());
        fctx_draw_string(fctx, data->text, data->font, alignment, FTextAnchorMiddle);
        fctx_end_fill(fctx);
    }
}

FctxTextLayer *fctx_text_layer_create(void) {
    log_func();
    FctxTextLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);
    data->text = NULL;
    data->font = NULL;
    memset(&data->handles, 0, sizeof(FctxTextLayerHandles));
    return this;
}

void fctx_text_layer_destroy(FctxTextLayer *this) {
    log_func();
    fctx_layer_destroy(this);
}

void fctx_text_layer_set_handles(FctxTextLayer *this, FctxTextLayerHandles handles) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    memcpy(&data->handles, &handles, sizeof(FctxTextLayerHandles));
    layer_mark_dirty(this);
}

void fctx_text_layer_set_text(FctxTextLayer *this, char *text) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->text = text;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_font(FctxTextLayer *this, FFont *font) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->font = font;
    layer_mark_dirty(this);
}
