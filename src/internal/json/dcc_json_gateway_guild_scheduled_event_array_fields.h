#ifndef DCC_JSON_GATEWAY_GUILD_SCHEDULED_EVENT_ARRAY_FIELDS_H
#define DCC_JSON_GATEWAY_GUILD_SCHEDULED_EVENT_ARRAY_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_guild_scheduled_event_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_scheduled_event_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
);
void dcc_json_gateway_finish_scheduled_event_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
);

#endif
