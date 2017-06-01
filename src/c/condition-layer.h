#pragma once
#include "fctx-layer.h"

typedef FctxLayer ConditionLayer;

ConditionLayer *condition_layer_create();
void condition_layer_destroy(ConditionLayer *this);