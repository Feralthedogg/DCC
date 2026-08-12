#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_message_search_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_create_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_scheduled_event_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_EVENT_CREATE, DCC_REST_POST);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EVENT_CREATE, (unsigned long long)guild_id);
  char *body = NULL;
  if (status == DCC_OK) status = dcc_task9_scheduled_body(params, 1, &body);
  if (status != DCC_OK) { free(body); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_create_guild_scheduled_event", DCC_REST_POST,
      path, body, body != NULL ? strlen(body) : 0U, "application/json", options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_delete_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_EVENT_DELETE, DCC_REST_DELETE);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EVENT_DELETE, (unsigned long long)guild_id, (unsigned long long)event_id);
  if (status != DCC_OK) { return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_delete_guild_scheduled_event", DCC_REST_DELETE,
      path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  return status;
}

dcc_status_t dcc_rest_get_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const dcc_rest_scheduled_event_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_EVENT_GET, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EVENT_GET, (unsigned long long)guild_id, (unsigned long long)event_id);
  dcc_rest_buffer_t query_buffer = {0};
  if (status == DCC_OK) status = dcc_task9_scheduled_query(query, &query_buffer);
  char *query_path = NULL;
  if (status == DCC_OK) status = dcc_endpoint_path_with_query(path, &query_buffer, &query_path);
  dcc_rest_buffer_deinit(&query_buffer);
  if (status != DCC_OK) { free(query_path); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_guild_scheduled_event", DCC_REST_GET,
      query_path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  free(query_path);
  return status;
}

dcc_status_t dcc_rest_get_guild_scheduled_events(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_scheduled_event_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DPP_GUILD_EVENTS_GET, DCC_REST_GET);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EVENTS_GET, (unsigned long long)guild_id);
  dcc_rest_buffer_t query_buffer = {0};
  if (status == DCC_OK) status = dcc_task9_scheduled_query(query, &query_buffer);
  char *query_path = NULL;
  if (status == DCC_OK) status = dcc_endpoint_path_with_query(path, &query_buffer, &query_path);
  dcc_rest_buffer_deinit(&query_buffer);
  if (status != DCC_OK) { free(query_path); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_get_guild_scheduled_events", DCC_REST_GET,
      query_path, NULL, 0U, NULL, options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      out_request);
  free(query_path);
  return status;
}

dcc_status_t dcc_rest_modify_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const dcc_scheduled_event_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
                        DCC_REST_ROUTE_DPP_GUILD_EVENT_EDIT, DCC_REST_PATCH);
  if (out_request != NULL) *out_request = NULL;
  char path[256];
  dcc_status_t status = dcc_rest_format_path(path, sizeof(path), DCC_REST_ROUTE_DPP_GUILD_EVENT_EDIT, (unsigned long long)guild_id, (unsigned long long)event_id);
  char *body = NULL;
  if (status == DCC_OK) status = dcc_task9_scheduled_body(params, 0, &body);
  if (status != DCC_OK) { free(body); return status; }
  status = dcc_task9_submit_owned(client, "dcc_rest_modify_guild_scheduled_event", DCC_REST_PATCH,
      path, body, body != NULL ? strlen(body) : 0U, "application/json", options, DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
      out_request);
  return status;
}
