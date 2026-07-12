#ifndef DCC_INTERACTION_REQUEST_STATE_INTERNAL_H
#define DCC_INTERACTION_REQUEST_STATE_INTERNAL_H

#include "internal/interactions/dcc_interaction_server_limits_internal.h"
#include "internal/json/dcc_json.h"

#include <dcc/interaction_server.h>

#include <stddef.h>
#include <stdint.h>

struct dcc_interaction_request {
    dcc_interaction_server_t *server;
    char *header_block;
    dcc_interaction_http_header_t headers[DCC_INTERACTION_HEADER_CAP];
    size_t header_count;
    char method[16];
    char path[256];
    char *body;
    size_t body_len;
    dcc_json_gateway_payload_t *payload;
    dcc_event_type_t event_type;
    uint8_t parsed;
    uint8_t response_set;
    uint16_t response_status;
    char *response_content_type;
    char *response_body;
    size_t response_body_len;
    uint64_t started_at_ns;
    uint64_t deadline_at_ns;
    uint8_t deadline_noted;
};

#endif
