#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer LocationLayer;

LocationLayer *location_layer_create(void);
void location_layer_destroy(LocationLayer *this);
