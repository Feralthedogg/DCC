#include "internal/rest/dcc_rest_routes_internal.h"

#include <string.h>

static int dcc_rest_route_span_eq_cstr(const char *span, size_t span_len, const char *literal) {
    size_t literal_len = strlen(literal);
    return span_len == literal_len && memcmp(span, literal, span_len) == 0;
}

int dcc_rest_route_span_is_digits(const char *span, size_t span_len) {
    if (span_len == 0) {
        return 0;
    }
    for (size_t i = 0; i < span_len; ++i) {
        if (span[i] < '0' || span[i] > '9') {
            return 0;
        }
    }
    return 1;
}

dcc_rest_route_resource_t dcc_rest_route_major_resource(const char *span, size_t span_len) {
    if (dcc_rest_route_span_eq_cstr(span, span_len, "channels")) {
        return DCC_REST_ROUTE_RESOURCE_CHANNELS;
    }
    if (dcc_rest_route_span_eq_cstr(span, span_len, "guilds")) {
        return DCC_REST_ROUTE_RESOURCE_GUILDS;
    }
    if (dcc_rest_route_span_eq_cstr(span, span_len, "webhooks")) {
        return DCC_REST_ROUTE_RESOURCE_WEBHOOKS;
    }
    if (dcc_rest_route_span_eq_cstr(span, span_len, "interactions")) {
        return DCC_REST_ROUTE_RESOURCE_INTERACTIONS;
    }
    if (dcc_rest_route_span_eq_cstr(span, span_len, "applications")) {
        return DCC_REST_ROUTE_RESOURCE_APPLICATIONS;
    }
    return DCC_REST_ROUTE_RESOURCE_NONE;
}

void dcc_rest_route_append(char *out, size_t out_size, size_t *pos, const char *value, size_t value_len) {
    if (out == NULL || pos == NULL || *pos >= out_size) {
        return;
    }
    while (value_len > 0 && *pos + 1U < out_size) {
        out[(*pos)++] = *value++;
        value_len--;
    }
    out[*pos] = '\0';
}
