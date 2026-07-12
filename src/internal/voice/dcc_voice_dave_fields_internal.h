#ifndef DCC_VOICE_DAVE_FIELDS_INTERNAL_H
#define DCC_VOICE_DAVE_FIELDS_INTERNAL_H

#include <dcc/dcc.h>

#include "internal/voice/dcc_voice_limits_internal.h"

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_voice_dave_backend dcc_voice_dave_backend_t;

#define DCC_VOICE_DAVE_STATE_FIELDS \
    uint8_t dave_enabled; \
    uint8_t dave_requested; \
    uint8_t dave_backend_ready; \
    uint8_t dave_media_ready; \
    dcc_voice_dave_version_t dave_version; \
    uint8_t dave_transition_pending; \
    uint8_t dave_transition_ready; \
    dcc_voice_dave_version_t dave_pending_version; \
    uint16_t dave_transition_id; \
    dcc_snowflake_t dave_self_user_id; \
    uint64_t dave_epoch; \
    uint64_t dave_encrypt_success; \
    uint64_t dave_encrypt_failures; \
    uint64_t dave_decrypt_success; \
    uint64_t dave_decrypt_failures; \
    uint64_t dave_missing_key_failures; \
    size_t dave_participant_count; \
    dcc_snowflake_t dave_participants[DCC_VOICE_SSRC_MAP_CAP]; \
    dcc_voice_dave_backend_t *dave_backend; \
    dcc_voice_dave_mls_fn dave_mls_handler; \
    void *dave_mls_user_data

#endif
