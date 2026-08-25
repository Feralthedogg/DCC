#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

dcc_status_t dcc_rest_get_gateway_bot(dcc_client_t *client,
                                      const dcc_rest_call_options_t *options,
                                      dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GET_GATEWAY_BOT, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  return status == DCC_OK
             ? dcc_task10_submit_empty(
                   client, "dcc_rest_get_gateway_bot", DCC_REST_GET,
                   DCC_REST_ROUTE_DPP_GET_GATEWAY_BOT, &resolved, out_request)
             : status;
}
