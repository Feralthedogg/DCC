#ifndef DCC_REST_ROUTES_INTERNAL_H
#define DCC_REST_ROUTES_INTERNAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_rest_route_resource {
    DCC_REST_ROUTE_RESOURCE_NONE = 0,
    DCC_REST_ROUTE_RESOURCE_CHANNELS,
    DCC_REST_ROUTE_RESOURCE_GUILDS,
    DCC_REST_ROUTE_RESOURCE_WEBHOOKS,
    DCC_REST_ROUTE_RESOURCE_INTERACTIONS,
    DCC_REST_ROUTE_RESOURCE_APPLICATIONS
} dcc_rest_route_resource_t;

int dcc_rest_route_span_is_digits(const char *span, size_t span_len);
dcc_rest_route_resource_t dcc_rest_route_major_resource(const char *span, size_t span_len);
void dcc_rest_route_append(char *out, size_t out_size, size_t *pos, const char *value, size_t value_len);

#ifdef __cplusplus
}
#endif

#endif
