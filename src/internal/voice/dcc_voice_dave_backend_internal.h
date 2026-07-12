#ifndef DCC_VOICE_DAVE_BACKEND_INTERNAL_H
#define DCC_VOICE_DAVE_BACKEND_INTERNAL_H

#include <dcc/voice.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_voice_dave_backend_load(dcc_voice_client_t *voice_client);
void dcc_voice_dave_backend_unload(dcc_voice_client_t *voice_client);
uint16_t dcc_voice_dave_backend_max_version(const dcc_voice_client_t *voice_client);
dcc_status_t dcc_voice_dave_backend_init_session(dcc_voice_client_t *voice_client);
void dcc_voice_dave_backend_reset_session(dcc_voice_client_t *voice_client);
dcc_status_t dcc_voice_dave_backend_handle_mls(
    dcc_voice_client_t *voice_client,
    const dcc_voice_dave_mls_frame_t *frame
);
dcc_status_t dcc_voice_dave_backend_set_protocol(
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_version_t version,
    uint8_t media_ready
);
dcc_status_t dcc_voice_dave_backend_encrypt(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    const uint8_t *frame,
    size_t frame_size,
    uint8_t **out,
    size_t *out_size
);
dcc_status_t dcc_voice_dave_backend_decrypt(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    const uint8_t *frame,
    size_t frame_size,
    uint8_t **out,
    size_t *out_size
);
void dcc_voice_dave_backend_remove_participant(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id
);

#ifdef __cplusplus
}
#endif

#endif
