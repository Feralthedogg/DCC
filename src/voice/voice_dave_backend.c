#include "internal/voice/dcc_voice_internal.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
typedef HMODULE dcc_dave_library_t;
#else
#include <dlfcn.h>
typedef void *dcc_dave_library_t;
#endif

typedef void *dcc_dave_session_handle_t;
typedef void *dcc_dave_result_handle_t;
typedef void *dcc_dave_ratchet_handle_t;
typedef void *dcc_dave_encryptor_handle_t;
typedef void *dcc_dave_decryptor_handle_t;

typedef uint16_t (*dcc_dave_max_version_fn)(void);
typedef void (*dcc_dave_free_fn)(void *);
typedef void (*dcc_dave_failure_fn)(const char *, const char *, void *);
typedef dcc_dave_session_handle_t (*dcc_dave_session_create_fn)(
    void *, const char *, dcc_dave_failure_fn, void *
);
typedef void (*dcc_dave_session_destroy_fn)(dcc_dave_session_handle_t);
typedef void (*dcc_dave_session_init_fn)(dcc_dave_session_handle_t, uint16_t, uint64_t, const char *);
typedef void (*dcc_dave_session_reset_fn)(dcc_dave_session_handle_t);
typedef void (*dcc_dave_session_set_version_fn)(dcc_dave_session_handle_t, uint16_t);
typedef void (*dcc_dave_session_set_external_fn)(dcc_dave_session_handle_t, const uint8_t *, size_t);
typedef void (*dcc_dave_session_process_proposals_fn)(
    dcc_dave_session_handle_t, const uint8_t *, size_t, const char **, size_t, uint8_t **, size_t *
);
typedef dcc_dave_result_handle_t (*dcc_dave_session_process_commit_fn)(
    dcc_dave_session_handle_t, const uint8_t *, size_t
);
typedef dcc_dave_result_handle_t (*dcc_dave_session_process_welcome_fn)(
    dcc_dave_session_handle_t, const uint8_t *, size_t, const char **, size_t
);
typedef void (*dcc_dave_session_get_key_package_fn)(dcc_dave_session_handle_t, uint8_t **, size_t *);
typedef dcc_dave_ratchet_handle_t (*dcc_dave_session_get_ratchet_fn)(dcc_dave_session_handle_t, const char *);
typedef bool (*dcc_dave_result_bool_fn)(dcc_dave_result_handle_t);
typedef void (*dcc_dave_result_destroy_fn)(dcc_dave_result_handle_t);
typedef void (*dcc_dave_ratchet_destroy_fn)(dcc_dave_ratchet_handle_t);
typedef dcc_dave_encryptor_handle_t (*dcc_dave_encryptor_create_fn)(void);
typedef void (*dcc_dave_encryptor_destroy_fn)(dcc_dave_encryptor_handle_t);
typedef void (*dcc_dave_encryptor_set_ratchet_fn)(dcc_dave_encryptor_handle_t, dcc_dave_ratchet_handle_t);
typedef void (*dcc_dave_encryptor_set_passthrough_fn)(dcc_dave_encryptor_handle_t, bool);
typedef void (*dcc_dave_encryptor_assign_codec_fn)(dcc_dave_encryptor_handle_t, uint32_t, int);
typedef size_t (*dcc_dave_encryptor_max_size_fn)(dcc_dave_encryptor_handle_t, int, size_t);
typedef int (*dcc_dave_encryptor_encrypt_fn)(
    dcc_dave_encryptor_handle_t, int, uint32_t, const uint8_t *, size_t, uint8_t *, size_t, size_t *
);
typedef dcc_dave_decryptor_handle_t (*dcc_dave_decryptor_create_fn)(void);
typedef void (*dcc_dave_decryptor_destroy_fn)(dcc_dave_decryptor_handle_t);
typedef void (*dcc_dave_decryptor_set_ratchet_fn)(dcc_dave_decryptor_handle_t, dcc_dave_ratchet_handle_t);
typedef void (*dcc_dave_decryptor_set_passthrough_fn)(dcc_dave_decryptor_handle_t, bool);
typedef size_t (*dcc_dave_decryptor_max_size_fn)(dcc_dave_decryptor_handle_t, int, size_t);
typedef int (*dcc_dave_decryptor_decrypt_fn)(
    dcc_dave_decryptor_handle_t, int, const uint8_t *, size_t, uint8_t *, size_t, size_t *
);

