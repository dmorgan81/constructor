#pragma once
#include "fctx-layer.h"

typedef FctxLayer TimeLayer;

TimeLayer *time_layer_create();
void time_layer_destroy(TimeLayer *this);
