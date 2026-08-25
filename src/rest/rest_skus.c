#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

dcc_status_t dcc_rest_get_skus(dcc_client_t *client,
                               dcc_snowflake_t application_id,
                               const dcc_rest_call_options_t *options,
                               dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_SKUS_GET, DCC_REST_GET);
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U,
                         out_request, &resolved);
  if (status != DCC_OK || application_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[96];
  status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_SKUS_GET,
                                (unsigned long long)application_id);
  return status == DCC_OK ? dcc_task10_submit_empty(client, "dcc_rest_get_skus",
                                                    DCC_REST_GET, path,
                                                    &resolved, out_request)
                          : status;
}
