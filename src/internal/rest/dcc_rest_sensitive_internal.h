#ifndef DCC_REST_SENSITIVE_INTERNAL_H
#define DCC_REST_SENSITIVE_INTERNAL_H

#include <dcc/error.h>

#include <stddef.h>

void dcc_endpoint_secure_zero(void *bytes, size_t byte_count);
void dcc_rest_sensitive_free(char *bytes, size_t byte_count);
dcc_status_t dcc_endpoint_sensitive_route_fingerprint(
    const char *encoded,
    size_t encoded_len,
    char out_hex[65]
);

#endif
