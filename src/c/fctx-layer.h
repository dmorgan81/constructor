#pragma once
#include <pebble.h>
#include <pebble-fctx/fctx.h>

typedef Layer FctxLayer;

typedef void (*FctxLayerUpdateProc)(FctxLayer *layer, FContext* fctx);

#define FSize(w, h) ((FSize){(w),(h)})
#define FSizeI(w, h) ((FSize){INT_TO_FIXED(w), INT_TO_FIXED(h)})
#define FRect(x, y, w, h) ((FRect){{(x), (y)}, {(w), (h)}})

static inline FRect g2frect(GRect g) {
    return (FRect) {
        .origin = g2fpoint(g.origin),
        .size = FSizeI(g.size.w, g.size.h)
    };
}

FctxLayer *fctx_layer_get_root_layer(Window *window);
FctxLayer *fctx_layer_create();
FctxLayer *fctx_layer_create_with_data(size_t data_size);
void fctx_layer_destroy(FctxLayer *this);
void fctx_layer_set_update_proc(FctxLayer *this, FctxLayerUpdateProc update_proc);
void fctx_layer_add_child(FctxLayer *this, FctxLayer *child);
void fctx_layer_remove_child(FctxLayer *this, FctxLayer *child);
void *fctx_layer_get_data(FctxLayer *this);
FRect fctx_layer_get_bounds(FctxLayer *this);
#ifdef PBL_PLATFORM_APLITE
#define fctx_layer_get_unobstructed_bounds(layer) fctx_layer_get_bounds(layer)
#else
FRect fctx_layer_get_unobstructed_bounds(FctxLayer *this);
#endif
