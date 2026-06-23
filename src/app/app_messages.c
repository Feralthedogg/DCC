#include <dcc/app.h>

#include <dcc/managed_message.h>
#include <dcc/rest/messages/create.h>
#include <dcc/rest/response_helpers.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct dcc_app_message_id_state {
    dcc_app_t *app;
    dcc_app_message_id_cb cb;
    void *user_data;
} dcc_app_message_id_state_t;

typedef struct dcc_app_message_thread_state {
    dcc_app_t *app;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
    dcc_thread_params_t thread;
    dcc_app_message_thread_cb cb;
    void *user_data;
    char *thread_name;
    char *message_json;
    dcc_snowflake_t *applied_tags;
} dcc_app_message_thread_state_t;

typedef struct dcc_app_managed_message_store_publish {
    dcc_store_managed_message_binding_t binding;
    dcc_managed_message_publish_cb cb;
    void *user_data;
} dcc_app_managed_message_store_publish_t;

static dcc_status_t dcc_app_message_id_status(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_message_id
) {
    if (response == NULL || out_message_id == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (response->error != DCC_OK) {
        return response->error;
    }
    if (response->status < 200U || response->status >= 300U) {
        return DCC_ERR_DISCORD;
    }
    return dcc_rest_response_message_id(response, out_message_id);
}

static dcc_status_t dcc_app_response_id_status(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_id
) {
    if (response == NULL || out_id == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (response->error != DCC_OK) {
        return response->error;
    }
    if (response->status < 200U || response->status >= 300U) {
        return DCC_ERR_DISCORD;
    }
    return dcc_rest_response_snowflake_field(response, "id", out_id);
}

static int dcc_app_thread_archive_duration_valid(dcc_channel_auto_archive_duration_t duration) {
    return duration == 0 ||
           duration == DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR ||
           duration == DCC_CHANNEL_AUTO_ARCHIVE_1_DAY ||
           duration == DCC_CHANNEL_AUTO_ARCHIVE_3_DAYS ||
           duration == DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK;
}

static char *dcc_app_message_thread_strdup(const char *value) {
    if (value == NULL) {
        return NULL;
    }
    size_t len = strlen(value);
    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1U);
    return copy;
}

static void dcc_app_message_thread_state_free(dcc_app_message_thread_state_t *state) {
    if (state == NULL) {
        return;
    }
    free(state->thread_name);
    free(state->message_json);
    free(state->applied_tags);
    free(state);
}

static dcc_status_t dcc_app_message_thread_state_copy(
    dcc_app_message_thread_state_t *state,
    const dcc_thread_params_t *thread
) {
    if (state == NULL ||
        thread == NULL ||
        thread->size < sizeof(*thread) ||
        thread->name == NULL ||
        thread->name[0] == '\0' ||
        strlen(thread->name) > 100U ||
        !dcc_app_thread_archive_duration_valid(thread->auto_archive_duration) ||
        (thread->applied_tag_count != 0U && thread->applied_tags == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    state->thread = *thread;
    state->thread.size = sizeof(state->thread);
    state->thread_name = dcc_app_message_thread_strdup(thread->name);
    if (state->thread_name == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->thread.name = state->thread_name;

    if (thread->message_json != NULL) {
        state->message_json = dcc_app_message_thread_strdup(thread->message_json);
        if (state->message_json == NULL) {
            return DCC_ERR_NOMEM;
        }
        state->thread.message_json = state->message_json;
    }

    if (thread->applied_tag_count != 0U) {
        if (thread->applied_tag_count > SIZE_MAX / sizeof(*thread->applied_tags)) {
            return DCC_ERR_NOMEM;
        }
        size_t bytes = thread->applied_tag_count * sizeof(*thread->applied_tags);
        state->applied_tags = (dcc_snowflake_t *)malloc(bytes);
        if (state->applied_tags == NULL) {
            return DCC_ERR_NOMEM;
        }
        memcpy(state->applied_tags, thread->applied_tags, bytes);
        state->thread.applied_tags = state->applied_tags;
    }

    return DCC_OK;
}

static void dcc_app_send_message_id_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    dcc_app_message_id_state_t *state = (dcc_app_message_id_state_t *)user_data;
    if (state == NULL) {
        return;
    }

    dcc_snowflake_t message_id = 0U;
    dcc_status_t status = dcc_app_message_id_status(response, &message_id);
    state->cb(state->app, response, message_id, status, state->user_data);
    free(state);
}

static void dcc_app_send_thread_created_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    dcc_app_message_thread_state_t *state = (dcc_app_message_thread_state_t *)user_data;
    if (state == NULL) {
        return;
    }

    dcc_snowflake_t thread_id = 0U;
    dcc_status_t status = dcc_app_response_id_status(response, &thread_id);
    if (state->cb != NULL) {
        state->cb(
            state->app,
            response,
            state->message_id,
            thread_id,
            status,
            state->user_data
        );
    }
    dcc_app_message_thread_state_free(state);
}

static void dcc_app_send_thread_message_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    dcc_app_message_thread_state_t *state = (dcc_app_message_thread_state_t *)user_data;
    if (state == NULL) {
        return;
    }

    dcc_snowflake_t message_id = 0U;
    dcc_status_t status = dcc_app_message_id_status(response, &message_id);
    if (status != DCC_OK) {
        if (state->cb != NULL) {
            state->cb(state->app, response, 0U, 0U, status, state->user_data);
        }
        dcc_app_message_thread_state_free(state);
        return;
    }

    state->message_id = message_id;
    status = dcc_app_create_thread_from_message(
        state->app,
        state->channel_id,
        message_id,
        &state->thread,
        dcc_app_send_thread_created_cb,
        state
    );
    if (status != DCC_OK) {
        if (state->cb != NULL) {
            state->cb(state->app, response, message_id, 0U, status, state->user_data);
        }
        dcc_app_message_thread_state_free(state);
    }
}

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

dcc_status_t dcc_app_send_with_id(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_app_message_id_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (cb == NULL) {
        return dcc_app_send(app, channel_id, message, NULL, NULL);
    }

    dcc_app_message_id_state_t *state =
        (dcc_app_message_id_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->app = app;
    state->cb = cb;
    state->user_data = user_data;

    dcc_status_t status = dcc_app_send(app, channel_id, message, dcc_app_send_message_id_cb, state);
    if (status != DCC_OK) {
        free(state);
    }
    return status;
}

dcc_status_t dcc_app_send_with_thread(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    const dcc_thread_params_t *thread,
    dcc_app_message_thread_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message == NULL || thread == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_message_thread_state_t *state =
        (dcc_app_message_thread_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->app = app;
    state->channel_id = channel_id;
    state->cb = cb;
    state->user_data = user_data;

    dcc_status_t status = dcc_app_message_thread_state_copy(state, thread);
    if (status == DCC_OK) {
        status = dcc_app_send(app, channel_id, message, dcc_app_send_thread_message_cb, state);
    }
    if (status != DCC_OK) {
        dcc_app_message_thread_state_free(state);
    }
    return status;
}

dcc_status_t dcc_app_send_with_thread_name(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
) {
    dcc_thread_params_t thread = {
        .size = sizeof(thread),
        .name = thread_name,
    };
    return dcc_app_send_with_thread(app, channel_id, message, &thread, cb, user_data);
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

dcc_status_t dcc_app_send_text_with_id(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    dcc_app_message_id_cb cb,
    void *user_data
) {
    if (content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_app_send_with_id(app, channel_id, &message, cb, user_data);
}

dcc_status_t dcc_app_send_text_with_thread(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
) {
    if (content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_app_send_with_thread_name(app, channel_id, &message, thread_name, cb, user_data);
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
