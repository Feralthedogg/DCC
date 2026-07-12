#include "internal/interactions/dcc_interaction_server_internal.h"

static _Thread_local dcc_json_gateway_payload_t dcc_interaction_payload_scratch;

dcc_status_t dcc_interaction_handle_request(dcc_interaction_request_t *request) {
    const dcc_interaction_route_entry_t *route = dcc_interaction_find_route(request->server, request);
    if (route == NULL) {
        route = dcc_interaction_find_prefix_route(request->server, request);
    }
    if (route != NULL && (route->flags & (uint32_t)DCC_INTERACTION_ROUTE_F_UNSIGNED) != 0U) {
        return dcc_interaction_call_route(route, request);
    }

    if (!dcc_interaction_ascii_eq(request->method, "POST")) {
        return dcc_interaction_request_reply_text(request, 405, "Method not allowed");
    }

    const char *signature = dcc_interaction_request_header(request, "X-Signature-Ed25519");
    const char *timestamp = dcc_interaction_request_header(request, "X-Signature-Timestamp");
    if (signature == NULL || timestamp == NULL) {
        return dcc_interaction_request_reply_text(request, 401, "Unsigned requests are not allowed");
    }
    if (!dcc_interaction_verify_signature(
            request->server->public_key_hex,
            timestamp,
            request->body,
            request->body_len,
            signature
        )) {
        return dcc_interaction_request_reply_text(request, 401, "Access denied");
    }
    if (route == NULL && dcc_interaction_has_signed_route_for_method(request->server, request->method)) {
        return dcc_interaction_request_reply_text(request, 404, "Interaction route not found");
    }
    if (dcc_interaction_server_replay_seen(request->server, timestamp, signature)) {
        atomic_fetch_add_explicit(&request->server->replayed_requests, 1U, memory_order_relaxed);
        return dcc_interaction_request_reply_text(request, 409, "Interaction request replayed");
    }

    request->payload = &dcc_interaction_payload_scratch;
    dcc_status_t st = dcc_json_parse_interaction_payload(request->body, request->body_len, request->payload);
    if (st != DCC_OK || !request->payload->has_interaction) {
        return dcc_interaction_request_reply_text(request, 400, "Invalid interaction payload");
    }
    request->parsed = 1U;
    request->event_type = dcc_interaction_event_type_from_payload(&request->payload->interaction);

    if (route != NULL) {
        return dcc_interaction_call_route(route, request);
    }
    if (request->server->callback != NULL) {
        request->server->callback(request->server, request, request->server->user_data);
    }
    if (!request->response_set) {
        return dcc_interaction_set_default_response(request);
    }
    return DCC_OK;
}
