#include "internal/rest/dcc_rest_channel_builder_json_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_channel_append_rate_limit_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
) {
    dcc_status_t status = DCC_OK;
    if (params->rate_limit_per_user != 0) {
        status = dcc_rest_json_append_u64_member(body, first, "rate_limit_per_user", params->rate_limit_per_user);
    }
    if (status == DCC_OK && params->default_thread_rate_limit_per_user != 0) {
        status = dcc_rest_json_append_u64_member(
            body,
            first,
            "default_thread_rate_limit_per_user",
            params->default_thread_rate_limit_per_user
        );
    }
    return status;
}

dcc_status_t dcc_rest_channel_append_voice_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
) {
    if (params->type != DCC_CHANNEL_VOICE) {
        return DCC_OK;
    }

    dcc_status_t status = dcc_rest_json_append_u64_member(body, first, "user_limit", params->user_limit);
    if (status == DCC_OK && params->bitrate_kbps != 0) {
        status = dcc_rest_json_append_u64_member(body, first, "bitrate", (uint64_t)params->bitrate_kbps * 1000U);
    }
    return status;
}
