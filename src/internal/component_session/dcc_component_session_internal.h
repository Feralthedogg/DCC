#ifndef DCC_COMPONENT_SESSION_INTERNAL_H
#define DCC_COMPONENT_SESSION_INTERNAL_H

#include <dcc/component_session.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_COMPONENT_SESSION_SIG_HEX_LEN 16U

typedef struct dcc_component_session_state {
    char session_id[DCC_COMPONENT_SESSION_ID_MAX + 1U];
    char nonce[DCC_COMPONENT_SESSION_NONCE_MAX + 1U];
    unsigned char *secret;
    size_t secret_len;
    uint64_t expires_at_ms;
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
    uint8_t lock_user;
    uint8_t lock_channel;
    uint8_t lock_guild;
    char **owned_custom_ids;
    size_t owned_custom_id_count;
    size_t owned_custom_id_cap;
} dcc_component_session_state_t;

typedef struct dcc_component_session_store_state {
    dcc_component_session_state_t *sessions;
    size_t count;
    size_t cap;
} dcc_component_session_store_state_t;

typedef struct dcc_component_session_parts {
    char session_id[DCC_COMPONENT_SESSION_ID_MAX + 1U];
    char action[DCC_COMPONENT_SESSION_ACTION_MAX + 1U];
    char nonce[DCC_COMPONENT_SESSION_NONCE_MAX + 1U];
    char sig[DCC_COMPONENT_SESSION_SIG_HEX_LEN + 1U];
} dcc_component_session_parts_t;

dcc_component_session_state_t *dcc_component_session_state(
    const dcc_component_session_t *session
);
void dcc_component_session_state_deinit(dcc_component_session_state_t *state);
dcc_status_t dcc_component_session_state_copy(
    dcc_component_session_state_t *dst,
    const dcc_component_session_state_t *src
);
dcc_status_t dcc_component_session_state_adopt_id(
    dcc_component_session_state_t *state,
    char *custom_id
);

int dcc_component_session_token_valid(const char *token, size_t max_len);
dcc_status_t dcc_component_session_make_random_hex(char *out, size_t hex_len);
dcc_status_t dcc_component_session_sign(
    const dcc_component_session_state_t *state,
    const char *action,
    const char *nonce,
    char out_sig[DCC_COMPONENT_SESSION_SIG_HEX_LEN + 1U]
);
int dcc_component_session_sig_equal(const char *a, const char *b);
dcc_status_t dcc_component_session_parse_custom_id(
    const char *custom_id,
    dcc_component_session_parts_t *parts
);
dcc_status_t dcc_component_session_build_custom_id_heap(
    const dcc_component_session_state_t *state,
    const char *action,
    char **out
);
dcc_component_session_verify_status_t dcc_component_session_verify_parts(
    const dcc_component_session_state_t *state,
    const dcc_component_session_parts_t *parts,
    const dcc_component_session_check_t *check
);

#ifdef __cplusplus
}
#endif

#endif