typedef struct dcc_dave_decryptor_entry {
    dcc_snowflake_t user_id;
    dcc_dave_ratchet_handle_t ratchet;
    dcc_dave_decryptor_handle_t decryptor;
} dcc_dave_decryptor_entry_t;

struct dcc_voice_dave_backend {
    dcc_dave_library_t library;
    uint16_t max_version;
    dcc_dave_session_handle_t session;
    uint8_t session_initialized;
    dcc_dave_ratchet_handle_t self_ratchet;
    dcc_dave_encryptor_handle_t encryptor;
    dcc_dave_decryptor_entry_t decryptors[DCC_VOICE_SSRC_MAP_CAP];
    size_t decryptor_count;
    dcc_dave_max_version_fn max_supported_protocol_version;
    dcc_dave_free_fn free_memory;
    dcc_dave_session_create_fn session_create;
    dcc_dave_session_destroy_fn session_destroy;
    dcc_dave_session_init_fn session_init;
    dcc_dave_session_reset_fn session_reset;
    dcc_dave_session_set_version_fn session_set_version;
    dcc_dave_session_set_external_fn session_set_external;
    dcc_dave_session_process_proposals_fn session_process_proposals;
    dcc_dave_session_process_commit_fn session_process_commit;
    dcc_dave_session_process_welcome_fn session_process_welcome;
    dcc_dave_session_get_key_package_fn session_get_key_package;
    dcc_dave_session_get_ratchet_fn session_get_ratchet;
    dcc_dave_result_bool_fn commit_is_failed;
    dcc_dave_result_bool_fn commit_is_ignored;
    dcc_dave_result_destroy_fn commit_destroy;
    dcc_dave_result_destroy_fn welcome_destroy;
    dcc_dave_ratchet_destroy_fn ratchet_destroy;
    dcc_dave_encryptor_create_fn encryptor_create;
    dcc_dave_encryptor_destroy_fn encryptor_destroy;
    dcc_dave_encryptor_set_ratchet_fn encryptor_set_ratchet;
    dcc_dave_encryptor_set_passthrough_fn encryptor_set_passthrough;
    dcc_dave_encryptor_assign_codec_fn encryptor_assign_codec;
    dcc_dave_encryptor_max_size_fn encryptor_max_size;
    dcc_dave_encryptor_encrypt_fn encryptor_encrypt;
    dcc_dave_decryptor_create_fn decryptor_create;
    dcc_dave_decryptor_destroy_fn decryptor_destroy;
    dcc_dave_decryptor_set_ratchet_fn decryptor_set_ratchet;
    dcc_dave_decryptor_set_passthrough_fn decryptor_set_passthrough;
    dcc_dave_decryptor_max_size_fn decryptor_max_size;
    dcc_dave_decryptor_decrypt_fn decryptor_decrypt;
};

static dcc_dave_library_t dcc_dave_library_open(const char *path) {
#if defined(_WIN32)
    return LoadLibraryA(path);
#else
    return dlopen(path, RTLD_NOW | RTLD_LOCAL);
#endif
}

static void dcc_dave_library_close(dcc_dave_library_t library) {
    if (library == NULL) return;
#if defined(_WIN32)
    (void)FreeLibrary(library);
#else
    (void)dlclose(library);
#endif
}

