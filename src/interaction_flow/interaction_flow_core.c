#include "internal/app/dcc_app_interaction_queue_internal.h"
#include "internal/app/dcc_app_interaction_registry_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_flow_create(dcc_client_t *client,
                             const dcc_interaction_t *interaction,
                             dcc_interaction_flow_t **out_flow) {
  if (out_flow != NULL)
    *out_flow = NULL;
  if (client == NULL || interaction == NULL || out_flow == NULL ||
      interaction->id == 0U || interaction->application_id == 0U ||
      interaction->token == NULL || interaction->token[0] == '\0')
    return DCC_ERR_INVALID_ARG;
  dcc_interaction_flow_t *flow = calloc(1U, sizeof(*flow));
  dcc_interaction_t *copy = calloc(1U, sizeof(*copy));
  size_t token_len = strlen(interaction->token);
  char *token = malloc(token_len + 1U);
  if (flow == NULL || copy == NULL || token == NULL) {
    free(token);
    free(copy);
    free(flow);
    return DCC_ERR_NOMEM;
  }
  memcpy(token, interaction->token, token_len + 1U);
  copy->id = interaction->id;
  copy->application_id = interaction->application_id;
  copy->guild_id = interaction->guild_id;
  copy->channel_id = interaction->channel_id;
  copy->message_id = interaction->message_id;
  copy->user_id = interaction->user_id;
  copy->type = interaction->type;
  copy->context = interaction->context;
  copy->has_context = interaction->has_context;
  copy->token = token;
  dcc_flow_initialize_internal(flow, client, copy);
  if (flow->queue == NULL) {
    free(token);
    free(copy);
    free(flow);
    return DCC_ERR_NOMEM;
  }
  flow->owns_interaction = 1U;
  dcc_status_t status = dcc_app_interaction_registry_register(client, flow);
  if (status != DCC_OK) {
    dcc_flow_destroy(flow);
    return status;
  }
  *out_flow = flow;
  return DCC_OK;
}

void dcc_flow_destroy(dcc_interaction_flow_t *flow) {
  if (flow == NULL)
    return;
  dcc_app_interaction_queue_release_owner(flow->queue);
}

void dcc_flow_free_storage(dcc_interaction_flow_t *flow) {
  if (flow == NULL || flow->owns_interaction == 0U)
    return;
  dcc_app_interaction_registry_retire(flow->client, flow);
  if (flow->owns_interaction != 0U && flow->interaction != NULL) {
    dcc_interaction_t *interaction = (dcc_interaction_t *)flow->interaction;
    if (interaction->token != NULL) {
      size_t length = strlen(interaction->token);
      dcc_endpoint_secure_zero((void *)interaction->token, length + 1U);
      free((void *)interaction->token);
    }
    free(interaction);
  }
  free(flow);
}

void dcc_flow_initialize_internal(dcc_interaction_flow_t *flow,
                                  dcc_client_t *client,
                                  const dcc_interaction_t *interaction) {
  if (flow != NULL) {
    memset(flow, 0, sizeof(*flow));
    flow->size = sizeof(*flow);
    flow->client = client;
    flow->interaction = interaction;
    flow->state = DCC_INTERACTION_FLOW_READY;
    flow->queue = dcc_app_interaction_queue_create(client);
    flow->owns_interaction = 0U;
    dcc_app_interaction_queue_attach_flow(flow->queue, flow);
  }
}

