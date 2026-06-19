#include "internal/voice/dcc_voice_opus_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_voice_opus_backend_destroy(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL || voice_client->opus_backend == NULL) {
        return;
    }
#if defined(DCC_HAVE_OPUS)
    dcc_voice_opus_backend_t *backend = (dcc_voice_opus_backend_t *)voice_client->opus_backend;
    if (voice_client->encoder == dcc_voice_opus_encode && voice_client->encoder_user_data == backend) {
        voice_client->encoder = NULL;
        voice_client->encoder_user_data = NULL;
    }
    if (voice_client->decoder == dcc_voice_opus_decode && voice_client->decoder_user_data == backend) {
        voice_client->decoder = NULL;
        voice_client->decoder_user_data = NULL;
    }
    opus_encoder_destroy(backend->encoder);
    opus_decoder_destroy(backend->decoder);
    memset(backend, 0, sizeof(*backend));
    free(backend);
#endif
    voice_client->opus_backend = NULL;
}
