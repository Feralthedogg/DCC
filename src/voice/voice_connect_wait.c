#include "internal/voice/dcc_voice_connect_wait_internal.h"

dcc_status_t dcc_voice_client_connect_and_wait_until_ready(
    dcc_voice_client_t *voice_client,
    const dcc_voice_connect_wait_options_t *options,
    dcc_voice_connect_wait_result_t *out
) {
    if (voice_client == NULL || (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_connect_wait_options_t effective;
    dcc_status_t status = dcc_voice_connect_wait_options_normalize(options, &effective);
    if (status != DCC_OK) {
        return status;
    }

    dcc_voice_connect_wait_result_t local_result;
    dcc_voice_connect_wait_result_t *result = out != NULL ? out : &local_result;
    dcc_voice_connect_wait_result_init(result);

    dcc_client_t *client = voice_client->client;
    if (client == NULL) {
        return DCC_ERR_STATE;
    }

    status = dcc_voice_connect_wait_descriptor(
        voice_client,
        client,
        &effective,
        result->descriptor_snapshots,
        &result->descriptor_completed,
        &result->connect_status
    );
    result->descriptor_status = status;
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_voice_client_wait_until_ready(voice_client, effective.ready_timeout_ms, &result->health);
    result->ready_status = status;
    return status;
}
