#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer BatteryLayer;

BatteryLayer *battery_layer_create(void);
void battery_layer_destroy(BatteryLayer *this);
