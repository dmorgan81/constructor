#pragma once
#include "fctx-layer.h"

typedef FctxLayer HeartRateLayer;

HeartRateLayer *heart_rate_layer_create(void);
void heart_rate_layer_destroy(HeartRateLayer *this);
