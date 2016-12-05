#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer ConnectionLayer;

ConnectionLayer *connection_layer_create();
void connection_layer_destroy(ConnectionLayer *this);
