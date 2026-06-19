#include "package_consumer_checks.h"

int dcc_package_consumer_check_interaction_server_api(void) {
    dcc_package_interaction_route_add_fn interaction_route_add = dcc_interaction_server_add_route;
    dcc_package_interaction_health_route_fn interaction_health_route =
        dcc_interaction_server_add_health_route;
    dcc_package_interaction_route_clear_fn interaction_route_clear = dcc_interaction_server_clear_routes;
    dcc_package_interaction_stats_fn interaction_stats = dcc_interaction_server_stats;
    dcc_package_interaction_state_fn interaction_state = dcc_interaction_server_get_state;
    dcc_package_interaction_health_fn interaction_health = dcc_interaction_server_health_snapshot;
    dcc_package_interaction_health_string_fn interaction_health_string = dcc_interaction_server_health_string;
    dcc_package_interaction_health_json_fn interaction_health_json = dcc_interaction_server_health_snapshot_json;
    dcc_status_t (*interaction_drain)(dcc_interaction_server_t *, uint32_t,
        dcc_interaction_server_state_t *) = dcc_interaction_server_drain;
    dcc_status_t (*interaction_wait_ready)(dcc_interaction_server_t *, uint32_t,
        dcc_interaction_server_health_snapshot_t *) = dcc_interaction_server_wait_until_ready;
    dcc_status_t (*interaction_wait_drained)(dcc_interaction_server_t *, uint32_t,
        dcc_interaction_server_health_snapshot_t *) = dcc_interaction_server_wait_until_drained;
    dcc_package_interaction_request_reply_type_fn interaction_request_reply_type =
        dcc_interaction_request_reply_type;
    dcc_package_interaction_request_reply_builder_fn interaction_request_reply_message_builder =
        dcc_interaction_request_reply_message_builder;
    dcc_package_interaction_request_reply_message_fn interaction_request_reply_message =
        dcc_interaction_request_reply_message;
    dcc_package_interaction_request_reply_message_fn interaction_request_reply_update_message =
        dcc_interaction_request_reply_update_message;
    dcc_package_interaction_request_reply_text_fn interaction_request_reply_message_text =
        dcc_interaction_request_reply_message_text;
    dcc_package_interaction_request_reply_text_fn interaction_request_reply_ephemeral_message_text =
        dcc_interaction_request_reply_ephemeral_message_text;
    dcc_package_interaction_request_reply_text_fn interaction_request_reply_update_message_text =
        dcc_interaction_request_reply_update_message_text;
    dcc_package_interaction_request_reply_modal_fn interaction_request_reply_modal =
        dcc_interaction_request_reply_modal;
    dcc_package_interaction_request_reply_autocomplete_fn interaction_request_reply_autocomplete =
        dcc_interaction_request_reply_autocomplete;
    dcc_package_interaction_request_reply_named_fn interaction_request_reply_pong =
        dcc_interaction_request_reply_pong;
    dcc_package_interaction_request_reply_named_fn interaction_request_reply_defer =
        dcc_interaction_request_reply_deferred_message;
    dcc_package_interaction_request_reply_named_fn interaction_request_reply_defer_update =
        dcc_interaction_request_reply_deferred_update;
    dcc_package_interaction_request_reply_named_fn interaction_request_reply_premium =
        dcc_interaction_request_reply_premium_required;
    dcc_message_builder_t direct_message_builder;
    dcc_message_builder_init(&direct_message_builder);
    if (dcc_message_builder_set_content(&direct_message_builder, "direct") != DCC_OK) {
        return 0;
    }

    return interaction_route_add != NULL &&
           interaction_health_route != NULL &&
           interaction_health_route(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_route_clear != NULL &&
           interaction_stats != NULL &&
           interaction_state != NULL &&
           interaction_health != NULL &&
           interaction_health_string != NULL &&
           interaction_health_json != NULL &&
           interaction_health(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_health_string(DCC_INTERACTION_SERVER_HEALTH_READY) != NULL &&
           interaction_drain != NULL &&
           interaction_wait_ready != NULL &&
           interaction_wait_drained != NULL &&
           interaction_wait_ready(NULL, 1, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_wait_drained(NULL, 1, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_type != NULL &&
           interaction_request_reply_message_builder != NULL &&
           interaction_request_reply_message != NULL &&
           interaction_request_reply_update_message != NULL &&
           interaction_request_reply_message_text != NULL &&
           interaction_request_reply_ephemeral_message_text != NULL &&
           interaction_request_reply_update_message_text != NULL &&
           interaction_request_reply_modal != NULL &&
           interaction_request_reply_autocomplete != NULL &&
           interaction_request_reply_pong != NULL &&
           interaction_request_reply_defer != NULL &&
           interaction_request_reply_defer_update != NULL &&
           interaction_request_reply_premium != NULL &&
           interaction_request_reply_type(NULL, (dcc_interaction_response_type_t)0) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_message_builder(NULL, (dcc_interaction_response_type_t)0,
               &direct_message_builder) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_message(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_update_message(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_message_text(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_ephemeral_message_text(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_update_message_text(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_modal(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           interaction_request_reply_autocomplete(NULL, NULL) == DCC_ERR_INVALID_ARG;
}
