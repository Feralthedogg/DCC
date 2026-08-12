#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_delete_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_AUTOMOD_RULE_DELETE, DCC_REST_DELETE);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_AUTOMOD_RULE_DELETE, (unsigned long long)guild_id, (unsigned long long)rule_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_delete_auto_moderation_rule", DCC_REST_DELETE,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_get_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_AUTOMOD_RULE_GET, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_AUTOMOD_RULE_GET, (unsigned long long)guild_id, (unsigned long long)rule_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_auto_moderation_rule", DCC_REST_GET,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_get_auto_moderation_rules(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_AUTOMOD_RULES_GET, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_AUTOMOD_RULES_GET, (unsigned long long)guild_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_auto_moderation_rules", DCC_REST_GET,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}
