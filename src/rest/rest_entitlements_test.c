#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

static dcc_status_t entitlement_id_request(
    dcc_client_t *client, const char *operation, const char *route,
    dcc_rest_method_t method, dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request) {
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || application_id == 0U || entitlement_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[136];
  status = dcc_rest_format_path(path, sizeof(path), route,
                                (unsigned long long)application_id,
                                (unsigned long long)entitlement_id);
  return status == DCC_OK
             ? dcc_task10_submit_empty(client, operation, method, path,
                                       &resolved, out_request)
             : status;
}

dcc_status_t dcc_rest_create_test_entitlement(
    dcc_client_t *client, dcc_snowflake_t application_id,
    const dcc_test_entitlement_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_ENTITLEMENT_TEST_CREATE, DCC_REST_POST);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || application_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[96];
  char *body = NULL;
  status = dcc_rest_format_path(path, sizeof(path),
                                DCC_REST_ROUTE_DPP_ENTITLEMENT_TEST_CREATE,
                                (unsigned long long)application_id);
  if (status == DCC_OK)
    status = dcc_task10_build_test_entitlement(params, &body);
  return status == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_create_test_entitlement",
                                 DCC_REST_POST, path, body, &resolved,
                                 out_request)
             : status;
}

dcc_status_t dcc_rest_delete_test_entitlement(
    dcc_client_t *client, dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_ENTITLEMENT_TEST_DELETE, DCC_REST_DELETE);
  return entitlement_id_request(client, "dcc_rest_delete_test_entitlement",
                                DCC_REST_ROUTE_DPP_ENTITLEMENT_TEST_DELETE,
                                DCC_REST_DELETE, application_id, entitlement_id,
                                options, out_request);
}

dcc_status_t dcc_rest_consume_entitlement(
    dcc_client_t *client, dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_ENTITLEMENT_CONSUME, DCC_REST_POST);
  return entitlement_id_request(client, "dcc_rest_consume_entitlement",
                                DCC_REST_ROUTE_DPP_ENTITLEMENT_CONSUME,
                                DCC_REST_POST, application_id, entitlement_id,
                                options, out_request);
}
