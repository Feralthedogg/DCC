#include "internal/app/dcc_app_interaction_queue_internal.h"
#include "internal/app/dcc_app_interaction_registry_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_windows_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"
#include "internal/rest/dcc_rest_submit_internal.h"

#include <dcc/rest/request.h>

#if !defined(_WIN32)
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dcc_app_interaction_action {
  struct dcc_app_interaction_action *next;
  struct dcc_app_interaction_queue *queue;
  char *path;
  char *body;
  const char *content_type;
  size_t body_len;
  dcc_rest_method_t method;
  uint8_t kind;
  uint8_t terminal_delivered;
  uint8_t is_initial;
  uint8_t terminal_original_exists;
  dcc_interaction_flow_state_t queued_state;
  dcc_interaction_flow_state_t terminal_state;
  size_t reserved_bytes;
  dcc_rest_result_fn callback;
  void *user_data;
} dcc_app_interaction_action_t;

#if defined(_WIN32)
typedef CRITICAL_SECTION dcc_app_interaction_mutex_t;
#else
typedef pthread_mutex_t dcc_app_interaction_mutex_t;
#endif

struct dcc_app_interaction_queue {
  dcc_app_interaction_mutex_t mutex;
  dcc_client_t *client;
  dcc_interaction_flow_t *flow;
  dcc_app_interaction_action_t *head;
  dcc_app_interaction_action_t *tail;
  uint8_t active;
  uint8_t failed;
  uint8_t owner_destroyed;
  uint8_t initial_admitted;
  uint8_t committed_original_exists;
  uint8_t projected_original_exists;
  dcc_interaction_flow_state_t committed_state;
  dcc_interaction_flow_state_t projected_state;
  size_t references;
  size_t action_count;
  size_t reserved_bytes;
  size_t identity_charge;
};

static int
dcc_app_interaction_mutex_init(dcc_app_interaction_mutex_t *mutex) {
#if defined(_WIN32)
  InitializeCriticalSection(mutex);
  return 0;
#else
  return pthread_mutex_init(mutex, NULL);
#endif
}

static void
dcc_app_interaction_mutex_destroy(dcc_app_interaction_mutex_t *mutex) {
#if defined(_WIN32)
  DeleteCriticalSection(mutex);
#else
  (void)pthread_mutex_destroy(mutex);
#endif
}

static void
dcc_app_interaction_mutex_lock(dcc_app_interaction_mutex_t *mutex) {
#if defined(_WIN32)
  EnterCriticalSection(mutex);
#else
  (void)pthread_mutex_lock(mutex);
#endif
}

static void
dcc_app_interaction_mutex_unlock(dcc_app_interaction_mutex_t *mutex) {
#if defined(_WIN32)
  LeaveCriticalSection(mutex);
#else
  (void)pthread_mutex_unlock(mutex);
#endif
}

static void dcc_app_resource_lock(dcc_client_t *client) {
  while (atomic_flag_test_and_set_explicit(&client->rest_resource_lock,
                                           memory_order_acquire)) {
  }
}

static void dcc_app_resource_unlock(dcc_client_t *client) {
  atomic_flag_clear_explicit(&client->rest_resource_lock, memory_order_release);
}

static void dcc_app_counter_increment(uint64_t *value) {
  if (*value != UINT64_MAX)
    ++*value;
}

