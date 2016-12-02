#pragma once
#include <pebble-fctx/ffont.h>

void fonts_init(void);
void fonts_deinit(void);
FFont *fonts_get(uint32_t resource_id);
