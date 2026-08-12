#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"

static dcc_status_t member_role(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    dcc_snowflake_t guild_id, dcc_snowflake_t user_id, dcc_snowflake_t role_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U || user_id == 0U || role_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[144];
  status = dcc_rest_format_path(
      path, sizeof(path),
      method == DCC_REST_PUT ? DCC_REST_ROUTE_DPP_GUILD_MEMBER_ADD_ROLE
                             : DCC_REST_ROUTE_DPP_GUILD_MEMBER_DELETE_ROLE,
      (unsigned long long)guild_id, (unsigned long long)user_id,
      (unsigned long long)role_id);
  return status == DCC_OK ? dcc_task8_submit(client, operation, method, path,
                                             NULL, &resolved, out_request)
                          : status;
}
dcc_status_t
dcc_rest_add_guild_member_role(dcc_client_t *client, dcc_snowflake_t guild_id,
                               dcc_snowflake_t user_id, dcc_snowflake_t role_id,
                               const dcc_rest_call_options_t *options,
                               dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/%llu/roles/%llu";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_MEMBER_ADD_ROLE, DCC_REST_PUT);
  return member_role(client, "dcc_rest_add_guild_member_role", DCC_REST_PUT,
                     guild_id, user_id, role_id, options, out_request);
}
dcc_status_t dcc_rest_remove_guild_member_role(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id,
    dcc_snowflake_t role_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/%llu/roles/%llu";
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      DCC_REST_ROUTE_DPP_GUILD_MEMBER_DELETE_ROLE, DCC_REST_DELETE);
  return member_role(client, "dcc_rest_remove_guild_member_role",
                     DCC_REST_DELETE, guild_id, user_id, role_id, options,
                     out_request);
}