static dcc_status_t dcc_app_interaction_action_reserve(
    dcc_app_interaction_queue_t *queue, size_t bytes) {
  dcc_client_t *client = queue->client;
  dcc_app_resource_lock(client);
  if (queue->action_count >= client->interaction_max_actions_per_queue) {
    dcc_app_counter_increment(
        &client->rest_runtime_stats.interaction_rejected_actions_per_queue);
    dcc_app_counter_increment(&client->rest_runtime_stats.admission_rejections);
    dcc_app_resource_unlock(client);
    return DCC_ERR_RESOURCE_LIMIT;
  }
  if (bytes > client->interaction_max_reserved_bytes_per_queue -
                  queue->reserved_bytes) {
    dcc_app_counter_increment(
        &client->rest_runtime_stats.interaction_rejected_bytes_per_queue);
    dcc_app_counter_increment(&client->rest_runtime_stats.admission_rejections);
    dcc_app_resource_unlock(client);
    return DCC_ERR_RESOURCE_LIMIT;
  }
  if (bytes > client->interaction_max_reserved_bytes_total -
                  (size_t)client->rest_runtime_stats.interaction_reserved_bytes) {
    dcc_app_counter_increment(
        &client->rest_runtime_stats.interaction_rejected_bytes_total);
    dcc_app_counter_increment(&client->rest_runtime_stats.admission_rejections);
    dcc_app_resource_unlock(client);
    return DCC_ERR_RESOURCE_LIMIT;
  }
  ++queue->action_count;
  queue->reserved_bytes += bytes;
  client->rest_runtime_stats.interaction_reserved_bytes += bytes;
  if (client->rest_runtime_stats.interaction_reserved_bytes >
      client->rest_runtime_stats.interaction_reserved_bytes_high_water)
    client->rest_runtime_stats.interaction_reserved_bytes_high_water =
        client->rest_runtime_stats.interaction_reserved_bytes;
  ++client->rest_runtime_stats.interaction_actions;
  if (client->rest_runtime_stats.interaction_actions >
      client->rest_runtime_stats.interaction_actions_high_water)
    client->rest_runtime_stats.interaction_actions_high_water =
        client->rest_runtime_stats.interaction_actions;
  dcc_app_resource_unlock(client);
  return DCC_OK;
}

static void dcc_app_interaction_action_release(
    dcc_app_interaction_queue_t *queue, size_t bytes) {
  dcc_app_resource_lock(queue->client);
  if (queue->action_count != 0U)
    --queue->action_count;
  if (queue->reserved_bytes >= bytes)
    queue->reserved_bytes -= bytes;
  if (queue->client->rest_runtime_stats.interaction_reserved_bytes >= bytes)
    queue->client->rest_runtime_stats.interaction_reserved_bytes -= bytes;
  if (queue->client->rest_runtime_stats.interaction_actions != 0U)
    --queue->client->rest_runtime_stats.interaction_actions;
  dcc_app_resource_unlock(queue->client);
}

static void
dcc_app_interaction_action_free(dcc_app_interaction_action_t *action) {
  if (action != NULL) {
    if (action->path != NULL)
      dcc_endpoint_secure_zero(action->path, strlen(action->path) + 1U);
    free(action->path);
    free(action->body);
    free(action);
  }
}

static dcc_status_t dcc_app_interaction_action_measure(
    dcc_app_interaction_action_t *action) {
  if (action == NULL || action->path == NULL)
    return DCC_ERR_INVALID_ARG;
  size_t charge = sizeof(*action);
  size_t path_bytes = strlen(action->path) + 1U;
  size_t body_bytes = action->body != NULL ? action->body_len + 1U : 0U;
  if (path_bytes > SIZE_MAX - charge ||
      body_bytes > SIZE_MAX - charge - path_bytes)
    return DCC_ERR_RESOURCE_LIMIT;
  action->reserved_bytes = charge + path_bytes + body_bytes;
  return DCC_OK;
}

