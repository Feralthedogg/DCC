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
    if (interaction != NULL) {
        dcc_message_builder_t message;
        dcc_message_builder_init(&message);
        dcc_rest_interaction_response_t response =
            DCC_REST_INTERACTION_RESPONSE_INIT;
        if (dcc_message_builder_set_content(&message, "worker slash") == DCC_OK &&
            dcc_rest_interaction_response_set_message(
                &response, &message) == DCC_OK) {
            (void)dcc_rest_interaction_response_create(
                client,
                interaction->id,
                interaction->token,
                &response,
                NULL,
                NULL
            );
        }
    }
    dcc_message_builder_t async_message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&async_message, "worker async") == DCC_OK) {
        dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
        dcc_rest_message_payload_init(&payload, &async_message);
        (void)dcc_rest_create_message(client, 123U, &payload, NULL, NULL);
    }
}

static const dcc_bot_module_handler_t isolated_handlers[] = {
    DCC_BOT_MODULE_ON(DCC_EVENT_READY, isolated_ready),
    DCC_BOT_MODULE_ON(DCC_EVENT_SLASH_COMMAND, isolated_slash),
};

DCC_BOT_MODULE(isolated_handlers)
