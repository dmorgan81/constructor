#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer StepLayer;

StepLayer *step_layer_create(void);
void step_layer_destroy(StepLayer *this);