dcc_app_interaction_queue_t *
dcc_app_interaction_queue_create(dcc_client_t *client,
                                 size_t identity_charge,
                                 dcc_status_t *out_status) {
  if (out_status != NULL)
    *out_status = DCC_ERR_INVALID_ARG;
  if (client == NULL || identity_charge == 0U || out_status == NULL)
    return NULL;
  dcc_app_resource_lock(client);
  if (identity_charge > client->interaction_max_reserved_bytes_per_queue ||
      identity_charge > client->interaction_max_reserved_bytes_total -
                            (size_t)client->rest_runtime_stats
                                .interaction_reserved_bytes) {
    dcc_app_counter_increment(
        identity_charge > client->interaction_max_reserved_bytes_per_queue
            ? &client->rest_runtime_stats.interaction_rejected_bytes_per_queue
            : &client->rest_runtime_stats.interaction_rejected_bytes_total);
    dcc_app_counter_increment(&client->rest_runtime_stats.admission_rejections);
    dcc_app_resource_unlock(client);
    *out_status = DCC_ERR_RESOURCE_LIMIT;
    return NULL;
  }
  client->rest_runtime_stats.interaction_reserved_bytes += identity_charge;
  if (client->rest_runtime_stats.interaction_reserved_bytes >
      client->rest_runtime_stats.interaction_reserved_bytes_high_water)
    client->rest_runtime_stats.interaction_reserved_bytes_high_water =
        client->rest_runtime_stats.interaction_reserved_bytes;
  dcc_app_resource_unlock(client);
  dcc_app_interaction_queue_t *queue = calloc(1U, sizeof(*queue));
  if (queue == NULL) {
    dcc_app_resource_lock(client);
    client->rest_runtime_stats.interaction_reserved_bytes -= identity_charge;
    dcc_app_resource_unlock(client);
    *out_status = DCC_ERR_NOMEM;
    return NULL;
  }
  if (dcc_app_interaction_mutex_init(&queue->mutex) != 0) {
    dcc_app_resource_lock(client);
    client->rest_runtime_stats.interaction_reserved_bytes -= identity_charge;
    dcc_app_resource_unlock(client);
    free(queue);
    *out_status = DCC_ERR_RUNTIME;
    return NULL;
  }
  queue->client = client;
  queue->committed_state = DCC_INTERACTION_FLOW_READY;
  queue->projected_state = DCC_INTERACTION_FLOW_READY;
  queue->references = 1U;
  queue->identity_charge = identity_charge;
  queue->reserved_bytes = identity_charge;
  *out_status = DCC_OK;
  return queue;
}

size_t dcc_app_interaction_queue_identity_charge(size_t token_len) {
  const size_t fixed = sizeof(dcc_app_interaction_queue_t) +
                       sizeof(dcc_interaction_flow_t) +
                       sizeof(dcc_interaction_t);
  return token_len < SIZE_MAX - fixed ? fixed + token_len + 1U : 0U;
}

void dcc_app_interaction_queue_attach_flow(dcc_app_interaction_queue_t *queue,
                                           dcc_interaction_flow_t *flow) {
  if (queue != NULL)
    queue->flow = flow;
}

dcc_interaction_flow_state_t
dcc_app_interaction_queue_state(dcc_app_interaction_queue_t *queue,
                                dcc_interaction_flow_state_t fallback) {
  if (queue == NULL)
    return fallback;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  dcc_interaction_flow_state_t state = queue->committed_state;
  if (queue->failed != 0U)
    state = DCC_INTERACTION_FLOW_FAILED;
  else if (queue->active != 0U && queue->head != NULL &&
           queue->head->terminal_delivered == 0U &&
           queue->head->queued_state != DCC_INTERACTION_FLOW_READY)
    state = queue->head->queued_state;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return state;
}

uint8_t dcc_app_interaction_queue_initial_admitted(
    dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return 0U;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  uint8_t admitted = queue->initial_admitted;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return admitted;
}

uint8_t dcc_app_interaction_queue_can_followup(
    dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return 0U;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  uint8_t allowed = queue->failed == 0U && queue->initial_admitted != 0U;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return allowed;
}

uint8_t dcc_app_interaction_queue_can_edit_original(
    dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return 0U;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  uint8_t allowed = queue->failed == 0U &&
                    queue->projected_original_exists != 0U;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return allowed;
}

void dcc_app_interaction_queue_mark(
    dcc_app_interaction_queue_t *queue,
    dcc_interaction_flow_state_t state,
    dcc_status_t status,
    uint8_t initial) {
  if (queue == NULL)
    return;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  dcc_interaction_flow_state_t next =
      status == DCC_OK ? state : DCC_INTERACTION_FLOW_FAILED;
  queue->committed_state = next;
  queue->projected_state = next;
  if (status != DCC_OK) {
    queue->failed = 1U;
  } else {
    if (initial != 0U)
      queue->initial_admitted = 1U;
    if (state == DCC_INTERACTION_FLOW_DEFERRED ||
        state == DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        state == DCC_INTERACTION_FLOW_DEFERRED_UPDATE ||
        state == DCC_INTERACTION_FLOW_REPLIED ||
        state == DCC_INTERACTION_FLOW_ORIGINAL_EDITED) {
      queue->committed_original_exists = 1U;
      queue->projected_original_exists = 1U;
    }
  }
  dcc_app_interaction_mutex_unlock(&queue->mutex);
}

