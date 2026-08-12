#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"

dcc_status_t dcc_rest_add_guild_member(dcc_client_t *client,
                                       dcc_snowflake_t guild_id,
                                       dcc_snowflake_t user_id,
                                       const dcc_rest_guild_member_add_t *body,
                                       const dcc_rest_call_options_t *options,
                                       dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/%llu";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_ADD_MEMBER, DCC_REST_PUT);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U || user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  resolved.flags |= DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY;
  char *json = NULL;
  status = dcc_task8_build_member_add(body, &json);
  char path[112];
  if (status == DCC_OK)
    status = dcc_rest_format_path(
        path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_ADD_MEMBER,
        (unsigned long long)guild_id, (unsigned long long)user_id);
  return dcc_task8_submit_built(status, client, "dcc_rest_add_guild_member",
                                DCC_REST_PUT, path, json, &resolved,
                                out_request);
}
dcc_status_t dcc_rest_modify_guild_member(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id,
    const dcc_rest_guild_member_update_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/%llu";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_EDIT_MEMBER, DCC_REST_PATCH);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U || user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char *json = NULL;
  status = dcc_task8_build_member_update(body, &json);
  char path[112];
  if (status == DCC_OK)
    status = dcc_rest_format_path(
        path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EDIT_MEMBER,
        (unsigned long long)guild_id, (unsigned long long)user_id);
  return dcc_task8_submit_built(status, client, "dcc_rest_modify_guild_member",
                                DCC_REST_PATCH, path, json, &resolved,
                                out_request);
}
dcc_status_t dcc_rest_remove_guild_member(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/%llu";
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      DCC_REST_ROUTE_DPP_GUILD_MEMBER_DELETE, DCC_REST_DELETE);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U || user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[112];
  status = dcc_rest_format_path(
      path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_MEMBER_DELETE,
      (unsigned long long)guild_id, (unsigned long long)user_id);
  return dcc_task8_submit_built(status, client, "dcc_rest_remove_guild_member",
                                DCC_REST_DELETE, path, NULL, &resolved,
                                out_request);
}
