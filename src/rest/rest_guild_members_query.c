#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"

#include <stdlib.h>

static dcc_status_t
members_query_submit(dcc_client_t *client, const char *operation,
                     const char *route, dcc_snowflake_t guild_id,
                     const dcc_rest_call_options_t *options,
                     dcc_rest_request_t **out_request,
                     dcc_status_t (*build)(const void *, dcc_rest_buffer_t *),
                     const void *query) {
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t query_buffer = {0};
  status = build(query, &query_buffer);
  char base[96];
  if (status == DCC_OK)
    status = dcc_rest_format_path(base, sizeof(base), route,
                                  (unsigned long long)guild_id);
  char *path = NULL;
  if (status == DCC_OK)
    status = dcc_endpoint_path_with_query(base, &query_buffer, &path);
  dcc_rest_buffer_deinit(&query_buffer);
  if (status == DCC_OK)
    status = dcc_task8_submit(client, operation, DCC_REST_GET, path, NULL,
                              &resolved, out_request);
  free(path);
  return status;
}
static dcc_status_t members_q(const void *q, dcc_rest_buffer_t *out) {
  return dcc_task8_query_members((const dcc_rest_guild_members_query_t *)q,
                                 out);
}
static dcc_status_t search_q(const void *q, dcc_rest_buffer_t *out) {
  return dcc_task8_query_member_search(
      (const dcc_rest_guild_member_search_query_t *)q, out);
}
dcc_status_t dcc_rest_get_guild_member(dcc_client_t *client,
                                       dcc_snowflake_t guild_id,
                                       dcc_snowflake_t user_id,
                                       const dcc_rest_call_options_t *options,
                                       dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/%llu";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_GET_MEMBER, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_task8_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT,
      DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request, &resolved);
  if (status != DCC_OK || guild_id == 0U || user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[112];
  status = dcc_rest_format_path(
      path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_GET_MEMBER,
      (unsigned long long)guild_id, (unsigned long long)user_id);
  return status == DCC_OK
             ? dcc_task8_submit(client, "dcc_rest_get_guild_member",
                                DCC_REST_GET, path, NULL, &resolved,
                                out_request)
             : status;
}
dcc_status_t
dcc_rest_list_guild_members(dcc_client_t *client, dcc_snowflake_t guild_id,
                            const dcc_rest_guild_members_query_t *query,
                            const dcc_rest_call_options_t *options,
                            dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_GET_MEMBERS, DCC_REST_GET);
  return members_query_submit(client, "dcc_rest_list_guild_members",
                              DCC_REST_ROUTE_DPP_GUILD_GET_MEMBERS, guild_id,
                              options, out_request, members_q, query);
}
dcc_status_t
dcc_rest_search_guild_members(dcc_client_t *client, dcc_snowflake_t guild_id,
                              const dcc_rest_guild_member_search_query_t *query,
                              const dcc_rest_call_options_t *options,
                              dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  (void)"/guilds/%llu/members/search";
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_SEARCH_MEMBERS, DCC_REST_GET);
  return members_query_submit(client, "dcc_rest_search_guild_members",
                              DCC_REST_ROUTE_DPP_GUILD_SEARCH_MEMBERS, guild_id,
                              options, out_request, search_q, query);
}
