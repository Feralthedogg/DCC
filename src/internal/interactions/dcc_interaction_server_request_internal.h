#ifndef DCC_INTERACTION_SERVER_REQUEST_INTERNAL_H
#define DCC_INTERACTION_SERVER_REQUEST_INTERNAL_H

#include "internal/interactions/dcc_interaction_server_internal.h"

dcc_status_t dcc_interaction_buffer_append(
    dcc_interaction_buffer_t *buffer,
    const char *data,
    size_t len
);
size_t dcc_interaction_find_header_end(const char *data, size_t len);
dcc_status_t dcc_interaction_parse_content_length(
    const char *headers,
    size_t header_len,
    size_t *out
);
dcc_status_t dcc_interaction_parse_headers(
    dcc_interaction_request_t *request,
    const char *headers,
    size_t header_len
);

#endif
