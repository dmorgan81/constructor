#pragma once
#include "fctx-layer.h"

typedef FctxLayer ConnectionLayer;

ConnectionLayer *connection_layer_create();
void connection_layer_destroy(ConnectionLayer *this);
