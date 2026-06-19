#ifndef DCC_JSON_GATEWAY_API_H
#define DCC_JSON_GATEWAY_API_H

#include <dcc/error.h>

#include <stddef.h>

#include "internal/json/dcc_json_gateway_payload_state.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_json_stage1_validate(const char *input, size_t len);
dcc_status_t dcc_json_parse_gateway_payload(const char *input, size_t len, dcc_json_gateway_payload_t *out);
dcc_status_t dcc_json_parse_interaction_payload(const char *input, size_t len, dcc_json_gateway_payload_t *out);

#ifdef __cplusplus
}
#endif

#endif
