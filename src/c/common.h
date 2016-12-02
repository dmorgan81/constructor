#pragma once
#include <pebble.h>
#include "logging.h"

#ifdef PBL_PLATFORM_EMERY
#define PBL_DISPLAY_LARGE
#define PBL_IF_DISPLAY_LARGE_ELSE(if_true, if_false) (if_true)
#define PBL_IF_DISPLAY_SMALL_ELESE(if_true, if_false) (if_false)
#else
#define PBL_DISPLAY_SMALL
#define PBL_IF_DISPLAY_LARGE_ELSE(if_true, if_false) (if_false)
#define PBL_IF_DISPLAY_SMALL_ELESE(if_true, if_false) (if_true)
#endif

/*
#define PBL_DISPLAY_WIDTH PBL_IF_ROUND_ELSE(180, PBL_IF_DISPLAY_LARGE_ELSE(200, 144))
#define PBL_DISPLAY_HEIGHT PBL_IF_ROUND_ELSE(180, PBL_IF_DISPLAY_LARGE_ELSE(228, 168))
*/