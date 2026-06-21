#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"

#include <stdlib.h>

dcc_status_t dcc_client_create(const dcc_client_options_t *options, dcc_client_t **out) {
    if (options == NULL || out == NULL || options->size < offsetof(dcc_client_options_t, token) + sizeof(options->token)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_t *client = (dcc_client_t *)calloc(1, sizeof(*client));
    if (client == NULL) {
        return DCC_ERR_NOMEM;
    }

    client->token = dcc_strdup(options->token != NULL ? options->token : "");
    if (client->token == NULL) {
        free(client);
        return DCC_ERR_NOMEM;
    }

#define DCC_OPTION_HAS(field) (options->size >= offsetof(dcc_client_options_t, field) + sizeof(options->field))
    if (DCC_OPTION_HAS(gateway_url)) {
        client->gateway_url = dcc_strdup(options->gateway_url != NULL ? options->gateway_url : "");
        if (client->gateway_url == NULL) {
            free(client->token);
            free(client);
            return DCC_ERR_NOMEM;
        }
    }

    client->intents = DCC_OPTION_HAS(intents) ? options->intents : 0;
    client->shard_id = DCC_OPTION_HAS(shard_id) ? options->shard_id : 0;
    client->shard_count = DCC_OPTION_HAS(shard_count) && options->shard_count != 0 ? options->shard_count : 1;
    client->rest_concurrency = DCC_OPTION_HAS(rest_concurrency) && options->rest_concurrency != 0 ? options->rest_concurrency : 4;
    client->enable_cache = DCC_OPTION_HAS(enable_cache) ? options->enable_cache : 0;
    client->infer_guild_id_from_channel =
        DCC_OPTION_HAS(infer_guild_id_from_channel) ? options->infer_guild_id_from_channel : 0;
    client->cache_policy.size = sizeof(client->cache_policy);
    client->cache_policy.max_guilds = DCC_OPTION_HAS(cache_max_guilds) ? options->cache_max_guilds : 0;
    client->cache_policy.max_channels = DCC_OPTION_HAS(cache_max_channels) ? options->cache_max_channels : 0;
    client->cache_policy.max_roles = DCC_OPTION_HAS(cache_max_roles) ? options->cache_max_roles : 0;
    client->cache_policy.max_members = DCC_OPTION_HAS(cache_max_members) ? options->cache_max_members : 0;
    client->cache_policy.max_users = DCC_OPTION_HAS(cache_max_users) ? options->cache_max_users : 0;
    client->cache_policy.max_messages = DCC_OPTION_HAS(cache_max_messages) ? options->cache_max_messages : 0;
    client->cache_policy.max_voice_states =
        DCC_OPTION_HAS(cache_max_voice_states) ? options->cache_max_voice_states : 0;
    client->gateway_recommended_shards = client->shard_count;
    client->gateway_max_concurrency =
        DCC_OPTION_HAS(gateway_max_concurrency) && options->gateway_max_concurrency != 0
            ? options->gateway_max_concurrency
            : 1;
    client->gateway_stop_runtime_on_error = 1U;
    client->log_fn = DCC_OPTION_HAS(log_fn) ? options->log_fn : NULL;
    client->log_user_data = DCC_OPTION_HAS(log_user_data) ? options->log_user_data : NULL;
#undef DCC_OPTION_HAS
    atomic_init(&client->inferred_channel_id, 0U);
    atomic_init(&client->inferred_guild_id, 0U);
    atomic_init(&client->started, false);
    atomic_init(&client->stopping, false);
    atomic_init(&client->gateway_task_running, false);
    atomic_init(&client->gateway_task_completed, false);
    atomic_init(&client->gateway_last_status, DCC_OK);
    atomic_flag_clear(&client->gateway_control_lock);
    atomic_init(&client->gateway_stop_requested, false);
    atomic_init(&client->gateway_reconnect_requested, false);
    atomic_init(&client->gateway_reconnect_resume, false);
    atomic_flag_clear(&client->dm_channel_cache_lock);
    atomic_flag_clear(&client->voice_clients_lock);
    dcc_event_bus_init(&client->events);
    atomic_flag_clear(&client->interaction_listeners_lock);
    dcc_cache_init(&client->cache);

    dcc_status_t st = dcc_rest_init(client);
    if (st != DCC_OK) {
        dcc_cache_deinit(&client->cache);
        dcc_event_bus_deinit(&client->events);
        free(client->gateway_url);
        free(client->token);
        free(client);
        return st;
    }

    *out = client;
    return DCC_OK;
}

void dcc_client_destroy(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    dcc_voice_client_stop_owned(client);
    (void)dcc_client_stop(client);
    dcc_voice_client_unbind_owner(client);
    dcc_runtime_shutdown(&client->runtime);
    dcc_rest_deinit(client);
    dcc_cache_deinit(&client->cache);
    dcc_event_bus_deinit(&client->events);
    dcc_client_interaction_listeners_deinit(client);
    free(client->gateway_resume_url);
    free(client->gateway_session_id);
    free(client->gateway_cached_url);
    free(client->gateway_url);
    free(client->token);
    free(client);
}
