#include <dcc/app.h>

#include <dcc/managed_message.h>
#include <dcc/rest/messages/create.h>

#include <stdlib.h>

typedef struct dcc_app_managed_message_store_publish {
    dcc_store_managed_message_binding_t binding;
    dcc_managed_message_publish_cb cb;
    void *user_data;
} dcc_app_managed_message_store_publish_t;

static void dcc_app_managed_message_store_publish_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    const dcc_managed_message_ref_t *new_ref,
    dcc_status_t storage_status,
    void *user_data
) {
    dcc_app_managed_message_store_publish_t *state =
        (dcc_app_managed_message_store_publish_t *)user_data;
    if (state == NULL) {
        return;
    }
    if (state->cb != NULL) {
        state->cb(client, response, new_ref, storage_status, state->user_data);
    }
    free(state);
}

dcc_status_t dcc_app_send(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_message_builder(
        dcc_app_client(app),
        channel_id,
        message,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_send_text(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_app_send(app, channel_id, &message, cb, user_data);
}

dcc_status_t dcc_app_send_json(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_message(
        dcc_app_client(app),
        channel_id,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_managed_message_publish_latest(
    dcc_app_t *app,
    const dcc_managed_message_options_t *options,
    dcc_managed_message_publish_cb cb,
    void *user_data
) {
    if (app == NULL || options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_managed_message_publish_latest(dcc_app_client(app), options, cb, user_data);
}

dcc_status_t dcc_app_managed_message_publish_latest_store(
    dcc_app_t *app,
    const char *key,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_managed_message_publish_cb cb,
    void *user_data
) {
    if (app == NULL || key == NULL || key[0] == '\0' || channel_id == 0U || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_store_t *store = dcc_app_store(app);
    if (store == NULL) {
        return DCC_ERR_STATE;
    }

    dcc_app_managed_message_store_publish_t *state =
        (dcc_app_managed_message_store_publish_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_store_managed_message_binding_init(&state->binding, store, key);
    state->cb = cb;
    state->user_data = user_data;

    dcc_managed_message_options_t options = {
        .size = sizeof(options),
        .channel_id = channel_id,
        .message = message,
        .load = dcc_store_load_managed_message_ref,
        .save = dcc_store_save_managed_message_ref,
        .storage_user_data = &state->binding,
    };
    dcc_status_t status = dcc_app_managed_message_publish_latest(
        app,
        &options,
        dcc_app_managed_message_store_publish_cb,
        state
    );
    if (status != DCC_OK) {
        free(state);
    }
    return status;
}
