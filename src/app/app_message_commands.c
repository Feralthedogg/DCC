#include "internal/app/dcc_app_internal.h"

#include <stdlib.h>
#include <string.h>

typedef struct dcc_app_message_command_state {
    dcc_app_t *app;
    dcc_app_message_command_fn handler;
    void *user_data;
    size_t prefix_len;
    size_t name_len;
    char data[];
} dcc_app_message_command_state_t;

static const char *dcc_app_message_command_prefix(
    const dcc_app_message_command_state_t *state
) {
    return state->data;
}

static const char *dcc_app_message_command_name(
    const dcc_app_message_command_state_t *state
) {
    return state->data + state->prefix_len + 1U;
}

static uint8_t dcc_app_message_command_is_space(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

static const char *dcc_app_message_command_match(
    const dcc_app_message_command_state_t *state,
    const char *content
) {
    if (state == NULL || content == NULL) {
        return NULL;
    }
    if (strncmp(content, dcc_app_message_command_prefix(state), state->prefix_len) != 0) {
        return NULL;
    }

    const char *cursor = content + state->prefix_len;
    if (strncmp(cursor, dcc_app_message_command_name(state), state->name_len) != 0) {
        return NULL;
    }

    cursor += state->name_len;
    if (*cursor != '\0' && !dcc_app_message_command_is_space(*cursor)) {
        return NULL;
    }
    while (dcc_app_message_command_is_space(*cursor)) {
        cursor++;
    }
    return cursor;
}

static void dcc_app_message_command_dispatch(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    (void)client;
    dcc_app_message_command_state_t *state = (dcc_app_message_command_state_t *)user_data;
    if (state == NULL || state->handler == NULL || event == NULL) {
        return;
    }

    const dcc_message_t *message = dcc_event_message(event);
    const char *args = message != NULL
        ? dcc_app_message_command_match(state, message->content)
        : NULL;
    if (args != NULL) {
        state->handler(state->app, message, args, event, state->user_data);
    }
}

dcc_status_t dcc_app_on_message_command(
    dcc_app_t *app,
    const char *prefix,
    const char *name,
    dcc_app_message_command_fn handler,
    void *user_data
) {
    if (app == NULL || prefix == NULL || prefix[0] == '\0' || name == NULL ||
        name[0] == '\0' || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t prefix_len = strlen(prefix);
    size_t name_len = strlen(name);
    for (size_t i = 0; i < name_len; ++i) {
        if (dcc_app_message_command_is_space(name[i])) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    if (prefix_len > SIZE_MAX - name_len - 2U ||
        sizeof(dcc_app_message_command_state_t) > SIZE_MAX - prefix_len - name_len - 2U) {
        return DCC_ERR_NOMEM;
    }

    size_t allocation_size = sizeof(dcc_app_message_command_state_t) + prefix_len + 1U + name_len + 1U;
    dcc_app_message_command_state_t *state =
        (dcc_app_message_command_state_t *)calloc(1U, allocation_size);
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }

    state->app = app;
    state->handler = handler;
    state->user_data = user_data;
    state->prefix_len = prefix_len;
    state->name_len = name_len;
    memcpy(state->data, prefix, prefix_len + 1U);
    memcpy(state->data + prefix_len + 1U, name, name_len + 1U);

    return dcc_app_on_event_listener_internal(
        app,
        DCC_EVENT_MESSAGE_CREATE,
        dcc_app_message_command_dispatch,
        state
    );
}
