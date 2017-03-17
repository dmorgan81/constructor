#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer DistanceLayer;

DistanceLayer *distance_layer_create();
void distance_layer_destroy(DistanceLayer *this);
