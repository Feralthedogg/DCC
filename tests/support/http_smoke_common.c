#include "http_smoke_support.h"

#include "internal/http/dcc_http.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_rest_task7_initializer_smoke(void) {
    dcc_channel_guild_params_t guild = {
        .present = UINT64_C(0),
        .name = NULL,
        .type = DCC_CHANNEL_TEXT,
        .topic = NULL,
        .bitrate = 0U,
        .user_limit = 0U,
        .rate_limit_per_user = 0U,
        .position = 0,
        .permission_overwrites = NULL,
        .permission_overwrite_count = 0U,
        .parent_id = 0U,
        .nsfw = 0U,
        .rtc_region = NULL,
        .video_quality_mode = 0U,
        .default_auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY,
        .default_reaction_emoji = {.emoji_id = 0U, .emoji_name = NULL},
        .available_tags = NULL,
        .available_tag_count = 0U,
        .default_sort_order = 0U,
        .default_forum_layout = DCC_CHANNEL_FORUM_LAYOUT_NOT_SET,
        .default_thread_rate_limit_per_user = 0U,
        .flags = UINT64_C(0),
        .nulls = UINT64_C(0),
    };
    dcc_channel_group_dm_params_t group_dm = {
        .present = UINT64_C(0),
        .name = NULL,
        .icon = NULL,
    };
    dcc_channel_thread_params_t channel_thread = {
        .present = UINT64_C(0),
        .name = NULL,
        .archived = 0U,
        .auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY,
        .locked = 0U,
        .invitable = 0U,
        .rate_limit_per_user = 0U,
        .flags = UINT64_C(0),
        .applied_tags = NULL,
        .applied_tag_count = 0U,
    };
    dcc_channel_params_t channel = {
        .size = sizeof(channel),
        .version = DCC_CHANNEL_PARAMS_VERSION,
        .kind = DCC_CHANNEL_PARAMS_GUILD,
        .payload = {.guild = guild},
    };
    dcc_channel_params_t group_dm_channel = {
        .size = sizeof(group_dm_channel),
        .version = DCC_CHANNEL_PARAMS_VERSION,
        .kind = DCC_CHANNEL_PARAMS_GROUP_DM,
        .payload = {.group_dm = group_dm},
    };
    dcc_channel_params_t thread_channel = {
        .size = sizeof(thread_channel),
        .version = DCC_CHANNEL_PARAMS_VERSION,
        .kind = DCC_CHANNEL_PARAMS_THREAD,
        .payload = {.thread = channel_thread},
    };
    dcc_channel_positions_params_t positions = {
        .size = sizeof(positions),
        .version = DCC_CHANNEL_POSITIONS_PARAMS_VERSION,
        .positions = NULL,
        .position_count = 0U,
    };
    dcc_channel_voice_status_params_t voice_status = {
        .size = sizeof(voice_status),
        .version = DCC_CHANNEL_VOICE_STATUS_PARAMS_VERSION,
        .present = UINT64_C(0),
        .status = NULL,
    };
    dcc_dm_channel_params_t dm = {
        .size = sizeof(dm),
        .version = DCC_DM_CHANNEL_PARAMS_VERSION,
        .recipient_id = 0U,
    };
    dcc_snowflake_t role_ids[] = {1U};
    dcc_invite_params_t invite = {
        .size = sizeof(invite),
        .version = DCC_INVITE_PARAMS_VERSION,
        .present = UINT64_C(0),
        .max_age = 0U,
        .max_uses = 0U,
        .temporary = 0U,
        .unique = 0U,
        .target_type = DCC_INVITE_TARGET_NONE,
        .target_user_id = 0U,
        .target_application_id = 0U,
        .target_users_filename = NULL,
        .target_users_data = NULL,
        .target_users_len = 0U,
        .role_ids = role_ids,
        .role_id_count = 1U,
    };
    dcc_thread_params_t thread = {
        .size = sizeof(thread),
        .version = DCC_THREAD_PARAMS_VERSION,
        .present = UINT64_C(0),
        .name = NULL,
        .auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY,
        .type = DCC_CHANNEL_PUBLIC_THREAD,
        .invitable = 0U,
        .rate_limit_per_user = 0U,
        .message = NULL,
        .applied_tags = NULL,
        .applied_tag_count = 0U,
    };
    dcc_guild_params_t guild_params = {
        .size = sizeof(guild_params),
        .guild_id = 0U,
        .name = NULL,
        .widget_enabled = 0U,
        .afk_channel_id = 0U,
        .afk_timeout = DCC_GUILD_AFK_60,
        .widget_channel_id = 0U,
        .default_message_notifications = DCC_GUILD_NOTIFY_ALL,
        .explicit_content_filter = DCC_GUILD_EXPLICIT_CONTENT_DISABLED,
        .mfa_level = DCC_GUILD_MFA_NONE,
        .system_channel_id = 0U,
        .premium_progress_bar_enabled = 0U,
        .rules_channel_id = 0U,
        .vanity_url_code = NULL,
        .description = NULL,
        .safety_alerts_channel_id = 0U,
        .banner = NULL,
        .discovery_splash = NULL,
        .splash = NULL,
        .icon = NULL,
    };

    return channel.size == sizeof(channel) &&
        channel.payload.guild.present == guild.present &&
        group_dm_channel.payload.group_dm.present == group_dm.present &&
        thread_channel.payload.thread.present == channel_thread.present &&
        group_dm.present == UINT64_C(0) &&
        channel_thread.present == UINT64_C(0) &&
        positions.version == DCC_CHANNEL_POSITIONS_PARAMS_VERSION &&
        voice_status.version == DCC_CHANNEL_VOICE_STATUS_PARAMS_VERSION &&
        dm.version == DCC_DM_CHANNEL_PARAMS_VERSION &&
        invite.role_id_count == 1U &&
        thread.version == DCC_THREAD_PARAMS_VERSION &&
        guild_params.size == sizeof(guild_params)
        ? 0
        : 1;
}

