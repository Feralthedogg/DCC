#include <dcc/dcc.h>

#include "internal/events/dcc_events_internal.h"

#include <stdio.h>
#include <string.h>

typedef struct route_state {
    int calls;
    dcc_component_session_verify_status_t status;
    char action[DCC_COMPONENT_SESSION_ACTION_MAX + 1U];
} route_state_t;

static int expect_status(
    dcc_component_session_verify_status_t actual,
    dcc_component_session_verify_status_t expected,
    const char *label
) {
    if (actual != expected) {
        fprintf(
            stderr,
            "%s: expected %s got %s\n",
            label,
            dcc_component_session_verify_status_string(expected),
            dcc_component_session_verify_status_string(actual)
        );
        return 0;
    }
    return 1;
}

static void on_component_session_action(
    dcc_client_t *client,
    const dcc_event_t *event,
    const dcc_component_session_result_t *result,
    void *user_data
) {
    (void)client;
    route_state_t *state = (route_state_t *)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (state == NULL || result == NULL || interaction == NULL || interaction->custom_id == NULL) {
        return;
    }
    state->calls++;
    state->status = result->status;
    memcpy(state->action, result->action, sizeof(state->action));
}

static int dispatch_component_event(
    dcc_client_t *client,
    const char *custom_id
) {
    char raw[512];
    int n = snprintf(
        raw,
        sizeof(raw),
        "{\"op\":0,\"s\":1,\"t\":\"INTERACTION_CREATE\",\"d\":{"
        "\"id\":\"1\",\"application_id\":\"1\",\"type\":3,"
        "\"guild_id\":\"7\",\"channel_id\":\"99\","
        "\"member\":{\"user\":{\"id\":\"42\"}},"
        "\"data\":{\"component_type\":2,\"custom_id\":\"%s\"}}}",
        custom_id
    );
    if (n <= 0 || (size_t)n >= sizeof(raw)) {
        return 0;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_BUTTON_CLICK;
    event.raw.type = DCC_EVENT_BUTTON_CLICK;
    event.raw.sequence = 1U;
    event.raw.name = "INTERACTION_CREATE";
    event.raw.json = raw;
    event.raw.json_len = (size_t)n;
    event.data.interaction.id = 1U;
    event.data.interaction.application_id = 1U;
    event.data.interaction.guild_id = 7U;
    event.data.interaction.channel_id = 99U;
    event.data.interaction.type = 3U;
    event.data.interaction.component_type = 2U;
    event.data.interaction.custom_id = custom_id;
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK;
}

int main(void) {
    static const unsigned char secret[] = "component-session-secret";
    dcc_component_session_options_t opts;
    dcc_component_session_options_init(&opts);

    dcc_component_session_t session;
    if (dcc_component_session_create(&opts, &session) != DCC_ERR_INVALID_ARG ||
        dcc_component_session_options_set_secret(&opts, secret, sizeof(secret) - 1U) != DCC_OK ||
        dcc_component_session_options_set_ids(&opts, "sess123", "nonce123") != DCC_OK ||
        dcc_component_session_options_set_ttl(&opts, 1000U, 5000U) != DCC_OK ||
        dcc_component_session_options_lock_user(&opts, 42U) != DCC_OK ||
        dcc_component_session_options_lock_channel(&opts, 99U) != DCC_OK ||
        dcc_component_session_options_lock_guild(&opts, 7U) != DCC_OK ||
        dcc_component_session_create(&opts, &session) != DCC_OK) {
        fprintf(stderr, "failed to create component session\n");
        return 1;
    }

    char custom_id[DCC_COMPONENT_SESSION_CUSTOM_ID_MAX + 1U];
    dcc_component_builder_t button;
    dcc_component_v2_builder_t button_v2;
    if (dcc_component_session_custom_id(&session, "bad:action", custom_id, sizeof(custom_id)) != DCC_ERR_INVALID_ARG ||
        dcc_component_session_button(&session, "next", "Next", DCC_BUTTON_PRIMARY, &button) != DCC_OK ||
        dcc_component_session_button_v2(&session, "prev", "Previous", DCC_BUTTON_SECONDARY, &button_v2) !=
            DCC_OK ||
        button.custom_id == NULL ||
        strncmp(button.custom_id, "dcc:v1:sess123:next:nonce123:", 29U) != 0 ||
        button_v2.custom_id == NULL ||
        strncmp(button_v2.custom_id, "dcc:v1:sess123:prev:nonce123:", 29U) != 0 ||
        strlen(button.custom_id) > DCC_COMPONENT_SESSION_CUSTOM_ID_MAX) {
        fprintf(stderr, "failed to build component session button\n");
        dcc_component_session_deinit(&session);
        return 1;
    }

    char *button_json = NULL;
    if (dcc_component_builder_build_json(&button, &button_json) != DCC_OK ||
        button_json == NULL ||
        strstr(button_json, "\"custom_id\":\"dcc:v1:sess123:next:nonce123:") == NULL) {
        fprintf(stderr, "failed to serialize component session button\n");
        dcc_component_builder_json_free(button_json);
        dcc_component_session_deinit(&session);
        return 1;
    }
    dcc_component_builder_json_free(button_json);

    char *button_v2_json = NULL;
    if (dcc_component_v2_builder_build_json(&button_v2, &button_v2_json) != DCC_OK ||
        button_v2_json == NULL ||
        strstr(button_v2_json, "\"type\":2") == NULL ||
        strstr(button_v2_json, "\"custom_id\":\"dcc:v1:sess123:prev:nonce123:") == NULL) {
        fprintf(stderr, "failed to serialize component session v2 button\n");
        dcc_component_v2_builder_json_free(button_v2_json);
        dcc_component_session_deinit(&session);
        return 1;
    }
    dcc_component_v2_builder_json_free(button_v2_json);

    dcc_component_session_check_t check = {
        .size = sizeof(check),
        .custom_id = button.custom_id,
        .now_ms = 2000U,
        .user_id = 42U,
        .channel_id = 99U,
        .guild_id = 7U,
    };
    dcc_component_session_result_t result;
    if (dcc_component_session_verify(&session, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_OK, "valid") ||
        strcmp(result.action, "next") != 0 ||
        result.expires_at_ms != 6000U) {
        dcc_component_session_deinit(&session);
        return 1;
    }

    check.user_id = 43U;
    if (dcc_component_session_verify(&session, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_WRONG_USER, "wrong user")) {
        dcc_component_session_deinit(&session);
        return 1;
    }
    check.user_id = 42U;
    check.channel_id = 100U;
    if (dcc_component_session_verify(&session, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_WRONG_CHANNEL, "wrong channel")) {
        dcc_component_session_deinit(&session);
        return 1;
    }
    check.channel_id = 99U;
    check.guild_id = 8U;
    if (dcc_component_session_verify(&session, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_WRONG_GUILD, "wrong guild")) {
        dcc_component_session_deinit(&session);
        return 1;
    }
    check.guild_id = 7U;
    check.now_ms = 6001U;
    if (dcc_component_session_verify(&session, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_EXPIRED, "expired")) {
        dcc_component_session_deinit(&session);
        return 1;
    }

    char tampered[DCC_COMPONENT_SESSION_CUSTOM_ID_MAX + 1U];
    memcpy(tampered, button.custom_id, strlen(button.custom_id) + 1U);
    tampered[strlen(tampered) - 1U] = tampered[strlen(tampered) - 1U] == '0' ? '1' : '0';
    check.custom_id = tampered;
    check.now_ms = 2000U;
    if (dcc_component_session_verify(&session, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_BAD_SIGNATURE, "tampered")) {
        dcc_component_session_deinit(&session);
        return 1;
    }

    dcc_component_session_store_t store;
    dcc_component_session_store_init(&store);
    check.custom_id = button.custom_id;
    if (dcc_component_session_store_add(&store, &session) != DCC_OK ||
        dcc_component_session_store_count(&store) != 1U ||
        dcc_component_session_store_verify(&store, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_OK, "store valid") ||
        dcc_component_session_store_sweep(&store, 6001U) != 1U ||
        dcc_component_session_store_count(&store) != 0U ||
        dcc_component_session_store_verify(&store, &check, &result) != DCC_OK ||
        !expect_status(result.status, DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND, "store swept")) {
        dcc_component_session_store_deinit(&store);
        dcc_component_session_deinit(&session);
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_component_session_listener_options_t listener_options;
    dcc_component_session_listener_options_init(&listener_options);
    listener_options.now_ms = 2000U;

    route_state_t route;
    memset(&route, 0, sizeof(route));
    dcc_component_session_listener_t listener;
    memset(&listener, 0, sizeof(listener));

    if (dcc_client_create(&client_options, &client) != DCC_OK ||
        dcc_client_on_component_session(
            client,
            &session,
            &listener_options,
            on_component_session_action,
            &route,
            &listener
        ) != DCC_OK ||
        listener.button_id == 0U ||
        !dispatch_component_event(client, button.custom_id) ||
        route.calls != 1 ||
        !expect_status(route.status, DCC_COMPONENT_SESSION_VERIFY_OK, "listener valid") ||
        strcmp(route.action, "next") != 0 ||
        dcc_client_off_component_session(client, &listener) != DCC_OK ||
        listener.state != NULL ||
        !dispatch_component_event(client, button.custom_id) ||
        route.calls != 1) {
        fprintf(stderr, "component session listener routing failed\n");
        dcc_client_off_component_session(client, &listener);
        dcc_client_destroy(client);
        dcc_component_session_store_deinit(&store);
        dcc_component_session_deinit(&session);
        return 1;
    }
    dcc_client_destroy(client);

    dcc_component_session_store_deinit(&store);
    dcc_component_session_deinit(&session);
    return 0;
}
