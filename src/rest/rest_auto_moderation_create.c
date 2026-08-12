#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_create_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_AUTOMOD_RULE_CREATE, DCC_REST_POST);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_AUTOMOD_RULE_CREATE, (unsigned long long)guild_id);
  char *body = NULL;
  if (status == DCC_OK) status = dcc_rest_build_auto_moderation_rule_body(params, 0, &body);
  if (status != DCC_OK) { free(body); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_create_auto_moderation_rule", DCC_REST_POST,
      path, body, body != NULL ? strlen(body) : 0U, "application/json", options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}