static void *dcc_dave_library_symbol(dcc_dave_library_t library, const char *name) {
#if defined(_WIN32)
    return (void *)(uintptr_t)GetProcAddress(library, name);
#else
    return dlsym(library, name);
#endif
}

#define DCC_DAVE_LOAD(backend, field, name) do { \
    void *dcc_dave_symbol_value = dcc_dave_library_symbol((backend)->library, (name)); \
    if (dcc_dave_symbol_value == NULL || sizeof((backend)->field) > sizeof(dcc_dave_symbol_value)) return -1; \
    memcpy(&(backend)->field, &dcc_dave_symbol_value, sizeof((backend)->field)); \
} while (0)

static int dcc_dave_load_symbols(dcc_voice_dave_backend_t *backend) {
    DCC_DAVE_LOAD(backend, max_supported_protocol_version, "daveMaxSupportedProtocolVersion");
    DCC_DAVE_LOAD(backend, free_memory, "daveFree");
    DCC_DAVE_LOAD(backend, session_create, "daveSessionCreate");
    DCC_DAVE_LOAD(backend, session_destroy, "daveSessionDestroy");
    DCC_DAVE_LOAD(backend, session_init, "daveSessionInit");
    DCC_DAVE_LOAD(backend, session_reset, "daveSessionReset");
    DCC_DAVE_LOAD(backend, session_set_version, "daveSessionSetProtocolVersion");
    DCC_DAVE_LOAD(backend, session_set_external, "daveSessionSetExternalSender");
    DCC_DAVE_LOAD(backend, session_process_proposals, "daveSessionProcessProposals");
    DCC_DAVE_LOAD(backend, session_process_commit, "daveSessionProcessCommit");
    DCC_DAVE_LOAD(backend, session_process_welcome, "daveSessionProcessWelcome");
    DCC_DAVE_LOAD(backend, session_get_key_package, "daveSessionGetMarshalledKeyPackage");
    DCC_DAVE_LOAD(backend, session_get_ratchet, "daveSessionGetKeyRatchet");
    DCC_DAVE_LOAD(backend, commit_is_failed, "daveCommitResultIsFailed");
    DCC_DAVE_LOAD(backend, commit_is_ignored, "daveCommitResultIsIgnored");
    DCC_DAVE_LOAD(backend, commit_destroy, "daveCommitResultDestroy");
    DCC_DAVE_LOAD(backend, welcome_destroy, "daveWelcomeResultDestroy");
    DCC_DAVE_LOAD(backend, ratchet_destroy, "daveKeyRatchetDestroy");
    DCC_DAVE_LOAD(backend, encryptor_create, "daveEncryptorCreate");
    DCC_DAVE_LOAD(backend, encryptor_destroy, "daveEncryptorDestroy");
    DCC_DAVE_LOAD(backend, encryptor_set_ratchet, "daveEncryptorSetKeyRatchet");
    DCC_DAVE_LOAD(backend, encryptor_set_passthrough, "daveEncryptorSetPassthroughMode");
    DCC_DAVE_LOAD(backend, encryptor_assign_codec, "daveEncryptorAssignSsrcToCodec");
    DCC_DAVE_LOAD(backend, encryptor_max_size, "daveEncryptorGetMaxCiphertextByteSize");
    DCC_DAVE_LOAD(backend, encryptor_encrypt, "daveEncryptorEncrypt");
    DCC_DAVE_LOAD(backend, decryptor_create, "daveDecryptorCreate");
    DCC_DAVE_LOAD(backend, decryptor_destroy, "daveDecryptorDestroy");
    DCC_DAVE_LOAD(backend, decryptor_set_ratchet, "daveDecryptorTransitionToKeyRatchet");
    DCC_DAVE_LOAD(backend, decryptor_set_passthrough, "daveDecryptorTransitionToPassthroughMode");
    DCC_DAVE_LOAD(backend, decryptor_max_size, "daveDecryptorGetMaxPlaintextByteSize");
    DCC_DAVE_LOAD(backend, decryptor_decrypt, "daveDecryptorDecrypt");
    return 0;
}

