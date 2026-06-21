#include "discord_game_internal.h"

int main(void) {
    load_env();

    const char *token = getenv("BOT_TOKEN");
    if (token == NULL || token[0] == '\0') {
        token = getenv("DISCORD_TOKEN");
    }
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "BOT_TOKEN or DISCORD_TOKEN environment variable not set (did you create .env?)\n");
        return 1;
    }

    discord_game_state_t state = {
        .guild_id = parse_snowflake_env("DCC_GAME_GUILD_ID"),
    };

    dcc_client_t *client = NULL;
    dcc_client_options_t opts =
        DCC_CLIENT_SHARDED_OPTIONS(token, DCC_INTENTS_DEFAULT, 0U, 1U);

    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    (void)dcc_client_on(client, DCC_EVENT_LOG, on_log, NULL, NULL);
    (void)dcc_client_on(client, DCC_EVENT_READY, on_ready, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SOCKET_CLOSE, on_socket_close, NULL, NULL);
    (void)dcc_client_on_slash_command(client, GAME_COMMAND_NAME, on_game_command, NULL, NULL);
    (void)dcc_client_on(client, DCC_EVENT_BUTTON_CLICK, on_game_button, NULL, NULL);

    st = dcc_client_start(client);
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    }

    if (st != DCC_OK) {
        fprintf(stderr, "run failed: %s: %s\n", dcc_status_string(st), dcc_client_last_error(client));
    }

    dcc_client_destroy(client);
    return st == DCC_OK ? 0 : 1;
}
