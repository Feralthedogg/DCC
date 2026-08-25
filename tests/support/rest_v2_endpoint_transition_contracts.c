#include <dcc/app.h>
#include <dcc/managed_message.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"
#include "rest_v2_endpoint_smoke_support.h"

#if !defined(_WIN32)

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum transition_call_kind {
  TRANSITION_CALL_MESSAGE,
  TRANSITION_CALL_WEBHOOK,
  TRANSITION_CALL_MANAGED
} transition_call_kind_t;

typedef struct transition_state {
  dcc_client_t *client;
  dcc_app_t app;
  transition_call_kind_t kind;
  char *payload;
  dcc_message_builder_t message;
  dcc_managed_message_options_t managed;
  pthread_t submit_thread;
  pthread_t intercept_thread;
  pthread_t callback_thread;
  atomic_uint entered;
  atomic_uint release;
  atomic_uint returned;
  atomic_uint callback_calls;
  atomic_uint request_count;
  dcc_status_t call_status;
  char methods[2][16];
  char paths[2][256];
  char bodies[2][256];
} transition_state_t;

static void transition_sleep_ms(unsigned milliseconds) {
  struct timespec delay = {
      .tv_sec = (time_t)(milliseconds / 1000U),
      .tv_nsec = (long)(milliseconds % 1000U) * 1000000L,
  };
  (void)nanosleep(&delay, NULL);
}

static dcc_status_t
transition_intercept(dcc_client_t *client, const char *method, const char *path,
                     const void *body, size_t body_len,
                     const char *content_type, dcc_rest_cb callback,
                     void *callback_user_data, void *intercept_user_data) {
  transition_state_t *state = (transition_state_t *)intercept_user_data;
  unsigned index = atomic_fetch_add_explicit(&state->request_count, 1U,
                                             memory_order_acq_rel);
  state->intercept_thread = pthread_self();
  if (index < 2U) {
    (void)snprintf(state->methods[index], sizeof(state->methods[index]), "%s",
                   method);
    (void)snprintf(state->paths[index], sizeof(state->paths[index]), "%s",
                   path);
    size_t copied = body_len < sizeof(state->bodies[index]) - 1U
                        ? body_len
                        : sizeof(state->bodies[index]) - 1U;
    if (copied != 0U) {
      memcpy(state->bodies[index], body, copied);
    }
    state->bodies[index][copied] = '\0';
  }
  (void)content_type;
  if (index == 0U) {
    atomic_store_explicit(&state->entered, 1U, memory_order_release);
    while (atomic_load_explicit(&state->release, memory_order_acquire) == 0U) {
      transition_sleep_ms(1U);
    }
  }
  static const char response_body[] = "{\"id\":\"42\"}";
  dcc_rest_response_t response = {
      .size = sizeof(response),
      .status = 200U,
      .error = DCC_OK,
      .body = response_body,
      .body_len = sizeof(response_body) - 1U,
  };
  callback(client, &response, callback_user_data);
  return DCC_OK;
}

static void transition_legacy_callback(dcc_client_t *client,
                                       const dcc_rest_response_t *response,
                                       void *user_data) {
  transition_state_t *state = (transition_state_t *)user_data;
  (void)client;
  (void)response;
  state->callback_thread = pthread_self();
  atomic_fetch_add_explicit(&state->callback_calls, 1U, memory_order_acq_rel);
}

static dcc_status_t transition_load_old(dcc_managed_message_ref_t *out,
                                        void *user_data) {
  (void)user_data;
  out->channel_id = 610U;
  out->message_id = 611U;
  return DCC_OK;
}

static void
transition_managed_callback(dcc_client_t *client,
                            const dcc_managed_message_publish_result_t *result,
                            void *user_data) {
  transition_state_t *state = (transition_state_t *)user_data;
  (void)client;
  (void)result;
  state->callback_thread = pthread_self();
  atomic_fetch_add_explicit(&state->callback_calls, 1U, memory_order_acq_rel);
}

static void *transition_submit(void *user_data) {
  transition_state_t *state = (transition_state_t *)user_data;
  state->submit_thread = pthread_self();
  switch (state->kind) {
  case TRANSITION_CALL_MESSAGE:
    state->call_status = dcc_app_send_json(&state->app, 600U, state->payload,
                                           transition_legacy_callback, state);
    break;
  case TRANSITION_CALL_WEBHOOK:
    state->call_status = dcc_app_execute_webhook(
        &state->app, 601U, "raw-token", "wait=true", state->payload,
        transition_legacy_callback, state);
    break;
  case TRANSITION_CALL_MANAGED:
    state->call_status = dcc_managed_message_publish_latest(
        state->client, &state->managed, transition_managed_callback, state);
    break;
  }
  atomic_store_explicit(&state->returned, 1U, memory_order_release);
  return NULL;
}

