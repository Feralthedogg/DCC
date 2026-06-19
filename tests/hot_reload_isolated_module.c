#include <dcc/dcc.h>
#include <dcc/hot_reload.h>
#include <dcc/rest.h>

#include <stdlib.h>

static void isolated_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)event;
    (void)user_data;
    const char *url = getenv("DCC_ISOLATED_TEST_URL");
    if (url == NULL || url[0] == '\0') {
        return;
    }
    (void)dcc_rest_request(client, "POST", url, "{\"worker\":true}", NULL, NULL);
}

static void isolated_slash(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    (void)dcc_rest_interaction_response_create_from_interaction(
        client,
        interaction,
        "{\"type\":4,\"data\":{\"content\":\"worker slash\"}}",
        NULL,
        NULL
    );
    (void)dcc_rest_request_async(
        client,
        "POST",
        "/channels/123/messages",
        "{\"content\":\"worker async\"}",
        NULL,
        NULL
    );
}

static const dcc_bot_module_handler_t isolated_handlers[] = {
    DCC_BOT_MODULE_ON(DCC_EVENT_READY, isolated_ready),
    DCC_BOT_MODULE_ON(DCC_EVENT_SLASH_COMMAND, isolated_slash),
};

DCC_BOT_MODULE(isolated_handlers)
