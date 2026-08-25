#include "internal/app/dcc_app_interaction_queue_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"

#include <dcc/rest/request.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dcc_app_interaction_action {
  struct dcc_app_interaction_action *next;
  struct dcc_app_interaction_queue *queue;
  char *path;
  char *body;
  dcc_rest_method_t method;
  uint8_t kind;
  dcc_rest_result_fn callback;
  void *user_data;
} dcc_app_interaction_action_t;

struct dcc_app_interaction_queue {
  pthread_mutex_t mutex;
  dcc_client_t *client;
  dcc_interaction_flow_t *flow;
  dcc_app_interaction_action_t *head;
  dcc_app_interaction_action_t *tail;
  uint8_t active;
  uint8_t failed;
  uint8_t owner_destroyed;
  size_t action_count;
};

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

static dcc_status_t
dcc_app_interaction_action_reserve(dcc_app_interaction_queue_t *queue) {
  dcc_client_t *client = queue->client;
  dcc_app_resource_lock(client);
  if (queue->action_count >= client->interaction_max_actions_per_queue) {
    dcc_app_counter_increment(
        &client->rest_runtime_stats.interaction_rejected_actions_per_queue);
    dcc_app_counter_increment(&client->rest_runtime_stats.admission_rejections);
    dcc_app_resource_unlock(client);
    return DCC_ERR_RESOURCE_LIMIT;
  }
  ++queue->action_count;
  ++client->rest_runtime_stats.interaction_actions;
  if (client->rest_runtime_stats.interaction_actions >
      client->rest_runtime_stats.interaction_actions_high_water)
    client->rest_runtime_stats.interaction_actions_high_water =
        client->rest_runtime_stats.interaction_actions;
  dcc_app_resource_unlock(client);
  return DCC_OK;
}

static void
dcc_app_interaction_action_release(dcc_app_interaction_queue_t *queue) {
  dcc_app_resource_lock(queue->client);
  if (queue->action_count != 0U)
    --queue->action_count;
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

dcc_app_interaction_queue_t *
dcc_app_interaction_queue_create(dcc_client_t *client) {
  dcc_app_interaction_queue_t *queue = calloc(1U, sizeof(*queue));
  if (queue == NULL)
    return NULL;
  if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
    free(queue);
    return NULL;
  }
  queue->client = client;
  return queue;
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
  pthread_mutex_lock(&queue->mutex);
  dcc_interaction_flow_state_t state = fallback;
  if (queue->failed != 0U)
    state = DCC_INTERACTION_FLOW_FAILED;
  else if (queue->active != 0U && queue->head != NULL &&
           queue->head->kind == 0U)
    state = DCC_INTERACTION_FLOW_INITIAL_QUEUED;
  else if (queue->active != 0U && queue->head != NULL &&
           queue->head->kind >= 3U)
    state = DCC_INTERACTION_FLOW_DEFERRED_QUEUED;
  pthread_mutex_unlock(&queue->mutex);
  return state;
}

static void
dcc_app_interaction_queue_finalize(dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return;
  dcc_app_interaction_action_t *action = queue->head;
  while (action != NULL) {
    dcc_app_interaction_action_t *next = action->next;
    dcc_app_interaction_action_free(action);
    action = next;
  }
  dcc_interaction_flow_t *flow = queue->flow;
  pthread_mutex_destroy(&queue->mutex);
  free(queue);
  dcc_flow_free_storage(flow);
}

void dcc_app_interaction_queue_release_owner(
    dcc_app_interaction_queue_t *queue) {
  if (queue == NULL)
    return;
  pthread_mutex_lock(&queue->mutex);
  queue->owner_destroyed = 1U;
  int finalize = queue->active == 0U && queue->head == NULL;
  pthread_mutex_unlock(&queue->mutex);
  if (finalize)
    dcc_app_interaction_queue_finalize(queue);
}

static dcc_status_t
dcc_app_interaction_queue_submit_head(dcc_app_interaction_queue_t *queue);

