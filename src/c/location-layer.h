#pragma once
#include "fctx-layer.h"

typedef FctxLayer LocationLayer;

LocationLayer *location_layer_create(void);
void location_layer_destroy(LocationLayer *this);
