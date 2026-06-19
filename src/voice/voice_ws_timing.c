#include "internal/voice/dcc_voice_ws_internal.h"

uint8_t dcc_voice_client_websocket_sleep(dcc_voice_client_t *voice_client, uint32_t ms) {
    uint32_t slept_ms = 0;
    while (slept_ms < ms) {
        if (dcc_voice_client_websocket_stop_requested(voice_client)) {
            return 1U;
        }
        uint32_t slice_ms = ms - slept_ms;
        if (slice_ms > 25U) {
            slice_ms = 25U;
        }
        dcc_voice_sleep_ms(slice_ms);
        slept_ms += slice_ms;
    }
    return dcc_voice_client_websocket_stop_requested(voice_client);
}
