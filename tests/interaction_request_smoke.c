#include "support/http_smoke_server.h"
#include <dcc/app.h>
#include <dcc/dcc.h>
#include <dcc/rest/core/async.h>
#include <dcc/rest/runtime.h>
#include "internal/rest/dcc_rest_intercept_internal.h"
#include "internal/app/dcc_app_internal.h"
#if defined(_WIN32)
int main(void) { return 0; }
#else
#include <pthread.h>
#include <poll.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)
typedef struct seen {
  dcc_rest_request_t *request;
  atomic_uint calls;
  dcc_status_t status;
  dcc_status_t self_wait;
  unsigned destroy;
  unsigned skip_handle;
  uint64_t flags;
  dcc_interaction_flow_t *destroy_flow;
} seen_t;
static void complete(dcc_client_t *client, const dcc_rest_result_t *result, void *data) {
  (void)client;
  seen_t *seen = data;
  seen->status = dcc_rest_result_status(result);
  seen->flags = result->flags;
  if (seen->destroy_flow != NULL) dcc_flow_destroy(seen->destroy_flow);
  if (!seen->skip_handle && seen->request != NULL) {
    const dcc_rest_result_t *own = NULL;
    seen->self_wait = dcc_rest_request_wait(seen->request, 10U, &own);
    if (seen->destroy) dcc_rest_request_destroy(seen->request);
  }
  atomic_fetch_add_explicit(&seen->calls, 1U, memory_order_release);
}
static void *run(void *data) { (void)dcc_client_wait(data); return NULL; }

typedef struct errors { atomic_uint calls; atomic_uint release; } errors_t;
static void observe(dcc_client_t *client, const dcc_error_t *error, void *data) {
  (void)client; (void)error;
  errors_t *errors = data;
  atomic_fetch_add_explicit(&errors->calls, 1U, memory_order_release);
  while (!atomic_load_explicit(&errors->release, memory_order_acquire)) usleep(100U);
}

