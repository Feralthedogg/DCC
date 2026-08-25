#include "internal/app/dcc_app_interaction_queue_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

dcc_status_t dcc_flow_show_modal(dcc_interaction_flow_t *flow,
                                 const dcc_modal_builder_t *modal,
                                 dcc_rest_result_fn cb, void *user_data) {
  if (flow != NULL && flow->queue != NULL) {
    if (modal == NULL)
      return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_t response =
        DCC_REST_INTERACTION_RESPONSE_INIT;
    dcc_status_t status =
        dcc_rest_interaction_response_set_modal(&response, modal);
    return status == DCC_OK
               ? dcc_app_interaction_queue_initial_response(
                     flow, &response, DCC_INTERACTION_FLOW_MODAL, cb,
                     user_data)
               : status;
  }
  dcc_status_t status = dcc_flow_require_context(flow);
  if (status != DCC_OK) {
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_FAILED, status);
    return status;
  }
  status = dcc_flow_claim_initial(flow);
  if (status != DCC_OK) {
    return status;
  }
  if (modal == NULL) {
    status = DCC_ERR_INVALID_ARG;
  }
  if (status == DCC_OK) {
    dcc_rest_interaction_response_t response =
        DCC_REST_INTERACTION_RESPONSE_INIT;
    status = dcc_rest_interaction_response_set_modal(&response, modal);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = cb;
    options.user_data = user_data;
    if (status == DCC_OK)
      status = dcc_rest_interaction_response_create(
          flow->client, flow->interaction->id, flow->interaction->token,
          &response, &options, NULL);
  }
  return dcc_flow_mark_initial(flow, DCC_INTERACTION_FLOW_MODAL, status);
}