static void
dcc_app_interaction_queue_finalize(dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return;
  dcc_app_interaction_action_t *action = queue->head;
  while (action != NULL) {
    dcc_app_interaction_action_t *next = action->next;
    dcc_app_interaction_action_release(queue, action->reserved_bytes);
    dcc_app_interaction_action_free(action);
    action = next;
  }
  dcc_interaction_flow_t *flow = queue->flow;
  dcc_app_interaction_registry_retire(queue->client, flow);
  dcc_app_resource_lock(queue->client);
  if (queue->client->rest_runtime_stats.interaction_reserved_bytes >=
      queue->identity_charge)
    queue->client->rest_runtime_stats.interaction_reserved_bytes -=
        queue->identity_charge;
  dcc_app_resource_unlock(queue->client);
  dcc_app_interaction_mutex_destroy(&queue->mutex);
  free(queue);
  dcc_flow_free_storage(flow);
}

void dcc_app_interaction_queue_release_owner(
    dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (queue->owner_destroyed == 0U) {
    queue->owner_destroyed = 1U;
    if (queue->references != 0U)
      --queue->references;
  }
  int finalize = queue->references == 0U && queue->active == 0U &&
                 queue->head == NULL;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  if (finalize)
    dcc_app_interaction_queue_finalize(queue);
}

dcc_status_t dcc_app_interaction_queue_retain_timer(
    dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  dcc_status_t status = DCC_OK;
  if (queue->owner_destroyed != 0U || queue->references == SIZE_MAX) {
    status = DCC_ERR_STATE;
  } else {
    ++queue->references;
  }
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return status;
}

void dcc_app_interaction_queue_release_timer(
    dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (queue->references != 0U)
    --queue->references;
  int finalize = queue->references == 0U && queue->active == 0U &&
                 queue->head == NULL;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  if (finalize)
    dcc_app_interaction_queue_finalize(queue);
}

static dcc_status_t
dcc_app_interaction_queue_submit_head(dcc_app_interaction_queue_t *queue);
static void dcc_app_interaction_queue_post_result(
    dcc_client_t *client, const dcc_rest_result_t *result, void *user_data);

static void dcc_app_interaction_queue_result(dcc_client_t *client,
                                             const dcc_rest_result_t *result,
                                             void *user_data) {
  dcc_app_interaction_action_t *action = user_data;
  dcc_app_interaction_queue_t *queue = action->queue;
  dcc_status_t status =
      result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (status != DCC_OK) {
    queue->failed = 1U;
    queue->committed_state = DCC_INTERACTION_FLOW_FAILED;
    queue->projected_state = DCC_INTERACTION_FLOW_FAILED;
    if (queue->flow != NULL)
      queue->flow->state = DCC_INTERACTION_FLOW_FAILED;
  } else {
    queue->committed_state = action->terminal_state;
    queue->committed_original_exists = action->terminal_original_exists;
    if (queue->flow != NULL)
      queue->flow->state = action->terminal_state;
  }
  action->terminal_delivered = 1U;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  if (action->callback != NULL)
    action->callback(client, result, action->user_data);
}

static void dcc_app_interaction_queue_deliver_synthetic(
    dcc_client_t *client, dcc_app_interaction_queue_t *queue,
    dcc_app_interaction_action_t *actions, dcc_status_t first_status) {
  dcc_status_t status = first_status;
  while (actions != NULL) {
    dcc_app_interaction_action_t *next = actions->next;
    dcc_rest_result_t synthetic = {
        .size = sizeof(synthetic),
        .version = DCC_REST_RESULT_VERSION,
        .transport_status = status,
    };
    dcc_rest_deliver_terminal_result(
        client, "dcc_interaction_queue", &synthetic, status, NULL, NULL,
        actions->callback, actions->user_data);
    dcc_app_interaction_mutex_lock(&queue->mutex);
    dcc_app_interaction_action_release(queue, actions->reserved_bytes);
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(actions);
    actions = next;
    status = DCC_ERR_STATE;
  }
}

