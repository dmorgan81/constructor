#include "common.h"
#include <pebble-fctx/fctx.h>
#include "fctx-text-layer.h"

typedef struct {
    GColor fill_color;
    uint32_t rotation;
    FPoint offset;
    FFont *font;
    int16_t em_height;
    GTextAlignment alignment;
    FTextAnchor anchor;
    char *text;
} Data;

void fctx_text_layer_draw(FctxLayer *this, FContext* fctx) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    if (data->text && data->font && data->em_height > 0) {
        fctx_set_rotation(fctx, data->rotation);
        fctx_set_offset(fctx, data->offset);
        fctx_set_text_em_height(fctx, data->font, data->em_height);
        fctx_set_fill_color(fctx, data->fill_color);
        fctx_begin_fill(fctx);
        fctx_draw_string(fctx, data->text, data->font, data->alignment, data->anchor);
        fctx_end_fill(fctx);
    }
}

FctxTextLayer *fctx_text_layer_create(void) {
    log_func();
    FctxTextLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, fctx_text_layer_draw);
    Data *data = fctx_layer_get_data(this);
    data->text = NULL;
    data->fill_color = GColorBlack;
    data->rotation = 0;
    data->offset = FPointZero;
    data->em_height = 0;
    data->alignment = GTextAlignmentLeft;
    data->anchor = FTextAnchorBaseline;
    return this;
}

void fctx_text_layer_destroy(FctxTextLayer *this) {
    log_func();
    fctx_layer_destroy(this);
}

void fctx_text_layer_set_text(FctxTextLayer *this, char *text) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->text = text;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_fill_color(FctxTextLayer *this, GColor color) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->fill_color = color;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_rotation(FctxTextLayer *this, uint32_t rotation) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->rotation = rotation;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_offset(FctxTextLayer *this, FPoint offset) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->offset = offset;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_font(FctxTextLayer *this, FFont *font) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->font = font;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_em_height(FctxTextLayer *this, int16_t pixels) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->em_height = pixels;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_alignment(FctxTextLayer *this, GTextAlignment alignment) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->alignment = alignment;
    layer_mark_dirty(this);
}

void fctx_text_layer_set_anchor(FctxTextLayer *this, FTextAnchor anchor) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->anchor = anchor;
    layer_mark_dirty(this);
}
