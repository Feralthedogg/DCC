#ifndef DCC_JSON_GATEWAY_ENTITLEMENT_BUFFERS_H
#define DCC_JSON_GATEWAY_ENTITLEMENT_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_entitlement_buffers {
    char *start_time;
    size_t start_time_cap;
    char *end_time;
    size_t end_time_cap;
} dcc_json_gateway_entitlement_buffers_t;

#endif
