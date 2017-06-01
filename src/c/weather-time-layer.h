#pragma once
#include "fctx-layer.h"

typedef FctxLayer WeatherTimeLayer;

WeatherTimeLayer *weather_time_layer_create(void);
void weather_time_layer_destroy(WeatherTimeLayer *this);
