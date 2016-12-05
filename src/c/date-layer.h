#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer DateLayer;

DateLayer *date_layer_create(void);
void date_layer_destroy(DateLayer *this);
