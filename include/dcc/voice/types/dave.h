#ifndef DCC_VOICE_TYPES_DAVE_H
#define DCC_VOICE_TYPES_DAVE_H

#include <dcc/voice/types/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef dcc_status_t (*dcc_voice_dave_mls_fn)(
    void *user_data,
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_mls_opcode_t opcode,
    uint16_t sequence,
    uint16_t transition_id,
    const uint8_t *payload,
    size_t payload_size
);

typedef struct dcc_voice_dave_mls_frame {
    size_t size;
    uint16_t sequence;
    dcc_voice_dave_mls_opcode_t opcode;
    uint16_t transition_id;
    uint8_t has_transition_id;
    const uint8_t *payload;
    size_t payload_size;
} dcc_voice_dave_mls_frame_t;

#ifdef __cplusplus
}
#endif

#endif
