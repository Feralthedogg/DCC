#ifndef DCC_CLIENT_VOICE_REGISTRY_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_VOICE_REGISTRY_STATE_FIELDS_INTERNAL_H

#include "internal/voice/dcc_voice_limits_internal.h"

#include <dcc/snowflake.h>
#include <dcc/voice.h>

#include <stdatomic.h>
#include <stdint.h>

#define DCC_CLIENT_VOICE_REGISTRY_STATE_FIELDS \
    atomic_flag voice_clients_lock; \
    dcc_snowflake_t self_user_id; \
    uint32_t voice_clients_len; \
    dcc_voice_client_t *voice_clients[DCC_VOICE_CLIENT_REGISTRY_CAP]

#endif
