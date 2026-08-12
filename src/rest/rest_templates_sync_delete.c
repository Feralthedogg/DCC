#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_delete_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_TEMPLATE_DELETE, DCC_REST_DELETE);
  if (out_request != NULL) *out_request = NULL;
  char *path = NULL;
  dcc_status_t status = dcc_rest_alloc_guild_template_path(&path, guild_id, code);
  if (status != DCC_OK) { free(path); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_delete_guild_template", DCC_REST_DELETE,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  free(path);
  return status;
}

dcc_status_t dcc_rest_sync_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_TEMPLATE_SYNC, DCC_REST_PUT);
  if (out_request != NULL) *out_request = NULL;
  char *path = NULL;
  dcc_status_t status = dcc_rest_alloc_guild_template_path(&path, guild_id, code);
  if (status != DCC_OK) { free(path); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_sync_guild_template", DCC_REST_PUT,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  free(path);
  return status;
}
