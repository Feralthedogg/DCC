#include <dcc/dcc.h>
#include <dcc/application_command.h>
#include <dcc/client.h>
#include <dcc/events.h>
#include <dcc/message.h>
#include <dcc/rest/application_commands.h>
#include <dcc/rest/interactions.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef struct pingpong_state {
    dcc_snowflake_t guild_id;
    int command_registration_attempted;
} pingpong_state_t;

static void load_env(void) {
    FILE *f = fopen(".env", "r");
    if (!f) return;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char *val = eq + 1;
            val[strcspn(val, "\r\n")] = '\0';
            setenv(line, val, 1);
        }
    }
    fclose(f);
}

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
        fprintf(stderr, "Slash command /핑 registration failed (%s): no response\n", scope);
        return;
    }

    if (response->error == DCC_OK && response->status >= 200 && response->status < 300) {
        printf("Slash command /핑 registered (%s, HTTP %u)\n", scope, (unsigned)response->status);
        return;
    }

    fprintf(
        stderr,
        "Slash command /핑 registration failed (%s): HTTP %u, %s",
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

static void register_ping_command(dcc_client_t *client, dcc_snowflake_t application_id, dcc_snowflake_t guild_id) {
    if (application_id == 0) {
        fprintf(stderr, "Cannot register /핑: application id was not available in READY\n");
        return;
    }

    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);

    dcc_status_t st = dcc_application_command_builder_set_name(&command, "핑");
    if (st == DCC_OK) {
        st = dcc_application_command_builder_set_description(&command, "퐁으로 응답합니다");
    }
    if (st == DCC_OK) {
        st = dcc_application_command_builder_set_type(&command, DCC_APPLICATION_COMMAND_CHAT_INPUT);
    }
    if (st == DCC_OK) {
        st = dcc_application_command_builder_set_dm_permission(&command, 1);
    }
    if (st != DCC_OK) {
        fprintf(stderr, "Cannot build /핑 command: %s\n", dcc_status_string(st));
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
                "Cannot scope /핑 to guild %llu: %s\n",
                (unsigned long long)guild_id,
                dcc_status_string(st)
            );
            return;
        }
        registration_options = &options;
        printf(
            "Registering /핑 as guild command for guild %llu\n",
            (unsigned long long)guild_id
        );
    } else {
        printf(
            "Registering /핑 for all guilds (global command; Discord can take a while to show it)\n"
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
        fprintf(stderr, "Could not send /핑 registration request: %s\n", dcc_status_string(st));
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

static void on_ping(dcc_client_t *client, const dcc_event_t *event, void *ud) {
    (void)ud;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (!interaction) return;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t current_ms = (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
    uint64_t interaction_ms = (interaction->id >> 22) + 1420070400000ULL;
    long long latency = (long long)(current_ms - interaction_ms);

    char footer_text[64];
    snprintf(footer_text, sizeof(footer_text), "%lld ms", latency);

    dcc_embed_builder_t embed;
    dcc_embed_builder_init(&embed);
    dcc_embed_builder_set_title(&embed, "🏓");
    dcc_embed_builder_set_description(&embed, "퐁!");
    dcc_embed_builder_set_color(&embed, 0xFF0000);
    dcc_embed_builder_set_footer(&embed, footer_text, NULL);

    dcc_message_builder_t msg;
    dcc_message_builder_init(&msg);
    dcc_message_builder_set_embeds(&msg, &embed, 1);

    dcc_rest_interaction_response_create_from_interaction_message_builder(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        &msg,
        NULL,
        NULL
    );
}

static void on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log != NULL) {
        fprintf(stderr, "[dcc:%d] %s\n", (int)log->level, log->message);
    }
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    pingpong_state_t *state = (pingpong_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready != NULL) {
        printf("PingPong Bot ready: shard %u/%u\n", ready->shard_id, ready->shard_count);
    }

    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    print_logged_in_user(data);

    if (state != NULL && !state->command_registration_attempted) {
        dcc_snowflake_t application_id = data != NULL ? data->application_id : 0;
        if (application_id == 0 && data != NULL) {
            application_id = data->user.id;
        }
        state->command_registration_attempted = 1;
        register_ping_command(client, application_id, state->guild_id);
    }
}

static void on_socket_close(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;
    const dcc_socket_close_event_t *close_ev = dcc_event_socket_close(event);
    if (close_ev) {
        printf("SOCKET CLOSED! Code: %u, Reason: %s\n", close_ev->code, close_ev->reason ? close_ev->reason : "null");
    }
}

int main(void) {
    load_env();

    const char *token = getenv("BOT_TOKEN");
    if (!token) {
        fprintf(stderr, "BOT_TOKEN environment variable not set (did you create .env?)\n");
        return 1;
    }

    pingpong_state_t state = {
        .guild_id = parse_snowflake_env("PINGPONG_GUILD_ID"),
    };

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = token,
        .intents = DCC_INTENT_GUILDS,
        .shard_id = 0,
        .shard_count = 1,
    };

    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    (void)dcc_client_on(client, DCC_EVENT_LOG, on_log, NULL, NULL);
    (void)dcc_client_on(client, DCC_EVENT_READY, on_ready, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SOCKET_CLOSE, on_socket_close, NULL, NULL);
    
    dcc_listener_id_t id = 0;
    (void)dcc_client_on_slash_command(client, "핑", on_ping, NULL, &id);

    st = dcc_client_start(client);
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    }

    if (st != DCC_OK) {
        fprintf(stderr, "run failed: %s: %s\n", dcc_status_string(st), dcc_client_last_error(client));
    }

    dcc_client_destroy(client);
    return (st == DCC_OK) ? 0 : 1;
}
