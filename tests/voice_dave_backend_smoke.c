#include "internal/voice/dcc_voice_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
#if defined(_WIN32)
    if (_putenv_s("DCC_DAVE_LIBRARY", DCC_FAKE_DAVE_PATH) != 0) return 1;
#else
    if (setenv("DCC_DAVE_LIBRARY", DCC_FAKE_DAVE_PATH, 1) != 0) return 1;
#endif
    dcc_client_options_t options = {
        .size = sizeof(options), .token = "", .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_voice_client_create(client, &voice) != DCC_OK ||
        dcc_voice_client_start_session(voice, 10U, 20U, 0U, 0U, 1U) != DCC_OK ||
        !dcc_voice_client_dave_backend_available(voice)) {
        fprintf(stderr, "failed to load fake official DAVE ABI\n");
        return 1;
    }
    voice->dave_self_user_id = 42U;
    voice->ssrc = 1234U;
    voice->dave_participants[0] = 42U;
    voice->dave_participants[1] = 43U;
    voice->dave_participant_count = 2U;
    memcpy(voice->session_id, "session", 8U);
    if (dcc_voice_dave_backend_init_session(voice) != DCC_OK) return 1;

    const uint8_t commit[] = {1U, 2U, 3U};
    dcc_voice_dave_mls_frame_t frame = {
        .size = sizeof(frame),
        .opcode = DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION,
        .transition_id = 7U,
        .has_transition_id = 1U,
        .payload = commit,
        .payload_size = sizeof(commit),
    };
    (void)dcc_voice_dave_backend_handle_mls(voice, &frame);
    if (!voice->dave_media_ready) return 1;

    const uint8_t plain[] = {0xf8U, 0xffU, 0xfeU};
    uint8_t *encrypted = NULL;
    size_t encrypted_size = 0U;
    uint8_t *decrypted = NULL;
    size_t decrypted_size = 0U;
    if (dcc_voice_dave_backend_encrypt(
            voice, voice->ssrc, plain, sizeof(plain), &encrypted, &encrypted_size
        ) != DCC_OK ||
        encrypted_size != sizeof(plain) + 1U || encrypted[0] != 0xdaU ||
        dcc_voice_dave_backend_decrypt(
            voice, 43U, encrypted, encrypted_size, &decrypted, &decrypted_size
        ) != DCC_OK ||
        decrypted_size != sizeof(plain) || memcmp(decrypted, plain, sizeof(plain)) != 0 ||
        voice->dave_encrypt_success != 1U || voice->dave_decrypt_success != 1U) {
        fprintf(stderr, "DAVE media round trip failed\n");
        free(encrypted); free(decrypted);
        return 1;
    }
    dcc_voice_dave_stats_t stats = {
        .size = sizeof(stats),
    };
    if (dcc_voice_client_dave_stats(voice, &stats) != DCC_OK ||
        stats.backend_available != 1U ||
        stats.media_ready != 1U ||
        stats.participant_count != 2U ||
        stats.encrypt_success != 1U ||
        stats.decrypt_success != 1U) {
        fprintf(stderr, "DAVE public stats mismatch\n");
        free(encrypted); free(decrypted);
        return 1;
    }
    free(encrypted); free(decrypted);
    dcc_voice_client_destroy(voice);
    dcc_client_destroy(client);
    return 0;
}