int run_internal_http_smoke(void) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start local http server: %s\n", strerror(errno));
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/ping", (unsigned)server.port);

    dcc_http_request_t request = {
        .method = "GET",
        .url = url,
        .timeout_ms = 5000,
    };
    dcc_http_response_t response;
    dcc_status_t st = dcc_http_perform(&request, &response);

    (void)pthread_join(thread, NULL);
    close(server.fd);

    if (st != DCC_OK) {
        fprintf(stderr, "dcc_http_perform failed: %s\n", dcc_status_string(st));
        return 1;
    }

    int ok = response.status == 200 &&
             response.body_len == 5 &&
             memcmp(response.body, "hello", 5) == 0;
    if (!ok) {
        fprintf(stderr, "unexpected response: status=%u body_len=%llu\n",
                response.status,
                (unsigned long long)response.body_len);
        dcc_http_response_deinit(&response);
        return 1;
    }

    dcc_http_response_deinit(&response);
    return 0;
}

int run_internal_http_chunked_smoke(void) {
    http_server_t server;
    pthread_t thread;
    if (start_server_mode(&server, &thread, 1) != 0) {
        fprintf(stderr, "failed to start local chunked http server: %s\n", strerror(errno));
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/chunked", (unsigned)server.port);

    dcc_http_request_t request = {
        .method = "GET",
        .url = url,
        .timeout_ms = 5000,
    };
    dcc_http_response_t response;
    dcc_status_t st = dcc_http_perform(&request, &response);

    (void)pthread_join(thread, NULL);
    close(server.fd);

    if (st != DCC_OK) {
        fprintf(stderr, "dcc_http_perform chunked failed: %s\n", dcc_status_string(st));
        return 1;
    }

    int ok = response.status == 200 &&
             response.body_len == 5 &&
             memcmp(response.body, "hello", 5) == 0;
    dcc_http_response_deinit(&response);
    if (!ok) {
        fprintf(stderr, "unexpected chunked response\n");
        return 1;
    }
    return 0;
}

void rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    rest_seen_t *seen = (rest_seen_t *)user_data;
    seen->called = 1;
    seen->status = response->status;
    seen->error = response->error;
    seen->body_len = response->body_len;
    if (response->body != NULL && response->body_len < sizeof(seen->body)) {
        memcpy(seen->body, response->body, response->body_len);
        seen->body[response->body_len] = '\0';
    }
}