static void dcc_dave_failure(const char *source, const char *reason, void *user_data) {
    dcc_voice_client_t *voice_client = (dcc_voice_client_t *)user_data;
    if (voice_client != NULL) {
        voice_client->dave_missing_key_failures++;
        dcc_set_error(voice_client->client, reason != NULL ? reason : source);
    }
}

static void dcc_dave_destroy_media(dcc_voice_dave_backend_t *backend) {
    if (backend == NULL) return;
    if (backend->encryptor != NULL) backend->encryptor_destroy(backend->encryptor);
    backend->encryptor = NULL;
    if (backend->self_ratchet != NULL) backend->ratchet_destroy(backend->self_ratchet);
    backend->self_ratchet = NULL;
    for (size_t i = 0; i < backend->decryptor_count; ++i) {
        if (backend->decryptors[i].decryptor != NULL) backend->decryptor_destroy(backend->decryptors[i].decryptor);
        if (backend->decryptors[i].ratchet != NULL) backend->ratchet_destroy(backend->decryptors[i].ratchet);
    }
    memset(backend->decryptors, 0, sizeof(backend->decryptors));
    backend->decryptor_count = 0U;
}

static size_t dcc_dave_recognized_users(
    const dcc_voice_client_t *voice_client,
    char storage[DCC_VOICE_SSRC_MAP_CAP][32],
    const char *users[DCC_VOICE_SSRC_MAP_CAP]
) {
    size_t count = 0U;
    for (size_t i = 0; i < voice_client->dave_participant_count && count < DCC_VOICE_SSRC_MAP_CAP; ++i) {
        int len = snprintf(storage[count], sizeof(storage[count]), "%llu",
            (unsigned long long)voice_client->dave_participants[i]);
        if (len > 0 && (size_t)len < sizeof(storage[count])) {
            users[count] = storage[count];
            count++;
        }
    }
    return count;
}

static dcc_status_t dcc_dave_install_ratchets(dcc_voice_client_t *voice_client) {
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    char self_id[32];
    int self_len = snprintf(self_id, sizeof(self_id), "%llu", (unsigned long long)voice_client->dave_self_user_id);
    if (backend == NULL || backend->session == NULL || self_len <= 0 || (size_t)self_len >= sizeof(self_id)) {
        return DCC_ERR_STATE;
    }
    dcc_dave_destroy_media(backend);
    backend->encryptor = backend->encryptor_create();
    backend->self_ratchet = backend->session_get_ratchet(backend->session, self_id);
    if (backend->encryptor == NULL || backend->self_ratchet == NULL) return DCC_ERR_STATE;
    backend->encryptor_set_ratchet(backend->encryptor, backend->self_ratchet);
    backend->encryptor_assign_codec(backend->encryptor, voice_client->ssrc, 1);
    backend->encryptor_set_passthrough(backend->encryptor, false);

    for (size_t i = 0; i < voice_client->dave_participant_count; ++i) {
        dcc_snowflake_t user_id = voice_client->dave_participants[i];
        if (user_id == voice_client->dave_self_user_id) continue;
        dcc_dave_decryptor_entry_t *entry = &backend->decryptors[backend->decryptor_count];
        char id[32];
        int len = snprintf(id, sizeof(id), "%llu", (unsigned long long)user_id);
        if (len <= 0 || (size_t)len >= sizeof(id)) continue;
        entry->ratchet = backend->session_get_ratchet(backend->session, id);
        entry->decryptor = backend->decryptor_create();
        if (entry->ratchet == NULL || entry->decryptor == NULL) {
            if (entry->decryptor != NULL) backend->decryptor_destroy(entry->decryptor);
            if (entry->ratchet != NULL) backend->ratchet_destroy(entry->ratchet);
            memset(entry, 0, sizeof(*entry));
            continue;
        }
        entry->user_id = user_id;
        backend->decryptor_set_ratchet(entry->decryptor, entry->ratchet);
        backend->decryptor_set_passthrough(entry->decryptor, false);
        backend->decryptor_count++;
    }
    voice_client->dave_media_ready = 1U;
    return DCC_OK;
}

