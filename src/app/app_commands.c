#include "internal/app/dcc_app_internal.h"
#include "internal/dcc_core_internal.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dcc_app_command_sync_state {
    dcc_app_t *app;
    dcc_app_command_sync_options_t options;
} dcc_app_command_sync_state_t;

static int dcc_app_command_sync_options_has_field(
    const dcc_app_command_sync_options_t *options,
    size_t offset,
    size_t field_size
) {
    return options != NULL &&
           (options->size == 0U ||
            (options->size >= offset && field_size <= options->size - offset));
}

static void dcc_app_command_sync_log_plan(
    dcc_client_t *client,
    const dcc_command_registry_plan_t *plan,
    uint8_t apply
) {
    char message[256];
    snprintf(
        message,
        sizeof(message),
        "app command sync %s: create=%zu update=%zu delete_stale=%zu noop=%zu scope=%s",
        apply ? "apply" : "plan",
        plan != NULL ? plan->create_count : 0U,
        plan != NULL ? plan->update_count : 0U,
        plan != NULL ? plan->delete_stale_count : 0U,
        plan != NULL ? plan->noop_count : 0U,
        plan != NULL && plan->guild_id != 0U ? "guild" : "global"
    );
    dcc_emit_log(client, DCC_LOG_INFO, message);
}

static void dcc_app_command_sync_log_status(
    dcc_client_t *client,
    dcc_log_level_t level,
    const char *prefix,
    dcc_status_t status
) {
    char message[192];
    snprintf(message, sizeof(message), "%s: %s", prefix, dcc_status_string(status));
    dcc_emit_log(client, level, message);
    if (level >= DCC_LOG_WARN) {
        dcc_set_error(client, message);
    }
}

static void dcc_app_command_sync_fetch_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    dcc_app_command_sync_state_t *state = (dcc_app_command_sync_state_t *)user_data;
    if (state == NULL || state->app == NULL) {
        free(state);
        return;
    }
    if (response == NULL || response->error != DCC_OK ||
        response->status < 200U || response->status >= 300U ||
        response->body == NULL) {
        dcc_app_command_sync_log_status(
            client,
            DCC_LOG_WARN,
            "app command sync remote fetch failed",
            response != NULL && response->error != DCC_OK ? response->error : DCC_ERR_DISCORD
        );
        free(state);
        return;
    }

    dcc_command_registry_remote_snapshot_t snapshot;
    dcc_status_t status = dcc_command_registry_remote_snapshot_parse_json(
        response->body,
        response->body_len,
        &snapshot
    );
    if (status != DCC_OK) {
        dcc_app_command_sync_log_status(client, DCC_LOG_WARN, "app command sync parse failed", status);
        free(state);
        return;
    }

    dcc_command_registry_plan_t plan;
    memset(&plan, 0, sizeof(plan));
    status = dcc_app_build_command_plan(
        state->app,
        &state->options.command_registry,
        snapshot.entries,
        snapshot.entry_count,
        &plan
    );
    dcc_command_registry_remote_snapshot_deinit(&snapshot);
    if (status != DCC_OK) {
        dcc_app_command_sync_log_status(client, DCC_LOG_WARN, "app command sync plan failed", status);
        free(state);
        return;
    }

    dcc_app_command_sync_log_plan(client, &plan, state->options.apply);
    if (state->options.apply) {
        status = dcc_app_apply_command_plan(
            state->app,
            state->options.application_id,
            &state->options.command_registry,
            &plan,
            NULL,
            NULL
        );
        if (status != DCC_OK) {
            dcc_app_command_sync_log_status(client, DCC_LOG_WARN, "app command sync apply failed", status);
        }
    }

    dcc_command_registry_plan_deinit(&plan);
    free(state);
}

static void dcc_app_command_sync_ready_cb(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    dcc_app_t *app = (dcc_app_t *)user_data;
    if (app == NULL || event == NULL || dcc_event_type(event) != DCC_EVENT_READY ||
        !app->command_sync_on_ready) {
        return;
    }
    if (app->command_sync_options.once && app->command_sync_ran) {
        return;
    }
    app->command_sync_ran = 1U;

    dcc_app_command_sync_state_t *state =
        (dcc_app_command_sync_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        dcc_app_command_sync_log_status(client, DCC_LOG_WARN, "app command sync allocation failed", DCC_ERR_NOMEM);
        return;
    }
    state->app = app;
    state->options = app->command_sync_options;
    if (state->options.application_id == 0U && state->options.infer_application_id) {
        const dcc_gateway_event_data_t *data = dcc_event_data(event);
        if (data != NULL) {
            state->options.application_id =
                data->application_id != 0U ? data->application_id : data->user_id;
        }
    }
    if (state->options.application_id == 0U) {
        dcc_app_command_sync_log_status(
            client,
            DCC_LOG_WARN,
            "app command sync application id inference failed",
            DCC_ERR_INVALID_ARG
        );
        free(state);
        return;
    }

    dcc_status_t status = dcc_command_registry_fetch_remote(
        client,
        state->options.application_id,
        &state->options.command_registry,
        dcc_app_command_sync_fetch_cb,
        state
    );
    if (status != DCC_OK) {
        dcc_app_command_sync_log_status(client, DCC_LOG_WARN, "app command sync remote fetch queue failed", status);
        free(state);
    }
}

