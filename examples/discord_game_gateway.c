#include "discord_game_internal.h"

static void print_logged_in_user(const dcc_gateway_event_data_t *data) {
    if (data == NULL || data->user.id == 0) {
        printf("Logged in as: unknown\n");
        return;
    }

    const dcc_user_t *user = &data->user;
    const char *username = user->username != NULL && user->username[0] != '\0' ? user->username : "unknown";

    printf("Logged in as: %s", username);
    if (user->discriminator != 0) {
        printf("#%04u", (unsigned)user->discriminator);
    }
    printf(" [id=%llu, bot=%s]\n", (unsigned long long)user->id, user->bot ? "true" : "false");
}

void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    discord_game_state_t *state = (discord_game_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready != NULL) {
        printf("Discord game bot ready: shard %u/%u\n", ready->shard_id, ready->shard_count);
    }

    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    print_logged_in_user(data);

    if (state == NULL || state->command_registration_attempted) {
        return;
    }

    dcc_snowflake_t application_id = data != NULL ? data->application_id : 0;
    if (application_id == 0 && data != NULL) {
        application_id = data->user.id;
    }

    state->command_registration_attempted = 1;
    register_game_command(client, application_id, state->guild_id);
}

void on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log != NULL) {
        fprintf(stderr, "[dcc:%d] %s\n", (int)log->level, log->message);
    }
}

void on_socket_close(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;
    const dcc_socket_close_event_t *close_ev = dcc_event_socket_close(event);
    if (close_ev != NULL) {
        printf("SOCKET CLOSED! Code: %u, Reason: %s\n", close_ev->code, close_ev->reason ? close_ev->reason : "null");
    }
}
