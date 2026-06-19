#ifndef DCC_VOICE_SSRC_INTERNAL_H
#define DCC_VOICE_SSRC_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_voice_client_find_ssrc(
    const dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    size_t *out_index
);
int dcc_voice_client_find_user(
    const dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    size_t *out_index
);
void dcc_voice_client_remove_map_index(dcc_voice_client_t *voice_client, size_t index);
void dcc_voice_client_unmap_user(dcc_voice_client_t *voice_client, dcc_snowflake_t user_id);

#ifdef __cplusplus
}
#endif

#endif
