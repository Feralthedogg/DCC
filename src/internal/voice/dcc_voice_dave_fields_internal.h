#ifndef DCC_VOICE_DAVE_FIELDS_INTERNAL_H
#define DCC_VOICE_DAVE_FIELDS_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#define DCC_VOICE_DAVE_STATE_FIELDS \
    uint8_t dave_enabled; \
    dcc_voice_dave_version_t dave_version; \
    uint8_t dave_transition_pending; \
    uint8_t dave_transition_ready; \
    dcc_voice_dave_version_t dave_pending_version; \
    uint16_t dave_transition_id; \
    dcc_voice_dave_mls_fn dave_mls_handler; \
    void *dave_mls_user_data

#endif
