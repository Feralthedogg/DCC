#ifndef DCC_APP_INTERACTION_QUEUE_INTERNAL_H
#define DCC_APP_INTERACTION_QUEUE_INTERNAL_H

#include <dcc/interaction_flow.h>

typedef struct dcc_app_interaction_queue dcc_app_interaction_queue_t;

dcc_app_interaction_queue_t *
dcc_app_interaction_queue_create(dcc_client_t *client);
void dcc_app_interaction_queue_attach_flow(dcc_app_interaction_queue_t *queue,
                                           dcc_interaction_flow_t *flow);
void dcc_app_interaction_queue_release_owner(
    dcc_app_interaction_queue_t *queue);
dcc_interaction_flow_state_t dcc_app_interaction_queue_state(
    dcc_app_interaction_queue_t *queue,
    dcc_interaction_flow_state_t fallback);
dcc_status_t
dcc_app_interaction_queue_reply(dcc_interaction_flow_t *flow,
                                const dcc_message_builder_t *message,
                                dcc_rest_result_fn callback, void *user_data);
dcc_status_t dcc_app_interaction_queue_edit_original(
    dcc_interaction_flow_t *flow, const dcc_message_builder_t *message,
    dcc_rest_result_fn callback, void *user_data);
dcc_status_t dcc_app_interaction_queue_followup(
    dcc_interaction_flow_t *flow, const dcc_message_builder_t *message,
    dcc_rest_result_fn callback, void *user_data);
dcc_status_t dcc_app_interaction_queue_defer(
    dcc_interaction_flow_t *flow, uint8_t ephemeral, uint8_t update,
    dcc_rest_result_fn callback, void *user_data);

#endif
