#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-fctx/fctx.h>
#include "fctx-rect-layer.h"

typedef struct {
    GColor border_color;
    uint16_t border_width;
    GColor fill_color;
    GTextAlignment alignment;
    uint32_t rotation;
    FSize size;
    FPoint offset;
} Data;

static void fctx_draw_rect(FContext *fctx, GTextAlignment alignment, FSize size) {
    log_func();
    fixed_t y = size.h / 2;

    fctx_begin_fill(fctx);
    if (alignment == GTextAlignmentCenter) {
        fixed_t x = size.w / 2;
        fctx_move_to(fctx, FPoint(-x, -y));
        fctx_line_to(fctx, FPoint(x, -y));
        fctx_line_to(fctx, FPoint(x, y));
        fctx_line_to(fctx, FPoint(-x, y));
    } else if (alignment == GTextAlignmentLeft) {
        fctx_move_to(fctx, FPoint(0, -y));
        fctx_line_to(fctx, FPoint(size.w, -y));
        fctx_line_to(fctx, FPoint(size.w, y));
        fctx_line_to(fctx, FPoint(0, y));
    } else {
        fctx_move_to(fctx, FPoint(0, -y));
        fctx_line_to(fctx, FPoint(-size.w, -y));
        fctx_line_to(fctx, FPoint(-size.w, y));
        fctx_line_to(fctx, FPoint(0, y));
    }
    fctx_close_path(fctx);

    fctx_end_fill(fctx);
}

void fctx_rect_layer_draw(FctxLayer *this, FContext *fctx) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FSize size = data->size;
    if (size.w == 0 || size.h == 0) return;

    fctx_set_rotation(fctx, data->rotation);
    fctx_set_offset(fctx, data->offset);

    if (!gcolor_equal(data->border_color, GColorClear) && data->border_width > 0) {
        fctx_set_fill_color(fctx, data->border_color);
        fctx_draw_rect(fctx, data->alignment, size);
    }

    if (gcolor_equal(data->fill_color, GColorClear)) return;

    size.w -= INT_TO_FIXED(data->border_width);
    size.h -= INT_TO_FIXED(data->border_width);
    if (size.w <= 0 || size.h <= 0) return;
    fctx_set_fill_color(fctx, data->fill_color);
    fctx_draw_rect(fctx, data->alignment, size);
}

FctxRectLayer *fctx_rect_layer_create(void) {
    log_func();
    FctxRectLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, fctx_rect_layer_draw);
    Data *data = fctx_layer_get_data(this);
    data->border_color = GColorClear;
    data->border_width = 0;
    data->fill_color = GColorClear;
    data->alignment = GTextAlignmentLeft;
    data->rotation = 0;
    data->size = FSize(0, 0);
    data->offset = FPointZero;
    return this;
}

void fctx_rect_layer_destroy(FctxRectLayer *this) {
    log_func();
    fctx_layer_destroy(this);
}

void fctx_rect_layer_set_border_color(FctxRectLayer *this, GColor color) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->border_color = color;
    layer_mark_dirty(this);
}

void fctx_rect_layer_set_border_width(FctxRectLayer *this, uint16_t width) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->border_width = width;
    layer_mark_dirty(this);
}

void fctx_rect_layer_set_fill_color(FctxRectLayer *this, GColor color) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->fill_color = color;
    layer_mark_dirty(this);
}

void fctx_rect_layer_set_alignment(FctxRectLayer *this, GTextAlignment alignment) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->alignment = alignment;
    layer_mark_dirty(this);
}

void fctx_rect_layer_set_rotation(FctxRectLayer *this, uint32_t rotation) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->rotation = rotation;
    layer_mark_dirty(this);
}

void fctx_rect_layer_set_size(FctxRectLayer *this, FSize size) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->size = size;
    layer_mark_dirty(this);
}

void fctx_rect_layer_set_offset(FctxRectLayer *this, FPoint offset) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    data->offset = offset;
    layer_mark_dirty(this);
}
#endif // PBL_PLATFORM_APLITE
