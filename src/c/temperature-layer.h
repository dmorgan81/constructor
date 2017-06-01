#pragma once
#include "fctx-layer.h"

typedef FctxLayer TemperatureLayer;

TemperatureLayer *temperature_layer_create();
void temperature_layer_destroy(TemperatureLayer *this);