static dcc_status_t dcc_dave_send_key_package(dcc_voice_client_t *voice_client) {
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    uint8_t *package = NULL;
    size_t package_size = 0U;
    backend->session_get_key_package(backend->session, &package, &package_size);
    if (package == NULL || package_size == 0U) return DCC_ERR_STATE;
    dcc_status_t status = dcc_voice_client_send_dave_mls_key_package(voice_client, package, package_size);
    backend->free_memory(package);
    return status;
}

dcc_status_t dcc_voice_dave_backend_load(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) return DCC_ERR_INVALID_ARG;
    if (voice_client->dave_backend != NULL) return DCC_OK;
#if !defined(DCC_WITH_DAVE)
    return DCC_ERR_STATE;
#else
    const char *configured = getenv("DCC_DAVE_LIBRARY");
#if defined(_WIN32)
    const char *candidates[] = {configured, "dave.dll", "libdave.dll"};
#elif defined(__APPLE__)
    const char *candidates[] = {configured, "libdave.dylib", "dave.dylib"};
#else
    const char *candidates[] = {configured, "libdave.so", "dave.so"};
#endif
    dcc_dave_library_t library = NULL;
    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
        if (candidates[i] != NULL && candidates[i][0] != '\0' &&
            (library = dcc_dave_library_open(candidates[i])) != NULL) break;
    }
    if (library == NULL) return DCC_ERR_STATE;
    dcc_voice_dave_backend_t *backend = calloc(1U, sizeof(*backend));
    if (backend == NULL) { dcc_dave_library_close(library); return DCC_ERR_NOMEM; }
    backend->library = library;
    if (dcc_dave_load_symbols(backend) != 0 ||
        (backend->max_version = backend->max_supported_protocol_version()) < DCC_VOICE_DAVE_VERSION_1) {
        dcc_dave_library_close(library);
        free(backend);
        return DCC_ERR_STATE;
    }
    voice_client->dave_backend = backend;
    voice_client->dave_backend_ready = 1U;
    return DCC_OK;
#endif
}

dcc_status_t dcc_voice_dave_backend_init_session(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL || voice_client->dave_backend == NULL ||
        voice_client->dave_self_user_id == 0U || voice_client->channel_id == 0U) return DCC_ERR_STATE;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    if (backend->session_initialized) return DCC_OK;
    if (backend->session == NULL) {
        backend->session = backend->session_create(NULL, voice_client->session_id, dcc_dave_failure, voice_client);
        if (backend->session == NULL) return DCC_ERR_STATE;
    }
    char self_id[32];
    int len = snprintf(self_id, sizeof(self_id), "%llu", (unsigned long long)voice_client->dave_self_user_id);
    if (len <= 0 || (size_t)len >= sizeof(self_id)) return DCC_ERR_STATE;
    backend->session_init(backend->session, (uint16_t)voice_client->dave_version, voice_client->channel_id, self_id);
    backend->session_initialized = 1U;
    return DCC_OK;
}

void dcc_voice_dave_backend_reset_session(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL || voice_client->dave_backend == NULL) return;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    dcc_dave_destroy_media(backend);
    if (backend->session != NULL) backend->session_reset(backend->session);
    backend->session_initialized = 0U;
    voice_client->dave_media_ready = 0U;
}

void dcc_voice_dave_backend_unload(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL || voice_client->dave_backend == NULL) return;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    dcc_dave_destroy_media(backend);
    if (backend->session != NULL) backend->session_destroy(backend->session);
    dcc_dave_library_close(backend->library);
    free(backend);
    voice_client->dave_backend = NULL;
    voice_client->dave_backend_ready = 0U;
    voice_client->dave_media_ready = 0U;
}

