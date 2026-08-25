#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t
dcc_rest_get_entitlements(dcc_client_t *client, dcc_snowflake_t application_id,
                          const dcc_rest_entitlement_query_t *query,
                          const dcc_rest_call_options_t *options,
                          dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_ENTITLEMENTS_GET, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || application_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char base[96];
  char *query_text = NULL;
  char *path = NULL;
  status = dcc_rest_format_path(base, sizeof(base),
                                DCC_REST_ROUTE_DPP_ENTITLEMENTS_GET,
                                (unsigned long long)application_id);
  if (status == DCC_OK)
    status = dcc_task10_entitlement_query(query, &query_text);
  if (status == DCC_OK) {
    size_t a = strlen(base), b = query_text != NULL ? strlen(query_text) : 0U;
    path = malloc(a + b + 1U);
    if (path == NULL)
      status = DCC_ERR_NOMEM;
    else {
      memcpy(path, base, a);
      if (b)
        memcpy(path + a, query_text, b);
      path[a + b] = '\0';
    }
  }
  free(query_text);
  if (status == DCC_OK)
    status =
        dcc_task10_submit_empty(client, "dcc_rest_get_entitlements",
                                DCC_REST_GET, path, &resolved, out_request);
  free(path);
  return status;
}
