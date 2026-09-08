/* DCC_DOC_SNIPPET_BEGIN(component-sessions) */
#include <dcc/component_session.h>
#include <dcc/events/accessors.h>
#include <dcc/interaction_flow.h>
#include <string.h>

/* The caller owns out_session until listeners have been removed. Supply
 * unpredictable secret bytes from private configuration, not a literal. */
dcc_status_t dcc_example_session_create(
    const void *secret, size_t secret_len, uint64_t now_ms,
    dcc_snowflake_t user_id, dcc_snowflake_t channel_id,
    dcc_snowflake_t guild_id, dcc_component_session_t *out_session
) {
    dcc_component_session_options_t options;
    dcc_component_session_options_init(&options);
    dcc_status_t status = dcc_component_session_options_set_secret(
        &options, secret, secret_len);
    if (status == DCC_OK)
        status = dcc_component_session_options_set_ttl(&options, now_ms, 300000U);
    if (status == DCC_OK)
        status = dcc_component_session_options_lock_user(&options, user_id);
    if (status == DCC_OK)
        status = dcc_component_session_options_lock_channel(&options, channel_id);
    if (status == DCC_OK)
        status = dcc_component_session_options_lock_guild(&options, guild_id);
    if (status == DCC_OK)
        status = dcc_component_session_create(&options, out_session);
    return status;
}

dcc_status_t dcc_example_session_page(dcc_interaction_flow_t *flow,
                                      dcc_component_session_t *session) {
    dcc_component_v2_builder_t buttons[1];
    dcc_status_t status = dcc_component_session_button_v2(
        session, "next", "Next", DCC_BUTTON_PRIMARY, &buttons[0]);
    if (status != DCC_OK) return status;
    dcc_component_v2_builder_t rows[] = {
        dcc_component_v2_action_row(buttons, sizeof(buttons) / sizeof(buttons[0]))
    };
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    status = dcc_message_builder_set_components_v2(
        &message, rows, sizeof(rows) / sizeof(rows[0]));
    if (status == DCC_OK)
        status = dcc_flow_reply_ex(flow, &message, NULL, NULL);
    /* rows, buttons and session-backed strings remain alive until admission
     * returns. Builders borrow arrays; accepted REST work copies its inputs. */
    return status;
}

void dcc_example_session_action(dcc_client_t *client, const dcc_event_t *event,
    const dcc_component_session_result_t *result, void *user_data
) {
    (void)user_data;
    /* Do not perform the action or send an action-success reply on failure. */
    if (result->status != DCC_COMPONENT_SESSION_VERIFY_OK) return;
    if (strcmp(result->action, "next") == 0) {
        dcc_interaction_flow_t *flow = NULL;
        if (dcc_flow_create(client, dcc_event_interaction(event), &flow) != DCC_OK)
            return;
        dcc_message_builder_t reply = DCC_MESSAGE_BUILDER_INIT;
        dcc_status_t status = dcc_message_builder_set_content(&reply, "Next page");
        if (status == DCC_OK)
            status = dcc_message_builder_set_flags(&reply, DCC_MESSAGE_FLAG_EPHEMERAL);
        if (status == DCC_OK) (void)dcc_flow_reply_ex(flow, &reply, NULL, NULL);
        dcc_flow_destroy(flow);
    }
}

/* cb receives the verification result: reject every status other than OK
 * before acting. Keep state and session alive through listener teardown. */
dcc_status_t dcc_example_session_listen(
    dcc_client_t *client, const dcc_component_session_t *session,
    dcc_component_session_action_cb cb, void *state,
    dcc_component_session_listener_t *out_listener
) {
    dcc_component_session_listener_options_t options;
    dcc_component_session_listener_options_init(&options);
    return dcc_client_on_component_session(
        client, session, &options, cb, state, out_listener);
}

dcc_status_t dcc_example_session_stop(dcc_client_t *client,
    dcc_component_session_listener_t *listener, dcc_component_session_t *session
) {
    dcc_status_t status = dcc_client_off_component_session(client, listener);
    if (status == DCC_OK) dcc_component_session_deinit(session);
    return status;
}
/* DCC_DOC_SNIPPET_END(component-sessions) */
