#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"
#include <stdlib.h>
dcc_status_t dcc_rest_get_guild(dcc_client_t *client, dcc_snowflake_t guild_id,
                                const dcc_rest_guild_query_t *query,
                                const dcc_rest_call_options_t *options,
                                dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_GET, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t q = {0};
  status = dcc_task8_query_guild(query, &q);
  char base[80];
  if (status == DCC_OK)
    status =
        dcc_rest_format_path(base, sizeof(base), DCC_REST_ROUTE_DPP_GUILD_GET,
                             (unsigned long long)guild_id);
  char *path = NULL;
  if (status == DCC_OK)
    status = dcc_endpoint_path_with_query(base, &q, &path);
  dcc_rest_buffer_deinit(&q);
  if (status == DCC_OK)
    status = dcc_task8_submit(client, "dcc_rest_get_guild", DCC_REST_GET, path,
                              NULL, &resolved, out_request);
  free(path);
  return status;
}
dcc_status_t dcc_rest_modify_guild(dcc_client_t *client,
                                   dcc_snowflake_t guild_id,
                                   const dcc_rest_guild_update_t *body,
                                   const dcc_rest_call_options_t *options,
                                   dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_EDIT, DCC_REST_PATCH);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char *json = NULL;
  status = dcc_task8_build_guild_update(body, &json);
  char path[80];
  if (status == DCC_OK)
    status =
        dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EDIT,
                             (unsigned long long)guild_id);
  return dcc_task8_submit_built(status, client, "dcc_rest_modify_guild",
                                DCC_REST_PATCH, path, json, &resolved,
                                out_request);
}
dcc_status_t dcc_rest_get_guild_preview(dcc_client_t *client,
                                        dcc_snowflake_t guild_id,
                                        const dcc_rest_call_options_t *options,
                                        dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/preview";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_GET_PREVIEW, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[96];
  status = dcc_rest_format_path(path, sizeof(path),
                                DCC_REST_ROUTE_DPP_GUILD_GET_PREVIEW,
                                (unsigned long long)guild_id);
  return dcc_task8_submit_built(status, client, "dcc_rest_get_guild_preview",
                                DCC_REST_GET, path, NULL, &resolved,
                                out_request);
}
