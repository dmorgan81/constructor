#ifndef PBL_PLATFORM_APLITE
#include "common.h"
#include <pebble-events/pebble-events.h>
#include <pebble-generic-weather/pebble-generic-weather.h>
#include <@smallstoneapps/linked-list/linked-list.h>
#include "geocode.h"
#include "weather.h"

static const uint32_t PERSIST_KEY_WEATHER_INFO = 2;
static const uint32_t PERSIST_KEY_WEATHER_STATUS = 3;

typedef struct {
    EventWeatherHandler handler;
    void *context;
} WeatherHandlerState;

typedef struct {
    GenericWeatherInfo *info;
    GenericWeatherStatus status;
} WeatherBundle;

static GenericWeatherStatus s_status = GenericWeatherStatusNotYetFetched;

static LinkedRoot *s_handler_list;

static uint16_t s_interval;
static const char *s_api_key;
static GenericWeatherProvider s_provider;
static bool s_use_feels_like;
static bool s_use_gps;

static bool s_connected;
static bool s_ready = false;

static EventHandle s_settings_event_handle;
static EventHandle s_connection_event_handle;
static EventHandle s_app_message_event_handle;

static AppTimer *s_timer;

static EventHandle s_geocode_event_handle;
static char s_location_name[GEOCODE_MAPQUEST_MAX_LOCATION_LEN];

static void cancel_timer(void) {
    log_func();
    if (s_timer) app_timer_cancel(s_timer);
    s_timer = NULL;
}

static bool each_weather_fetched(void *this, void *context) {
    log_func();
    WeatherHandlerState *state = (WeatherHandlerState *) this;
    WeatherBundle *bundle = (WeatherBundle *) context;
    state->handler(bundle->info, bundle->status, state->context);
    return true;
}

static void generic_weather_fetch_callback(GenericWeatherInfo *info, GenericWeatherStatus status) {
    log_func();
    s_status = status;

    WeatherBundle bundle = {
        .info = info,
        .status = status
    };
    linked_list_foreach(s_handler_list, each_weather_fetched, &bundle);
}

static void app_timer_callback(void *context) {
    log_func();
    generic_weather_fetch(generic_weather_fetch_callback);
    s_timer = app_timer_register(s_interval * 1000, app_timer_callback, NULL);
}

static void fetch_or_setup_timer(void) {
    log_func();
    GenericWeatherInfo *info = generic_weather_peek();
    time_t now = time(NULL);
    logd("%ld - %ld", now, info->timestamp);
    if (now - info->timestamp > s_interval) {
        generic_weather_fetch(generic_weather_fetch_callback);
        s_timer = app_timer_register(s_interval * 1000, app_timer_callback, NULL);
    } else {
        logd("%ld", s_interval - (now - info->timestamp));
        s_timer = app_timer_register((s_interval - (now - info->timestamp)) * 1000, app_timer_callback, NULL);
    }
}

static void pebble_app_connection_handler(bool connected) {
    log_func();
    if (!connected && s_timer) cancel_timer();
    else if (s_connected != connected) fetch_or_setup_timer();
    s_connected = connected;
}

static void geocode_handler(GeocodeMapquestCoordinates *coordinates, GeocodeMapquestStatus status, void *context) {
    log_func();
    if (status == GeocodeMapquestStatusAvailable) {
        generic_weather_set_location((GenericWeatherCoordinates) {
            .latitude = coordinates->latitude,
            .longitude = coordinates->longitude
        });
    } else if (status != GeocodeMapquestStatusPending) {
        generic_weather_set_location(GENERIC_WEATHER_GPS_LOCATION);
    }
    if (status != GeocodeMapquestStatusPending) {
        cancel_timer();
        if (s_ready && s_connected && linked_list_count(s_handler_list) > 0) {
            generic_weather_fetch(generic_weather_fetch_callback);
            s_timer = app_timer_register(s_interval * 1000, app_timer_callback, NULL);
        }
    }
}

