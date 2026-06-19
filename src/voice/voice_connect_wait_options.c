#include "internal/voice/dcc_voice_connect_wait_internal.h"

#include <stddef.h>
#include <string.h>

static uint8_t dcc_voice_connect_wait_has_field(size_t size, size_t offset, size_t field_size) {
    return size >= offset && size - offset >= field_size ? 1U : 0U;
}

#define DCC_VOICE_CONNECT_WAIT_HAS(options, field) \
    dcc_voice_connect_wait_has_field( \
        (options)->size, \
        offsetof(dcc_voice_connect_wait_options_t, field), \
        sizeof((options)->field) \
    )

void dcc_voice_connect_wait_options_init(dcc_voice_connect_wait_options_t *options) {
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->descriptor_timeout_ms = 30000U;
    options->ready_timeout_ms = 30000U;
}

dcc_status_t dcc_voice_connect_wait_options_normalize(
    const dcc_voice_connect_wait_options_t *options,
    dcc_voice_connect_wait_options_t *out
) {
    if (options == NULL ||
        !dcc_voice_connect_wait_has_field(
            options->size,
            offsetof(dcc_voice_connect_wait_options_t, channel_id),
            sizeof(options->channel_id)
        ) ||
        options->guild_id == 0 ||
        options->channel_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_connect_wait_options_init(out);
    out->guild_id = options->guild_id;
    out->channel_id = options->channel_id;
    out->self_mute = DCC_VOICE_CONNECT_WAIT_HAS(options, self_mute) ? options->self_mute : 0U;
    out->self_deaf = DCC_VOICE_CONNECT_WAIT_HAS(options, self_deaf) ? options->self_deaf : 0U;
    out->enable_dave = DCC_VOICE_CONNECT_WAIT_HAS(options, enable_dave) ? options->enable_dave : 0U;
    if (DCC_VOICE_CONNECT_WAIT_HAS(options, descriptor_timeout_ms)) {
        out->descriptor_timeout_ms = options->descriptor_timeout_ms;
    }
    if (DCC_VOICE_CONNECT_WAIT_HAS(options, ready_timeout_ms)) {
        out->ready_timeout_ms = options->ready_timeout_ms;
    }
    return DCC_OK;
}