void rest_result_cb(dcc_client_t *client, const dcc_rest_result_t *result, void *user_data) {
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = result != NULL ? result->http_status : 0U,
        .error = result != NULL ? dcc_rest_result_status(result) : DCC_ERR_INVALID_ARG,
        .body = result != NULL ? result->body : NULL,
        .body_len = result != NULL ? result->body_len : 0U,
    };
    rest_cb(client, &response, user_data);
}

dcc_rest_call_options_t rest_call_options(void *user_data) {
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = rest_result_cb;
    options.user_data = user_data;
    return options;
}

dcc_rest_call_options_t rest_call_options_from_legacy(dcc_rest_cb cb, void *user_data) {
    (void)cb;
    return rest_call_options(user_data);
}

static void *rest_client_runtime_main(void *arg) {
    (void)dcc_client_wait((dcc_client_t *)arg);
    return NULL;
}

dcc_status_t rest_activate_client(dcc_client_t *client) {
    dcc_status_t status = dcc_client_start(client);
    if (status != DCC_OK) {
        return status;
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, rest_client_runtime_main, client) != 0) {
        (void)dcc_client_stop(client);
        return DCC_ERR_RUNTIME;
    }
    if (pthread_detach(thread) != 0) {
        (void)dcc_client_stop(client);
        (void)pthread_join(thread, NULL);
        return DCC_ERR_RUNTIME;
    }
    return DCC_OK;
}

dcc_status_t rest_await_submission(
    dcc_client_t *client,
    dcc_status_t admission_status
) {
    if (admission_status != DCC_OK) {
        return admission_status;
    }
    return dcc_rest_async_wait(client, 5000U);
}

void rate_limited_cb(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    rest_seen_t *seen = (rest_seen_t *)user_data;
    const dcc_rate_limited_event_t *limited = dcc_event_rate_limited(event);
    if (seen == NULL || limited == NULL) {
        return;
    }
    seen->rate_limited_called = 1;
    seen->retry_after = limited->retry_after;
    seen->global = limited->global;
    if (limited->method != NULL) {
        snprintf(seen->method, sizeof(seen->method), "%s", limited->method);
    }
    if (limited->path != NULL) {
        snprintf(seen->path, sizeof(seen->path), "%s", limited->path);
    }
}

void async_rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    rest_async_seen_t *seen = (rest_async_seen_t *)user_data;
    if (seen == NULL || response == NULL) {
        (void)dcc_client_stop(client);
        return;
    }

    int index = seen->called;
    if (index >= 0 && index < 2) {
        seen->statuses[index] = response->status;
        seen->errors[index] = response->error;
        if (response->body != NULL && response->body_len < sizeof(seen->bodies[index])) {
            memcpy(seen->bodies[index], response->body, response->body_len);
            seen->bodies[index][response->body_len] = '\0';
        }
    }
    seen->called++;
    if (seen->called >= 2) {
        (void)dcc_client_stop(client);
    }
}

void async_order_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    rest_async_order_item_t *item = (rest_async_order_item_t *)user_data;
    if (item == NULL || item->state == NULL || response == NULL) {
        (void)dcc_client_stop(client);
        return;
    }

    rest_async_order_state_t *state = item->state;
    int index = state->called;
    if (index >= 0 && index < 4) {
        state->labels[index] = item->label;
        state->statuses[index] = response->status;
        state->errors[index] = response->error;
    }
    state->called++;
    if (state->expected > 0 && state->called >= state->expected) {
        (void)dcc_client_stop(client);
    }
}