dcc_status_t dcc_flow_set_started_at(dcc_interaction_flow_t *flow,
                                     uint64_t started_at_ms) {
  if (flow == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  flow->started_at_ms = started_at_ms;
  return DCC_OK;
}

dcc_interaction_flow_state_t
dcc_flow_state(const dcc_interaction_flow_t *flow) {
  return flow != NULL
             ? dcc_app_interaction_queue_state(flow->queue, flow->state)
             : DCC_INTERACTION_FLOW_FAILED;
}

static uint8_t dcc_flow_has_response_flags(const dcc_interaction_flow_t *flow) {
  if (flow == NULL) {
    return 0U;
  }
  const size_t field_offset = offsetof(dcc_interaction_flow_t, response_flags);
  return flow->size >= field_offset &&
                 sizeof(flow->response_flags) <= flow->size - field_offset
             ? 1U
             : 0U;
}

uint8_t dcc_flow_initial_sent(const dcc_interaction_flow_t *flow) {
  if (flow == NULL) {
    return 0U;
  }
  if (dcc_flow_has_response_flags(flow)) {
    return (flow->response_flags &
            DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED) != 0U
               ? 1U
               : 0U;
  }
  return flow->state != DCC_INTERACTION_FLOW_READY ? 1U : 0U;
}

uint8_t dcc_flow_initial_claimed(const dcc_interaction_flow_t *flow) {
  return dcc_flow_has_response_flags(flow) &&
                 (flow->response_flags &
                  DCC_INTERACTION_FLOW_RESPONSE_INITIAL_CLAIMED) != 0U
             ? 1U
             : 0U;
}

dcc_status_t dcc_flow_claim_initial(dcc_interaction_flow_t *flow) {
  if (flow == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  if (dcc_flow_has_response_flags(flow)) {
    const uint64_t unavailable =
        DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED |
        DCC_INTERACTION_FLOW_RESPONSE_INITIAL_CLAIMED;
    if ((flow->response_flags & unavailable) != 0U) {
      return DCC_ERR_STATE;
    }
    flow->response_flags |= DCC_INTERACTION_FLOW_RESPONSE_INITIAL_CLAIMED;
    return DCC_OK;
  }

  /* A historical 56-byte prefix has no response_flags storage. Its state is
   * the documented coarse fallback, so FAILED is used as an in-progress
   * sentinel until the owning initial response commits or rolls back. */
  if (flow->state != DCC_INTERACTION_FLOW_READY) {
    return DCC_ERR_STATE;
  }
  flow->state = DCC_INTERACTION_FLOW_FAILED;
  return DCC_OK;
}

dcc_status_t dcc_flow_mark_initial(dcc_interaction_flow_t *flow,
                                   dcc_interaction_flow_state_t state,
                                   dcc_status_t status) {
  if (flow == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  if (status == DCC_OK) {
    if (dcc_flow_has_response_flags(flow)) {
      flow->response_flags &= ~DCC_INTERACTION_FLOW_RESPONSE_INITIAL_CLAIMED;
      flow->response_flags |= DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED;
    }
    flow->state = state;
  } else {
    if (dcc_flow_has_response_flags(flow)) {
      flow->response_flags &= ~DCC_INTERACTION_FLOW_RESPONSE_INITIAL_CLAIMED;
    }
    flow->state = DCC_INTERACTION_FLOW_FAILED;
  }
  return status;
}

dcc_status_t dcc_flow_require_ready(dcc_interaction_flow_t *flow) {
  if (flow == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  if (dcc_flow_has_response_flags(flow)) {
    return dcc_flow_initial_sent(flow) || dcc_flow_initial_claimed(flow)
               ? DCC_ERR_STATE
               : DCC_OK;
  }
  return flow->state == DCC_INTERACTION_FLOW_READY ? DCC_OK : DCC_ERR_STATE;
}

dcc_status_t dcc_flow_require_context(const dcc_interaction_flow_t *flow) {
  if (flow == NULL || flow->client == NULL || flow->interaction == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return DCC_OK;
}

void dcc_flow_mark(dcc_interaction_flow_t *flow,
                   dcc_interaction_flow_state_t state, dcc_status_t status) {
  if (flow != NULL) {
    flow->state = status == DCC_OK ? state : DCC_INTERACTION_FLOW_FAILED;
  }
}

const char *dcc_flow_state_string(dcc_interaction_flow_state_t state) {
  switch (state) {
  case DCC_INTERACTION_FLOW_READY:
    return "ready";
  case DCC_INTERACTION_FLOW_DEFERRED:
    return "deferred";
  case DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL:
    return "deferred_ephemeral";
  case DCC_INTERACTION_FLOW_DEFERRED_UPDATE:
    return "deferred_update";
  case DCC_INTERACTION_FLOW_REPLIED:
    return "replied";
  case DCC_INTERACTION_FLOW_ORIGINAL_EDITED:
    return "original_edited";
  case DCC_INTERACTION_FLOW_FOLLOWED_UP:
    return "followed_up";
  case DCC_INTERACTION_FLOW_MODAL:
    return "modal";
  case DCC_INTERACTION_FLOW_FAILED:
    return "failed";
  case DCC_INTERACTION_FLOW_INITIAL_QUEUED:
    return "initial_queued";
  case DCC_INTERACTION_FLOW_DEFERRED_QUEUED:
    return "deferred_queued";
  default:
    return "unknown";
  }
}
