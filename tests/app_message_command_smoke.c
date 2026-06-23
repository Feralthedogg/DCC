#include <dcc/dcc.h>
#include <dcc/sugar.h>

#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"

#include <stdio.h>
#include <string.h>

typedef struct app_message_command_smoke_state {
    unsigned calls;
    unsigned bad;
    const char *last_args;
    dcc_snowflake_t last_message_id;
} app_message_command_smoke_state_t;

static void on_message_command(
    dcc_app_t *app,
    const dcc_message_t *message,
    const char *args,
    const dcc_event_t *event,
    void *user_data
) {
    app_message_command_smoke_state_t *state =
        (app_message_command_smoke_state_t *)user_data;
    state->calls++;
    state->last_args = args;
    state->last_message_id = message != NULL ? message->id : 0U;
    if (app == NULL || message == NULL || args == NULL || event == NULL ||
        event->type != DCC_EVENT_MESSAGE_CREATE) {
        state->bad = 1U;
    }
}

static dcc_status_t dispatch_message(dcc_app_t *app, dcc_snowflake_t id, const char *content) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_MESSAGE_CREATE;
    event.data.message.id = id;
    event.data.message.channel_id = 100U;
    event.data.message.guild_id = 200U;
    event.data.message.author.id = 300U;
    event.data.message.content = content;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

int main(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS | DCC_INTENT_GUILD_MESSAGES | DCC_INTENT_MESSAGE_CONTENT;

    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK || app == NULL) {
        fprintf(stderr, "app create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    app_message_command_smoke_state_t direct = {0};
    app_message_command_smoke_state_t extension = {0};
    app_message_command_smoke_state_t definition = {0};
    dcc_app_extension_t cog =
        DCC_COG(
            "message.commands",
            DCC_COG_MESSAGE_COMMANDS(
                DCC_MESSAGE_COMMAND_DATA("?", "state", on_message_command, &extension)
            )
        );
    dcc_app_definition_t definition_routes =
        DCC_APP(
            "message.commands.definition",
            DCC_APP_MESSAGE_COMMANDS(
                DCC_MESSAGE_COMMAND_DATA("#", "top", on_message_command, &definition)
            )
        );

    if (dcc_app_on_message_command(NULL, "!", "ping", on_message_command, &direct) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_on_message_command(app, "", "ping", on_message_command, &direct) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_on_message_command(app, "!", "", on_message_command, &direct) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_on_message_command(app, "!", "bad name", on_message_command, &direct) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_on_message_command(app, "!", "ping", NULL, &direct) != DCC_ERR_INVALID_ARG ||
        dcc_app_on_message_command(app, "!", "ping", on_message_command, &direct) != DCC_OK ||
        dcc_app_extension_register(app, &cog) != DCC_OK ||
        dcc_app_apply(app, &definition_routes) != DCC_OK) {
        fprintf(stderr, "message command registration contract failed\n");
        dcc_app_destroy(app);
        return 1;
    }

    if (dispatch_message(app, 1U, "!ping alpha beta") != DCC_OK ||
        direct.calls != 1U ||
        direct.last_message_id != 1U ||
        strcmp(direct.last_args, "alpha beta") != 0 ||
        dispatch_message(app, 2U, "!ping") != DCC_OK ||
        direct.calls != 2U ||
        strcmp(direct.last_args, "") != 0 ||
        dispatch_message(app, 3U, "!ping   spaced") != DCC_OK ||
        direct.calls != 3U ||
        strcmp(direct.last_args, "spaced") != 0 ||
        dispatch_message(app, 4U, "!pingpong nope") != DCC_OK ||
        dispatch_message(app, 5U, "/ping nope") != DCC_OK ||
        direct.calls != 3U ||
        dispatch_message(app, 6U, "?state from extension") != DCC_OK ||
        extension.calls != 1U ||
        strcmp(extension.last_args, "from extension") != 0 ||
        dispatch_message(app, 7U, "#top from definition") != DCC_OK ||
        definition.calls != 1U ||
        strcmp(definition.last_args, "from definition") != 0 ||
        direct.bad != 0U ||
        extension.bad != 0U ||
        definition.bad != 0U) {
        fprintf(
            stderr,
            "message command dispatch failed: direct=%u ext=%u def=%u "
            "direct_bad=%u ext_bad=%u def_bad=%u args=%s ext_args=%s def_args=%s\n",
            direct.calls,
            extension.calls,
            definition.calls,
            direct.bad,
            extension.bad,
            definition.bad,
            direct.last_args != NULL ? direct.last_args : "(null)",
            extension.last_args != NULL ? extension.last_args : "(null)",
            definition.last_args != NULL ? definition.last_args : "(null)"
        );
        dcc_app_destroy(app);
        return 1;
    }

    dcc_app_destroy(app);
    return 0;
}
