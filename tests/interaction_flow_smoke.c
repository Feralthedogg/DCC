#include "support/http_smoke_server.h"

#include <dcc/app.h>
#include <dcc/dcc.h>
#include <dcc/rest/core/async.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"

#if defined(_WIN32)
int main(void) { return 0; }
#else

#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

_Static_assert(
    offsetof(dcc_interaction_flow_t, response_flags) >= 56U,
    "response flags must not reuse the historical 56-byte flow object");
_Static_assert(offsetof(dcc_interaction_flow_t, response_flags) +
                       sizeof(((dcc_interaction_flow_t *)0)->response_flags) >
                   56U,
               "response flags must extend beyond the historical flow object");
_Static_assert(
    sizeof(dcc_interaction_flow_t) > 56U,
    "the current flow layout must honestly advertise its larger allocation");

typedef struct flow_seen {
  int called;
  uint16_t status;
  dcc_status_t error;
} flow_seen_t;

typedef struct flow_runner {
  dcc_client_t *client;
  dcc_status_t status;
} flow_runner_t;

static void *flow_runner_main(void *user_data) {
  flow_runner_t *runner = (flow_runner_t *)user_data;
  runner->status = dcc_client_wait(runner->client);
  return NULL;
}

static void flow_client_shutdown(dcc_client_t *client,
                                 pthread_t runner_thread) {
  (void)dcc_client_stop(client);
  (void)pthread_join(runner_thread, NULL);
  dcc_client_destroy(client);
}

static void flow_rest_cb(dcc_client_t *client,
                         const dcc_rest_result_t *response, void *user_data) {
  (void)client;
  flow_seen_t *seen = (flow_seen_t *)user_data;
  if (seen != NULL && response != NULL) {
    seen->called = 1;
    seen->status = response->http_status;
    seen->error = dcc_rest_result_status(response);
  }
}

static int expect_request(dcc_client_t *client, http_server_t *server,
                          pthread_t thread, dcc_status_t status,
                          const flow_seen_t *seen, const char *method,
                          const char *path, const char *body) {
  dcc_status_t drain_status =
      status == DCC_OK ? dcc_rest_async_wait(client, 5000U) : DCC_ERR_STATE;
  (void)pthread_join(thread, NULL);
  close(server->fd);
  if (status != DCC_OK || drain_status != DCC_OK || seen == NULL ||
      !seen->called || seen->status != 200 ||
      strcmp(server->method, method) != 0 || strcmp(server->path, path) != 0 ||
      strcmp(server->body, body) != 0) {
    fprintf(stderr,
            "unexpected flow request: st=%s called=%d status=%u method=%s "
            "path=%s body=%s\n",
            dcc_status_string(status), seen != NULL ? seen->called : 0,
            seen != NULL ? seen->status : 0, server->method, server->path,
            server->body);
    return 0;
  }
  return 1;
}

static int start_flow_server(http_server_t *server, pthread_t *thread) {
  if (start_server(server, thread) != 0) {
    fprintf(stderr, "failed to start flow server\n");
    return 0;
  }
  set_api_base_for_server(server);
  return 1;
}

static dcc_status_t set_message(dcc_message_builder_t *message,
                                const char *content) {
  dcc_message_builder_init(message);
  return dcc_message_builder_set_content(message, content);
}

typedef enum flow_reentry_variant {
  FLOW_REENTRY_REPLY = 0,
  FLOW_REENTRY_DEFER,
  FLOW_REENTRY_DEFER_EPHEMERAL,
  FLOW_REENTRY_DEFER_UPDATE,
  FLOW_REENTRY_MODAL,
  FLOW_REENTRY_AUTOCOMPLETE,
  FLOW_REENTRY_UPDATE,
  FLOW_REENTRY_MAYBE_AUTO_DEFER
} flow_reentry_variant_t;

