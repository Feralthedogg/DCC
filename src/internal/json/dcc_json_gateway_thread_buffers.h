#ifndef DCC_JSON_GATEWAY_THREAD_BUFFERS_H
#define DCC_JSON_GATEWAY_THREAD_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_thread_buffers {
    char *name;
    size_t name_cap;
    char *topic;
    size_t topic_cap;
} dcc_json_gateway_thread_buffers_t;

#endif
