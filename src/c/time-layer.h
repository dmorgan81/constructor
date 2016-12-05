#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer TimeLayer;

TimeLayer *time_layer_create(void);
void time_layer_destroy(TimeLayer *this);