typedef struct flow_reentry_case {
  dcc_ctx_t ctx;
  dcc_message_builder_t message;
  dcc_modal_builder_t modal;
  dcc_autocomplete_builder_t autocomplete;
  flow_reentry_variant_t nested_variant;
  atomic_int nested_status;
  atomic_uint request_count;
  atomic_uint callback_count;
  atomic_bool allow_callback;
} flow_reentry_case_t;

typedef struct flow_reentry_expectation {
  flow_reentry_variant_t outer_variant;
  flow_reentry_variant_t nested_variant;
  dcc_status_t nested_status;
  unsigned request_count;
  dcc_interaction_flow_state_t final_state;
  const char *name;
} flow_reentry_expectation_t;

typedef struct historical_flow_reentry {
  dcc_interaction_flow_t *flow;
  atomic_uint request_count;
  atomic_uint callback_count;
  atomic_int nested_status;
  atomic_bool allow_callback;
} historical_flow_reentry_t;

static dcc_status_t flow_reentry_invoke(flow_reentry_case_t *state,
                                        flow_reentry_variant_t variant,
                                        dcc_rest_result_fn cb,
                                        void *user_data) {
  switch (variant) {
  case FLOW_REENTRY_REPLY:
    return dcc_flow_reply(state->ctx.flow, &state->message, cb, user_data);
  case FLOW_REENTRY_DEFER:
    return dcc_flow_defer(state->ctx.flow, cb, user_data);
  case FLOW_REENTRY_DEFER_EPHEMERAL:
    return dcc_flow_defer_ephemeral(state->ctx.flow, cb, user_data);
  case FLOW_REENTRY_DEFER_UPDATE:
    return dcc_flow_defer_update(state->ctx.flow, cb, user_data);
  case FLOW_REENTRY_MODAL:
    return dcc_flow_show_modal(state->ctx.flow, &state->modal, cb, user_data);
  case FLOW_REENTRY_AUTOCOMPLETE:
    return dcc_ctx_reply_autocomplete(&state->ctx, &state->autocomplete, cb,
                                      user_data);
  case FLOW_REENTRY_UPDATE:
    return dcc_ctx_update_message(&state->ctx, &state->message, cb, user_data);
  case FLOW_REENTRY_MAYBE_AUTO_DEFER:
    return dcc_flow_maybe_auto_defer(state->ctx.flow, 2U, cb, user_data);
  }
  return DCC_ERR_INVALID_ARG;
}

static void flow_reentry_outer_cb(dcc_client_t *client,
                                  const dcc_rest_result_t *response,
                                  void *user_data) {
  (void)client;
  flow_reentry_case_t *state = (flow_reentry_case_t *)user_data;
  if (state == NULL || response == NULL || response->http_status != 204U) {
    return;
  }
  (void)atomic_fetch_add_explicit(&state->callback_count, 1U,
                                  memory_order_relaxed);
  atomic_store_explicit(
      &state->nested_status,
      flow_reentry_invoke(state, state->nested_variant, NULL, NULL),
      memory_order_release);
}

static dcc_status_t flow_reentry_intercept(dcc_client_t *client,
                                           const char *method, const char *path,
                                           const void *body, size_t body_len,
                                           const char *content_type,
                                           dcc_rest_cb cb, void *user_data,
                                           void *intercept_user_data) {
  (void)method;
  (void)path;
  (void)body;
  (void)body_len;
  (void)content_type;
  flow_reentry_case_t *state = (flow_reentry_case_t *)intercept_user_data;
  if (state == NULL) {
    return DCC_ERR_STATE;
  }
  (void)atomic_fetch_add_explicit(&state->request_count, 1U,
                                  memory_order_relaxed);
  if (cb != NULL) {
    while (
        !atomic_load_explicit(&state->allow_callback, memory_order_acquire)) {
      usleep(100U);
    }
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = 204U,
        .error = DCC_OK,
    };
    cb(client, &response, user_data);
  }
  return DCC_OK;
}

