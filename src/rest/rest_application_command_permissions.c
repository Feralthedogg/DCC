#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_edit_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const dcc_guild_command_permissions_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_COMMAND_EDIT_PERMISSIONS, DCC_REST_PUT);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_COMMAND_EDIT_PERMISSIONS, (unsigned long long)application_id, (unsigned long long)guild_id, (unsigned long long)command_id);
  char *body = NULL;
  if (status == DCC_OK) status = dcc_task9_permissions_body(params, &body);
  if (status != DCC_OK) { free(body); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_edit_guild_command_permissions", DCC_REST_PUT,
      path, body, body != NULL ? strlen(body) : 0U, "application/json", options, DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_get_all_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_COMMANDS_GET_PERMISSIONS, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_COMMANDS_GET_PERMISSIONS, (unsigned long long)application_id, (unsigned long long)guild_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_all_guild_command_permissions", DCC_REST_GET,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_get_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_COMMAND_GET_PERMISSIONS, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_COMMAND_GET_PERMISSIONS, (unsigned long long)application_id, (unsigned long long)guild_id, (unsigned long long)command_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_guild_command_permissions", DCC_REST_GET,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}