dcc_status_t dcc_app_build_command_plan(
    dcc_app_t *app,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    dcc_command_registry_plan_t *out
) {
    if (app == NULL || out == NULL || (remote == NULL && remote_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    const dcc_command_registry_options_t *effective =
        options != NULL ? options : &app->command_registry_options;
    return dcc_command_registry_build_plan(&app->registry, effective, remote, remote_count, out);
}

dcc_status_t dcc_app_apply_command_plan(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_plan_t *plan,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U || plan == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const dcc_command_registry_options_t *effective =
        options != NULL ? options : &app->command_registry_options;
    return dcc_command_registry_apply(app->client, application_id, effective, plan, cb, user_data);
}

dcc_status_t dcc_app_sync_commands_from_json(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const char *remote_json,
    size_t remote_json_len,
    dcc_command_registry_plan_t *out_plan,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U || remote_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_command_registry_remote_snapshot_t snapshot;
    dcc_status_t status =
        dcc_command_registry_remote_snapshot_parse_json(remote_json, remote_json_len, &snapshot);
    if (status != DCC_OK) {
        return status;
    }

    dcc_command_registry_plan_t local_plan;
    dcc_command_registry_plan_t *plan = out_plan != NULL ? out_plan : &local_plan;
    status = dcc_app_build_command_plan(app, options, snapshot.entries, snapshot.entry_count, plan);
    if (status == DCC_OK) {
        status = dcc_app_apply_command_plan(app, application_id, options, plan, cb, user_data);
    }
    if (out_plan == NULL) {
        dcc_command_registry_plan_deinit(plan);
    }
    dcc_command_registry_remote_snapshot_deinit(&snapshot);
    return status;
}

dcc_status_t dcc_app_register_command_sync_listener(dcc_app_t *app) {
    if (app == NULL || app->client == NULL ||
        (app->command_sync_options.application_id == 0U &&
         !app->command_sync_options.infer_application_id)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (app->command_sync_listener_registered) {
        return DCC_OK;
    }
    dcc_listener_id_t id = 0U;
    dcc_status_t status = dcc_client_on(
        app->client,
        DCC_EVENT_READY,
        dcc_app_command_sync_ready_cb,
        app,
        &id
    );
    if (status == DCC_OK) {
        app->command_sync_listener_id = id;
        app->command_sync_listener_registered = 1U;
    }
    return status;
}

dcc_status_t dcc_app_sync_commands_on_ready(
    dcc_app_t *app,
    const dcc_app_command_sync_options_t *options
) {
    if (app == NULL || options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_command_sync_options_t effective;
    dcc_app_command_sync_options_init(&effective);
    effective.command_registry = app->command_registry_options;
    if (dcc_app_command_sync_options_has_field(
            options,
            offsetof(dcc_app_command_sync_options_t, application_id),
            sizeof(options->application_id)
        )) {
        effective.application_id = options->application_id;
    }
    if (dcc_app_command_sync_options_has_field(
            options,
            offsetof(dcc_app_command_sync_options_t, command_registry),
            sizeof(options->command_registry)
        ) && options->command_registry.size != 0U) {
        effective.command_registry = options->command_registry;
    }
    if (dcc_app_command_sync_options_has_field(
            options,
            offsetof(dcc_app_command_sync_options_t, apply),
            sizeof(options->apply)
        )) {
        effective.apply = options->apply != 0U;
    }
    if (dcc_app_command_sync_options_has_field(
            options,
            offsetof(dcc_app_command_sync_options_t, once),
            sizeof(options->once)
        )) {
        effective.once = options->once != 0U;
    }
    if (dcc_app_command_sync_options_has_field(
            options,
            offsetof(dcc_app_command_sync_options_t, infer_application_id),
            sizeof(options->infer_application_id)
        )) {
        effective.infer_application_id = options->infer_application_id != 0U;
    }
    if (effective.application_id == 0U && !effective.infer_application_id) {
        return DCC_ERR_INVALID_ARG;
    }

    app->command_sync_options = effective;
    app->command_sync_on_ready = 1U;
    return dcc_app_register_command_sync_listener(app);
}

dcc_status_t dcc_app_disable_command_sync_on_ready(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    app->command_sync_on_ready = 0U;
    app->command_sync_ran = 0U;
    if (app->command_sync_listener_registered) {
        dcc_status_t status = dcc_client_off(app->client, DCC_EVENT_READY, app->command_sync_listener_id);
        if (status != DCC_OK) {
            return status;
        }
        app->command_sync_listener_registered = 0U;
        app->command_sync_listener_id = 0U;
    }
    return DCC_OK;
}
