#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer TemperatureLayer;

TemperatureLayer *temperature_layer_create();
void temperature_layer_destroy(TemperatureLayer *this);
