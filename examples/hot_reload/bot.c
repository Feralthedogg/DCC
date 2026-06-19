#include <dcc/dcc.h>
#include <dcc/application_command.h>
#include <dcc/events.h>
#include <dcc/hot_reload.h>
#include <dcc/message.h>
#include <dcc/rest/application_commands.h>
#include <dcc/rest/interactions.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOT_RELOAD_TEST_COMMAND_NAME "테스트"
#define HOT_RELOAD_TEST_EMBED_DESCRIPTION "시발된다!!"
#if defined(__has_include)
#if __has_include("hot_reload_stamp.h")
#include "hot_reload_stamp.h"
#endif
#endif
#ifndef DCC_HOT_RELOAD_TEST_BUILD_STAMP
#define DCC_HOT_RELOAD_TEST_BUILD_STAMP "manual"
#endif

typedef struct hot_reload_test_state {
    uint64_t generation;
    uint64_t ready_count;
    uint64_t command_count;
    dcc_snowflake_t guild_id;
    int command_registration_attempted;
} hot_reload_test_state_t;

static dcc_snowflake_t parse_snowflake_env(const char *name) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return 0;
    }

    errno = 0;
    char *end = NULL;
    unsigned long long parsed = strtoull(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0') {
        fprintf(stderr, "%s must be a numeric Discord snowflake; ignoring it\n", name);
        return 0;
    }

    return (dcc_snowflake_t)parsed;
}

static void on_command_registered(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    const char *scope = user_data != NULL ? (const char *)user_data : "unknown";

    if (response == NULL) {
        fprintf(stderr, "Slash command /%s registration failed (%s): no response\n", HOT_RELOAD_TEST_COMMAND_NAME, scope);
        return;
    }

    if (response->error == DCC_OK && response->status >= 200 && response->status < 300) {
        printf(
            "Slash command /%s registered (%s, HTTP %u)\n",
            HOT_RELOAD_TEST_COMMAND_NAME,
            scope,
            (unsigned)response->status
        );
        return;
    }

    fprintf(
        stderr,
        "Slash command /%s registration failed (%s): HTTP %u, %s",
        HOT_RELOAD_TEST_COMMAND_NAME,
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

static void register_test_command(dcc_client_t *client, dcc_snowflake_t application_id, dcc_snowflake_t guild_id) {
    if (application_id == 0) {
        fprintf(stderr, "Cannot register /%s: application id was not available in READY\n", HOT_RELOAD_TEST_COMMAND_NAME);
        return;
    }

    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);

    dcc_status_t status = dcc_application_command_builder_set_name(&command, HOT_RELOAD_TEST_COMMAND_NAME);
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_description(&command, "핫 리로드 테스트 임베드를 보냅니다");
    }
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_type(&command, DCC_APPLICATION_COMMAND_CHAT_INPUT);
    }
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_dm_permission(&command, 1);
    }
    if (status != DCC_OK) {
        fprintf(stderr, "Cannot build /%s command: %s\n", HOT_RELOAD_TEST_COMMAND_NAME, dcc_status_string(status));
        return;
    }

    dcc_application_command_registration_options_t options;
    const dcc_application_command_registration_options_t *registration_options = NULL;

    dcc_application_command_registration_options_init(&options);
    if (guild_id != 0) {
        status = dcc_application_command_registration_options_set_guild(&options, guild_id);
        if (status != DCC_OK) {
            fprintf(
                stderr,
                "Cannot scope /%s to guild %llu: %s\n",
                HOT_RELOAD_TEST_COMMAND_NAME,
                (unsigned long long)guild_id,
                dcc_status_string(status)
            );
            return;
        }
        registration_options = &options;
        printf(
            "Registering /%s as guild command for guild %llu\n",
            HOT_RELOAD_TEST_COMMAND_NAME,
            (unsigned long long)guild_id
        );
    } else {
        printf(
            "Registering /%s for all guilds (global command; Discord can take a while to show it)\n",
            HOT_RELOAD_TEST_COMMAND_NAME
        );
    }

    status = dcc_rest_create_application_command_builder(
        client,
        application_id,
        registration_options,
        &command,
        on_command_registered,
        guild_id != 0 ? "guild" : "global"
    );

    if (status != DCC_OK) {
        fprintf(
            stderr,
            "Could not send /%s registration request: %s\n",
            HOT_RELOAD_TEST_COMMAND_NAME,
            dcc_status_string(status)
        );
    }
}

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
    if (user->global_name != NULL && user->global_name[0] != '\0') {
        printf(" (%s)", user->global_name);
    }
    printf(" [id=%llu, bot=%s]\n", (unsigned long long)user->id, user->bot ? "true" : "false");
}

