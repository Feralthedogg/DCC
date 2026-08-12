#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_create_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_guild_sticker_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_STICKER_CREATE, DCC_REST_POST);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_STICKER_CREATE, (unsigned long long)guild_id);
  char *body = NULL;
  size_t body_len = 0U;
  const char *content_type = NULL;
  if (status == DCC_OK) status = dcc_task9_sticker_multipart(params, &body, &body_len, &content_type);
  if (status != DCC_OK) { free(body); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_create_guild_sticker", DCC_REST_POST,
      path, body, body_len, content_type, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}
