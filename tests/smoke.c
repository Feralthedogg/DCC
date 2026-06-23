#include <dcc/dcc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct smoke_state {
    int ready_seen;
    int log_seen;
} smoke_state_t;

static void smoke_on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    smoke_state_t *state = (smoke_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready != NULL && ready->shard_count == 1) {
        state->ready_seen = 1;
    }
    (void)dcc_client_stop(client);
}

static void smoke_on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    smoke_state_t *state = (smoke_state_t *)user_data;
    if (dcc_event_log(event) != NULL) {
        state->log_seen = 1;
    }
}

static int run_cache_smoke(dcc_client_t *client) {
    if (!dcc_cache_enabled(client)) {
        fprintf(stderr, "cache should be enabled\n");
        return 1;
    }

    char username[] = "cache-user";
    dcc_user_t user = {
        .id = 42,
        .username = username,
    };
    if (dcc_cache_put_user(client, &user) != DCC_OK) {
        fprintf(stderr, "dcc_cache_put_user failed\n");
        return 1;
    }
    username[0] = 'X';

    const dcc_user_t *cached_user = dcc_cache_get_user(client, 42);
    if (cached_user == NULL || cached_user->username == NULL || strcmp(cached_user->username, "cache-user") != 0) {
        fprintf(stderr, "cached user did not keep a deep copy\n");
        return 1;
    }

    dcc_guild_t guild = {
        .id = 100,
        .name = "guild",
    };
    dcc_channel_t channel = {
        .id = 200,
        .guild_id = 100,
        .name = "general",
        .topic = "ops",
    };
    dcc_role_t role = {
        .id = 300,
        .guild_id = 100,
        .name = "admin",
    };
    dcc_snowflake_t role_ids[] = {300};
    dcc_member_t member = {
        .guild_id = 100,
        .user = {
            .id = 42,
            .username = "member-user",
        },
        .role_ids = role_ids,
        .role_ids_count = 1,
        .nick = "nick",
    };
    dcc_message_t message = {
        .id = 400,
        .channel_id = 200,
        .guild_id = 100,
        .author = user,
        .content = "hello",
    };
    dcc_voice_state_t voice_state = {
        .guild_id = 100,
        .channel_id = 200,
        .user_id = 42,
        .session_id = "voice-session",
    };

    if (dcc_cache_put_guild(client, &guild) != DCC_OK ||
        dcc_cache_put_channel(client, &channel) != DCC_OK ||
        dcc_cache_put_role(client, &role) != DCC_OK ||
        dcc_cache_put_member(client, &member) != DCC_OK ||
        dcc_cache_put_message(client, &message) != DCC_OK ||
        dcc_cache_put_voice_state(client, &voice_state) != DCC_OK) {
        fprintf(stderr, "cache put failed\n");
        return 1;
    }

    if (dcc_cache_get_guild(client, 100) == NULL ||
        dcc_cache_get_channel(client, 200) == NULL ||
        dcc_cache_get_role(client, 100, 300) == NULL ||
        dcc_cache_get_member(client, 100, 42) == NULL ||
        dcc_cache_get_message(client, 400) == NULL ||
        dcc_cache_get_voice_state(client, 100, 42) == NULL) {
        fprintf(stderr, "cache get failed\n");
        return 1;
    }

    dcc_snowflake_t inferred_guild_id = 0;
    if (dcc_cache_infer_guild_id_from_channel(client, 200, &inferred_guild_id) != DCC_OK ||
        inferred_guild_id != 100 ||
        dcc_client_infer_guild_id_from_channel(client, 200, &inferred_guild_id) != DCC_OK ||
        inferred_guild_id != 100) {
        fprintf(stderr, "channel guild inference failed\n");
        return 1;
    }
    if (dcc_client_update_voice_state(client, 0, 200, 0, 0) != DCC_ERR_STATE) {
        fprintf(stderr, "voice state auto guild inference did not reach gateway state validation\n");
        return 1;
    }

    dcc_cache_counts_t counts = {
        .size = sizeof(counts),
    };
    if (dcc_cache_counts(client, &counts) != DCC_OK ||
        counts.guilds != 1 ||
        counts.channels != 1 ||
        counts.roles != 1 ||
        counts.members != 1 ||
        counts.users != 1 ||
        counts.messages != 1 ||
        counts.voice_states != 1) {
        fprintf(stderr, "cache counts invalid\n");
        return 1;
    }

    if (!dcc_cache_remove_role(client, 100, 300) ||
        dcc_cache_remove_role(client, 100, 300) ||
        dcc_cache_get_role(client, 100, 300) != NULL) {
        fprintf(stderr, "cache role remove failed\n");
        return 1;
    }
    if (!dcc_cache_remove_channel(client, 200) ||
        dcc_client_infer_guild_id_from_channel(client, 200, &inferred_guild_id) != DCC_ERR_NOT_FOUND) {
        fprintf(stderr, "channel guild inference remove failed\n");
        return 1;
    }

    dcc_cache_clear(client);
    memset(&counts, 0, sizeof(counts));
    counts.size = sizeof(counts);
    if (dcc_cache_counts(client, &counts) != DCC_OK ||
        counts.guilds != 0 ||
        counts.channels != 0 ||
        counts.roles != 0 ||
        counts.members != 0 ||
        counts.users != 0 ||
        counts.messages != 0 ||
        counts.voice_states != 0) {
        fprintf(stderr, "cache clear failed\n");
        return 1;
    }

    return 0;
}

