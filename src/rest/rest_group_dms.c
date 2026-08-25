#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

dcc_status_t dcc_rest_add_group_dm_recipient(
    dcc_client_t *client, dcc_snowflake_t channel_id, dcc_snowflake_t user_id,
    const dcc_group_dm_recipient_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BEARER,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GDM_ADD, DCC_REST_PUT);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BEARER,
      DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY, out_request, &resolved);
  if (status != DCC_OK || channel_id == 0U || user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[112];
  char *body = NULL;
  status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GDM_ADD,
                                (unsigned long long)channel_id,
                                (unsigned long long)user_id);
  if (status == DCC_OK)
    status = dcc_task10_build_group_dm(params, &body);
  return status == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_add_group_dm_recipient",
                                 DCC_REST_PUT, path, body, &resolved,
                                 out_request)
             : status;
}

dcc_status_t dcc_rest_remove_group_dm_recipient(
    dcc_client_t *client, dcc_snowflake_t channel_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BEARER,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GDM_REMOVE, DCC_REST_DELETE);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BEARER, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || channel_id == 0U || user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[112];
  status = dcc_rest_format_path(
      path, sizeof(path), DCC_REST_ROUTE_DPP_GDM_REMOVE,
      (unsigned long long)channel_id, (unsigned long long)user_id);
  return status == DCC_OK ? dcc_task10_submit_empty(
                                client, "dcc_rest_remove_group_dm_recipient",
                                DCC_REST_DELETE, path, &resolved, out_request)
                          : status;
}
