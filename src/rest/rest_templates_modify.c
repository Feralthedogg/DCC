#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_modify_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    const dcc_template_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_TEMPLATE_MODIFY, DCC_REST_PATCH);
  if (out_request != NULL) *out_request = NULL;
  char *path = NULL;
  dcc_status_t status = dcc_rest_alloc_guild_template_path(&path, guild_id, code);
  char *body = NULL;
  if (status == DCC_OK) status = dcc_task9_template_body(params, 0, &body);
  if (status != DCC_OK) { free(body); free(path); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_modify_guild_template", DCC_REST_PATCH,
      path, body, body != NULL ? strlen(body) : 0U, "application/json", options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  free(path);
  return status;
}
