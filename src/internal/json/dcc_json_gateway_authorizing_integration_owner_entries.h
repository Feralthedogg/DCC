#ifndef DCC_JSON_GATEWAY_AUTHORIZING_INTEGRATION_OWNER_ENTRIES_H
#define DCC_JSON_GATEWAY_AUTHORIZING_INTEGRATION_OWNER_ENTRIES_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_authorizing_integration_owner_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
);

#endif