static void historical_flow_outer_cb(dcc_client_t *client,
                                     const dcc_rest_result_t *response,
                                     void *user_data) {
  (void)client;
  historical_flow_reentry_t *state = (historical_flow_reentry_t *)user_data;
  if (state == NULL || response == NULL || response->http_status != 204U) {
    return;
  }
  (void)atomic_fetch_add_explicit(&state->callback_count, 1U,
                                  memory_order_relaxed);
  atomic_store_explicit(&state->nested_status,
                        dcc_flow_defer(state->flow, NULL, NULL),
                        memory_order_release);
}

static dcc_status_t
historical_flow_intercept(dcc_client_t *client, const char *method,
                          const char *path, const void *body, size_t body_len,
                          const char *content_type, dcc_rest_cb cb,
                          void *user_data, void *intercept_user_data) {
  (void)method;
  (void)path;
  (void)body;
  (void)body_len;
  (void)content_type;
  historical_flow_reentry_t *state =
      (historical_flow_reentry_t *)intercept_user_data;
  if (state == NULL) {
    return DCC_ERR_STATE;
  }
  (void)atomic_fetch_add_explicit(&state->request_count, 1U,
                                  memory_order_relaxed);
  if (cb != NULL) {
    while (
        !atomic_load_explicit(&state->allow_callback, memory_order_acquire)) {
      usleep(100U);
    }
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = 204U,
        .error = DCC_OK,
    };
    cb(client, &response, user_data);
  }
  return DCC_OK;
}

static int check_initial_reentry_claims(dcc_client_t *client) {
  static const flow_reentry_expectation_t cases[] = {
      {FLOW_REENTRY_REPLY, FLOW_REENTRY_DEFER, DCC_ERR_STATE, 1U,
       DCC_INTERACTION_FLOW_REPLIED, "reply -> defer"},
      {FLOW_REENTRY_DEFER, FLOW_REENTRY_REPLY, DCC_OK, 2U,
       DCC_INTERACTION_FLOW_ORIGINAL_EDITED, "defer -> reply"},
      {FLOW_REENTRY_DEFER_EPHEMERAL, FLOW_REENTRY_MODAL, DCC_ERR_STATE, 1U,
       DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL, "ephemeral defer -> modal"},
      {FLOW_REENTRY_DEFER_UPDATE, FLOW_REENTRY_REPLY, DCC_OK, 2U,
       DCC_INTERACTION_FLOW_ORIGINAL_EDITED, "defer update -> reply"},
      {FLOW_REENTRY_MODAL, FLOW_REENTRY_AUTOCOMPLETE, DCC_ERR_STATE, 1U,
       DCC_INTERACTION_FLOW_MODAL, "modal -> autocomplete"},
      {FLOW_REENTRY_AUTOCOMPLETE, FLOW_REENTRY_UPDATE, DCC_ERR_STATE, 1U,
       DCC_INTERACTION_FLOW_REPLIED, "autocomplete -> update"},
      {FLOW_REENTRY_UPDATE, FLOW_REENTRY_MODAL, DCC_ERR_STATE, 1U,
       DCC_INTERACTION_FLOW_REPLIED, "update -> modal"},
      {FLOW_REENTRY_MAYBE_AUTO_DEFER, FLOW_REENTRY_REPLY, DCC_OK, 2U,
       DCC_INTERACTION_FLOW_ORIGINAL_EDITED, "auto defer -> reply"},
  };
  dcc_interaction_t interaction = {
      .id = 777U,
      .application_id = 888U,
      .token = "reentry-token",
  };
  for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
    flow_reentry_case_t state;
    memset(&state, 0, sizeof(state));
    atomic_init(&state.nested_status, DCC_ERR_RUNTIME);
    atomic_init(&state.request_count, 0U);
    atomic_init(&state.callback_count, 0U);
    atomic_init(&state.allow_callback, false);
    state.ctx.client = client;
    state.ctx.interaction = &interaction;
    if (dcc_flow_create(client, &interaction, &state.ctx.flow) != DCC_OK)
      return 1;
    state.nested_variant = cases[i].nested_variant;
    state.message =
        (dcc_message_builder_t){.size = sizeof(dcc_message_builder_t),
                                .version = DCC_MESSAGE_BUILDER_VERSION,
                                .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT,
                                .content = "reentry"};
    state.modal = (dcc_modal_builder_t){
        .size = sizeof(dcc_modal_builder_t),
        .version = DCC_MODAL_BUILDER_VERSION,
        .present = DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID |
                   DCC_MODAL_BUILDER_PRESENT_TITLE |
                   DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON,
        .custom_id = "reentry-modal",
        .title = "Reentry",
        .components_json = "[]"};
    dcc_autocomplete_builder_init(&state.autocomplete);
    if (cases[i].outer_variant == FLOW_REENTRY_MAYBE_AUTO_DEFER &&
        (dcc_flow_set_started_at(state.ctx.flow, 1U) != DCC_OK ||
         dcc_flow_auto_defer(state.ctx.flow, 1U) != DCC_OK)) {
      return 1;
    }
    dcc_rest_set_interceptor(client, flow_reentry_intercept, &state);
    uint64_t started_at = test_now_ms();
    dcc_status_t outer_status = flow_reentry_invoke(
        &state, cases[i].outer_variant, flow_reentry_outer_cb, &state);
    uint64_t elapsed_ms = test_now_ms() - started_at;
    atomic_store_explicit(&state.allow_callback, true, memory_order_release);
    dcc_status_t drain_status = outer_status == DCC_OK
                                    ? dcc_rest_async_wait(client, 5000U)
                                    : DCC_ERR_STATE;
    dcc_status_t nested_status = (dcc_status_t)atomic_load_explicit(
        &state.nested_status, memory_order_acquire);
    unsigned request_count =
        atomic_load_explicit(&state.request_count, memory_order_acquire);
    unsigned callback_count =
        atomic_load_explicit(&state.callback_count, memory_order_acquire);
    if (outer_status != DCC_OK || drain_status != DCC_OK ||
        nested_status != cases[i].nested_status ||
        request_count != cases[i].request_count || callback_count != 1U ||
        elapsed_ms >= 100U ||
        dcc_flow_state(state.ctx.flow) != cases[i].final_state ||
        !dcc_ctx_response_sent(&state.ctx)) {
      fprintf(stderr,
              "initial reentry claim failed (%s): outer=%d nested=%d "
              "requests=%u callbacks=%u elapsed=%llu state=%d sent=%u\n",
              cases[i].name, outer_status, nested_status, request_count,
              callback_count, (unsigned long long)elapsed_ms,
              dcc_flow_state(state.ctx.flow),
              dcc_ctx_response_sent(&state.ctx));
      dcc_rest_set_interceptor(client, NULL, NULL);
      return 1;
    }
  }
  dcc_rest_set_interceptor(client, NULL, NULL);
  return 0;
}

