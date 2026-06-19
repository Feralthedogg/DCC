#include "internal/voice/dcc_voice_opus_internal.h"

#include <stdlib.h>

uint8_t dcc_voice_opus_is_available(void) {
#if defined(DCC_HAVE_OPUS)
    return 1U;
#else
    return 0U;
#endif
}

dcc_status_t dcc_voice_client_enable_opus(
    dcc_voice_client_t *voice_client,
    const dcc_voice_opus_options_t *options
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options != NULL && options->size < sizeof(options->size)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_opus_config_t config;
    dcc_status_t status = dcc_voice_opus_resolve_options(options, &config);
    if (status != DCC_OK) {
        return status;
    }

#if !defined(DCC_HAVE_OPUS)
    (void)config;
    return DCC_ERR_STATE;
#else
    dcc_voice_opus_backend_t *backend = (dcc_voice_opus_backend_t *)calloc(1U, sizeof(*backend));
    if (backend == NULL) {
        return DCC_ERR_NOMEM;
    }

    int opus_error = OPUS_OK;
    backend->sample_rate = config.sample_rate;
    backend->channels = config.channels;
    backend->application = config.application;
    backend->encoder = opus_encoder_create(
        (opus_int32)config.sample_rate,
        (int)config.channels,
        (int)config.application,
        &opus_error
    );
    if (backend->encoder == NULL || opus_error != OPUS_OK) {
        dcc_status_t status = opus_error != OPUS_OK ? dcc_voice_status_from_opus(opus_error) : DCC_ERR_NOMEM;
        opus_encoder_destroy(backend->encoder);
        free(backend);
        return status;
    }
    backend->decoder = opus_decoder_create((opus_int32)config.sample_rate, (int)config.channels, &opus_error);
    if (backend->decoder == NULL || opus_error != OPUS_OK) {
        dcc_status_t status = opus_error != OPUS_OK ? dcc_voice_status_from_opus(opus_error) : DCC_ERR_NOMEM;
        opus_encoder_destroy(backend->encoder);
        opus_decoder_destroy(backend->decoder);
        free(backend);
        return status;
    }

    dcc_voice_opus_backend_destroy(voice_client);
    voice_client->opus_backend = backend;
    voice_client->encoder = dcc_voice_opus_encode;
    voice_client->encoder_user_data = backend;
    voice_client->decoder = dcc_voice_opus_decode;
    voice_client->decoder_user_data = backend;
    return DCC_OK;
#endif
}

dcc_status_t dcc_voice_client_disable_opus(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_voice_opus_backend_destroy(voice_client);
    return DCC_OK;
}
