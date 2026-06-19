#ifndef DCC_COMPONENT_SESSION_H
#define DCC_COMPONENT_SESSION_H

#include <dcc/component.h>
#include <dcc/error.h>
#include <dcc/events/base.h>
#include <dcc/export.h>
#include <dcc/snowflake.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_COMPONENT_SESSION_CUSTOM_ID_MAX 100U
#define DCC_COMPONENT_SESSION_ID_MAX 16U
#define DCC_COMPONENT_SESSION_ACTION_MAX 32U
#define DCC_COMPONENT_SESSION_NONCE_MAX 16U

typedef enum dcc_component_session_verify_status {
    DCC_COMPONENT_SESSION_VERIFY_OK = 0,
    DCC_COMPONENT_SESSION_VERIFY_BAD_FORMAT,
    DCC_COMPONENT_SESSION_VERIFY_BAD_SIGNATURE,
    DCC_COMPONENT_SESSION_VERIFY_EXPIRED,
    DCC_COMPONENT_SESSION_VERIFY_WRONG_USER,
    DCC_COMPONENT_SESSION_VERIFY_WRONG_CHANNEL,
    DCC_COMPONENT_SESSION_VERIFY_WRONG_GUILD,
    DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND
} dcc_component_session_verify_status_t;

typedef struct dcc_component_session_options {
    size_t size;
    const void *secret;
    size_t secret_len;
    const char *session_id;
    const char *nonce;
    uint64_t now_ms;
    uint64_t ttl_ms;
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
    uint8_t lock_user;
    uint8_t lock_channel;
    uint8_t lock_guild;
} dcc_component_session_options_t;

typedef struct dcc_component_session {
    size_t size;
    void *state;
} dcc_component_session_t;

typedef struct dcc_component_session_check {
    size_t size;
    const char *custom_id;
    uint64_t now_ms;
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
} dcc_component_session_check_t;

typedef struct dcc_component_session_result {
    size_t size;
    dcc_component_session_verify_status_t status;
    char session_id[DCC_COMPONENT_SESSION_ID_MAX + 1U];
    char action[DCC_COMPONENT_SESSION_ACTION_MAX + 1U];
    char nonce[DCC_COMPONENT_SESSION_NONCE_MAX + 1U];
    uint64_t expires_at_ms;
} dcc_component_session_result_t;

typedef struct dcc_component_session_store {
    size_t size;
    void *state;
} dcc_component_session_store_t;

typedef uint64_t (*dcc_component_session_now_fn)(void *user_data);
typedef void (*dcc_component_session_action_cb)(
    dcc_client_t *client,
    const dcc_event_t *event,
    const dcc_component_session_result_t *result,
    void *user_data
);

typedef struct dcc_component_session_listener_options {
    size_t size;
    uint64_t now_ms;
    dcc_component_session_now_fn now_fn;
    void *now_user_data;
    uint8_t listen_buttons;
    uint8_t listen_selects;
    uint8_t listen_forms;
} dcc_component_session_listener_options_t;

typedef struct dcc_component_session_listener {
    size_t size;
    dcc_listener_id_t button_id;
    dcc_listener_id_t select_id;
    dcc_listener_id_t form_id;
    void *state;
} dcc_component_session_listener_t;

DCC_API void dcc_component_session_options_init(dcc_component_session_options_t *options);
DCC_API dcc_status_t dcc_component_session_options_set_secret(
    dcc_component_session_options_t *options,
    const void *secret,
    size_t secret_len
);
DCC_API dcc_status_t dcc_component_session_options_set_ids(
    dcc_component_session_options_t *options,
    const char *session_id,
    const char *nonce
);
DCC_API dcc_status_t dcc_component_session_options_set_ttl(
    dcc_component_session_options_t *options,
    uint64_t now_ms,
    uint64_t ttl_ms
);
DCC_API dcc_status_t dcc_component_session_options_lock_user(
    dcc_component_session_options_t *options,
    dcc_snowflake_t user_id
);
DCC_API dcc_status_t dcc_component_session_options_lock_channel(
    dcc_component_session_options_t *options,
    dcc_snowflake_t channel_id
);
DCC_API dcc_status_t dcc_component_session_options_lock_guild(
    dcc_component_session_options_t *options,
    dcc_snowflake_t guild_id
);

DCC_API dcc_status_t dcc_component_session_create(
    const dcc_component_session_options_t *options,
    dcc_component_session_t *out
);
DCC_API void dcc_component_session_deinit(dcc_component_session_t *session);
DCC_API dcc_status_t dcc_component_session_custom_id(
    dcc_component_session_t *session,
    const char *action,
    char *out,
    size_t out_len
);
DCC_API dcc_status_t dcc_component_session_button(
    dcc_component_session_t *session,
    const char *action,
    const char *label,
    dcc_button_style_t style,
    dcc_component_builder_t *out
);
DCC_API dcc_status_t dcc_component_session_verify(
    const dcc_component_session_t *session,
    const dcc_component_session_check_t *check,
    dcc_component_session_result_t *result
);
DCC_API const char *dcc_component_session_verify_status_string(
    dcc_component_session_verify_status_t status
);

DCC_API void dcc_component_session_store_init(dcc_component_session_store_t *store);
DCC_API void dcc_component_session_store_deinit(dcc_component_session_store_t *store);
DCC_API dcc_status_t dcc_component_session_store_add(
    dcc_component_session_store_t *store,
    const dcc_component_session_t *session
);
DCC_API dcc_status_t dcc_component_session_store_verify(
    const dcc_component_session_store_t *store,
    const dcc_component_session_check_t *check,
    dcc_component_session_result_t *result
);
DCC_API size_t dcc_component_session_store_sweep(
    dcc_component_session_store_t *store,
    uint64_t now_ms
);
DCC_API size_t dcc_component_session_store_count(const dcc_component_session_store_t *store);

DCC_API void dcc_component_session_listener_options_init(
    dcc_component_session_listener_options_t *options
);
DCC_API dcc_status_t dcc_client_on_component_session(
    dcc_client_t *client,
    const dcc_component_session_t *session,
    const dcc_component_session_listener_options_t *options,
    dcc_component_session_action_cb cb,
    void *user_data,
    dcc_component_session_listener_t *out_listener
);
DCC_API dcc_status_t dcc_client_on_component_session_store(
    dcc_client_t *client,
    const dcc_component_session_store_t *store,
    const dcc_component_session_listener_options_t *options,
    dcc_component_session_action_cb cb,
    void *user_data,
    dcc_component_session_listener_t *out_listener
);
DCC_API dcc_status_t dcc_client_off_component_session(
    dcc_client_t *client,
    dcc_component_session_listener_t *listener
);

#ifdef __cplusplus
}
#endif

#endif