static void dcc_app_interaction_queue_post_result(
    dcc_client_t *client, const dcc_rest_result_t *result, void *user_data) {
  dcc_app_interaction_action_t *action = user_data;
  if (action == NULL || action->queue == NULL)
    return;
  dcc_app_interaction_queue_t *queue = action->queue;
  dcc_status_t terminal_status =
      result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME;
  dcc_app_interaction_action_t *dependent = NULL;
  dcc_status_t dependent_status = DCC_ERR_STATE;

  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (queue->head == action) {
    queue->head = action->next;
    if (queue->head == NULL)
      queue->tail = NULL;
  }
  dcc_app_interaction_action_release(queue, action->reserved_bytes);
  queue->active = 0U;
  if (terminal_status != DCC_OK) {
    dependent = queue->head;
    queue->head = NULL;
    queue->tail = NULL;
    if (dependent != NULL)
      queue->active = 1U;
    dependent_status = terminal_status == DCC_ERR_CANCELED
                           ? DCC_ERR_CANCELED
                           : DCC_ERR_STATE;
  } else if (queue->head != NULL && queue->failed == 0U) {
    dcc_status_t admission = dcc_app_interaction_queue_submit_head(queue);
    if (admission != DCC_OK) {
      queue->failed = 1U;
      if (queue->flow != NULL)
        queue->flow->state = DCC_INTERACTION_FLOW_FAILED;
      dependent = queue->head;
      queue->head = NULL;
      queue->tail = NULL;
      if (dependent != NULL)
        queue->active = 1U;
      dependent_status = admission;
    }
  }
  dcc_app_interaction_mutex_unlock(&queue->mutex);

  dcc_app_interaction_action_free(action);
  dcc_app_interaction_queue_deliver_synthetic(
      client, queue, dependent, dependent_status);
  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (dependent != NULL)
    queue->active = 0U;
  int finalize = queue->references == 0U && queue->active == 0U &&
                 queue->head == NULL;
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  if (finalize)
    dcc_app_interaction_queue_finalize(queue);
}

static dcc_status_t
dcc_app_interaction_queue_submit_head(dcc_app_interaction_queue_t *queue) {
  dcc_app_interaction_action_t *action = queue->head;
  if (action == NULL || queue->active != 0U)
    return DCC_OK;
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = dcc_app_interaction_queue_result;
  options.user_data = action;
  dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
  description.method = action->method;
  description.path = action->path;
  description.content_type = action->content_type;
  description.body = action->body;
  description.body_len = action->body_len;
  description.options = &options;
  queue->active = 1U;
  dcc_status_t status = dcc_rest_submit_operation_with_post_hook(
      queue->client, &description, "dcc_interaction_queue", 1U,
      dcc_app_interaction_queue_post_result, action, NULL);
  if (status != DCC_OK)
    queue->active = 0U;
  return status;
}

static dcc_status_t
dcc_app_interaction_queue_path(const dcc_interaction_t *interaction,
                               uint8_t kind, char **out_path) {
  *out_path = NULL;
  if (interaction == NULL || interaction->id == 0U ||
      interaction->application_id == 0U || interaction->token == NULL ||
      interaction->token[0] == '\0')
    return DCC_ERR_INVALID_ARG;
  char *token = NULL;
  dcc_status_t status =
      dcc_rest_escape_path_segment(interaction->token, &token);
  int length = -1;
  if (status == DCC_OK) {
    const char *format =
        kind == 0U ? "/interactions/%llu/%s/callback"
                   : (kind == 1U ? "/webhooks/%llu/%s/messages/@original"
                                 : "/webhooks/%llu/%s");
    dcc_snowflake_t id =
        kind == 0U ? interaction->id : interaction->application_id;
    length = snprintf(NULL, 0, format, (unsigned long long)id, token);
    if (length >= 0) {
      *out_path = malloc((size_t)length + 1U);
      if (*out_path == NULL)
        status = DCC_ERR_NOMEM;
      else
        snprintf(*out_path, (size_t)length + 1U, format, (unsigned long long)id,
                 token);
    } else {
      status = DCC_ERR_INVALID_ARG;
    }
  }
  free(token);
  return status;
}

