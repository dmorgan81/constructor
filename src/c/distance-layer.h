#pragma once
#include "fctx-layer.h"

typedef FctxLayer DistanceLayer;

DistanceLayer *distance_layer_create();
void distance_layer_destroy(DistanceLayer *this);
