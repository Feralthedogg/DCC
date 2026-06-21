#include <dcc/managed_message.h>
#include <dcc/rest/messages/create.h>
#include <dcc/rest/messages/edit.h>
#include <dcc/rest/response_helpers.h>

#include <stdlib.h>

typedef struct dcc_managed_message_publish_state {
    dcc_snowflake_t channel_id;
    dcc_managed_message_ref_t new_ref;
    char *payload_json;
    dcc_managed_message_save_fn save;
    void *storage_user_data;
    dcc_managed_message_publish_cb cb;
    void *user_data;
} dcc_managed_message_publish_state_t;

static int dcc_managed_message_rest_ok(const dcc_rest_response_t *response) {
    return response != NULL &&
        response->error == DCC_OK &&
        response->status >= 200U &&
        response->status < 300U;
}

static void dcc_managed_message_publish_state_free(dcc_managed_message_publish_state_t *state) {
    if (state == NULL) {
        return;
    }
    dcc_message_builder_json_free(state->payload_json);
    free(state);
}

static void dcc_managed_message_emit_error(
    dcc_client_t *client,
    dcc_managed_message_publish_state_t *state,
    dcc_status_t status
) {
    if (state->cb != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .error = status,
        };
        state->cb(client, &response, &state->new_ref, DCC_OK, state->user_data);
    }
    dcc_managed_message_publish_state_free(state);
}

static void dcc_managed_message_create_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    dcc_managed_message_publish_state_t *state = (dcc_managed_message_publish_state_t *)user_data;
    dcc_status_t storage_status = DCC_OK;

    if (dcc_managed_message_rest_ok(response)) {
        dcc_snowflake_t message_id = 0;
        dcc_status_t parse_status = dcc_rest_response_message_id(response, &message_id);
        if (parse_status == DCC_OK) {
            state->new_ref.channel_id = state->channel_id;
            state->new_ref.message_id = message_id;
            if (state->save != NULL) {
                storage_status = state->save(&state->new_ref, state->storage_user_data);
            }
        } else {
            storage_status = parse_status;
        }
    }

    if (state->cb != NULL) {
        state->cb(client, response, &state->new_ref, storage_status, state->user_data);
    }
    dcc_managed_message_publish_state_free(state);
}

static dcc_status_t dcc_managed_message_create(
    dcc_client_t *client,
    dcc_managed_message_publish_state_t *state,
    int emit_callback_on_error
) {
    dcc_status_t status = dcc_rest_create_message(
        client,
        state->channel_id,
        state->payload_json,
        dcc_managed_message_create_cb,
        state
    );
    if (status != DCC_OK && emit_callback_on_error) {
        dcc_managed_message_emit_error(client, state, status);
    }
    return status;
}

static void dcc_managed_message_delete_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    dcc_managed_message_publish_state_t *state = (dcc_managed_message_publish_state_t *)user_data;
    if (response != NULL &&
        (dcc_managed_message_rest_ok(response) || response->status == 404U)) {
        (void)dcc_managed_message_create(client, state, 1);
        return;
    }

    if (state->cb != NULL) {
        state->cb(client, response, &state->new_ref, DCC_OK, state->user_data);
    }
    dcc_managed_message_publish_state_free(state);
}

dcc_status_t dcc_managed_message_publish_latest(
    dcc_client_t *client,
    const dcc_managed_message_options_t *options,
    dcc_managed_message_publish_cb cb,
    void *user_data
) {
    if (client == NULL ||
        options == NULL ||
        options->channel_id == 0 ||
        options->message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_managed_message_ref_t old_ref = {
        .channel_id = options->channel_id,
    };
    if (options->load != NULL) {
        dcc_status_t load_status = options->load(&old_ref, options->storage_user_data);
        if (load_status == DCC_ERR_NOT_FOUND) {
            old_ref.channel_id = options->channel_id;
            old_ref.message_id = 0;
        } else if (load_status != DCC_OK) {
            return load_status;
        }
    }
    if (old_ref.channel_id == 0) {
        old_ref.channel_id = options->channel_id;
    }

    dcc_managed_message_publish_state_t *state =
        (dcc_managed_message_publish_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->channel_id = options->channel_id;
    state->save = options->save;
    state->storage_user_data = options->storage_user_data;
    state->cb = cb;
    state->user_data = user_data;

    dcc_status_t status = dcc_message_builder_build_json(options->message, &state->payload_json);
    if (status != DCC_OK) {
        dcc_managed_message_publish_state_free(state);
        return status;
    }

    if (!options->keep_previous && old_ref.message_id != 0) {
        status = dcc_rest_delete_message(
            client,
            old_ref.channel_id,
            old_ref.message_id,
            dcc_managed_message_delete_cb,
            state
        );
        if (status != DCC_OK) {
            dcc_managed_message_publish_state_free(state);
        }
        return status;
    }

    status = dcc_managed_message_create(client, state, 0);
    if (status != DCC_OK) {
        dcc_managed_message_publish_state_free(state);
    }
    return status;
}
