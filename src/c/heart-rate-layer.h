#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer HeartRateLayer;

HeartRateLayer *heart_rate_layer_create(void);
void heart_rate_layer_destroy(HeartRateLayer *this);
