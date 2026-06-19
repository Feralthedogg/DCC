#ifndef DCC_INTERACTION_FLOW_H
#define DCC_INTERACTION_FLOW_H

#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/objects/interactions.h>
#include <dcc/rest/base.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_interaction_flow_state {
    DCC_INTERACTION_FLOW_READY = 0,
    DCC_INTERACTION_FLOW_DEFERRED,
    DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL,
    DCC_INTERACTION_FLOW_DEFERRED_UPDATE,
    DCC_INTERACTION_FLOW_REPLIED,
    DCC_INTERACTION_FLOW_ORIGINAL_EDITED,
    DCC_INTERACTION_FLOW_FOLLOWED_UP,
    DCC_INTERACTION_FLOW_MODAL,
    DCC_INTERACTION_FLOW_FAILED
} dcc_interaction_flow_state_t;

typedef struct dcc_interaction_flow {
    size_t size;
    dcc_client_t *client;
    const dcc_interaction_t *interaction;
    dcc_interaction_flow_state_t state;
    uint64_t started_at_ms;
    uint64_t auto_defer_after_ms;
    uint8_t auto_defer_ephemeral;
} dcc_interaction_flow_t;

DCC_API void dcc_flow_init(
    dcc_interaction_flow_t *flow,
    dcc_client_t *client,
    const dcc_interaction_t *interaction
);
DCC_API dcc_status_t dcc_flow_set_started_at(dcc_interaction_flow_t *flow, uint64_t started_at_ms);
DCC_API dcc_status_t dcc_flow_auto_defer(dcc_interaction_flow_t *flow, uint64_t after_ms);
DCC_API dcc_status_t dcc_flow_auto_defer_ephemeral(dcc_interaction_flow_t *flow, uint64_t after_ms);
DCC_API dcc_status_t dcc_flow_maybe_auto_defer(
    dcc_interaction_flow_t *flow,
    uint64_t now_ms,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_flow_defer(
    dcc_interaction_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_flow_defer_ephemeral(
    dcc_interaction_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_flow_defer_update(
    dcc_interaction_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_flow_reply(
    dcc_interaction_flow_t *flow,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_flow_edit_original(
    dcc_interaction_flow_t *flow,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_flow_followup(
    dcc_interaction_flow_t *flow,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_flow_show_modal(
    dcc_interaction_flow_t *flow,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_interaction_flow_state_t dcc_flow_state(const dcc_interaction_flow_t *flow);
DCC_API const char *dcc_flow_state_string(dcc_interaction_flow_state_t state);

#ifdef __cplusplus
}
#endif

#endif