static int transition_one(dcc_client_t *client, transition_call_kind_t kind,
                          const char *original_body, const char *first_method,
                          const char *first_path, const char *second_method,
                          const char *second_path) {
  transition_state_t state;
  memset(&state, 0, sizeof(state));
  state.client = client;
  state.app.client = client;
  state.kind = kind;
  atomic_init(&state.entered, 0U);
  atomic_init(&state.release, 0U);
  atomic_init(&state.returned, 0U);
  atomic_init(&state.callback_calls, 0U);
  atomic_init(&state.request_count, 0U);
  state.payload = (char *)malloc(128U);
  if (state.payload == NULL) {
    return 1;
  }
  (void)snprintf(state.payload, 128U, "%s", original_body);
  state.message = (dcc_message_builder_t)DCC_MESSAGE_BUILDER_INIT;
  if (dcc_message_builder_set_content(&state.message, "managed-owned") !=
      DCC_OK) {
    free(state.payload);
    return 1;
  }
  state.managed = (dcc_managed_message_options_t){
      .size = sizeof(state.managed),
      .channel_id = 610U,
      .message = &state.message,
      .load = transition_load_old,
  };

  dcc_rest_set_interceptor(client, transition_intercept, &state);
  pthread_t submitter;
  if (pthread_create(&submitter, NULL, transition_submit, &state) != 0) {
    free(state.payload);
    return 1;
  }
  int entered = endpoint_wait_for_atomic(&state.entered, 1U, 1000U);
  int returned_before_release =
      entered && endpoint_wait_for_atomic(&state.returned, 1U, 100U);
  if (returned_before_release) {
    (void)snprintf(state.payload, 128U, "mutated-after-return");
    (void)dcc_message_builder_set_content(&state.message, "mutated-managed");
  }
  atomic_store_explicit(&state.release, 1U, memory_order_release);
  (void)pthread_join(submitter, NULL);
  (void)dcc_rest_async_wait(client, 3000U);

  unsigned expected_requests = second_method != NULL ? 2U : 1U;
  int ok =
      entered && returned_before_release && state.call_status == DCC_OK &&
      atomic_load_explicit(&state.request_count, memory_order_acquire) ==
          expected_requests &&
      atomic_load_explicit(&state.callback_calls, memory_order_acquire) == 1U &&
      !pthread_equal(state.submit_thread, state.intercept_thread) &&
      !pthread_equal(state.submit_thread, state.callback_thread) &&
      strcmp(state.methods[0], first_method) == 0 &&
      strcmp(state.paths[0], first_path) == 0 &&
      (kind == TRANSITION_CALL_MANAGED ||
       strstr(state.bodies[0], original_body) != NULL);
  if (ok && second_method != NULL) {
    ok = strcmp(state.methods[1], second_method) == 0 &&
         strcmp(state.paths[1], second_path) == 0 &&
         strstr(state.bodies[1], "managed-owned") != NULL;
  }
  if (!ok) {
    fprintf(stderr,
            "transition kind=%d entered=%d returned=%d status=%s requests=%u "
            "callbacks=%u "
            "method0=%s path0=%s body0=%s method1=%s path1=%s body1=%s\n",
            (int)kind, entered, returned_before_release,
            dcc_status_string(state.call_status),
            atomic_load_explicit(&state.request_count, memory_order_acquire),
            atomic_load_explicit(&state.callback_calls, memory_order_acquire),
            state.methods[0], state.paths[0], state.bodies[0], state.methods[1],
            state.paths[1], state.bodies[1]);
  }
  free(state.payload);
  return ok ? 0 : 1;
}

int endpoint_transition_submission_contract(dcc_client_t *client,
                                            endpoint_capture_t *capture) {
  if (transition_one(client, TRANSITION_CALL_MESSAGE, "raw-message-owned",
                     "POST", "/channels/600/messages", NULL, NULL) != 0 ||
      transition_one(client, TRANSITION_CALL_WEBHOOK, "raw-webhook-owned",
                     "POST", "/webhooks/601/raw-token?wait=true", NULL,
                     NULL) != 0 ||
      transition_one(client, TRANSITION_CALL_MANAGED, "", "DELETE",
                     "/channels/610/messages/611", "POST",
                     "/channels/610/messages") != 0) {
    dcc_rest_set_interceptor(client, NULL, NULL);
    return 1;
  }
  dcc_rest_set_interceptor(client, NULL, NULL);
  endpoint_capture_reset(capture, 0U);
  atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
  return 0;
}

int endpoint_transition_rejection_contract(dcc_client_t *client) {
  transition_state_t state;
  memset(&state, 0, sizeof(state));
  state.app.client = client;
  atomic_init(&state.callback_calls, 0U);
  dcc_status_t status = dcc_app_send_json(&state.app, 600U, "{}",
                                          transition_legacy_callback, &state);
  transition_sleep_ms(25U);
  dcc_status_t null_client = dcc_endpoint_submit_legacy_raw(
      NULL, DCC_REST_POST, "/local-reject", "wait=true", "application/json",
      "{}", 2U, transition_legacy_callback, &state);
  dcc_status_t invalid_method = dcc_endpoint_submit_legacy_raw(
      client, (dcc_rest_method_t)99, "/local-reject", "wait=true",
      "application/json", "{}", 2U, transition_legacy_callback, &state);
  dcc_status_t invalid_body_pair = dcc_endpoint_submit_legacy_raw(
      client, DCC_REST_POST, "/local-reject", "wait=true", NULL, "{}", 2U,
      transition_legacy_callback, &state);
  if (status != DCC_ERR_STATE || null_client != DCC_ERR_INVALID_ARG ||
      invalid_method != DCC_ERR_INVALID_ARG ||
      invalid_body_pair != DCC_ERR_INVALID_ARG ||
      atomic_load_explicit(&state.callback_calls, memory_order_acquire) != 0U) {
    fprintf(stderr,
            "transition local rejection status=%s null=%s method=%s body=%s "
            "callbacks=%u\n",
            dcc_status_string(status), dcc_status_string(null_client),
            dcc_status_string(invalid_method),
            dcc_status_string(invalid_body_pair),
            atomic_load_explicit(&state.callback_calls, memory_order_acquire));
    return 1;
  }
  return 0;
}

#else

int endpoint_transition_submission_contract(dcc_client_t *client,
                                            endpoint_capture_t *capture) {
  (void)client;
  (void)capture;
  return 0;
}

int endpoint_transition_rejection_contract(dcc_client_t *client) {
  (void)client;
  return 0;
}

#endif
