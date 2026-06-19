#ifndef DCC_JSON_GATEWAY_D_CORE_GUILD_ID_FIELDS_H
#define DCC_JSON_GATEWAY_D_CORE_GUILD_ID_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_d_core_guild_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
);
void dcc_json_gateway_apply_d_core_guild_id(
    dcc_json_gateway_payload_t *out,
    uint64_t value
);
void dcc_json_gateway_mark_d_core_guild_id(
    dcc_json_gateway_payload_t *out
);

#endif
