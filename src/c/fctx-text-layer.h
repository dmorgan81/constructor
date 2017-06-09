#pragma once
#include "common.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fctx-layer.h"

typedef struct {
    EnamelGColorFunction fill_color;
    EnamelStringFunction alignment;
    EnamelInt32Function rotation;
    EnamelInt32Function font_size;
    EnamelInt32Function offset_x;
    EnamelInt32Function offset_y;
} FctxTextLayerHandles;

typedef FctxLayer FctxTextLayer;

FctxTextLayer *fctx_text_layer_create(void);
void fctx_text_layer_destroy(FctxTextLayer *this);
void fctx_text_layer_set_handles(FctxTextLayer *this, FctxTextLayerHandles handles);
void fctx_text_layer_set_text(FctxTextLayer *this, char *text);
void fctx_text_layer_set_font(FctxTextLayer *this, FFont *font);