static dcc_status_t dcc_app_interaction_queue_enqueue(
    dcc_interaction_flow_t *flow, const dcc_message_builder_t *message,
    dcc_rest_result_fn callback, void *user_data, uint8_t kind,
    dcc_interaction_flow_state_t projected_state) {
  if (flow == NULL || flow->queue == NULL || flow->interaction == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_app_interaction_queue_t *queue = flow->queue;
  dcc_app_interaction_action_t *action = calloc(1U, sizeof(*action));
  if (action == NULL)
    return DCC_ERR_NOMEM;
  action->queue = queue;
  action->content_type = "application/json";
  action->callback = callback;
  action->user_data = user_data;

  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (queue->failed != 0U || queue->owner_destroyed != 0U) {
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return DCC_ERR_STATE;
  }
  if (kind == UINT8_MAX) {
    if (queue->initial_admitted == 0U) {
      kind = 0U;
      projected_state = DCC_INTERACTION_FLOW_REPLIED;
    } else if ((queue->projected_state == DCC_INTERACTION_FLOW_DEFERRED ||
                queue->projected_state ==
                    DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
                queue->projected_state ==
                    DCC_INTERACTION_FLOW_DEFERRED_UPDATE) &&
               queue->projected_original_exists != 0U) {
      kind = 1U;
      projected_state = DCC_INTERACTION_FLOW_ORIGINAL_EDITED;
    } else {
      kind = 2U;
      projected_state = DCC_INTERACTION_FLOW_FOLLOWED_UP;
    }
  }
  const uint8_t is_initial = kind == 0U || kind >= 3U;
  if ((is_initial != 0U && queue->initial_admitted != 0U) ||
      (kind == 1U && queue->projected_original_exists == 0U) ||
      (kind == 2U && queue->initial_admitted == 0U) ||
      (message == NULL && kind < 3U)) {
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return message == NULL && kind < 3U ? DCC_ERR_INVALID_ARG : DCC_ERR_STATE;
  }
  action->method = kind == 1U ? DCC_REST_PATCH : DCC_REST_POST;
  action->kind = kind;
  action->is_initial = is_initial;
  action->queued_state = kind >= 3U
                             ? DCC_INTERACTION_FLOW_DEFERRED_QUEUED
                             : (kind == 0U
                                    ? DCC_INTERACTION_FLOW_INITIAL_QUEUED
                                    : DCC_INTERACTION_FLOW_READY);
  action->terminal_state = projected_state;
  action->terminal_original_exists =
      is_initial != 0U ? 1U : queue->projected_original_exists;
  dcc_status_t status = dcc_app_interaction_queue_path(
      flow->interaction, kind >= 3U ? 0U : kind, &action->path);
  dcc_endpoint_body_t built = {0};
  if (status == DCC_OK && (kind == 1U || kind == 2U)) {
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, message);
    status = dcc_endpoint_build_message_body(&payload, &built);
  } else if (status == DCC_OK) {
    dcc_rest_interaction_response_t response =
        DCC_REST_INTERACTION_RESPONSE_INIT;
    dcc_message_builder_t ephemeral = DCC_MESSAGE_BUILDER_INIT;
    if (kind == 0U) {
      status = dcc_rest_interaction_response_set_message(&response, message);
    } else if (kind == 3U) {
      status = dcc_rest_interaction_response_set_deferred_message(&response,
                                                                  NULL);
    } else if (kind == 4U) {
      status = dcc_message_builder_set_flags(
          &ephemeral, DCC_INTERACTION_FLOW_FLAG_EPHEMERAL);
      if (status == DCC_OK)
        status = dcc_rest_interaction_response_set_deferred_message(
            &response, &ephemeral);
    } else {
      status = dcc_rest_interaction_response_set_deferred_update(&response);
    }
    if (status == DCC_OK)
      status = dcc_endpoint_build_interaction_body(&response, &built);
  }
  if (status == DCC_OK) {
    action->body = built.data;
    action->body_len = built.len;
    action->content_type = built.content_type;
    built = (dcc_endpoint_body_t){0};
  }
  dcc_endpoint_body_deinit(&built);
  if (status != DCC_OK) {
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return status;
  }

  status = dcc_app_interaction_action_measure(action);
  if (status == DCC_OK)
    status = dcc_app_interaction_action_reserve(queue,
                                                action->reserved_bytes);
  if (status != DCC_OK) {
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return status;
  }
  dcc_interaction_flow_state_t previous = queue->projected_state;
  uint8_t previous_original_exists = queue->projected_original_exists;
  if (queue->tail != NULL)
    queue->tail->next = action;
  else
    queue->head = action;
  queue->tail = action;
  queue->flow = flow;
  queue->projected_state = projected_state;
  queue->projected_original_exists = action->terminal_original_exists;
  flow->state = projected_state;
  status = dcc_app_interaction_queue_submit_head(queue);
  if (status != DCC_OK) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->projected_state = previous;
    queue->projected_original_exists = previous_original_exists;
    flow->state = previous;
    dcc_app_interaction_action_release(queue, action->reserved_bytes);
    dcc_app_interaction_action_free(action);
  } else if (is_initial != 0U) {
    queue->initial_admitted = 1U;
    flow->response_flags |= DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED;
  }
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return status;
}

