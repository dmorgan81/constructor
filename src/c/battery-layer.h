#pragma once
#include "fctx-layer.h"

typedef FctxLayer BatteryLayer;

BatteryLayer *battery_layer_create(void);
void battery_layer_destroy(BatteryLayer *this);
