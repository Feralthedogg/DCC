#ifndef DCC_JSON_GATEWAY_ENTITLEMENT_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_ENTITLEMENT_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_entitlement_object(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    char *start_time,
    size_t start_time_cap,
    char *end_time,
    size_t end_time_cap
);

#endif