static int run_cache_policy_smoke(dcc_client_t *client) {
    dcc_cache_clear(client);

    dcc_cache_policy_t policy = {
        .size = sizeof(policy),
        .max_users = 2,
        .max_messages = 1,
    };
    if (dcc_cache_set_policy(client, &policy) != DCC_OK) {
        fprintf(stderr, "cache set policy failed\n");
        return 1;
    }

    dcc_cache_policy_t current = {
        .size = sizeof(current),
    };
    if (dcc_cache_get_policy(client, &current) != DCC_OK ||
        current.max_users != 2 ||
        current.max_messages != 1) {
        fprintf(stderr, "cache get policy failed\n");
        return 1;
    }

    dcc_user_t user_one = {
        .id = 1,
        .username = "one",
    };
    dcc_user_t user_two = {
        .id = 2,
        .username = "two",
    };
    dcc_user_t user_one_updated = {
        .id = 1,
        .username = "one-updated",
    };
    dcc_user_t user_three = {
        .id = 3,
        .username = "three",
    };

    if (dcc_cache_put_user(client, &user_one) != DCC_OK ||
        dcc_cache_put_user(client, &user_two) != DCC_OK ||
        dcc_cache_put_user(client, &user_one_updated) != DCC_OK ||
        dcc_cache_put_user(client, &user_three) != DCC_OK) {
        fprintf(stderr, "cache policy user puts failed\n");
        return 1;
    }

    const dcc_user_t *cached_one = dcc_cache_get_user(client, 1);
    dcc_cache_counts_t counts = {
        .size = sizeof(counts),
    };
    if (dcc_cache_counts(client, &counts) != DCC_OK ||
        counts.users != 2 ||
        cached_one == NULL ||
        cached_one->username == NULL ||
        strcmp(cached_one->username, "one-updated") != 0 ||
        dcc_cache_get_user(client, 2) != NULL ||
        dcc_cache_get_user(client, 3) == NULL) {
        fprintf(stderr, "cache user eviction invalid\n");
        return 1;
    }

    dcc_message_t first_message = {
        .id = 10,
        .channel_id = 100,
        .content = "first",
    };
    dcc_message_t second_message = {
        .id = 11,
        .channel_id = 100,
        .content = "second",
    };
    if (dcc_cache_put_message(client, &first_message) != DCC_OK ||
        dcc_cache_put_message(client, &second_message) != DCC_OK ||
        dcc_cache_get_message(client, 10) != NULL ||
        dcc_cache_get_message(client, 11) == NULL) {
        fprintf(stderr, "cache message eviction invalid\n");
        return 1;
    }

    policy.max_users = 1;
    if (dcc_cache_set_policy(client, &policy) != DCC_OK ||
        dcc_cache_counts(client, &counts) != DCC_OK ||
        counts.users != 1 ||
        dcc_cache_get_user(client, 1) != NULL ||
        dcc_cache_get_user(client, 3) == NULL) {
        fprintf(stderr, "cache policy trim invalid\n");
        return 1;
    }

    memset(&policy, 0, sizeof(policy));
    policy.size = sizeof(policy);
    if (dcc_cache_set_policy(client, &policy) != DCC_OK) {
        fprintf(stderr, "cache policy reset failed\n");
        return 1;
    }
    dcc_cache_clear(client);
    return 0;
}