int main(void) {
  (void)signal(SIGPIPE, SIG_IGN);

  dcc_client_t *client = NULL;
  dcc_client_options_t opts = {
      .size = sizeof(opts),
      .token = "",
      .intents = DCC_INTENT_GUILDS,
  };
  dcc_status_t status = dcc_client_create(&opts, &client);
  if (status != DCC_OK || dcc_client_start(client) != DCC_OK) {
    fprintf(stderr, "failed to create flow client: %s\n",
            dcc_status_string(status));
    dcc_client_destroy(client);
    return 1;
  }
  flow_runner_t runner = {.client = client, .status = DCC_ERR_STATE};
  pthread_t runner_thread;
  if (pthread_create(&runner_thread, NULL, flow_runner_main, &runner) != 0) {
    dcc_client_destroy(client);
    return 1;
  }
  dcc_interaction_t interaction = {
      .id = 555U,
      .application_id = 666U,
      .token = "tok",
  };
  dcc_interaction_flow_t *flow = NULL;
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK) {
    flow_client_shutdown(client, runner_thread);
    return 1;
  }

  dcc_message_builder_t message;
  http_server_t server;
  pthread_t thread;
  flow_seen_t seen;

  if (set_message(&message, "hello") != DCC_OK ||
      !start_flow_server(&server, &thread)) {
    flow_client_shutdown(client, runner_thread);
    return 1;
  }
  memset(&seen, 0, sizeof(seen));
  status = dcc_flow_reply(flow, &message, flow_rest_cb, &seen);
  if (!expect_request(client, &server, thread, status, &seen, "POST",
                      "/interactions/555/tok/callback",
                      "{\"type\":4,\"data\":{\"content\":\"hello\"}}") ||
      dcc_flow_state(flow) != DCC_INTERACTION_FLOW_REPLIED) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }

  if (set_message(&message, "again") != DCC_OK ||
      !start_flow_server(&server, &thread)) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }
  memset(&seen, 0, sizeof(seen));
  status = dcc_flow_reply(flow, &message, flow_rest_cb, &seen);
  if (!expect_request(client, &server, thread, status, &seen, "POST",
                      "/webhooks/666/tok", "{\"content\":\"again\"}") ||
      dcc_flow_state(flow) != DCC_INTERACTION_FLOW_FOLLOWED_UP) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }

  dcc_flow_destroy(flow);
  flow = NULL;
  interaction.id = 556U;
  interaction.token = "tok-auto";
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK ||
      dcc_flow_set_started_at(flow, 1000U) != DCC_OK ||
      dcc_flow_auto_defer_ephemeral(flow, 1500U) != DCC_OK ||
      dcc_flow_maybe_auto_defer(flow, 2000U, flow_rest_cb, &seen) != DCC_OK ||
      dcc_flow_state(flow) != DCC_INTERACTION_FLOW_READY ||
      !start_flow_server(&server, &thread)) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }
  memset(&seen, 0, sizeof(seen));
  status = dcc_flow_maybe_auto_defer(flow, 2600U, flow_rest_cb, &seen);
  if (!expect_request(client, &server, thread, status, &seen, "POST",
                      "/interactions/556/tok-auto/callback",
                      "{\"type\":5,\"data\":{\"flags\":64}}") ||
      dcc_flow_state(flow) != DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }

  if (set_message(&message, "done") != DCC_OK ||
      !start_flow_server(&server, &thread)) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }
  memset(&seen, 0, sizeof(seen));
  status = dcc_flow_reply(flow, &message, flow_rest_cb, &seen);
  if (!expect_request(client, &server, thread, status, &seen, "PATCH",
                      "/webhooks/666/tok-auto/messages/@original",
                      "{\"content\":\"done\"}") ||
      dcc_flow_state(flow) != DCC_INTERACTION_FLOW_ORIGINAL_EDITED ||
      strcmp(dcc_flow_state_string(dcc_flow_state(flow)), "original_edited") !=
          0) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }

  dcc_flow_destroy(flow);
  flow = NULL;
  interaction.id = 557U;
  interaction.token = "tok-retry";
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK)
    return 1;
  dcc_message_builder_t invalid_message;
  dcc_message_builder_init(&invalid_message);
  invalid_message.present |= DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
  invalid_message.content = NULL;
  if (dcc_flow_reply(flow, &invalid_message, flow_rest_cb, &seen) !=
          DCC_ERR_INVALID_ARG ||
      dcc_flow_state(flow) != DCC_INTERACTION_FLOW_READY ||
      set_message(&message, "recovered") != DCC_OK ||
      !start_flow_server(&server, &thread)) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }
  memset(&seen, 0, sizeof(seen));
  status = dcc_flow_reply(flow, &message, flow_rest_cb, &seen);
  if (!expect_request(client, &server, thread, status, &seen, "POST",
                      "/interactions/557/tok-retry/callback",
                      "{\"type\":4,\"data\":{\"content\":\"recovered\"}}") ||
      dcc_flow_state(flow) != DCC_INTERACTION_FLOW_REPLIED) {
    (void)unsetenv("DCC_DISCORD_API_BASE");
    flow_client_shutdown(client, runner_thread);
    return 1;
  }

  (void)unsetenv("DCC_DISCORD_API_BASE");
  dcc_flow_destroy(flow);
  flow_client_shutdown(client, runner_thread);
  return 0;
}

#endif
