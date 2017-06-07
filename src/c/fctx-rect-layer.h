#pragma once
#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include "fctx-layer.h"

typedef FctxLayer FctxRectLayer;

FctxRectLayer *fctx_rect_layer_create(void);
void fctx_rect_layer_destroy(FctxRectLayer *this);
void fctx_rect_layer_set_border_color(FctxRectLayer *this, GColor color);
void fctx_rect_layer_set_border_width(FctxRectLayer *this, uint16_t width);
void fctx_rect_layer_set_fill_color(FctxRectLayer *this, GColor color);
void fctx_rect_layer_set_alignment(FctxRectLayer *this, GTextAlignment alignment);
void fctx_rect_layer_set_rotation(FctxRectLayer *this, uint32_t rotation);
void fctx_rect_layer_set_size(FctxRectLayer *this, FSize size);
void fctx_rect_layer_set_offset(FctxRectLayer *this, FPoint offset);