static void dcc_app_interaction_queue_result(dcc_client_t *client,
                                             const dcc_rest_result_t *result,
                                             void *user_data) {
  dcc_app_interaction_action_t *action = user_data;
  dcc_app_interaction_queue_t *queue = action->queue;
  dcc_status_t status =
      result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME;
  if (action->callback != NULL)
    action->callback(client, result, action->user_data);
  pthread_mutex_lock(&queue->mutex);
  dcc_app_interaction_action_t *dependent = NULL;
  if (queue->head == action) {
    queue->head = action->next;
    if (queue->head == NULL)
      queue->tail = NULL;
  }
  dcc_app_interaction_action_release(queue);
  queue->active = 0U;
  if (status != DCC_OK) {
    queue->failed = 1U;
    if (queue->flow != NULL)
      queue->flow->state = DCC_INTERACTION_FLOW_FAILED;
    dependent = queue->head;
    queue->head = NULL;
    queue->tail = NULL;
  }
  dcc_app_interaction_action_free(action);
  if (dependent != NULL) {
    pthread_mutex_unlock(&queue->mutex);
    dcc_status_t dependent_status =
        status == DCC_ERR_CANCELED ? DCC_ERR_CANCELED : DCC_ERR_STATE;
    while (dependent != NULL) {
      dcc_app_interaction_action_t *next = dependent->next;
      dcc_rest_result_t synthetic = {
          .size = sizeof(synthetic),
          .version = DCC_REST_RESULT_VERSION,
          .transport_status = dependent_status,
      };
      if (dependent->callback != NULL)
        dependent->callback(client, &synthetic, dependent->user_data);
      pthread_mutex_lock(&queue->mutex);
      dcc_app_interaction_action_release(queue);
      pthread_mutex_unlock(&queue->mutex);
      dcc_app_interaction_action_free(dependent);
      dependent = next;
    }
    pthread_mutex_lock(&queue->mutex);
  }
  if (queue->head != NULL && queue->failed == 0U)
    (void)dcc_app_interaction_queue_submit_head(queue);
  int finalize = queue->active == 0U && queue->head == NULL &&
                 queue->owner_destroyed != 0U;
  pthread_mutex_unlock(&queue->mutex);
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
  description.content_type = "application/json";
  description.body = action->body;
  description.body_len = strlen(action->body);
  description.options = &options;
  queue->active = 1U;
  dcc_status_t status = dcc_rest_submit(queue->client, &description, NULL);
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
  if (flow == NULL || (message == NULL && kind < 3U) || flow->queue == NULL ||
      flow->interaction == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_app_interaction_queue_t *queue = flow->queue;
  dcc_app_interaction_action_t *action = calloc(1U, sizeof(*action));
  if (action == NULL)
    return DCC_ERR_NOMEM;
  action->queue = queue;
  action->method = kind == 1U ? DCC_REST_PATCH : DCC_REST_POST;
  action->kind = kind;
  action->callback = callback;
  action->user_data = user_data;
  dcc_status_t status = dcc_app_interaction_queue_path(
      flow->interaction, kind >= 3U ? 0U : kind, &action->path);
  char *message_json = NULL;
  if (status == DCC_OK && kind < 3U)
    status = dcc_message_builder_build_json(message, &message_json);
  if (status == DCC_OK && kind == 0U) {
    size_t length = strlen(message_json) + sizeof("{\"type\":4,\"data\":}");
    action->body = malloc(length);
    if (action->body == NULL)
      status = DCC_ERR_NOMEM;
    else
      snprintf(action->body, length, "{\"type\":4,\"data\":%s}", message_json);
    dcc_message_builder_json_free(message_json);
    message_json = NULL;
  } else if (status == DCC_OK && kind < 3U) {
    action->body = message_json;
    message_json = NULL;
  } else if (status == DCC_OK) {
    const char *literal =
        kind == 3U ? "{\"type\":5}"
                   : (kind == 4U ? "{\"type\":5,\"data\":{\"flags\":64}}"
                                 : "{\"type\":6}");
    size_t length = strlen(literal);
    action->body = malloc(length + 1U);
    if (action->body == NULL)
      status = DCC_ERR_NOMEM;
    else
      memcpy(action->body, literal, length + 1U);
  }
  dcc_message_builder_json_free(message_json);
  if (status != DCC_OK) {
    dcc_app_interaction_action_free(action);
    return status;
  }

  pthread_mutex_lock(&queue->mutex);
  if (queue->failed != 0U) {
    pthread_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return DCC_ERR_STATE;
  }
  status = dcc_app_interaction_action_reserve(queue);
  if (status != DCC_OK) {
    pthread_mutex_unlock(&queue->mutex);
    dcc_app_interaction_action_free(action);
    return status;
  }
  dcc_interaction_flow_state_t previous = flow->state;
  if (queue->tail != NULL)
    queue->tail->next = action;
  else
    queue->head = action;
  queue->tail = action;
  queue->flow = flow;
  flow->state = projected_state;
  status = dcc_app_interaction_queue_submit_head(queue);
  if (status != DCC_OK) {
    queue->head = NULL;
    queue->tail = NULL;
    flow->state = previous;
    dcc_app_interaction_action_release(queue);
    dcc_app_interaction_action_free(action);
  }
  pthread_mutex_unlock(&queue->mutex);
  return status;
}

dcc_status_t
dcc_app_interaction_queue_reply(dcc_interaction_flow_t *flow,
                                const dcc_message_builder_t *message,
                                dcc_rest_result_fn callback, void *user_data) {
  if (flow == NULL)
    return DCC_ERR_INVALID_ARG;
  uint8_t kind = 2U;
  dcc_interaction_flow_state_t projected = DCC_INTERACTION_FLOW_FOLLOWED_UP;
  if (!dcc_flow_initial_sent(flow)) {
    kind = 0U;
    projected = DCC_INTERACTION_FLOW_REPLIED;
  } else if (flow->state == DCC_INTERACTION_FLOW_DEFERRED ||
             flow->state == DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
             flow->state == DCC_INTERACTION_FLOW_DEFERRED_UPDATE) {
    kind = 1U;
    projected = DCC_INTERACTION_FLOW_ORIGINAL_EDITED;
  }
  dcc_status_t status = dcc_app_interaction_queue_enqueue(
      flow, message, callback, user_data, kind, projected);
  if (status == DCC_OK && kind == 0U)
    flow->response_flags |= DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED;
  return status;
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
  if (flow == NULL || dcc_flow_initial_sent(flow))
    return flow == NULL ? DCC_ERR_INVALID_ARG : DCC_ERR_STATE;
  uint8_t kind = update != 0U ? 5U : (ephemeral != 0U ? 4U : 3U);
  dcc_interaction_flow_state_t projected =
      update != 0U ? DCC_INTERACTION_FLOW_DEFERRED_UPDATE
                   : (ephemeral != 0U ? DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL
                                      : DCC_INTERACTION_FLOW_DEFERRED);
  dcc_status_t status = dcc_app_interaction_queue_enqueue(
      flow, NULL, callback, user_data, kind, projected);
  if (status == DCC_OK)
    flow->response_flags |= DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED;
  return status;
}