static int run_rest_response_helper_smoke(void) {
    const char response_body[] =
        "{\"id\":\"1518234972190802092\",\"channel_id\":1518200161208238111,"
        "\"guild_id\":\"1182756143693578320\"}";
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = 200,
        .error = DCC_OK,
        .body = response_body,
        .body_len = strlen(response_body),
    };

    dcc_snowflake_t snowflake = 0;
    if (dcc_rest_response_message_id(&response, &snowflake) != DCC_OK ||
        snowflake != 1518234972190802092ULL) {
        fprintf(stderr, "message id response helper failed\n");
        return 1;
    }
    if (dcc_rest_response_channel_id(&response, &snowflake) != DCC_OK ||
        snowflake != 1518200161208238111ULL) {
        fprintf(stderr, "channel id response helper failed\n");
        return 1;
    }
    if (dcc_rest_response_guild_id(&response, &snowflake) != DCC_OK ||
        snowflake != 1182756143693578320ULL) {
        fprintf(stderr, "guild id response helper failed\n");
        return 1;
    }
    if (dcc_rest_response_snowflake_field(&response, "missing", &snowflake) != DCC_ERR_NOT_FOUND) {
        fprintf(stderr, "missing snowflake field helper failed\n");
        return 1;
    }

    const char nested_body[] = "{\"nested\":{\"id\":\"1\"},\"id\":\"2\"}";
    response.body = nested_body;
    response.body_len = strlen(nested_body);
    if (dcc_rest_response_message_id(&response, &snowflake) != DCC_OK ||
        snowflake != 2ULL) {
        fprintf(stderr, "top-level snowflake helper failed\n");
        return 1;
    }

    const char malformed_body[] = "{\"id\":\"12x\"}";
    response.body = malformed_body;
    response.body_len = strlen(malformed_body);
    if (dcc_rest_response_message_id(&response, &snowflake) != DCC_ERR_JSON) {
        fprintf(stderr, "malformed snowflake helper failed\n");
        return 1;
    }

    return 0;
}

int main(void) {
    dcc_permission_t perms = DCC_PERMISSION_SEND_MESSAGES | DCC_PERMISSION_BYPASS_SLOWMODE;
    if (!dcc_permissions_has(perms, DCC_PERMISSION_BYPASS_SLOWMODE) ||
        dcc_permissions_can(perms, DCC_PERMISSION_BAN_MEMBERS) ||
        !dcc_permissions_can(DCC_PERMISSION_ADMINISTRATOR, DCC_PERMISSION_BAN_MEMBERS)) {
        fprintf(stderr, "permission helpers invalid\n");
        return 1;
    }
    if (run_rest_response_helper_smoke() != 0) {
        return 1;
    }

    smoke_state_t state = {0};
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .shard_count = 1,
        .enable_cache = 1,
        .infer_guild_id_from_channel = 1,
    };

    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    if (run_cache_smoke(client) != 0) {
        dcc_client_destroy(client);
        return 1;
    }
    if (run_cache_policy_smoke(client) != 0) {
        dcc_client_destroy(client);
        return 1;
    }

    st = dcc_client_on(client, DCC_EVENT_READY, smoke_on_ready, &state, NULL);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_on READY failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }
    st = dcc_client_on(client, DCC_EVENT_LOG, smoke_on_log, &state, NULL);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_on LOG failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }

    st = dcc_client_start(client);
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    }
    if (st != DCC_OK) {
        fprintf(stderr, "client run failed: %s (%s)\n", dcc_status_string(st), dcc_client_last_error(client));
        dcc_client_destroy(client);
        return 1;
    }

    dcc_runtime_stats_t stats = {
        .size = sizeof(stats),
    };
    st = dcc_client_runtime_stats(client, &stats);
    if (st != DCC_OK || stats.tasks_spawned == 0 || stats.events_dispatched == 0) {
        fprintf(stderr, "runtime stats invalid\n");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_client_destroy(client);

    if (!state.ready_seen || !state.log_seen) {
        fprintf(stderr, "expected ready and log events\n");
        return 1;
    }

    return 0;
}