/* Catches missing output clearing, dropped options and callbacks on rejection. */
static int invalid(dcc_interaction_flow_t *flow, dcc_ctx_t *ctx) {
  seen_t seen = {0};
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = complete; options.user_data = &seen;
  dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
  dcc_rest_request_t *out = (dcc_rest_request_t *)1;
  CHECK(dcc_ctx_reply_ex(NULL, &message, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  out = (dcc_rest_request_t *)1;
  CHECK(dcc_flow_reply_ex(flow, NULL, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  out = (dcc_rest_request_t *)1;
  CHECK(dcc_flow_defer_ex(NULL, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  out = (dcc_rest_request_t *)1;
  CHECK(dcc_ctx_edit_original_ex(ctx, NULL, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  out = (dcc_rest_request_t *)1;
  CHECK(dcc_ctx_followup_ex(ctx, NULL, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  options.version++;
  out = (dcc_rest_request_t *)1;
  CHECK(dcc_ctx_defer_ex(ctx, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  options.version = DCC_REST_CALL_OPTIONS_VERSION;
  options.flags = UINT64_MAX;
  out = (dcc_rest_request_t *)1;
  CHECK(dcc_flow_defer_ex(flow, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  options.flags = 0;
  options.audit_log_reason = "unsupported";
  CHECK(dcc_ctx_edit_original_ex(ctx, &message, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  options.audit_log_reason = NULL; options.auth_mode = DCC_REST_AUTH_NONE;
  CHECK(dcc_flow_edit_original_ex(flow, &message, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  CHECK(dcc_ctx_followup_ex(ctx, &message, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  CHECK(dcc_flow_followup_ex(flow, &message, &options, &out) == DCC_ERR_INVALID_ARG && out == NULL);
  CHECK(atomic_load(&seen.calls) == 0U);
  return 0;
}

/* Real HTTP transport: queued cancellation must not send a second request;
 * destroying either owner must not suppress accepted terminal delivery. */
static int queued(unsigned destroy_second, unsigned cancel_first, unsigned no_output) {
  http_server_t server;
  pthread_t server_thread, runtime;
  CHECK(start_server_mode(&server, &server_thread, cancel_first == 2U ? 6 : 0) == 0);
  server.delay_ms = 350U;
  set_api_base_for_server(&server);
  dcc_client_options_t config = {.size = sizeof(config), .token = "", .rest_concurrency = 2U};
  dcc_client_t *client = NULL;
  CHECK(dcc_client_create(&config, &client) == DCC_OK);
  CHECK(dcc_client_start(client) == DCC_OK);
  errors_t errors = {0};
  atomic_store(&errors.release, cancel_first == 2U ? 0U : 1U);
  CHECK(dcc_client_on_error(client, observe, &errors) == DCC_OK);
  CHECK(pthread_create(&runtime, NULL, run, client) == 0);
  dcc_interaction_t interaction = {.id = 700U, .application_id = 701U, .token = "advanced-token"};
  dcc_interaction_flow_t *flow = NULL;
  CHECK(dcc_flow_create(client, &interaction, &flow) == DCC_OK);
  dcc_ctx_t ctx = {.flow = flow, .client = client, .interaction = &interaction};
  CHECK(invalid(flow, &ctx) == 0);
  seen_t first = {0}, second = {0}, third = {0};
  first.skip_handle = cancel_first == 4U;
  second.skip_handle = destroy_second == 1U;
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = complete; options.user_data = &first;
  options.flags = DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY | DCC_REST_CALL_FLAG_SENSITIVE_RESULT_BODY;
  char content[] = "copied stack input";
  dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
  CHECK(dcc_message_builder_set_content(&message, content) == DCC_OK);
  CHECK(dcc_ctx_reply_ex(&ctx, &message, &options, &first.request) == DCC_OK);
  CHECK(first.request != NULL);
  memset(content, 'X', sizeof(content) - 1U);
  uint64_t deadline = test_now_ms() + 3000U;
  while (atomic_load_explicit(&server.requests_seen, memory_order_acquire) == 0U && test_now_ms() < deadline) usleep(1000U);
  CHECK(atomic_load(&server.requests_seen) == 1U);
  options.user_data = &second;
  second.destroy = destroy_second == 2U;
  CHECK(dcc_flow_followup_ex(flow, &message, &options, no_output ? NULL : &second.request) == DCC_OK);
  if (!no_output) {
    CHECK(second.request != NULL && !dcc_rest_request_completed(second.request));
    if (destroy_second == 1U) dcc_rest_request_destroy(second.request);
    else { CHECK(dcc_rest_request_cancel(second.request) == DCC_OK); CHECK(dcc_rest_request_cancel(second.request) == DCC_OK); }
  }
  CHECK(atomic_load(&second.calls) == 0U);
  options.user_data = &third;
  CHECK(dcc_ctx_followup_ex(&ctx, &message, &options, NULL) == DCC_OK);
  if (cancel_first == 1U || no_output) CHECK(dcc_rest_request_cancel(first.request) == DCC_OK);
  if (cancel_first == 4U) {
    dcc_rest_request_destroy(first.request);
  }
  if (cancel_first == 3U) CHECK(dcc_client_stop(client) == DCC_OK);
  if (cancel_first == 2U) {
    deadline = test_now_ms() + 3000U;
    while (!atomic_load(&errors.calls) && test_now_ms() < deadline) usleep(100U);
    CHECK(atomic_load(&errors.calls) == 1U && atomic_load(&first.calls) == 1U);
    dcc_rest_request_t *invalid_output = (dcc_rest_request_t *)1;
    CHECK(dcc_ctx_followup_ex(&ctx, NULL, NULL, &invalid_output) == DCC_ERR_INVALID_ARG && invalid_output == NULL);
    invalid_output = (dcc_rest_request_t *)1;
    CHECK(dcc_ctx_followup_ex(&ctx, &message, &options, &invalid_output) == DCC_ERR_STATE && invalid_output == NULL);
    const dcc_rest_result_t *pending = NULL;
    CHECK(dcc_rest_request_wait(first.request, 10U, &pending) == DCC_ERR_TIMEOUT && pending == NULL);
    CHECK(atomic_load(&second.calls) == 0U);
    atomic_store_explicit(&errors.release, 1U, memory_order_release);
  }
  dcc_flow_destroy(flow);
  const dcc_rest_result_t *result = NULL;
  if (cancel_first != 4U) {
    CHECK(dcc_rest_request_wait(first.request, 5000U, &result) == DCC_OK);
    CHECK(dcc_rest_result_status(result) == first.status && atomic_load(&first.calls) == 1U);
    CHECK(first.self_wait == DCC_ERR_STATE);
  }
  if (second.request && !second.destroy && destroy_second != 1U) {
    CHECK(dcc_rest_request_wait(second.request, 5000U, &result) == DCC_OK);
    if (cancel_first == 3U) CHECK(dcc_rest_result_status(result) == DCC_ERR_STATE || dcc_rest_result_status(result) == DCC_ERR_CANCELED);
    else CHECK(dcc_rest_result_status(result) == (cancel_first == 2U ? DCC_ERR_STATE : DCC_ERR_CANCELED));
    CHECK(second.self_wait == DCC_ERR_STATE);
    dcc_rest_request_destroy(second.request);
  }
  CHECK(dcc_rest_async_wait(client, 5000U) == DCC_OK);
  CHECK(atomic_load(&first.calls) == 1U);
  CHECK(atomic_load(&second.calls) == 1U);
  if (cancel_first == 3U) CHECK(second.status == DCC_ERR_STATE || second.status == DCC_ERR_CANCELED);
  else CHECK(second.status == (cancel_first == 2U ? DCC_ERR_STATE : DCC_ERR_CANCELED));
  CHECK((second.flags & DCC_REST_RESULT_FLAG_SENSITIVE_BODY) != 0U);
  CHECK(atomic_load(&third.calls) == 1U && third.status != DCC_OK);
  CHECK(atomic_load(&errors.calls) == (first.status == DCC_OK ? 2U : 3U));
  dcc_rest_runtime_stats_t stats = {.size = sizeof(stats), .version = DCC_REST_RUNTIME_STATS_VERSION};
  CHECK(dcc_rest_runtime_stats(client, &stats) == DCC_OK);
  CHECK(stats.interaction_actions == 0U && stats.interaction_reserved_bytes == 0U);
  CHECK(pthread_join(server_thread, NULL) == 0);
  struct pollfd listener = {.fd = server.fd, .events = POLLIN};
  CHECK(poll(&listener, 1U, 0) == 0); /* No second transport even connected. */
  close(server.fd);
  CHECK(server.request_count == 1U);
  CHECK(strcmp(server.body, "{\"type\":4,\"data\":{\"content\":\"copied stack input\"}}") == 0);
  if (cancel_first != 4U) dcc_rest_request_destroy(first.request);
  CHECK(dcc_client_stop(client) == DCC_OK);
  CHECK(pthread_join(runtime, NULL) == 0);
  dcc_client_destroy(client);
  return 0;
}

/* Catches wrong operation selection, ephemeral default, lost copied options,
 * early handle release after flow teardown and cancellation after completion. */
static int success(unsigned variant) {
  http_server_t server;
  pthread_t server_thread, runtime;
  CHECK(start_server_mode(&server, &server_thread, 7) == 0);
  server.delay_ms = 30U;
  set_api_base_for_server(&server);
  dcc_client_options_t config = {.size = sizeof(config), .token = "", .rest_concurrency = 2U};
  dcc_client_t *client = NULL;
  CHECK(dcc_client_create(&config, &client) == DCC_OK);
  CHECK(dcc_client_start(client) == DCC_OK);
  CHECK(pthread_create(&runtime, NULL, run, client) == 0);
  dcc_interaction_t interaction = {.id = 710U, .application_id = 711U, .token = "success-token"};
  dcc_interaction_flow_t *flow = NULL;
  CHECK(dcc_flow_create(client, &interaction, &flow) == DCC_OK);
  dcc_ctx_t ctx = {.flow = flow, .client = client, .interaction = &interaction};
  seen_t first = {0}, second = {0};
  if (variant == 3U) second.destroy_flow = flow;
  dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
  CHECK(dcc_message_builder_set_content(&message, "hello") == DCC_OK);
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = complete; options.user_data = &first;
  dcc_status_t admission = variant == 0U ? dcc_flow_defer_ex(flow, NULL, &first.request)
      : variant == 1U ? dcc_ctx_defer_ex(&ctx, &options, &first.request)
      : variant == 2U ? dcc_flow_reply_ex(flow, &message, &options, &first.request)
      : dcc_ctx_reply_ex(&ctx, &message, &options, &first.request);
  CHECK(admission == DCC_OK && first.request != NULL);
  options.user_data = &second;
  options.flags = DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY | DCC_REST_CALL_FLAG_SENSITIVE_RESULT_BODY;
  admission = variant == 0U ? dcc_flow_edit_original_ex(flow, &message, &options, &second.request)
      : variant == 1U ? dcc_ctx_edit_original_ex(&ctx, &message, &options, &second.request)
      : variant == 2U ? dcc_flow_followup_ex(flow, &message, &options, &second.request)
      : dcc_ctx_followup_ex(&ctx, &message, &options, &second.request);
  CHECK(admission == DCC_OK && second.request != NULL);
  options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
  if (variant != 3U) dcc_flow_destroy(flow);
  const dcc_rest_result_t *result = NULL;
  CHECK(dcc_rest_request_wait(second.request, 5000U, &result) == DCC_OK);
  CHECK(second.status == DCC_OK && atomic_load(&second.calls) == 1U);
  CHECK(result->body_len == 12U && memcmp(result->body, "{\"id\":\"900\"}", 12U) == 0);
  CHECK((result->flags & DCC_REST_RESULT_FLAG_SENSITIVE_BODY) != 0U);
  CHECK(dcc_rest_request_cancel(second.request) == DCC_OK && dcc_rest_request_cancel(second.request) == DCC_OK);
  CHECK(dcc_rest_request_wait(first.request, 5000U, &result) == DCC_OK);
  CHECK(dcc_rest_result_status(result) == DCC_OK);
  CHECK(atomic_load(&first.calls) == (variant == 0U ? 0U : 1U));
  if (variant != 0U) CHECK(first.self_wait == DCC_ERR_STATE);
  CHECK(second.self_wait == DCC_ERR_STATE);
  CHECK(dcc_rest_async_wait(client, 5000U) == DCC_OK);
  CHECK(pthread_join(server_thread, NULL) == 0);
  close(server.fd);
  CHECK(server.request_count == 2U);
  CHECK(strcmp(server.bodies[0], variant < 2U ? "{\"type\":5}" : "{\"type\":4,\"data\":{\"content\":\"hello\"}}") == 0);
  CHECK(strcmp(server.bodies[1], "{\"content\":\"hello\"}") == 0);
  CHECK(strcmp(server.methods[1], variant < 2U ? "PATCH" : "POST") == 0);
  CHECK(strcmp(server.paths[1], variant < 2U ? "/webhooks/711/success-token/messages/@original" : "/webhooks/711/success-token") == 0);
  dcc_rest_request_destroy(first.request);
  dcc_rest_request_destroy(second.request);
  CHECK(dcc_client_stop(client) == DCC_OK);
  CHECK(pthread_join(runtime, NULL) == 0);
  dcc_client_destroy(client);
  return 0;
}

typedef struct order {
  atomic_uint entered;
  atomic_uint release;
  char path[3][128];
} order_t;
static dcc_status_t ordered_transport(dcc_client_t *client, const char *method,
    const char *path, const void *body, size_t len, const char *type,
    dcc_rest_cb cb, void *data, void *context) {
  (void)method; (void)body; (void)len; (void)type;
  order_t *order = context;
  unsigned index = atomic_fetch_add(&order->entered, 1U);
  if (index < 3U) snprintf(order->path[index], sizeof(order->path[index]), "%s", path);
  if (index == 0U) while (!atomic_load_explicit(&order->release, memory_order_acquire)) usleep(100U);
  dcc_rest_response_t response = {.size = sizeof(response), .status = 204U, .error = DCC_OK};
  if (cb) cb(client, &response, data);
  return DCC_OK;
}

/* Catches dropping a queued action's priority: HIGH must beat a previously
 * admitted NORMAL request when the single transport slot becomes available.
 * The race variants catch duplicate completion and lost cancellation at attach. */
static int priority_and_race(unsigned race) {
  dcc_client_options_t config = {.size = sizeof(config), .token = "", .rest_concurrency = 1U};
  dcc_client_t *client = NULL;
  pthread_t runtime;
  CHECK(dcc_client_create(&config, &client) == DCC_OK);
  CHECK(dcc_client_start(client) == DCC_OK);
  CHECK(pthread_create(&runtime, NULL, run, client) == 0);
  order_t order = {0};
  dcc_rest_set_interceptor(client, ordered_transport, &order);
  dcc_interaction_t interaction = {.id = 720U, .application_id = 721U, .token = "priority-token"};
  dcc_interaction_flow_t *flow = NULL;
  CHECK(dcc_flow_create(client, &interaction, &flow) == DCC_OK);
  seen_t first = {0}, second = {0};
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = complete; options.user_data = &first;
  CHECK(dcc_flow_defer_ex(flow, &options, NULL) == DCC_OK);
  uint64_t deadline = test_now_ms() + 3000U;
  while (!atomic_load(&order.entered) && test_now_ms() < deadline) usleep(100U);
  CHECK(atomic_load(&order.entered) == 1U);
  dcc_rest_request_desc_t low = DCC_REST_REQUEST_DESC_INIT;
  low.path = "/channels/900/messages";
  CHECK(dcc_rest_submit(client, &low, NULL) == DCC_OK);
  dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
  CHECK(dcc_message_builder_set_content(&message, "priority") == DCC_OK);
  options.priority = DCC_REST_PRIORITY_HIGH;
  options.user_data = &second;
  CHECK(dcc_flow_reply_ex(flow, &message, &options, &second.request) == DCC_OK);
  options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
  dcc_flow_destroy(flow);
  atomic_store_explicit(&order.release, 1U, memory_order_release);
  if (race) CHECK(dcc_rest_request_cancel(second.request) == DCC_OK);
  const dcc_rest_result_t *result = NULL;
  CHECK(dcc_rest_request_wait(second.request, 5000U, &result) == DCC_OK);
  CHECK(atomic_load(&second.calls) == 1U && dcc_rest_result_status(result) == second.status);
  CHECK(second.status == DCC_OK || (race && second.status == DCC_ERR_CANCELED));
  CHECK(dcc_rest_async_wait(client, 5000U) == DCC_OK);
  CHECK(atomic_load(&first.calls) == 1U && first.status == DCC_OK);
  if (!race) {
    CHECK(atomic_load(&order.entered) == 3U);
    CHECK(strcmp(order.path[1], "/webhooks/721/priority-token/messages/@original") == 0);
    CHECK(strcmp(order.path[2], "/channels/900/messages") == 0);
  }
  dcc_rest_runtime_stats_t stats = {.size = sizeof(stats), .version = DCC_REST_RUNTIME_STATS_VERSION};
  CHECK(dcc_rest_runtime_stats(client, &stats) == DCC_OK);
  CHECK(stats.interaction_actions == 0U && stats.interaction_reserved_bytes == 0U);
  dcc_rest_request_destroy(second.request);
  CHECK(dcc_client_stop(client) == DCC_OK);
  CHECK(pthread_join(runtime, NULL) == 0);
  dcc_client_destroy(client);
  return 0;
}

/* Catches leaked unpublished caller/action references and lost output clearing
 * when transport admission fails after the logical handle was allocated. */
static int rejected_admission(void) {
  dcc_client_options_t config = {.size = sizeof(config), .token = "", .rest_max_queued_bytes = 64U, .rest_max_request_bytes = 64U};
  dcc_client_t *client = NULL;
  CHECK(dcc_client_create(&config, &client) == DCC_OK);
  CHECK(dcc_client_start(client) == DCC_OK);
  dcc_interaction_t interaction = {.id = 730U, .application_id = 731U, .token = "reject-token"};
  dcc_interaction_flow_t *flow = NULL;
  CHECK(dcc_flow_create(client, &interaction, &flow) == DCC_OK);
  seen_t seen = {0};
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = complete; options.user_data = &seen;
  dcc_rest_request_t *out = (dcc_rest_request_t *)1;
  CHECK(dcc_flow_defer_ex(flow, &options, &out) == DCC_ERR_RESOURCE_LIMIT);
  CHECK(out == NULL && atomic_load(&seen.calls) == 0U);
  dcc_flow_destroy(flow);
  dcc_rest_runtime_stats_t stats = {.size = sizeof(stats), .version = DCC_REST_RUNTIME_STATS_VERSION};
  CHECK(dcc_rest_runtime_stats(client, &stats) == DCC_OK);
  CHECK(stats.interaction_actions == 0U && stats.interaction_reserved_bytes == 0U);
  CHECK(stats.queued_requests == 0U && stats.queued_request_bytes == 0U);
  CHECK(dcc_client_stop(client) == DCC_OK);
  (void)dcc_client_wait(client);
  dcc_client_destroy(client);
  return 0;
}
int main(void) {
  signal(SIGPIPE, SIG_IGN);
  CHECK(queued(0U, 0U, 0U) == 0);
  CHECK(queued(1U, 0U, 0U) == 0);
  CHECK(queued(2U, 1U, 0U) == 0);
  CHECK(queued(0U, 1U, 1U) == 0);
  CHECK(queued(0U, 2U, 0U) == 0);
  CHECK(queued(0U, 3U, 0U) == 0);
  CHECK(queued(0U, 4U, 0U) == 0);
  for (unsigned variant = 0U; variant < 4U; ++variant) CHECK(success(variant) == 0);
  CHECK(priority_and_race(0U) == 0);
  for (unsigned i = 0U; i < 32U; ++i) CHECK(priority_and_race(1U) == 0);
  CHECK(rejected_admission() == 0);
  return 0;
}
#endif
