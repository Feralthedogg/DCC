#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_set_receive_sequence(dcc_voice_client_t *voice_client, int32_t sequence) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->receive_sequence = sequence;
    return DCC_OK;
}

int32_t dcc_voice_client_receive_sequence(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->receive_sequence : -1;
}
