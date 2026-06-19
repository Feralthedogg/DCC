#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_routes_internal.h"

#include <string.h>

void dcc_rest_route_key(const char *method, const char *path, char *out, size_t out_size) {
    if (out == NULL || out_size == 0) {
        return;
    }
    out[0] = '\0';
    if (method == NULL || path == NULL) {
        return;
    }

    const char *route = path;
    const char *scheme = strstr(path, "://");
    if (scheme != NULL) {
        const char *after_authority = strchr(scheme + 3, '/');
        route = after_authority != NULL ? after_authority : "/";
    }

    const char *query = strchr(route, '?');
    size_t route_len = query != NULL ? (size_t)(query - route) : strlen(route);
    size_t pos = 0;

    while (*method != '\0' && pos + 1U < out_size) {
        out[pos++] = *method++;
    }
    if (pos + 1U < out_size) {
        out[pos++] = ' ';
    }

    const char *p = route;
    const char *end = route + route_len;
    dcc_rest_route_resource_t next_major = DCC_REST_ROUTE_RESOURCE_NONE;
    int preserve_next_token = 0;

    while (p < end && pos + 1U < out_size) {
        if (*p == '/') {
            out[pos++] = *p++;
            out[pos] = '\0';
            continue;
        }

        const char *segment = p;
        while (p < end && *p != '/') {
            p++;
        }
        size_t segment_len = (size_t)(p - segment);
        int digits = dcc_rest_route_span_is_digits(segment, segment_len);

        if (preserve_next_token) {
            dcc_rest_route_append(out, out_size, &pos, segment, segment_len);
            preserve_next_token = 0;
            next_major = DCC_REST_ROUTE_RESOURCE_NONE;
        } else if (digits && next_major != DCC_REST_ROUTE_RESOURCE_NONE) {
            dcc_rest_route_append(out, out_size, &pos, segment, segment_len);
            preserve_next_token =
                next_major == DCC_REST_ROUTE_RESOURCE_WEBHOOKS ||
                next_major == DCC_REST_ROUTE_RESOURCE_INTERACTIONS;
            next_major = DCC_REST_ROUTE_RESOURCE_NONE;
        } else if (digits) {
            dcc_rest_route_append(out, out_size, &pos, ":id", 3U);
            next_major = DCC_REST_ROUTE_RESOURCE_NONE;
        } else {
            dcc_rest_route_append(out, out_size, &pos, segment, segment_len);
            next_major = dcc_rest_route_major_resource(segment, segment_len);
            preserve_next_token = 0;
        }
    }
    out[pos] = '\0';
}