void async_record_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    rest_async_seen_t *seen = (rest_async_seen_t *)user_data;
    if (seen == NULL || response == NULL) {
        return;
    }

    int index = seen->called;
    if (index >= 0 && index < 2) {
        seen->statuses[index] = response->status;
        seen->errors[index] = response->error;
        if (response->body != NULL && response->body_len < sizeof(seen->bodies[index])) {
            memcpy(seen->bodies[index], response->body, response->body_len);
            seen->bodies[index][response->body_len] = '\0';
        }
    }
    seen->called++;
}

void *client_wait_thread_main(void *arg) {
    client_wait_thread_state_t *state = (client_wait_thread_state_t *)arg;
    if (state == NULL || state->client == NULL) {
        return NULL;
    }
    state->status = dcc_client_wait(state->client);
    return NULL;
}

void *event_wait_thread_main(void *arg) {
    event_wait_thread_state_t *state = (event_wait_thread_state_t *)arg;
    if (state == NULL || state->client == NULL) {
        return NULL;
    }
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_event(state->client, state->type, state->timeout_ms, &state->snapshot);
    return NULL;
}

int run_public_event_wait_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "event wait client create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    dcc_event_snapshot_t invalid_snapshot = {
        .size = sizeof(invalid_snapshot),
    };
    if (dcc_client_wait_for_event(NULL, DCC_EVENT_READY, 1, &invalid_snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event(client, DCC_EVENT_READY, 1, &invalid_snapshot) != DCC_ERR_STATE) {
        fprintf(stderr, "event wait invalid handling failed\n");
        dcc_client_destroy(client);
        return 1;
    }

    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "event wait client start failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }

    event_wait_thread_state_t ready_wait;
    memset(&ready_wait, 0, sizeof(ready_wait));
    ready_wait.client = client;
    ready_wait.type = DCC_EVENT_READY;
    ready_wait.timeout_ms = 2000;

    pthread_t ready_thread;
    if (pthread_create(&ready_thread, NULL, event_wait_thread_main, &ready_wait) != 0) {
        fprintf(stderr, "event wait ready thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        dcc_client_destroy(client);
        return 1;
    }

    usleep(10000);

    client_wait_thread_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_STATE,
    };
    pthread_t wait_thread;
    if (pthread_create(&wait_thread, NULL, client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "event wait runtime thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(ready_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    (void)pthread_join(ready_thread, NULL);
    if (ready_wait.status != DCC_OK ||
        ready_wait.snapshot.type != DCC_EVENT_READY ||
        strcmp(ready_wait.snapshot.name, "READY") != 0 ||
        strcmp(ready_wait.snapshot.raw_name, "READY") != 0 ||
        ready_wait.snapshot.shard_id != 0) {
        fprintf(stderr,
                "event wait ready failed: status=%s type=%d name=%s raw=%s shard=%u\n",
                dcc_status_string(ready_wait.status),
                (int)ready_wait.snapshot.type,
                ready_wait.snapshot.name,
                ready_wait.snapshot.raw_name,
                ready_wait.snapshot.shard_id);
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_event_snapshot_t timeout_snapshot = {
        .size = sizeof(timeout_snapshot),
    };
    st = dcc_client_wait_for_event(client, DCC_EVENT_MESSAGE_CREATE, 25, &timeout_snapshot);
    (void)dcc_client_stop(client);
    (void)pthread_join(wait_thread, NULL);
    dcc_client_destroy(client);

    if (st != DCC_ERR_TIMEOUT || wait_state.status != DCC_OK) {
        fprintf(stderr,
                "event wait timeout failed: wait=%s runtime=%s\n",
                dcc_status_string(st),
                dcc_status_string(wait_state.status));
        return 1;
    }
    return 0;
}

#endif
