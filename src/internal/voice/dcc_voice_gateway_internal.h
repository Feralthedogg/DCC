#ifndef DCC_VOICE_GATEWAY_INTERNAL_H
#define DCC_VOICE_GATEWAY_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_json dcc_json_t;

int dcc_voice_gateway_json_u64(const dcc_json_t *json, uint64_t *out);
int dcc_voice_gateway_json_u32(const dcc_json_t *json, uint32_t *out);
int dcc_voice_gateway_json_i32(const dcc_json_t *json, int32_t *out);
int dcc_voice_gateway_json_u16(const dcc_json_t *json, uint16_t *out);
dcc_voice_dave_version_t dcc_voice_gateway_dave_version_from_u32(uint32_t version);
dcc_status_t dcc_voice_gateway_parse_secret_key(dcc_voice_client_t *voice_client, const dcc_json_t *json);
void dcc_voice_gateway_select_mode_from_ready(dcc_voice_client_t *voice_client, const dcc_json_t *modes);

#ifdef __cplusplus
}
#endif

#endif
