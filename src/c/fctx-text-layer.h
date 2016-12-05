#pragma once
#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "fctx-layer.h"

typedef FctxLayer FctxTextLayer;

FctxTextLayer *fctx_text_layer_create();
FctxTextLayer *fctx_text_layer_create_with_data(size_t data_size);
void fctx_text_layer_destroy(FctxTextLayer *this);
void *fctx_text_layer_get_data(FctxTextLayer *this);
void fctx_text_layer_set_text(FctxTextLayer *this, char *text);
void fctx_text_layer_set_fill_color(FctxTextLayer *this, GColor color);
void fctx_text_layer_set_rotation(FctxTextLayer *this, uint32_t rotation);
void fctx_text_layer_set_offset(FctxTextLayer *this, FPoint offset);
void fctx_text_layer_set_font(FctxTextLayer *this, FFont *font);
void fctx_text_layer_set_em_height(FctxTextLayer *this, int16_t pixels);
void fctx_text_layer_set_alignment(FctxTextLayer *this, GTextAlignment alignment);
void fctx_text_layer_set_anchor(FctxTextLayer *this, FTextAnchor anchor);
