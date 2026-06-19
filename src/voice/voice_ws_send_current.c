#include "internal/voice/dcc_voice_internal.h"
#include "internal/voice/dcc_voice_ws_internal.h"

dcc_status_t dcc_voice_client_send_current_ws(
    dcc_voice_client_t *voice_client,
    dcc_ws_opcode_t opcode,
    const void *data,
    size_t data_size
) {
    if (voice_client == NULL || data == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_ws_t *ws = NULL;
    dcc_status_t status = dcc_voice_client_websocket_retain_current(voice_client, &ws);
    if (status == DCC_OK) {
        status = dcc_ws_send(ws, opcode, data, data_size);
        dcc_ws_destroy(ws);
    }
    return status;
}
