#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer ConditionLayer;

ConditionLayer *condition_layer_create();
void condition_layer_destroy(ConditionLayer *this);