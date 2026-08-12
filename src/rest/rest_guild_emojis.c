#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_create_guild_emoji(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_EMOJI_CREATE, DCC_REST_POST);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EMOJI_CREATE, (unsigned long long)guild_id);
  char *body = NULL;
  if (status == DCC_OK) status = dcc_task9_emoji_body(params, DCC_EMOJI_PARAMS_PRESENT_NAME | DCC_EMOJI_PARAMS_PRESENT_IMAGE | DCC_EMOJI_PARAMS_PRESENT_ROLES, DCC_EMOJI_PARAMS_PRESENT_NAME | DCC_EMOJI_PARAMS_PRESENT_IMAGE, &body);
  if (status != DCC_OK) { free(body); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_create_guild_emoji", DCC_REST_POST,
      path, body, body != NULL ? strlen(body) : 0U, "application/json", options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_delete_guild_emoji(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_EMOJI_DELETE, DCC_REST_DELETE);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EMOJI_DELETE, (unsigned long long)guild_id, (unsigned long long)emoji_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_delete_guild_emoji", DCC_REST_DELETE,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_get_guild_emoji(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_EMOJI_GET, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EMOJI_GET, (unsigned long long)guild_id, (unsigned long long)emoji_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_guild_emoji", DCC_REST_GET,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_get_guild_emojis(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_EMOJIS_GET, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EMOJIS_GET, (unsigned long long)guild_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_guild_emojis", DCC_REST_GET,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_modify_guild_emoji(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    const dcc_emoji_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_EMOJI_EDIT, DCC_REST_PATCH);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EMOJI_EDIT, (unsigned long long)guild_id, (unsigned long long)emoji_id);
  char *body = NULL;
  if (status == DCC_OK) status = dcc_task9_emoji_body(params, DCC_EMOJI_PARAMS_PRESENT_NAME | DCC_EMOJI_PARAMS_PRESENT_ROLES, 0U, &body);
  if (status != DCC_OK) { free(body); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_modify_guild_emoji", DCC_REST_PATCH,
      path, body, body != NULL ? strlen(body) : 0U, "application/json", options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}
