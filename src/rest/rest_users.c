#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"
#include <stdlib.h>
dcc_status_t dcc_rest_get_user(dcc_client_t *client, dcc_snowflake_t user_id,
                               const dcc_rest_call_options_t *options,
                               dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_USER_GET, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[64];
  status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_USER_GET,
                                (unsigned long long)user_id);
  return status == DCC_OK ? dcc_task10_submit_empty(client, "dcc_rest_get_user",
                                                    DCC_REST_GET, path,
                                                    &resolved, out_request)
                          : status;
}
dcc_status_t
dcc_rest_get_current_user_connections(dcc_client_t *client,
                                      const dcc_rest_call_options_t *options,
                                      dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_CURRENT_USER_CONNECTIONS_GET, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BEARER, 0U,
                         out_request, &resolved);
  return status == DCC_OK ? dcc_task10_submit_empty(
                                client, "dcc_rest_get_current_user_connections",
                                DCC_REST_GET,
                                DCC_REST_ROUTE_DPP_CURRENT_USER_CONNECTIONS_GET,
                                &resolved, out_request)
                          : status;
}
dcc_status_t dcc_rest_get_current_user_guilds(
    dcc_client_t *client, const dcc_rest_current_user_guilds_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/users/@me/guilds";
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_CURRENT_USER_GET_GUILDS, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER,
      DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved);
  if (status != DCC_OK)
    return status;
  dcc_rest_buffer_t q = {0};
  status = dcc_task8_query_current_user_guilds(query, &q);
  char *path = NULL;
  if (status == DCC_OK)
    status = dcc_endpoint_path_with_query(
        DCC_REST_ROUTE_DPP_CURRENT_USER_GET_GUILDS, &q, &path);
  dcc_rest_buffer_deinit(&q);
  if (status == DCC_OK)
    status = dcc_task8_submit(client, "dcc_rest_get_current_user_guilds",
                              DCC_REST_GET, path, NULL, &resolved, out_request);
  free(path);
  return status;
}
dcc_status_t dcc_rest_leave_guild(dcc_client_t *client,
                                  dcc_snowflake_t guild_id,
                                  const dcc_rest_call_options_t *options,
                                  dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/users/@me/guilds/%llu";
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DPP_CURRENT_USER_LEAVE_GUILD, DCC_REST_DELETE);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[96];
  status = dcc_rest_format_path(path, sizeof(path),
                                DCC_REST_ROUTE_DPP_CURRENT_USER_LEAVE_GUILD,
                                (unsigned long long)guild_id);
  return status == DCC_OK
             ? dcc_task8_submit(client, "dcc_rest_leave_guild", DCC_REST_DELETE,
                                path, NULL, &resolved, out_request)
             : status;
}
