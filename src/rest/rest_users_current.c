#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

static dcc_status_t current_user_get(dcc_client_t *client,
                                     const char *operation, const char *route,
                                     dcc_endpoint_auth_policy_t auth,
                                     const dcc_rest_call_options_t *options,
                                     dcc_rest_request_t **out_request) {
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, auth, 0U, out_request, &resolved);
  return status == DCC_OK
             ? dcc_task10_submit_empty(client, operation, DCC_REST_GET, route,
                                       &resolved, out_request)
             : status;
}

dcc_status_t
dcc_rest_get_current_application(dcc_client_t *client,
                                 const dcc_rest_call_options_t *options,
                                 dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_CURRENT_APPLICATION_GET, DCC_REST_GET);
  return current_user_get(client, "dcc_rest_get_current_application",
                          DCC_REST_ROUTE_DPP_CURRENT_APPLICATION_GET,
                          DCC_ENDPOINT_AUTH_POLICY_BOT, options, out_request);
}

dcc_status_t dcc_rest_get_current_user(dcc_client_t *client,
                                       const dcc_rest_call_options_t *options,
                                       dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_CURRENT_USER_GET, DCC_REST_GET);
  return current_user_get(
      client, "dcc_rest_get_current_user", DCC_REST_ROUTE_DPP_CURRENT_USER_GET,
      DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, options, out_request);
}

dcc_status_t dcc_rest_modify_current_user(
    dcc_client_t *client, const dcc_current_user_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_CURRENT_USER_EDIT, DCC_REST_PATCH);
  dcc_rest_call_options_t resolved;
  char *body = NULL;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status == DCC_OK)
    status = dcc_task10_build_current_user(params, &body);
  return status == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_modify_current_user",
                                 DCC_REST_PATCH,
                                 DCC_REST_ROUTE_DPP_CURRENT_USER_EDIT, body,
                                 &resolved, out_request)
             : status;
}
