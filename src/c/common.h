#pragma once
#include <pebble.h>
#include "logging.h"
#include "enamel.h"

#ifdef PBL_PLATFORM_APLITE
#define PBL_IF_LOW_MEM_ELSE(if_true, if_false) (if_true)
#else
#define PBL_IF_LOW_MEM_ELSE(if_true, if_false) (if_false)
#endif
