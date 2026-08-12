#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"
dcc_status_t dcc_rest_modify_current_guild_member(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_current_guild_member_update_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/@me";
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      DCC_REST_ROUTE_DPP_GUILD_CURRENT_MEMBER_EDIT, DCC_REST_PATCH);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char *json = NULL;
  status = dcc_task8_build_current_member_update(body, &json);
  char path[96];
  if (status == DCC_OK)
    status = dcc_rest_format_path(path, sizeof(path),
                                  DCC_REST_ROUTE_DPP_GUILD_CURRENT_MEMBER_EDIT,
                                  (unsigned long long)guild_id);
  return dcc_task8_submit_built(
      status, client, "dcc_rest_modify_current_guild_member", DCC_REST_PATCH,
      path, json, &resolved, out_request);
}
dcc_status_t dcc_rest_set_current_guild_member_nickname(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_current_guild_member_nickname_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/@me/nick";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_SET_NICKNAME, DCC_REST_PATCH);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char *json = NULL;
  status = dcc_task8_build_current_member_nickname(body, &json);
  char path[96];
  if (status == DCC_OK)
    status = dcc_rest_format_path(path, sizeof(path),
                                  DCC_REST_ROUTE_DPP_GUILD_SET_NICKNAME,
                                  (unsigned long long)guild_id);
  return dcc_task8_submit_built(
      status, client, "dcc_rest_set_current_guild_member_nickname",
      DCC_REST_PATCH, path, json, &resolved, out_request);
}
