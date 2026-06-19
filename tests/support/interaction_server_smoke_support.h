#ifndef DCC_TESTS_INTERACTION_SERVER_SMOKE_SUPPORT_H
#define DCC_TESTS_INTERACTION_SERVER_SMOKE_SUPPORT_H

#include <dcc/dcc.h>

#include <llam/platform.h>

#if LLAM_PLATFORM_POSIX

#include <openssl/evp.h>

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

typedef struct interaction_http_response {
    int status;
    char raw[8192];
    char body[4096];
} interaction_http_response_t;

typedef struct interaction_seen {
    int called;
    int ping_seen;
    int slash_seen;
    int health_seen;
    int prefix_seen;
    int shadow_seen;
    int slow_seen;
    int failed;
    int modal_seen;
    int autocomplete_seen;
    int ephemeral_seen;
    int button_seen;
    atomic_uint slow_entered;
    atomic_uint slow_release;
} interaction_seen_t;

typedef struct interaction_wait_state {
    dcc_interaction_server_t *server;
    dcc_status_t status;
} interaction_wait_state_t;

typedef struct interaction_slow_client_state {
    uint16_t port;
    int ok;
    interaction_http_response_t response;
} interaction_slow_client_state_t;

void interaction_hex_encode(const unsigned char *data, size_t len, char *out);
int interaction_sign(EVP_PKEY *key, const char *timestamp, const char *body, char signature_hex[129]);
int interaction_send_request(
    uint16_t port,
    const char *method,
    const char *path,
    const char *body,
    const char *timestamp,
    const char *signature_hex,
    interaction_http_response_t *out
);
int interaction_send_raw_request(uint16_t port, const char *request, interaction_http_response_t *out);
int interaction_send_oversized_header(uint16_t port, interaction_http_response_t *out);
int interaction_send_interaction_request(
    uint16_t port,
    const char *body,
    const char *timestamp,
    const char *signature_hex,
    interaction_http_response_t *out
);

void interaction_health_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
);
void interaction_prefix_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
);
void interaction_shadow_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
);
void interaction_slow_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
);
void interaction_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
);
void *interaction_wait_thread(void *arg);
void *interaction_slow_client_thread(void *arg);

int interaction_configure_smoke_routes(dcc_interaction_server_t *server, interaction_seen_t *seen);
int interaction_validate_start_state(dcc_interaction_server_t *server, uint16_t port);
int interaction_run_request_smoke(
    dcc_interaction_server_t *server,
    EVP_PKEY *key,
    const char *public_key_hex,
    uint16_t port,
    interaction_seen_t *seen
);
int interaction_run_health_and_drain_smoke(
    dcc_interaction_server_t *server,
    uint16_t port,
    interaction_seen_t *seen
);

#endif

#endif
