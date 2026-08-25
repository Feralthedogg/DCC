#include "internal/app/dcc_app_interaction_queue_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

dcc_status_t dcc_flow_defer(dcc_interaction_flow_t *flow, dcc_rest_result_fn cb,
                            void *user_data) {
  if (flow != NULL && flow->queue != NULL)
    return dcc_app_interaction_queue_defer(flow, 0U, 0U, cb, user_data);
  dcc_status_t status = dcc_flow_require_context(flow);
  if (status != DCC_OK) {
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_FAILED, status);
    return status;
  }
  status = dcc_flow_claim_initial(flow);
  if (status != DCC_OK) {
    return status;
  }
  dcc_rest_interaction_response_t response = DCC_REST_INTERACTION_RESPONSE_INIT;
  status = dcc_rest_interaction_response_set_deferred_message(&response, NULL);
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = cb;
  options.user_data = user_data;
  if (status == DCC_OK)
    status = dcc_rest_interaction_response_create(
        flow->client, flow->interaction->id, flow->interaction->token,
        &response, &options, NULL);
  return dcc_flow_mark_initial(flow, DCC_INTERACTION_FLOW_DEFERRED, status);
}

dcc_status_t dcc_flow_defer_ephemeral(dcc_interaction_flow_t *flow,
                                      dcc_rest_result_fn cb, void *user_data) {
  if (flow != NULL && flow->queue != NULL)
    return dcc_app_interaction_queue_defer(flow, 1U, 0U, cb, user_data);
  dcc_status_t status = dcc_flow_require_context(flow);
  if (status != DCC_OK) {
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_FAILED, status);
    return status;
  }
  status = dcc_flow_claim_initial(flow);
  if (status != DCC_OK) {
    return status;
  }
  dcc_message_builder_t message;
  if (status == DCC_OK) {
    dcc_message_builder_init(&message);
    status = dcc_message_builder_set_flags(&message,
                                           DCC_INTERACTION_FLOW_FLAG_EPHEMERAL);
  }
  if (status == DCC_OK) {
    dcc_rest_interaction_response_t response =
        DCC_REST_INTERACTION_RESPONSE_INIT;
    status =
        dcc_rest_interaction_response_set_deferred_message(&response, &message);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = cb;
    options.user_data = user_data;
    if (status == DCC_OK)
      status = dcc_rest_interaction_response_create(
          flow->client, flow->interaction->id, flow->interaction->token,
          &response, &options, NULL);
  }
  return dcc_flow_mark_initial(flow, DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL,
                               status);
}

dcc_status_t dcc_flow_defer_update(dcc_interaction_flow_t *flow,
                                   dcc_rest_result_fn cb, void *user_data) {
  if (flow != NULL && flow->queue != NULL)
    return dcc_app_interaction_queue_defer(flow, 0U, 1U, cb, user_data);
  dcc_status_t status = dcc_flow_require_context(flow);
  if (status != DCC_OK) {
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_FAILED, status);
    return status;
  }
  status = dcc_flow_claim_initial(flow);
  if (status != DCC_OK) {
    return status;
  }
  dcc_rest_interaction_response_t response = DCC_REST_INTERACTION_RESPONSE_INIT;
  status = dcc_rest_interaction_response_set_deferred_update(&response);
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = cb;
  options.user_data = user_data;
  if (status == DCC_OK)
    status = dcc_rest_interaction_response_create(
        flow->client, flow->interaction->id, flow->interaction->token,
        &response, &options, NULL);
  return dcc_flow_mark_initial(flow, DCC_INTERACTION_FLOW_DEFERRED_UPDATE,
                               status);
}