uint16_t dcc_voice_dave_backend_max_version(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL && voice_client->dave_backend != NULL
        ? voice_client->dave_backend->max_version : 0U;
}

dcc_status_t dcc_voice_dave_backend_set_protocol(
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_version_t version,
    uint8_t media_ready
) {
    if (voice_client == NULL || voice_client->dave_backend == NULL) return DCC_ERR_STATE;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    if (backend->session == NULL) {
        dcc_status_t status = dcc_voice_dave_backend_init_session(voice_client);
        if (status != DCC_OK) return status;
    }
    backend->session_set_version(backend->session, (uint16_t)version);
    if (backend->encryptor != NULL) backend->encryptor_set_passthrough(backend->encryptor, !media_ready || version == 0);
    for (size_t i = 0; i < backend->decryptor_count; ++i) {
        backend->decryptor_set_passthrough(backend->decryptors[i].decryptor, !media_ready || version == 0);
    }
    voice_client->dave_media_ready = version == 0 ? 1U : (media_ready ? 1U : 0U);
    return DCC_OK;
}

dcc_status_t dcc_voice_dave_backend_handle_mls(
    dcc_voice_client_t *voice_client,
    const dcc_voice_dave_mls_frame_t *frame
) {
    if (voice_client == NULL || frame == NULL || voice_client->dave_backend == NULL) return DCC_ERR_INVALID_ARG;
    dcc_status_t status = dcc_voice_dave_backend_init_session(voice_client);
    if (status != DCC_OK) return status;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    char storage[DCC_VOICE_SSRC_MAP_CAP][32];
    const char *users[DCC_VOICE_SSRC_MAP_CAP];
    size_t user_count = dcc_dave_recognized_users(voice_client, storage, users);
    switch (frame->opcode) {
        case DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER:
            backend->session_set_external(backend->session, frame->payload, frame->payload_size);
            return dcc_dave_send_key_package(voice_client);
        case DCC_VOICE_DAVE_MLS_PROPOSALS: {
            uint8_t *commit = NULL;
            size_t commit_size = 0U;
            backend->session_process_proposals(
                backend->session, frame->payload, frame->payload_size, users, user_count, &commit, &commit_size
            );
            if (commit == NULL || commit_size == 0U) return DCC_ERR_STATE;
            status = dcc_voice_client_send_dave_mls_commit_welcome(voice_client, commit, commit_size);
            backend->free_memory(commit);
            return status;
        }
        case DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION: {
            dcc_dave_result_handle_t result = backend->session_process_commit(
                backend->session, frame->payload, frame->payload_size
            );
            if (result == NULL || backend->commit_is_failed(result)) {
                if (result != NULL) backend->commit_destroy(result);
                (void)dcc_voice_client_send_dave_invalid_commit_welcome(voice_client, frame->transition_id);
                (void)dcc_dave_send_key_package(voice_client);
                return DCC_ERR_DISCORD;
            }
            bool ignored = backend->commit_is_ignored(result);
            backend->commit_destroy(result);
            if (!ignored && (status = dcc_dave_install_ratchets(voice_client)) != DCC_OK) return status;
            voice_client->dave_transition_ready = 1U;
            return dcc_voice_client_send_dave_transition_ready(voice_client, frame->transition_id);
        }
        case DCC_VOICE_DAVE_MLS_WELCOME: {
            dcc_dave_result_handle_t result = backend->session_process_welcome(
                backend->session, frame->payload, frame->payload_size, users, user_count
            );
            if (result == NULL) {
                (void)dcc_voice_client_send_dave_invalid_commit_welcome(voice_client, frame->transition_id);
                return DCC_ERR_DISCORD;
            }
            backend->welcome_destroy(result);
            status = dcc_dave_install_ratchets(voice_client);
            if (status != DCC_OK) return status;
            voice_client->dave_transition_ready = 1U;
            return dcc_voice_client_send_dave_transition_ready(voice_client, frame->transition_id);
        }
        default:
            return DCC_ERR_INVALID_ARG;
    }
}

