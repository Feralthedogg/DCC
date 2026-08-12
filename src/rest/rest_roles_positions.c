#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"
dcc_status_t dcc_rest_modify_guild_role_positions(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_role_position_t *positions, size_t position_count,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/roles";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_ROLES_EDIT_POSITION, DCC_REST_PATCH);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char *json = NULL;
  status = dcc_task8_build_role_positions(positions, position_count, &json);
  char path[80];
  if (status == DCC_OK)
    status = dcc_rest_format_path(path, sizeof(path),
                                  DCC_REST_ROUTE_DPP_ROLES_EDIT_POSITION,
                                  (unsigned long long)guild_id);
  return dcc_task8_submit_built(
      status, client, "dcc_rest_modify_guild_role_positions", DCC_REST_PATCH,
      path, json, &resolved, out_request);
}
