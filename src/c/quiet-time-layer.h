#pragma once
#include "fctx-layer.h"

typedef FctxLayer QuietTimeLayer;

QuietTimeLayer *quiet_time_layer_create(void);
void quiet_time_layer_destroy(QuietTimeLayer *this);