dcc_status_t dcc_voice_dave_backend_encrypt(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    const uint8_t *frame,
    size_t frame_size,
    uint8_t **out,
    size_t *out_size
) {
    if (out == NULL || out_size == NULL) return DCC_ERR_INVALID_ARG;
    *out = NULL; *out_size = 0U;
    if (voice_client == NULL || frame == NULL || voice_client->dave_backend == NULL ||
        !voice_client->dave_media_ready || voice_client->dave_backend->encryptor == NULL) return DCC_ERR_STATE;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    size_t capacity = backend->encryptor_max_size(backend->encryptor, 0, frame_size);
    if (capacity < frame_size || capacity == 0U) return DCC_ERR_STATE;
    uint8_t *encrypted = malloc(capacity);
    if (encrypted == NULL) return DCC_ERR_NOMEM;
    size_t written = 0U;
    int result = backend->encryptor_encrypt(
        backend->encryptor, 0, ssrc, frame, frame_size, encrypted, capacity, &written
    );
    if (result != 0 || written == 0U || written > capacity) {
        free(encrypted); voice_client->dave_encrypt_failures++;
        if (result == 2) voice_client->dave_missing_key_failures++;
        return DCC_ERR_STATE;
    }
    voice_client->dave_encrypt_success++;
    *out = encrypted; *out_size = written;
    return DCC_OK;
}

dcc_status_t dcc_voice_dave_backend_decrypt(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    const uint8_t *frame,
    size_t frame_size,
    uint8_t **out,
    size_t *out_size
) {
    if (out == NULL || out_size == NULL) return DCC_ERR_INVALID_ARG;
    *out = NULL; *out_size = 0U;
    if (voice_client == NULL || frame == NULL || voice_client->dave_backend == NULL ||
        !voice_client->dave_media_ready) return DCC_ERR_STATE;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    dcc_dave_decryptor_entry_t *entry = NULL;
    for (size_t i = 0; i < backend->decryptor_count; ++i) {
        if (backend->decryptors[i].user_id == user_id) { entry = &backend->decryptors[i]; break; }
    }
    if (entry == NULL) { voice_client->dave_missing_key_failures++; return DCC_ERR_STATE; }
    size_t capacity = backend->decryptor_max_size(entry->decryptor, 0, frame_size);
    if (capacity == 0U) return DCC_ERR_STATE;
    uint8_t *plain = malloc(capacity);
    if (plain == NULL) return DCC_ERR_NOMEM;
    size_t written = 0U;
    int result = backend->decryptor_decrypt(entry->decryptor, 0, frame, frame_size, plain, capacity, &written);
    if (result != 0 || written == 0U || written > capacity) {
        free(plain); voice_client->dave_decrypt_failures++;
        if (result == 2) voice_client->dave_missing_key_failures++;
        return DCC_ERR_STATE;
    }
    voice_client->dave_decrypt_success++;
    *out = plain; *out_size = written;
    return DCC_OK;
}

void dcc_voice_dave_backend_remove_participant(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id
) {
    if (voice_client == NULL || voice_client->dave_backend == NULL) return;
    dcc_voice_dave_backend_t *backend = voice_client->dave_backend;
    for (size_t i = 0; i < backend->decryptor_count; ++i) {
        if (backend->decryptors[i].user_id != user_id) continue;
        backend->decryptor_destroy(backend->decryptors[i].decryptor);
        backend->ratchet_destroy(backend->decryptors[i].ratchet);
        backend->decryptors[i] = backend->decryptors[backend->decryptor_count - 1U];
        memset(&backend->decryptors[backend->decryptor_count - 1U], 0, sizeof(backend->decryptors[0]));
        backend->decryptor_count--;
        return;
    }
}
