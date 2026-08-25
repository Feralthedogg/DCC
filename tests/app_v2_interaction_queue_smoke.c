#include <dcc/dcc.h>
#include <dcc/interaction_flow.h>

#include "internal/rest/dcc_rest_intercept_internal.h"

#if defined(_WIN32)
int main(void) { return 0; }
#else

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct queue_red_state {
  atomic_uint request_count;
  atomic_uint callback_count;
  atomic_uint error_count;
  atomic_uint observer_entered;
  atomic_uint observer_release;
  atomic_bool release_first;
  uint16_t response_status;
} queue_red_state_t;

static void queue_red_error_observer(dcc_client_t *client,
                                     const dcc_error_t *error,
                                     void *user_data) {
  (void)client;
  (void)error;
  queue_red_state_t *state = (queue_red_state_t *)user_data;
  atomic_store_explicit(&state->observer_entered, 1U, memory_order_release);
  while (atomic_load_explicit(&state->observer_release, memory_order_acquire) ==
         0U)
    usleep(100U);
}

static int queue_red_wait_for(const atomic_uint *value, unsigned expected) {
  for (size_t i = 0U; i < 50000U; ++i) {
    if (atomic_load_explicit(value, memory_order_acquire) == expected)
      return 1;
    usleep(100U);
  }
  return 0;
}

static void queue_red_callback(dcc_client_t *client,
                               const dcc_rest_result_t *response,
                               void *user_data) {
  (void)client;
  queue_red_state_t *state = (queue_red_state_t *)user_data;
  if (state != NULL && response != NULL) {
    atomic_fetch_add_explicit(&state->callback_count, 1U, memory_order_acq_rel);
    if (dcc_rest_result_status(response) != DCC_OK)
      atomic_fetch_add_explicit(&state->error_count, 1U, memory_order_acq_rel);
  }
}

static dcc_status_t queue_red_interceptor(dcc_client_t *client,
                                          const char *method, const char *path,
                                          const void *body, size_t body_len,
                                          const char *content_type,
                                          dcc_rest_cb callback, void *user_data,
                                          void *intercept_user_data) {
  (void)method;
  (void)path;
  (void)body;
  (void)body_len;
  (void)content_type;
  queue_red_state_t *state = (queue_red_state_t *)intercept_user_data;
  atomic_fetch_add_explicit(&state->request_count, 1U, memory_order_acq_rel);
  while (!atomic_load_explicit(&state->release_first, memory_order_acquire))
    usleep(100U);
  if (callback != NULL) {
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = state->response_status != 0U ? state->response_status : 204U,
        .error = DCC_OK};
    callback(client, &response, user_data);
  }
  return DCC_OK;
}

typedef struct queue_red_runner {
  dcc_client_t *client;
} queue_red_runner_t;

static void *queue_red_runtime(void *user_data) {
  queue_red_runner_t *runner = (queue_red_runner_t *)user_data;
  (void)dcc_client_wait(runner->client);
  return NULL;
}

