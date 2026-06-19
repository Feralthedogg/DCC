#ifndef DCC_INTERACTION_SERVER_REQUEST_H
#define DCC_INTERACTION_SERVER_REQUEST_H

#include <dcc/autocomplete.h>
#include <dcc/interaction_server/base.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/rest/types/interactions.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API const char *dcc_interaction_request_method(const dcc_interaction_request_t *request);
DCC_API const char *dcc_interaction_request_path(const dcc_interaction_request_t *request);
DCC_API const char *dcc_interaction_request_header(
    const dcc_interaction_request_t *request,
    const char *name
);
DCC_API const dcc_interaction_http_header_t *dcc_interaction_request_headers(
    const dcc_interaction_request_t *request,
    size_t *out_count
);
DCC_API const char *dcc_interaction_request_body(const dcc_interaction_request_t *request);
DCC_API size_t dcc_interaction_request_body_len(const dcc_interaction_request_t *request);
DCC_API const dcc_interaction_t *dcc_interaction_request_interaction(
    const dcc_interaction_request_t *request
);
DCC_API dcc_event_type_t dcc_interaction_request_event_type(
    const dcc_interaction_request_t *request
);
DCC_API uint8_t dcc_interaction_request_is_ping(const dcc_interaction_request_t *request);

DCC_API dcc_status_t dcc_interaction_request_set_response(
    dcc_interaction_request_t *request,
    uint16_t status,
    const char *content_type,
    const void *body,
    size_t body_len
);
DCC_API dcc_status_t dcc_interaction_request_reply_json(
    dcc_interaction_request_t *request,
    const char *json_body
);
DCC_API dcc_status_t dcc_interaction_request_reply_type(
    dcc_interaction_request_t *request,
    dcc_interaction_response_type_t type
);
DCC_API dcc_status_t dcc_interaction_request_reply_message_builder(
    dcc_interaction_request_t *request,
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message
);
DCC_API dcc_status_t dcc_interaction_request_reply_message(
    dcc_interaction_request_t *request,
    const dcc_message_builder_t *message
);
DCC_API dcc_status_t dcc_interaction_request_reply_update_message(
    dcc_interaction_request_t *request,
    const dcc_message_builder_t *message
);
DCC_API dcc_status_t dcc_interaction_request_reply_message_text(
    dcc_interaction_request_t *request,
    const char *content
);
DCC_API dcc_status_t dcc_interaction_request_reply_ephemeral_message_text(
    dcc_interaction_request_t *request,
    const char *content
);
DCC_API dcc_status_t dcc_interaction_request_reply_update_message_text(
    dcc_interaction_request_t *request,
    const char *content
);
DCC_API dcc_status_t dcc_interaction_request_reply_modal(
    dcc_interaction_request_t *request,
    const dcc_modal_builder_t *modal
);
DCC_API dcc_status_t dcc_interaction_request_reply_autocomplete(
    dcc_interaction_request_t *request,
    const dcc_autocomplete_builder_t *autocomplete
);
DCC_API dcc_status_t dcc_interaction_request_reply_pong(dcc_interaction_request_t *request);
DCC_API dcc_status_t dcc_interaction_request_reply_deferred_message(
    dcc_interaction_request_t *request
);
DCC_API dcc_status_t dcc_interaction_request_reply_deferred_update(
    dcc_interaction_request_t *request
);
DCC_API dcc_status_t dcc_interaction_request_reply_premium_required(
    dcc_interaction_request_t *request
);
DCC_API dcc_status_t dcc_interaction_request_reply_text(
    dcc_interaction_request_t *request,
    uint16_t status,
    const char *text
);

#ifdef __cplusplus
}
#endif

#endif
