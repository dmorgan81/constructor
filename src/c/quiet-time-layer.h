#pragma once
#include "fctx-text-layer.h"

typedef FctxLayer QuietTimeLayer;

QuietTimeLayer *quiet_time_layer_create();
void quiet_time_layer_destroy(QuietTimeLayer *this);
