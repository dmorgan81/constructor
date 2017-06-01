#pragma once
#include "fctx-layer.h"

typedef FctxLayer StepLayer;

StepLayer *step_layer_create(void);
void step_layer_destroy(StepLayer *this);
