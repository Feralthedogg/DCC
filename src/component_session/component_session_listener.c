#include "internal/component_session/dcc_component_session_internal.h"

#include <dcc/events/accessors.h>
#include <dcc/events/listeners.h>

#include "internal/json/dcc_json.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct dcc_component_session_listener_state {
    dcc_component_session_store_t store;
    dcc_component_session_action_cb cb;
    void *user_data;
    uint64_t fixed_now_ms;
    dcc_component_session_now_fn now_fn;
    void *now_user_data;
    uint8_t listen_buttons;
    uint8_t listen_selects;
    uint8_t listen_forms;
} dcc_component_session_listener_state_t;

void dcc_component_session_listener_options_init(
    dcc_component_session_listener_options_t *options
) {
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->listen_buttons = 1U;
    options->listen_selects = 1U;
    options->listen_forms = 1U;
}

static uint64_t dcc_component_session_wall_now_ms(void) {
    time_t now = time(NULL);
    if (now <= (time_t)0) {
        return 0;
    }
    return (uint64_t)now * 1000U;
}

static uint64_t dcc_component_session_listener_now(
    const dcc_component_session_listener_state_t *state
) {
    if (state != NULL && state->now_fn != NULL) {
        return state->now_fn(state->now_user_data);
    }
    if (state != NULL && state->fixed_now_ms != 0U) {
        return state->fixed_now_ms;
    }
    return dcc_component_session_wall_now_ms();
}

static int dcc_component_session_parse_snowflake_string(
    const char *text,
    dcc_snowflake_t *out
) {
    if (text == NULL || text[0] == '\0' || out == NULL) {
        return 0;
    }
    uint64_t value = 0;
    for (const char *p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') {
            return 0;
        }
        uint64_t digit = (uint64_t)(*p - '0');
        if (value > (UINT64_MAX - digit) / 10U) {
            return 0;
        }
        value = value * 10U + digit;
    }
    *out = (dcc_snowflake_t)value;
    return 1;
}

static int dcc_component_session_json_snowflake(
    const dcc_json_t *json,
    dcc_snowflake_t *out
) {
    if (json == NULL || out == NULL) {
        return 0;
    }
    uint64_t value = 0;
    if (dcc_json_u64(json, &value) == 0) {
        *out = (dcc_snowflake_t)value;
        return 1;
    }
    return dcc_component_session_parse_snowflake_string(dcc_json_string(json), out);
}

static dcc_snowflake_t dcc_component_session_user_id_from_payload(
    const dcc_json_t *payload
) {
    if (payload == NULL || dcc_json_typeof(payload) != DCC_JSON_OBJECT) {
        return 0;
    }
    dcc_snowflake_t user_id = 0;
    const dcc_json_t *user = dcc_json_object_get(payload, "user");
    if (dcc_component_session_json_snowflake(dcc_json_object_get(user, "id"), &user_id)) {
        return user_id;
    }
    const dcc_json_t *member = dcc_json_object_get(payload, "member");
    user = dcc_json_object_get(member, "user");
    if (dcc_component_session_json_snowflake(dcc_json_object_get(user, "id"), &user_id)) {
        return user_id;
    }
    return 0;
}

static dcc_snowflake_t dcc_component_session_event_user_id(
    const dcc_event_t *event
) {
    const dcc_raw_event_t *raw = dcc_event_raw(event);
    if (raw == NULL || raw->json == NULL || raw->json_len == 0U) {
        return 0;
    }

    dcc_json_t *root = NULL;
    if (dcc_json_parse(raw->json, raw->json_len, &root) != DCC_OK || root == NULL) {
        return 0;
    }

    const dcc_json_t *payload = root;
    const dcc_json_t *d = dcc_json_object_get(root, "d");
    if (d != NULL && dcc_json_typeof(d) == DCC_JSON_OBJECT) {
        payload = d;
    }
    dcc_snowflake_t user_id = dcc_component_session_user_id_from_payload(payload);
    dcc_json_free(root);
    return user_id;
}

static uint8_t dcc_component_session_result_is_routeable(
    dcc_component_session_verify_status_t status
) {
    return status != DCC_COMPONENT_SESSION_VERIFY_BAD_FORMAT &&
           status != DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND;
}

static void dcc_component_session_listener_trampoline(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    dcc_component_session_listener_state_t *state =
        (dcc_component_session_listener_state_t *)user_data;
    if (state == NULL || state->cb == NULL) {
        return;
    }

    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (interaction == NULL || interaction->custom_id == NULL) {
        return;
    }

    dcc_component_session_check_t check;
    memset(&check, 0, sizeof(check));
    check.size = sizeof(check);
    check.custom_id = interaction->custom_id;
    check.now_ms = dcc_component_session_listener_now(state);
    check.user_id = dcc_component_session_event_user_id(event);
    check.channel_id = interaction->channel_id;
    check.guild_id = interaction->guild_id;

    dcc_component_session_result_t result;
    if (dcc_component_session_store_verify(&state->store, &check, &result) != DCC_OK ||
        !dcc_component_session_result_is_routeable(result.status)) {
        return;
    }

    state->cb(client, event, &result, state->user_data);
}

