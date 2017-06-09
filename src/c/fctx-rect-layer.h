#pragma once
#include "common.h"
#include <pebble-fctx/fctx.h>
#include "fctx-layer.h"

typedef struct {
    EnamelGColorFunction border_color;
    EnamelInt32Function border_width;
    EnamelGColorFunction fill_color;
    EnamelStringFunction alignment;
    EnamelInt32Function rotation;
    EnamelInt32Function size_w;
    EnamelInt32Function size_h;
    EnamelInt32Function offset_x;
    EnamelInt32Function offset_y;
} FctxRectLayerHandles;

typedef FctxLayer FctxRectLayer;

FctxRectLayer *fctx_rect_layer_create(void);
void fctx_rect_layer_destroy(FctxRectLayer *this);
void fctx_rect_layer_set_handles(FctxRectLayer *this, FctxRectLayerHandles handles);