static int is_test_command(const dcc_interaction_t *interaction) {
    return interaction != NULL &&
           interaction->name != NULL &&
           strcmp(interaction->name, HOT_RELOAD_TEST_COMMAND_NAME) == 0;
}

static void on_test_command(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (!is_test_command(interaction)) {
        return;
    }

    if (state != NULL) {
        state->command_count++;
    }

    dcc_embed_builder_t embed;
    dcc_embed_builder_init(&embed);
    dcc_embed_builder_set_title(&embed, "핫 리로드 테스트");
    dcc_embed_builder_set_description(&embed, HOT_RELOAD_TEST_EMBED_DESCRIPTION);
    dcc_embed_builder_set_color(&embed, 0x57F287);

    char footer_text[96];
    snprintf(
        footer_text,
        sizeof(footer_text),
        "generation %llu | build %s",
        (unsigned long long)(state != NULL ? state->generation : 0),
        DCC_HOT_RELOAD_TEST_BUILD_STAMP
    );
    dcc_embed_builder_set_footer(&embed, footer_text, NULL);

    printf(
        "responding /%s: generation=%llu, build=%s, embed=\"%s\"\n",
        HOT_RELOAD_TEST_COMMAND_NAME,
        (unsigned long long)(state != NULL ? state->generation : 0),
        DCC_HOT_RELOAD_TEST_BUILD_STAMP,
        HOT_RELOAD_TEST_EMBED_DESCRIPTION
    );

    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_message_builder_set_embeds(&message, &embed, 1);

    dcc_status_t status = dcc_rest_interaction_response_create_from_interaction_message_builder(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        &message,
        NULL,
        NULL
    );
    if (status != DCC_OK) {
        fprintf(stderr, "Failed to respond to /%s: %s\n", HOT_RELOAD_TEST_COMMAND_NAME, dcc_status_string(status));
    }
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (state != NULL) {
        state->ready_count++;
    }

    printf(
        "Hot reload test bot ready: shard %u/%u, generation=%llu, ready_count=%llu\n",
        ready != NULL ? ready->shard_id : 0,
        ready != NULL ? ready->shard_count : 0,
        (unsigned long long)(state != NULL ? state->generation : 0),
        (unsigned long long)(state != NULL ? state->ready_count : 0)
    );

    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    print_logged_in_user(data);

    if (state != NULL && !state->command_registration_attempted) {
        dcc_snowflake_t application_id = data != NULL ? data->application_id : 0;
        if (application_id == 0 && data != NULL) {
            application_id = data->user.id;
        }
        state->command_registration_attempted = 1;
        register_test_command(client, application_id, state->guild_id);
    }
}

static dcc_status_t hot_reload_test_load(dcc_bot_module_ctx_t *ctx, void *user_data) {
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)user_data;
    if (state == NULL) {
        return DCC_ERR_STATE;
    }

    state->generation = dcc_bot_module_generation(ctx);
    state->guild_id = parse_snowflake_env("DCC_HOT_RELOAD_GUILD_ID");
    state->command_registration_attempted = 0;

    printf(
        "hot reload test module loaded: generation=%llu, build=%s\n",
        (unsigned long long)state->generation,
        DCC_HOT_RELOAD_TEST_BUILD_STAMP
    );
    return DCC_OK;
}

static void hot_reload_test_unload(dcc_bot_module_ctx_t *ctx, void *user_data) {
    (void)ctx;
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)user_data;
    printf(
        "hot reload test module unloading: generation=%llu, commands=%llu\n",
        (unsigned long long)(state != NULL ? state->generation : 0),
        (unsigned long long)(state != NULL ? state->command_count : 0)
    );
}

static const dcc_bot_module_handler_t hot_reload_test_handlers[] = {
    DCC_BOT_MODULE_ON(DCC_EVENT_READY, on_ready),
    DCC_BOT_MODULE_ON(DCC_EVENT_SLASH_COMMAND, on_test_command),
};

DCC_BOT_MODULE_WITH_STATE(
    hot_reload_test_state_t,
    hot_reload_test_handlers,
    hot_reload_test_load,
    hot_reload_test_unload
)
