#include "internal/dcc_replay_tool.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_replay_tool_runtime_init(
    const dcc_replay_tool_options_t *options,
    dcc_replay_tool_runtime_t *runtime
) {
    if (options == NULL || runtime == NULL || options->module_path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(runtime, 0, sizeof(*runtime));
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = options->token != NULL ? options->token : "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&client_options, &runtime->client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_hot_reload_options_t hot_options;
    memset(&hot_options, 0, sizeof(hot_options));
    hot_options.size = sizeof(hot_options);
    hot_options.backend = options->in_process
        ? DCC_HOT_RELOAD_BACKEND_IN_PROCESS
        : DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER;
    hot_options.worker_path = options->worker_path;
    hot_options.worker_health_timeout_ms = options->worker_health_timeout_ms;
    hot_options.worker_drain_timeout_ms = options->worker_drain_timeout_ms;

    status = dcc_hot_reload_create(
        runtime->client,
        options->module_path,
        &hot_options,
        &runtime->hot_reload
    );
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(runtime->hot_reload);
    }
    if (status != DCC_OK) {
        dcc_replay_tool_runtime_deinit(runtime);
        return status;
    }
    runtime->gateway_session.client = runtime->client;
    return DCC_OK;
}

void dcc_replay_tool_runtime_deinit(dcc_replay_tool_runtime_t *runtime) {
    if (runtime == NULL) {
        return;
    }
    dcc_hot_reload_destroy(runtime->hot_reload);
    dcc_client_destroy(runtime->client);
    memset(runtime, 0, sizeof(*runtime));
}

dcc_status_t dcc_replay_tool_dispatch_frame(
    dcc_replay_tool_runtime_t *runtime,
    const char *json,
    size_t json_len
) {
    if (runtime == NULL || runtime->client == NULL || json == NULL || json_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    static _Thread_local dcc_json_gateway_payload_t payload;
    dcc_status_t status = dcc_json_parse_gateway_payload(json, json_len, &payload);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_gateway_session_handle_payload(
        &runtime->gateway_session,
        &payload,
        json,
        json_len
    );
}
