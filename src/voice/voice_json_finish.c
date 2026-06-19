#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_json_finish(dcc_voice_json_buffer_t *buf, size_t *out_len) {
    if (buf == NULL || buf->data == NULL || buf->cap == 0 || buf->overflow || buf->len >= buf->cap) {
        if (out_len != NULL && buf != NULL) {
            *out_len = buf->len;
        }
        return DCC_ERR_NOMEM;
    }
    buf->data[buf->len] = '\0';
    if (out_len != NULL) {
        *out_len = buf->len;
    }
    return DCC_OK;
}
