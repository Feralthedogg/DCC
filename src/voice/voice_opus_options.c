#include "internal/voice/dcc_voice_opus_internal.h"

#include <stddef.h>

#define DCC_VOICE_OPUS_DEFAULT_SAMPLE_RATE 48000U
#define DCC_VOICE_OPUS_DEFAULT_CHANNELS 2U

static int dcc_voice_opus_sample_rate_valid(uint32_t sample_rate) {
    switch (sample_rate) {
        case 8000U:
        case 12000U:
        case 16000U:
        case 24000U:
        case 48000U:
            return 1;
        default:
            return 0;
    }
}

static int dcc_voice_opus_application_valid(dcc_voice_opus_application_t application) {
    switch (application) {
        case DCC_VOICE_OPUS_APPLICATION_VOIP:
        case DCC_VOICE_OPUS_APPLICATION_AUDIO:
        case DCC_VOICE_OPUS_APPLICATION_RESTRICTED_LOWDELAY:
            return 1;
        default:
            return 0;
    }
}

dcc_status_t dcc_voice_opus_resolve_options(
    const dcc_voice_opus_options_t *options,
    dcc_voice_opus_config_t *out
) {
    if (out == NULL || (options != NULL && options->size < sizeof(options->size))) {
        return DCC_ERR_INVALID_ARG;
    }

#define DCC_VOICE_OPUS_OPT_HAS(field) \
    (options != NULL && options->size >= offsetof(dcc_voice_opus_options_t, field) + sizeof(options->field))
    dcc_voice_opus_config_t config = {
        .sample_rate = DCC_VOICE_OPUS_OPT_HAS(sample_rate) && options->sample_rate != 0
            ? options->sample_rate
            : DCC_VOICE_OPUS_DEFAULT_SAMPLE_RATE,
        .channels = DCC_VOICE_OPUS_OPT_HAS(channels) && options->channels != 0
            ? options->channels
            : DCC_VOICE_OPUS_DEFAULT_CHANNELS,
        .application = DCC_VOICE_OPUS_OPT_HAS(application) && options->application != 0
            ? options->application
            : DCC_VOICE_OPUS_APPLICATION_AUDIO,
    };
#undef DCC_VOICE_OPUS_OPT_HAS

    if (!dcc_voice_opus_sample_rate_valid(config.sample_rate) ||
        (config.channels != 1U && config.channels != 2U) ||
        !dcc_voice_opus_application_valid(config.application)) {
        return DCC_ERR_INVALID_ARG;
    }

    *out = config;
    return DCC_OK;
}
