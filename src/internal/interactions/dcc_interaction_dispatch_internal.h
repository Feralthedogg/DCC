#ifndef DCC_INTERACTION_DISPATCH_INTERNAL_H
#define DCC_INTERACTION_DISPATCH_INTERNAL_H

#include "internal/interactions/dcc_interaction_request_state_internal.h"
#include "internal/interactions/dcc_interaction_server_state_internal.h"

#include <dcc/interaction_server.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_interaction_handle_request(dcc_interaction_request_t *request);
dcc_event_type_t dcc_interaction_event_type_from_payload(const dcc_interaction_t *interaction);
dcc_status_t dcc_interaction_call_route(
    const dcc_interaction_route_entry_t *route,
    dcc_interaction_request_t *request
);
dcc_status_t dcc_interaction_set_default_response(dcc_interaction_request_t *request);
void dcc_interaction_server_note_response(dcc_interaction_server_t *server, uint16_t status);
void dcc_interaction_request_free(dcc_interaction_request_t *request);
dcc_status_t dcc_interaction_request_set_response_copy(
    dcc_interaction_request_t *request,
    uint16_t status,
    const char *content_type,
    const void *body,
    size_t body_len
);

#ifdef __cplusplus
}
#endif

#endif