dcc_status_t dcc_app_interaction_queue_initial_response(
    dcc_interaction_flow_t *flow,
    const dcc_rest_interaction_response_t *response,
    dcc_interaction_flow_state_t terminal_state,
    dcc_rest_result_fn callback,
    void *user_data) {
  if (flow == NULL || flow->queue == NULL || flow->interaction == NULL ||
      response == NULL)
    return DCC_ERR_INVALID_ARG;
  if (dcc_flow_initial_sent(flow))
    return DCC_ERR_STATE;

  dcc_app_interaction_action_t *action = calloc(1U, sizeof(*action));
  if (action == NULL)
    return DCC_ERR_NOMEM;
  action->queue = flow->queue;
  action->method = DCC_REST_POST;
  action->kind = 6U;
  action->is_initial = 1U;
  action->queued_state =
      response->type == DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE ||
              response->type == DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE
          ? DCC_INTERACTION_FLOW_DEFERRED_QUEUED
          : DCC_INTERACTION_FLOW_INITIAL_QUEUED;
  action->terminal_state = terminal_state;
  action->terminal_original_exists =
      response->type == DCC_INTERACTION_RESPONSE_AUTOCOMPLETE ||
              response->type == DCC_INTERACTION_RESPONSE_MODAL
          ? 0U
          : 1U;
  action->callback = callback;
  action->user_data = user_data;

  dcc_endpoint_body_t body = {0};
  dcc_status_t status = dcc_endpoint_build_interaction_body(response, &body);
  if (status == DCC_OK)
    status = dcc_app_interaction_queue_path(flow->interaction, 0U,
                                            &action->path);
  if (status == DCC_OK) {
    action->body = body.data;
    action->body_len = body.len;
    action->content_type = body.content_type;
    body = (dcc_endpoint_body_t){0};
  }
  dcc_endpoint_body_deinit(&body);
  if (status != DCC_OK) {
    dcc_app_interaction_action_free(action);
    return status;
  }

  dcc_app_interaction_queue_t *queue = flow->queue;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (queue->failed != 0U || queue->owner_destroyed != 0U ||
      queue->initial_admitted != 0U) {
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return DCC_ERR_STATE;
  }
  status = dcc_app_interaction_action_measure(action);
  if (status == DCC_OK)
    status = dcc_app_interaction_action_reserve(queue,
                                                action->reserved_bytes);
  if (status == DCC_OK) {
    dcc_interaction_flow_state_t previous = queue->projected_state;
    uint8_t previous_original_exists = queue->projected_original_exists;
    if (queue->tail != NULL)
      queue->tail->next = action;
    else
      queue->head = action;
    queue->tail = action;
    queue->flow = flow;
    queue->projected_state = terminal_state;
    queue->projected_original_exists = action->terminal_original_exists;
    flow->state = terminal_state;
    status = dcc_app_interaction_queue_submit_head(queue);
    if (status == DCC_OK) {
      queue->initial_admitted = 1U;
      flow->response_flags |= DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED;
    } else {
      queue->head = NULL;
      queue->tail = NULL;
      queue->projected_state = previous;
      queue->projected_original_exists = previous_original_exists;
      flow->state = previous;
      dcc_app_interaction_action_release(queue, action->reserved_bytes);
      dcc_app_interaction_action_free(action);
    }
  }
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return status;
}