static void settings_handler(void *context) {
    log_func();
    const char *api_key = enamel_get_WEATHER_KEY();
    GenericWeatherProvider provider = atoi(enamel_get_WEATHER_PROVIDER());
    uint32_t interval = atoi(enamel_get_WEATHER_INTERVAL()) * SECONDS_PER_MINUTE;
    bool use_feels_like = enamel_get_WEATHER_FEELS_LIKE();
    bool use_gps = enamel_get_WEATHER_USE_GPS();
    const char *location_name = enamel_get_WEATHER_LOCATION_NAME();
    bool fetch_weather = false;

    if (strcmp(api_key, s_api_key) != 0) {
        generic_weather_set_api_key(api_key);
        s_api_key = api_key;
        fetch_weather = true;
    }

    if (provider != s_provider) {
        generic_weather_set_provider(provider);
        s_provider = provider;
        fetch_weather = true;
    }

    if (interval != s_interval) {
        s_interval = interval;
        fetch_weather = true;
    }

    if (use_feels_like != s_use_feels_like) {
        generic_weather_set_feels_like(use_feels_like);
        s_use_feels_like = use_feels_like;
        fetch_weather = true;
    }

    if (use_gps != s_use_gps || strcmp(location_name, s_location_name) != 0) {
        s_use_gps = use_gps;
        strncpy(s_location_name, location_name, sizeof(s_location_name));
        if (use_gps) {
            generic_weather_set_location(GENERIC_WEATHER_GPS_LOCATION);
            fetch_weather = true;
        } else {
            geocode_fetch(s_location_name);
            fetch_weather = false;
        }
    }

    if (fetch_weather) {
        cancel_timer();
        if (s_ready && s_connected && linked_list_count(s_handler_list) > 0) {
            generic_weather_fetch(generic_weather_fetch_callback);
            s_timer = app_timer_register(s_interval * 1000, app_timer_callback, NULL);
        }
    }
}

static void inbox_received(DictionaryIterator *iterator, void *context) {
    log_func();
    Tuple *tuple = dict_find(iterator, MESSAGE_KEY_APP_READY);
    if (tuple && !s_ready) {
        s_ready = true;
        if (s_connected && linked_list_count(s_handler_list) > 0) fetch_or_setup_timer();
    }
}

void weather_init(void) {
    log_func();
    s_status = persist_exists(PERSIST_KEY_WEATHER_STATUS) ? persist_read_int(PERSIST_KEY_WEATHER_STATUS) : GenericWeatherStatusNotYetFetched;

    s_handler_list = linked_list_create_root();

    s_interval = atoi(enamel_get_WEATHER_INTERVAL()) * SECONDS_PER_MINUTE;
    s_api_key = enamel_get_WEATHER_KEY();
    s_provider = atoi(enamel_get_WEATHER_PROVIDER());
    s_use_feels_like = enamel_get_WEATHER_FEELS_LIKE();
    s_use_gps = enamel_get_WEATHER_USE_GPS();

    generic_weather_init();
    geocode_init();

    generic_weather_set_api_key(s_api_key);
    generic_weather_set_provider(s_provider);
    generic_weather_set_feels_like(s_use_feels_like);
    generic_weather_load(PERSIST_KEY_WEATHER_INFO);

    strncpy(s_location_name, enamel_get_WEATHER_LOCATION_NAME(), sizeof(s_location_name));
    GeocodeMapquestCoordinates *coordinates = geocode_peek();
    if (s_use_gps || coordinates == NULL || strlen(s_location_name) == 0) {
        generic_weather_set_location(GENERIC_WEATHER_GPS_LOCATION);
    } else {
        generic_weather_set_location((GenericWeatherCoordinates) {
            .latitude = coordinates->latitude,
            .longitude = coordinates->longitude
        });
    }
    s_geocode_event_handle = events_geocode_subscribe(geocode_handler, NULL);

    s_settings_event_handle = enamel_settings_received_subscribe(settings_handler, NULL);

    s_connected = connection_service_peek_pebble_app_connection();
    s_connection_event_handle = events_connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = pebble_app_connection_handler
    });

    s_app_message_event_handle = events_app_message_subscribe_handlers((EventAppMessageHandlers) {
        .received = inbox_received
    }, NULL);
}

void weather_deinit(void) {
    log_func();
    cancel_timer();

    events_app_message_unsubscribe(s_app_message_event_handle);
    events_connection_service_unsubscribe(s_connection_event_handle);
    enamel_settings_received_unsubscribe(s_settings_event_handle);
    events_geocode_unsubscribe(s_geocode_event_handle);
    generic_weather_save(PERSIST_KEY_WEATHER_INFO);
    generic_weather_deinit();

    persist_write_int(PERSIST_KEY_WEATHER_STATUS, s_status);

    geocode_deinit();

    free(s_handler_list);
}

EventHandle events_weather_subscribe(EventWeatherHandler handler, void *context) {
    log_func();
    WeatherHandlerState *this = malloc(sizeof(WeatherHandlerState));
    this->handler = handler;
    this->context = context;

    uint16_t count = linked_list_count(s_handler_list);
    linked_list_append(s_handler_list, this);

    if (count == 0 && s_ready && s_connected) fetch_or_setup_timer();
    return this;
}

void events_weather_unsubscribe(EventHandle handle) {
    log_func();

    int16_t index = linked_list_find(s_handler_list, handle);
    if (index == -1) return;

    free(linked_list_get(s_handler_list, index));
    linked_list_remove(s_handler_list, index);

    if (linked_list_count(s_handler_list) == 0) cancel_timer();
}

GenericWeatherStatus weather_status_peek(void) {
    log_func();
    return s_status;
}
#endif // PBL_PLATFORM_APLITE
