#include "internal/voice/dcc_voice_ws_internal.h"

dcc_status_t dcc_voice_ws_send_initial_payload(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint8_t resume,
    dcc_ws_t *ws
) {
    char payload[1536];
    size_t payload_len = 0;
    dcc_status_t status = resume
        ? dcc_voice_client_build_resume_payload(voice_client, payload, sizeof(payload), &payload_len)
        : dcc_voice_client_build_identify_payload(voice_client, self_user_id, payload, sizeof(payload), &payload_len);
    if (status == DCC_OK) {
        status = dcc_ws_send_text(ws, payload, payload_len);
    }
    return status;
}
