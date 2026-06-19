#include "discord_game_internal.h"

static void on_command_registered(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    const char *scope = user_data != NULL ? (const char *)user_data : "unknown";

    if (response == NULL) {
        fprintf(stderr, "Slash command /%s registration failed (%s): no response\n", GAME_COMMAND_NAME, scope);
        return;
    }

    if (response->error == DCC_OK && response->status >= 200 && response->status < 300) {
        printf("Slash command /%s registered (%s, HTTP %u)\n", GAME_COMMAND_NAME, scope, (unsigned)response->status);
        return;
    }

    fprintf(
        stderr,
        "Slash command /%s registration failed (%s): HTTP %u, %s",
        GAME_COMMAND_NAME,
        scope,
        (unsigned)response->status,
        dcc_status_string(response->error)
    );
    if (response->body != NULL && response->body_len > 0) {
        size_t body_len = response->body_len > 512 ? 512 : response->body_len;
        fprintf(stderr, ": %.*s%s", (int)body_len, response->body, response->body_len > body_len ? "..." : "");
    }
    fputc('\n', stderr);
}

void register_game_command(dcc_client_t *client, dcc_snowflake_t application_id, dcc_snowflake_t guild_id) {
    if (application_id == 0) {
        fprintf(stderr, "Cannot register /%s: application id was not available in READY\n", GAME_COMMAND_NAME);
        return;
    }

    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);

    dcc_status_t st = dcc_application_command_builder_set_name(&command, GAME_COMMAND_NAME);
    if (st == DCC_OK) {
        st = dcc_application_command_builder_set_description(&command, "Start a Tic Tac Toe game");
    }
    if (st == DCC_OK) {
        st = dcc_application_command_builder_set_type(&command, DCC_APPLICATION_COMMAND_CHAT_INPUT);
    }
    if (st == DCC_OK) {
        st = dcc_application_command_builder_set_dm_permission(&command, 1);
    }
    if (st != DCC_OK) {
        fprintf(stderr, "Cannot build /%s command: %s\n", GAME_COMMAND_NAME, dcc_status_string(st));
        return;
    }

    dcc_application_command_registration_options_t options;
    const dcc_application_command_registration_options_t *registration_options = NULL;

    dcc_application_command_registration_options_init(&options);
    if (guild_id != 0) {
        st = dcc_application_command_registration_options_set_guild(&options, guild_id);
        if (st != DCC_OK) {
            fprintf(
                stderr,
                "Cannot scope /%s to guild %llu: %s\n",
                GAME_COMMAND_NAME,
                (unsigned long long)guild_id,
                dcc_status_string(st)
            );
            return;
        }
        registration_options = &options;
        printf(
            "Registering /%s as guild command for guild %llu\n",
            GAME_COMMAND_NAME,
            (unsigned long long)guild_id
        );
    } else {
        printf(
            "Registering /%s for all guilds (global command; Discord can take a while to show it)\n",
            GAME_COMMAND_NAME
        );
    }
    st = dcc_rest_create_application_command_builder(
        client,
        application_id,
        registration_options,
        &command,
        on_command_registered,
        guild_id != 0 ? "guild" : "global"
    );

    if (st != DCC_OK) {
        fprintf(stderr, "Could not send /%s registration request: %s\n", GAME_COMMAND_NAME, dcc_status_string(st));
    }
}
