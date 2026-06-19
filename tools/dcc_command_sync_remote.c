#include "internal/dcc_command_sync.h"

#include <stdlib.h>
#include <string.h>

void dcc_command_sync_fetch_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    dcc_command_sync_remote_body_t *state =
        (dcc_command_sync_remote_body_t *)user_data;
    if (state == NULL || response == NULL) {
        return;
    }
    state->called = 1;
    state->status = response->status;
    state->error = response->error;
    if (response->body != NULL && response->body_len != 0U) {
        state->body = (char *)malloc(response->body_len + 1U);
        if (state->body == NULL) {
            state->error = DCC_ERR_NOMEM;
            return;
        }
        memcpy(state->body, response->body, response->body_len);
        state->body[response->body_len] = '\0';
        state->body_len = response->body_len;
    }
}

void dcc_command_sync_apply_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    dcc_command_sync_remote_body_t *state =
        (dcc_command_sync_remote_body_t *)user_data;
    if (state == NULL || response == NULL) {
        return;
    }
    state->called = 1;
    state->status = response->status;
    state->error = response->error;
    if (response->status < 200U || response->status >= 300U || response->error != DCC_OK) {
        fprintf(
            stderr,
            "command sync REST write failed: status=%u error=%s\n",
            response->status,
            dcc_status_string(response->error)
        );
        if (state->error == DCC_OK) {
            state->error = DCC_ERR_DISCORD;
        }
    }
}

dcc_status_t dcc_command_sync_read_remote_snapshot(
    const dcc_command_sync_options_t *options,
    dcc_client_t *client,
    dcc_command_registry_remote_snapshot_t *out
) {
    char *json = NULL;
    size_t json_len = 0;
    dcc_status_t status = DCC_OK;

    if (options->remote_path != NULL) {
        status = dcc_command_sync_load_file(options->remote_path, &json, &json_len);
        if (status != DCC_OK) {
            return status;
        }
    } else {
        dcc_command_registry_options_t registry_options;
        dcc_command_registry_options_init(&registry_options);
        dcc_status_t option_status = options->guild_id != 0
            ? dcc_command_registry_options_set_guild(&registry_options, options->guild_id)
            : dcc_command_registry_options_set_global(&registry_options);
        if (option_status != DCC_OK) {
            return option_status;
        }
        dcc_command_sync_remote_body_t body = {0};
        status = dcc_command_registry_fetch_remote(
            client,
            options->application_id,
            &registry_options,
            dcc_command_sync_fetch_cb,
            &body
        );
        if (status == DCC_OK &&
            (!body.called || body.error != DCC_OK || body.status < 200U || body.status >= 300U)) {
            status = body.error != DCC_OK ? body.error : DCC_ERR_DISCORD;
        }
        if (status == DCC_OK) {
            json = body.body;
            json_len = body.body_len;
            body.body = NULL;
        }
        free(body.body);
        if (status != DCC_OK) {
            return status;
        }
    }

    status = dcc_command_registry_remote_snapshot_parse_json(json, json_len, out);
    free(json);
    return status;
}

dcc_status_t dcc_command_sync_format_plan(
    const dcc_command_registry_plan_t *plan,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    size_t needed = 0;
    dcc_status_t status = dcc_command_registry_plan_format(plan, NULL, 0U, &needed);
    if (status != DCC_OK || needed > SIZE_MAX - 1U) {
        return status != DCC_OK ? status : DCC_ERR_NOMEM;
    }
    char *text = (char *)malloc(needed + 1U);
    if (text == NULL) {
        return DCC_ERR_NOMEM;
    }
    status = dcc_command_registry_plan_format(plan, text, needed + 1U, NULL);
    if (status != DCC_OK) {
        free(text);
        return status;
    }
    *out = text;
    return DCC_OK;
}
