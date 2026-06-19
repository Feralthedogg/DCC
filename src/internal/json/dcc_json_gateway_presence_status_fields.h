#ifndef DCC_JSON_GATEWAY_PRESENCE_STATUS_FIELDS_H
#define DCC_JSON_GATEWAY_PRESENCE_STATUS_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_presence_buffers.h"
#include "internal/json/dcc_json_keys.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_presence_client_status(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    char *desktop_status,
    size_t desktop_status_cap,
    char *mobile_status,
    size_t mobile_status_cap,
    char *web_status,
    size_t web_status_cap
);
dcc_status_t dcc_json_gateway_parse_presence_client_status_fields(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    const dcc_json_gateway_presence_status_buffers_t *buffers
);
dcc_status_t dcc_json_gateway_parse_presence_client_status_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    char *desktop_status,
    size_t desktop_status_cap,
    char *mobile_status,
    size_t mobile_status_cap,
    char *web_status,
    size_t web_status_cap,
    int *handled
);

#endif
