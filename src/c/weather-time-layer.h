#pragma once
#include "fctx-text-layer.h"

typedef FctxTextLayer WeatherTimeLayer;

WeatherTimeLayer *weather_time_layer_create(void);
void weather_time_layer_destroy(WeatherTimeLayer *this);