int main(void) {
  dcc_client_options_t options = {
      .size = sizeof(options),
      .token = "",
      .intents = DCC_INTENT_GUILDS,
      .rest_concurrency = 2U,
  };
  dcc_client_t *client = NULL;
  if (dcc_client_create(&options, &client) != DCC_OK ||
      dcc_client_start(client) != DCC_OK)
    return 1;
  queue_red_runner_t runner = {.client = client};
  pthread_t runtime;
  if (pthread_create(&runtime, NULL, queue_red_runtime, &runner) != 0)
    return 1;

  queue_red_state_t state;
  memset(&state, 0, sizeof(state));
  atomic_init(&state.request_count, 0U);
  atomic_init(&state.callback_count, 0U);
  atomic_init(&state.error_count, 0U);
  atomic_init(&state.observer_entered, 0U);
  atomic_init(&state.observer_release, 0U);
  atomic_init(&state.release_first, false);
  dcc_rest_set_interceptor(client, queue_red_interceptor, &state);

  dcc_interaction_t interaction = {
      .id = 700U, .application_id = 701U, .token = "ordered-token"};
  dcc_interaction_flow_t *flow = NULL;
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK)
    return 1;
  dcc_status_t overflow_status = dcc_flow_auto_defer(flow, UINT64_MAX);
  dcc_message_builder_t first = DCC_MESSAGE_BUILDER_INIT;
  first.present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
  first.content = "first";
  dcc_message_builder_t second = DCC_MESSAGE_BUILDER_INIT;
  second.present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
  second.content = "second";

  dcc_status_t first_status =
      dcc_flow_reply(flow, &first, queue_red_callback, &state);
  dcc_status_t second_status =
      dcc_flow_reply(flow, &second, queue_red_callback, &state);
  usleep(50000U);
  unsigned before_release =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  dcc_interaction_flow_state_t queued_state = dcc_flow_state(flow);
  atomic_store_explicit(&state.release_first, true, memory_order_release);
  dcc_status_t drain = dcc_rest_async_wait(client, 5000U);
  unsigned final_requests =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  unsigned callbacks =
      atomic_load_explicit(&state.callback_count, memory_order_acquire);
  dcc_interaction_flow_state_t final_state = dcc_flow_state(flow);

  dcc_rest_set_interceptor(client, NULL, NULL);
  dcc_flow_destroy(flow);
  if (overflow_status != DCC_ERR_INVALID_ARG || first_status != DCC_OK ||
      second_status != DCC_OK ||
      before_release != 1U ||
      queued_state != DCC_INTERACTION_FLOW_INITIAL_QUEUED ||
      strcmp(dcc_flow_state_string(queued_state), "initial_queued") != 0 ||
      final_requests != 2U || callbacks != 2U || drain != DCC_OK ||
      final_state != DCC_INTERACTION_FLOW_FOLLOWED_UP) {
    fprintf(stderr,
            "interaction FIFO RED: first=%d second=%d before=%u final=%u "
            "callbacks=%u drain=%d queued=%d final_state=%d\n",
            first_status, second_status, before_release, final_requests,
            callbacks, drain, queued_state, final_state);
    return 1;
  }

  memset(&state, 0, sizeof(state));
  atomic_init(&state.request_count, 0U);
  atomic_init(&state.callback_count, 0U);
  atomic_init(&state.error_count, 0U);
  atomic_init(&state.observer_entered, 0U);
  atomic_init(&state.observer_release, 0U);
  atomic_init(&state.release_first, false);
  dcc_rest_set_interceptor(client, queue_red_interceptor, &state);
  interaction.id = 702U;
  interaction.token = "defer-token";
  flow = NULL;
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK)
    return 1;
  dcc_status_t defer_status = dcc_flow_defer(flow, queue_red_callback, &state);
  dcc_status_t edit_status =
      dcc_flow_reply(flow, &second, queue_red_callback, &state);
  usleep(50000U);
  before_release =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  queued_state = dcc_flow_state(flow);
  atomic_store_explicit(&state.release_first, true, memory_order_release);
  drain = dcc_rest_async_wait(client, 5000U);
  final_requests =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  callbacks = atomic_load_explicit(&state.callback_count, memory_order_acquire);
  final_state = dcc_flow_state(flow);
  dcc_rest_set_interceptor(client, NULL, NULL);
  dcc_flow_destroy(flow);
  if (defer_status != DCC_OK || edit_status != DCC_OK || before_release != 1U ||
      queued_state != DCC_INTERACTION_FLOW_DEFERRED_QUEUED ||
      strcmp(dcc_flow_state_string(queued_state), "deferred_queued") != 0 ||
      final_requests != 2U || callbacks != 2U || drain != DCC_OK ||
      final_state != DCC_INTERACTION_FLOW_ORIGINAL_EDITED) {
    fprintf(stderr,
            "defer FIFO RED: defer=%d edit=%d before=%u final=%u callbacks=%u "
            "drain=%d queued=%d final_state=%d\n",
            defer_status, edit_status, before_release, final_requests,
            callbacks, drain, queued_state, final_state);
    return 1;
  }

  memset(&state, 0, sizeof(state));
  atomic_init(&state.request_count, 0U);
  atomic_init(&state.callback_count, 0U);
  atomic_init(&state.error_count, 0U);
  atomic_init(&state.observer_entered, 0U);
  atomic_init(&state.observer_release, 0U);
  atomic_init(&state.release_first, false);
  dcc_rest_set_interceptor(client, queue_red_interceptor, &state);
  interaction.id = 706U;
  interaction.token = "modal-token";
  flow = NULL;
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK)
    return 1;
  dcc_component_builder_t text_input;
  dcc_component_builder_t row;
  dcc_modal_builder_t modal = DCC_MODAL_BUILDER_INIT;
  dcc_component_builder_init(&text_input, DCC_COMPONENT_TEXT_INPUT);
  dcc_component_builder_init(&row, DCC_COMPONENT_ACTION_ROW);
  dcc_modal_builder_init(&modal);
  if (dcc_component_builder_set_custom_id(&text_input, "field") != DCC_OK ||
      dcc_component_builder_set_text_input_style(
          &text_input, DCC_TEXT_INPUT_SHORT) != DCC_OK ||
      dcc_component_builder_set_label(&text_input, "Field") != DCC_OK ||
      dcc_component_builder_set_children(&row, &text_input, 1U) != DCC_OK ||
      dcc_modal_builder_set_custom_id(&modal, "ordered-modal") != DCC_OK ||
      dcc_modal_builder_set_title(&modal, "Ordered") != DCC_OK ||
      dcc_modal_builder_set_components(&modal, &row, 1U) != DCC_OK)
    return 1;
  dcc_status_t modal_status =
      dcc_flow_show_modal(flow, &modal, queue_red_callback, &state);
  dcc_status_t modal_followup_status =
      dcc_flow_reply(flow, &second, queue_red_callback, &state);
  usleep(50000U);
  before_release =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  queued_state = dcc_flow_state(flow);
  atomic_store_explicit(&state.release_first, true, memory_order_release);
  drain = dcc_rest_async_wait(client, 5000U);
  final_requests =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  callbacks = atomic_load_explicit(&state.callback_count, memory_order_acquire);
  final_state = dcc_flow_state(flow);
  dcc_rest_set_interceptor(client, NULL, NULL);
  dcc_flow_destroy(flow);
  if (modal_status != DCC_OK || modal_followup_status != DCC_OK ||
      before_release != 1U ||
      queued_state != DCC_INTERACTION_FLOW_INITIAL_QUEUED ||
      final_requests != 2U || callbacks != 2U || drain != DCC_OK ||
      final_state != DCC_INTERACTION_FLOW_FOLLOWED_UP) {
    fprintf(stderr,
            "modal FIFO RED: modal=%d followup=%d before=%u final=%u "
            "callbacks=%u drain=%d queued=%d final_state=%d\n",
            modal_status, modal_followup_status, before_release, final_requests,
            callbacks, drain, queued_state, final_state);
    return 1;
  }

  memset(&state, 0, sizeof(state));
  atomic_init(&state.request_count, 0U);
  atomic_init(&state.callback_count, 0U);
  atomic_init(&state.error_count, 0U);
  atomic_init(&state.observer_entered, 0U);
  atomic_init(&state.observer_release, 0U);
  atomic_init(&state.release_first, false);
  state.response_status = 500U;
  dcc_rest_set_interceptor(client, queue_red_interceptor, &state);
  if (dcc_client_on_error(client, queue_red_error_observer, &state) != DCC_OK)
    return 1;
  interaction.id = 705U;
  interaction.token = "failure-token";
  flow = NULL;
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK)
    return 1;
  first_status = dcc_flow_reply(flow, &first, queue_red_callback, &state);
  second_status = dcc_flow_reply(flow, &second, queue_red_callback, &state);
  atomic_store_explicit(&state.release_first, true, memory_order_release);
  int observer_entered = queue_red_wait_for(&state.observer_entered, 1U);
  unsigned callbacks_before_observer_return =
      atomic_load_explicit(&state.callback_count, memory_order_acquire);
  atomic_store_explicit(&state.observer_release, 1U, memory_order_release);
  drain = dcc_rest_async_wait(client, 5000U);
  final_requests =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  callbacks = atomic_load_explicit(&state.callback_count, memory_order_acquire);
  unsigned errors =
      atomic_load_explicit(&state.error_count, memory_order_acquire);
  final_state = dcc_flow_state(flow);
  (void)dcc_client_on_error(client, NULL, NULL);
  dcc_rest_set_interceptor(client, NULL, NULL);
  dcc_flow_destroy(flow);
  if (first_status != DCC_OK || second_status != DCC_OK || drain != DCC_OK ||
      !observer_entered || callbacks_before_observer_return != 1U ||
      final_requests != 1U || callbacks != 2U || errors != 2U ||
      final_state != DCC_INTERACTION_FLOW_FAILED) {
    fprintf(stderr,
            "interaction failure cascade RED: first=%d second=%d drain=%d "
            "requests=%u callbacks=%u callbacks_before_observer=%u "
            "observer=%d errors=%u state=%d\n",
            first_status, second_status, drain, final_requests, callbacks,
            callbacks_before_observer_return, observer_entered, errors,
            final_state);
    return 1;
  }

  (void)dcc_client_stop(client);
  (void)pthread_join(runtime, NULL);
  dcc_client_destroy(client);

  dcc_client_options_t bad_options = options;
  bad_options.rest_max_request_bytes = 100U;
  bad_options.rest_max_queued_bytes = 99U;
  dcc_client_t *bad_client = NULL;
  if (dcc_client_create(&bad_options, &bad_client) != DCC_ERR_INVALID_ARG ||
      bad_client != NULL)
    return 1;

  dcc_client_options_t limited_options = options;
  limited_options.interaction_max_actions_per_queue = 1U;
  limited_options.interaction_max_live_queues = 1U;
  client = NULL;
  if (dcc_client_create(&limited_options, &client) != DCC_OK ||
      dcc_client_start(client) != DCC_OK)
    return 1;
  runner.client = client;
  if (pthread_create(&runtime, NULL, queue_red_runtime, &runner) != 0)
    return 1;
  memset(&state, 0, sizeof(state));
  atomic_init(&state.request_count, 0U);
  atomic_init(&state.callback_count, 0U);
  atomic_init(&state.error_count, 0U);
  atomic_init(&state.observer_entered, 0U);
  atomic_init(&state.observer_release, 0U);
  atomic_init(&state.release_first, false);
  dcc_rest_set_interceptor(client, queue_red_interceptor, &state);
  interaction.id = 703U;
  interaction.token = "limited-token";
  flow = NULL;
  if (dcc_flow_create(client, &interaction, &flow) != DCC_OK)
    return 1;
  dcc_interaction_t other_interaction = interaction;
  other_interaction.id = 704U;
  other_interaction.token = "other-token";
  dcc_interaction_flow_t *other_flow = NULL;
  dcc_status_t live_limit_status =
      dcc_flow_create(client, &other_interaction, &other_flow);
  first_status = dcc_flow_reply(flow, &first, queue_red_callback, &state);
  second_status = dcc_flow_reply(flow, &second, queue_red_callback, &state);
  usleep(50000U);
  before_release =
      atomic_load_explicit(&state.request_count, memory_order_acquire);
  dcc_rest_runtime_stats_t resource_stats = DCC_REST_RUNTIME_STATS_INIT;
  dcc_status_t stats_status = dcc_rest_runtime_stats(client, &resource_stats);
  atomic_store_explicit(&state.release_first, true, memory_order_release);
  drain = dcc_rest_async_wait(client, 5000U);
  callbacks = atomic_load_explicit(&state.callback_count, memory_order_acquire);
  dcc_rest_set_interceptor(client, NULL, NULL);
  dcc_flow_destroy(flow);
  dcc_interaction_flow_t *duplicate_flow = NULL;
  dcc_status_t duplicate_status =
      dcc_flow_create(client, &interaction, &duplicate_flow);
  if (duplicate_flow != NULL)
    dcc_flow_destroy(duplicate_flow);
  (void)dcc_client_stop(client);
  (void)pthread_join(runtime, NULL);
  dcc_client_destroy(client);
  if (first_status != DCC_OK || second_status != DCC_ERR_RESOURCE_LIMIT ||
      live_limit_status != DCC_ERR_RESOURCE_LIMIT ||
      duplicate_status != DCC_ERR_STATE || before_release != 1U ||
      callbacks != 1U || drain != DCC_OK || stats_status != DCC_OK ||
      resource_stats.interaction_actions != 1U ||
      resource_stats.interaction_rejected_actions_per_queue != 1U ||
      resource_stats.interaction_rejected_live_queues != 1U) {
    fprintf(stderr,
            "interaction action limit RED: first=%d second=%d before=%u "
            "callbacks=%u drain=%d stats=%d actions=%llu rejected=%llu "
            "live_limit=%d duplicate=%d\n",
            first_status, second_status, before_release, callbacks, drain,
            stats_status,
            (unsigned long long)resource_stats.interaction_actions,
            (unsigned long long)
                resource_stats.interaction_rejected_actions_per_queue,
            live_limit_status, duplicate_status);
    return 1;
  }
  return 0;
}

#endif
