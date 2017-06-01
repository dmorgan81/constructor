#include "common.h"
#include <pebble-fctx/ffont.h>
#include <@smallstoneapps/linked-list/linked-list.h>
#include "fonts.h"

#ifdef PBL_PLATFORM_APLITE
static FFont *s_font;
#else
typedef struct {
    uint32_t resource_id;
    FFont *font;
} Data;

static LinkedRoot *s_fonts;
#endif

void fonts_init(void) {
    log_func();
#ifdef PBL_PLATFORM_APLITE
    s_font = ffont_create_from_resource(RESOURCE_ID_LECO_FFONT);
#else
    s_fonts = linked_list_create_root();
#endif
}

#ifndef PBL_PLATFORM_APLITE
static bool linked_list_destroy_callback(void *object, void *context) {
    log_func();
    Data *data = (Data *) object;
    ffont_destroy(data->font);
    free(data);
    return true;
}
#endif

void fonts_deinit(void) {
#ifdef PBL_PLATFORM_APLITE
    ffont_destroy(s_font);
#else
    linked_list_foreach(s_fonts, linked_list_destroy_callback, NULL);
    linked_list_clear(s_fonts);
    free(s_fonts);
#endif
}

#ifndef PBL_PLATFORM_APLITE
static bool linked_list_find_by_resource_id(void* object1, void* object2) {
    log_func();
    return ((uint32_t) object1) == ((Data *) object2)->resource_id;
}
#endif

FFont *fonts_get(uint32_t resource_id) {
    log_func();
#ifdef PBL_PLATFORM_APLITE
    return s_font;
#else
    int16_t index = linked_list_find_compare(s_fonts, (void *) resource_id, linked_list_find_by_resource_id);
    if (index == -1) {
        Data *data = malloc(sizeof(Data));
        data->resource_id = resource_id;
        data->font = ffont_create_from_resource(resource_id);
        linked_list_append(s_fonts, data);
        return data->font;
    } else {
        return ((Data *) linked_list_get(s_fonts, index))->font;
    }
#endif
}
