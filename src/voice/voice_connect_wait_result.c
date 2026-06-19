#include "internal/voice/dcc_voice_connect_wait_internal.h"

#include <string.h>

void dcc_voice_connect_wait_result_init(dcc_voice_connect_wait_result_t *out) {
    if (out == NULL) {
        return;
    }

    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);
    out->connect_status = DCC_ERR_STATE;
    out->descriptor_status = DCC_ERR_STATE;
    out->ready_status = DCC_ERR_STATE;
    out->descriptor_snapshots[0].size = sizeof(out->descriptor_snapshots[0]);
    out->descriptor_snapshots[1].size = sizeof(out->descriptor_snapshots[1]);
    out->health.size = sizeof(out->health);
}
