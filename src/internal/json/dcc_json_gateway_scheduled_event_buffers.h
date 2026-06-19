#ifndef DCC_JSON_GATEWAY_SCHEDULED_EVENT_BUFFERS_H
#define DCC_JSON_GATEWAY_SCHEDULED_EVENT_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_scheduled_event_buffers {
    char *name;
    size_t name_cap;
    char *description;
    size_t description_cap;
    char *image;
    size_t image_cap;
    char *location;
    size_t location_cap;
    char *start_time;
    size_t start_time_cap;
    char *end_time;
    size_t end_time_cap;
    char *creator_username;
    size_t creator_username_cap;
    char *creator_global_name;
    size_t creator_global_name_cap;
    char *creator_avatar;
    size_t creator_avatar_cap;
} dcc_json_gateway_scheduled_event_buffers_t;

#endif
