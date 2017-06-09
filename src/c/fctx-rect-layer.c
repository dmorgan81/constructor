#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-fctx/fctx.h>
#include "fctx-rect-layer.h"

typedef struct {
    FctxRectLayerHandles handles;
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

static void update_proc(FctxLayer *this, FContext *fctx) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    FctxRectLayerHandles handles = data->handles;
    FSize size = FSizeI(handles.size_w(), handles.size_h());
    if (size.w == 0 || size.h == 0) return;

    uint32_t rotation = DEG_TO_TRIGANGLE(handles.rotation());
    fctx_set_rotation(fctx, rotation);

    FPoint offset = FPointI(handles.offset_x(), handles.offset_y());
#ifdef PBL_QUICK_VIEW_ENABLED
    fctx_set_offset(fctx, fpoint_add(fctx_layer_get_offset(this), offset));
#else
    fctx_set_offset(fctx, offset);
#endif

    GTextAlignment alignment = atoi(handles.alignment());
    GColor border_color = handles.border_color();
    uint16_t border_width = handles.border_width();
    if (!gcolor_equal(border_color, GColorClear) && border_width > 0) {
        fctx_set_fill_color(fctx, border_color);
        fctx_draw_rect(fctx, alignment, size);
    }

    GColor fill_color = handles.fill_color();
    if (gcolor_equal(fill_color, GColorClear)) return;

    size.w -= INT_TO_FIXED(border_width);
    size.h -= INT_TO_FIXED(border_width);
    if (size.w <= 0 || size.h <= 0) return;
    fctx_set_fill_color(fctx, fill_color);
    fctx_draw_rect(fctx, alignment, size);
}

FctxRectLayer *fctx_rect_layer_create(void) {
    log_func();
    FctxRectLayer *this = fctx_layer_create_with_data(sizeof(Data));
    fctx_layer_set_update_proc(this, update_proc);
    Data *data = fctx_layer_get_data(this);
    memset(&data->handles, 0, sizeof(FctxRectLayerHandles));
    return this;
}

void fctx_rect_layer_destroy(FctxRectLayer *this) {
    log_func();
    fctx_layer_destroy(this);
}

void fctx_rect_layer_set_handles(FctxRectLayer *this, FctxRectLayerHandles handles) {
    log_func();
    Data *data = fctx_layer_get_data(this);
    memcpy(&data->handles, &handles, sizeof(FctxRectLayerHandles));
    layer_mark_dirty(this);
}
#endif // PBL_PLATFORM_APLITE
