#ifndef DCC_JSON_GATEWAY_PRESENCE_STATUS_BUFFERS_H
#define DCC_JSON_GATEWAY_PRESENCE_STATUS_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_presence_status_buffers {
    char *desktop_status;
    size_t desktop_status_cap;
    char *mobile_status;
    size_t mobile_status_cap;
    char *web_status;
    size_t web_status_cap;
} dcc_json_gateway_presence_status_buffers_t;

#endif
