#ifndef DCC_JSON_GATEWAY_RESOLVED_BUFFERS_H
#define DCC_JSON_GATEWAY_RESOLVED_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_attachment_buffers {
    char *filename;
    size_t filename_cap;
    char *description;
    size_t description_cap;
    char *url;
    size_t url_cap;
    char *proxy_url;
    size_t proxy_url_cap;
    char *content_type;
    size_t content_type_cap;
    char *waveform;
    size_t waveform_cap;
} dcc_json_gateway_attachment_buffers_t;

typedef struct dcc_json_gateway_resolved_message_buffers {
    char *content;
    size_t content_cap;
    char *timestamp;
    size_t timestamp_cap;
    char *edited_timestamp;
    size_t edited_timestamp_cap;
    char *author_username;
    size_t author_username_cap;
    char *author_global_name;
    size_t author_global_name_cap;
    char *author_avatar;
    size_t author_avatar_cap;
} dcc_json_gateway_resolved_message_buffers_t;

#endif