dcc_status_t dcc_app_interaction_queue_delete_original(
    dcc_interaction_flow_t *flow,
    dcc_rest_result_fn callback,
    void *user_data) {
  if (flow == NULL || flow->queue == NULL || flow->interaction == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_app_interaction_action_t *action = calloc(1U, sizeof(*action));
  if (action == NULL)
    return DCC_ERR_NOMEM;
  action->queue = flow->queue;
  action->method = DCC_REST_DELETE;
  action->kind = 7U;
  action->callback = callback;
  action->user_data = user_data;

  dcc_app_interaction_queue_t *queue = flow->queue;
  dcc_app_interaction_mutex_lock(&queue->mutex);
  if (queue->failed != 0U || queue->owner_destroyed != 0U ||
      queue->projected_original_exists == 0U) {
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return DCC_ERR_STATE;
  }
  dcc_status_t status =
      dcc_app_interaction_queue_path(flow->interaction, 1U, &action->path);
  if (status == DCC_OK)
    status = dcc_app_interaction_action_measure(action);
  if (status == DCC_OK)
    status = dcc_app_interaction_action_reserve(queue,
                                                action->reserved_bytes);
  if (status != DCC_OK) {
    dcc_app_interaction_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return status;
  }
  action->terminal_state = queue->projected_state;
  action->terminal_original_exists = 0U;
  if (queue->tail != NULL)
    queue->tail->next = action;
  else
    queue->head = action;
  queue->tail = action;
  queue->projected_original_exists = 0U;
  status = dcc_app_interaction_queue_submit_head(queue);
  if (status != DCC_OK) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->projected_original_exists = 1U;
    dcc_app_interaction_action_release(queue, action->reserved_bytes);
    dcc_app_interaction_action_free(action);
  }
  dcc_app_interaction_mutex_unlock(&queue->mutex);
  return status;
}

dcc_status_t
dcc_app_interaction_queue_reply(dcc_interaction_flow_t *flow,
                                const dcc_message_builder_t *message,
                                dcc_rest_result_fn callback, void *user_data) {
  if (flow == NULL)
    return DCC_ERR_INVALID_ARG;
  return dcc_app_interaction_queue_enqueue(
      flow, message, callback, user_data, UINT8_MAX,
      DCC_INTERACTION_FLOW_READY);
}

dcc_status_t dcc_app_interaction_queue_edit_original(
    dcc_interaction_flow_t *flow, const dcc_message_builder_t *message,
    dcc_rest_result_fn callback, void *user_data) {
  return dcc_app_interaction_queue_enqueue(
      flow, message, callback, user_data, 1U,
      DCC_INTERACTION_FLOW_ORIGINAL_EDITED);
}

dcc_status_t dcc_app_interaction_queue_followup(
    dcc_interaction_flow_t *flow, const dcc_message_builder_t *message,
    dcc_rest_result_fn callback, void *user_data) {
  return dcc_app_interaction_queue_enqueue(
      flow, message, callback, user_data, 2U, DCC_INTERACTION_FLOW_FOLLOWED_UP);
}

dcc_status_t dcc_app_interaction_queue_defer(dcc_interaction_flow_t *flow,
                                             uint8_t ephemeral, uint8_t update,
                                             dcc_rest_result_fn callback,
                                             void *user_data) {
  if (flow == NULL)
    return DCC_ERR_INVALID_ARG;
  uint8_t kind = update != 0U ? 5U : (ephemeral != 0U ? 4U : 3U);
  dcc_interaction_flow_state_t projected =
      update != 0U ? DCC_INTERACTION_FLOW_DEFERRED_UPDATE
                   : (ephemeral != 0U ? DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL
                                      : DCC_INTERACTION_FLOW_DEFERRED);
  return dcc_app_interaction_queue_enqueue(
      flow, NULL, callback, user_data, kind, projected);
}