static dcc_status_t dcc_component_session_listener_copy_store(
    dcc_component_session_store_t *dst,
    const dcc_component_session_store_t *src
) {
    if (dst == NULL || src == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_session_store_init(dst);
    const dcc_component_session_store_state_t *src_state =
        (const dcc_component_session_store_state_t *)src->state;
    if (src_state == NULL || src_state->count == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < src_state->count; ++i) {
        dcc_component_session_t session;
        memset(&session, 0, sizeof(session));
        session.size = sizeof(session);
        session.state = (void *)&src_state->sessions[i];
        dcc_status_t status = dcc_component_session_store_add(dst, &session);
        if (status != DCC_OK) {
            dcc_component_session_store_deinit(dst);
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_session_listener_state_new(
    const dcc_component_session_store_t *store,
    const dcc_component_session_listener_options_t *options,
    dcc_component_session_action_cb cb,
    void *user_data,
    dcc_component_session_listener_state_t **out_state
) {
    if (out_state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_state = NULL;

    dcc_component_session_listener_state_t *state =
        (dcc_component_session_listener_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_component_session_listener_copy_store(&state->store, store);
    if (status != DCC_OK) {
        free(state);
        return status;
    }

    state->cb = cb;
    state->user_data = user_data;
    state->listen_buttons = 1U;
    state->listen_selects = 1U;
    state->listen_forms = 1U;
    if (options != NULL && options->size >= sizeof(*options)) {
        state->fixed_now_ms = options->now_ms;
        state->now_fn = options->now_fn;
        state->now_user_data = options->now_user_data;
        state->listen_buttons = options->listen_buttons;
        state->listen_selects = options->listen_selects;
        state->listen_forms = options->listen_forms;
    }
    *out_state = state;
    return DCC_OK;
}

static void dcc_component_session_listener_state_free(
    dcc_component_session_listener_state_t *state
) {
    if (state == NULL) {
        return;
    }
    dcc_component_session_store_deinit(&state->store);
    free(state);
}

static dcc_status_t dcc_component_session_register_listener_type(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint8_t enabled,
    dcc_component_session_listener_state_t *state,
    dcc_listener_id_t *out_id
) {
    if (!enabled) {
        return DCC_OK;
    }
    return dcc_client_on(
        client,
        type,
        dcc_component_session_listener_trampoline,
        state,
        out_id
    );
}

dcc_status_t dcc_client_on_component_session_store(
    dcc_client_t *client,
    const dcc_component_session_store_t *store,
    const dcc_component_session_listener_options_t *options,
    dcc_component_session_action_cb cb,
    void *user_data,
    dcc_component_session_listener_t *out_listener
) {
    if (client == NULL || store == NULL || cb == NULL || out_listener == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_component_session_listener_t listener;
    memset(&listener, 0, sizeof(listener));
    listener.size = sizeof(listener);

    dcc_component_session_listener_state_t *state = NULL;
    dcc_status_t status = dcc_component_session_listener_state_new(
        store,
        options,
        cb,
        user_data,
        &state
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!state->listen_buttons && !state->listen_selects && !state->listen_forms) {
        dcc_component_session_listener_state_free(state);
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_component_session_register_listener_type(
        client,
        DCC_EVENT_BUTTON_CLICK,
        state->listen_buttons,
        state,
        &listener.button_id
    );
    if (status == DCC_OK) {
        status = dcc_component_session_register_listener_type(
            client,
            DCC_EVENT_SELECT_CLICK,
            state->listen_selects,
            state,
            &listener.select_id
        );
    }
    if (status == DCC_OK) {
        status = dcc_component_session_register_listener_type(
            client,
            DCC_EVENT_FORM_SUBMIT,
            state->listen_forms,
            state,
            &listener.form_id
        );
    }

    if (status != DCC_OK) {
        if (listener.button_id != 0U) {
            (void)dcc_client_off(client, DCC_EVENT_BUTTON_CLICK, listener.button_id);
        }
        if (listener.select_id != 0U) {
            (void)dcc_client_off(client, DCC_EVENT_SELECT_CLICK, listener.select_id);
        }
        if (listener.form_id != 0U) {
            (void)dcc_client_off(client, DCC_EVENT_FORM_SUBMIT, listener.form_id);
        }
        dcc_component_session_listener_state_free(state);
        return status;
    }

    listener.state = state;
    *out_listener = listener;
    return DCC_OK;
}

dcc_status_t dcc_client_on_component_session(
    dcc_client_t *client,
    const dcc_component_session_t *session,
    const dcc_component_session_listener_options_t *options,
    dcc_component_session_action_cb cb,
    void *user_data,
    dcc_component_session_listener_t *out_listener
) {
    if (session == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_session_store_t store;
    dcc_component_session_store_init(&store);
    dcc_status_t status = dcc_component_session_store_add(&store, session);
    if (status == DCC_OK) {
        status = dcc_client_on_component_session_store(
            client,
            &store,
            options,
            cb,
            user_data,
            out_listener
        );
    }
    dcc_component_session_store_deinit(&store);
    return status;
}

dcc_status_t dcc_client_off_component_session(
    dcc_client_t *client,
    dcc_component_session_listener_t *listener
) {
    if (client == NULL || listener == NULL || listener->state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t first_status = DCC_OK;
    if (listener->button_id != 0U) {
        dcc_status_t status = dcc_client_off(client, DCC_EVENT_BUTTON_CLICK, listener->button_id);
        if (status != DCC_OK && first_status == DCC_OK) {
            first_status = status;
        }
    }
    if (listener->select_id != 0U) {
        dcc_status_t status = dcc_client_off(client, DCC_EVENT_SELECT_CLICK, listener->select_id);
        if (status != DCC_OK && first_status == DCC_OK) {
            first_status = status;
        }
    }
    if (listener->form_id != 0U) {
        dcc_status_t status = dcc_client_off(client, DCC_EVENT_FORM_SUBMIT, listener->form_id);
        if (status != DCC_OK && first_status == DCC_OK) {
            first_status = status;
        }
    }

    dcc_component_session_listener_state_free(
        (dcc_component_session_listener_state_t *)listener->state
    );
    memset(listener, 0, sizeof(*listener));
    return first_status;
}
