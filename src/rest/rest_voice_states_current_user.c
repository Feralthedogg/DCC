#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

dcc_status_t dcc_rest_get_current_user_voice_state(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_CURRENT_USER_GET_VOICE_STATE, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[88];
  status = dcc_rest_format_path(path, sizeof(path),
                                DCC_REST_ROUTE_DPP_CURRENT_USER_GET_VOICE_STATE,
                                (unsigned long long)guild_id);
  return status == DCC_OK ? dcc_task10_submit_empty(
                                client, "dcc_rest_get_current_user_voice_state",
                                DCC_REST_GET, path, &resolved, out_request)
                          : status;
}

dcc_status_t dcc_rest_modify_current_user_voice_state(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_current_user_voice_state_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_CURRENT_USER_SET_VOICE_STATE, DCC_REST_PATCH);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[88];
  char *body = NULL;
  status = dcc_rest_format_path(path, sizeof(path),
                                DCC_REST_ROUTE_DPP_CURRENT_USER_SET_VOICE_STATE,
                                (unsigned long long)guild_id);
  if (status == DCC_OK)
    status = dcc_task10_build_current_voice(params, &body);
  return status == DCC_OK
             ? dcc_task10_submit(
                   client, "dcc_rest_modify_current_user_voice_state",
                   DCC_REST_PATCH, path, body, &resolved, out_request)
             : status;
}
