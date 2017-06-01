#pragma once
#include "fctx-layer.h"

typedef FctxLayer DateLayer;

DateLayer *date_layer_create(void);
void date_layer_destroy(DateLayer *this);
