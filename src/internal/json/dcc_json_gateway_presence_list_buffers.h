#ifndef DCC_JSON_GATEWAY_PRESENCE_LIST_BUFFERS_H
#define DCC_JSON_GATEWAY_PRESENCE_LIST_BUFFERS_H

#include <dcc/objects.h>

#include <stddef.h>

typedef struct dcc_json_gateway_presence_list_buffers {
    char *username;
    size_t username_cap;
    char *global_name;
    size_t global_name_cap;
    char *avatar;
    size_t avatar_cap;
    char *status_text;
    size_t status_text_cap;
    char *desktop_status;
    size_t desktop_status_cap;
    char *mobile_status;
    size_t mobile_status_cap;
    char *web_status;
    size_t web_status_cap;
    dcc_gateway_activity_t *activities;
    char *activity_names;
    size_t activity_name_stride;
    char *activity_states;
    size_t activity_state_stride;
    char *activity_details;
    size_t activity_detail_stride;
    char *activity_urls;
    size_t activity_url_stride;
    char *activity_emoji_names;
    size_t activity_emoji_name_stride;
} dcc_json_gateway_presence_list_buffers_t;

#endif
