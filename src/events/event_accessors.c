#include "internal/events/dcc_event_state_internal.h"

dcc_event_type_t dcc_event_type(const dcc_event_t *event) {
    return event != NULL ? event->type : DCC_EVENT_MAX;
}

const char *dcc_event_name(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    if (event->raw.name != NULL) {
        return event->raw.name;
    }
    return dcc_event_type_name(event->type);
}

uint32_t dcc_event_shard_id(const dcc_event_t *event) {
    return event != NULL ? event->shard_id : 0;
}

void dcc_event_cancel(const dcc_event_t *event) {
    if (event == NULL) {
        return;
    }
    ((dcc_event_t *)event)->cancelled = 1;
}

uint8_t dcc_event_is_cancelled(const dcc_event_t *event) {
    return event != NULL ? event->cancelled : 0;
}

const dcc_raw_event_t *dcc_event_raw(const dcc_event_t *event) {
    return event != NULL && event->raw.name != NULL ? &event->raw : NULL;
}

const dcc_ready_event_t *dcc_event_ready(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_READY ? &event->data.ready : NULL;
}

const dcc_ready_event_t *dcc_event_resumed(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_RESUMED ? &event->data.ready : NULL;
}

const dcc_log_event_t *dcc_event_log(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_LOG ? &event->data.log : NULL;
}

const dcc_socket_close_event_t *dcc_event_socket_close(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_SOCKET_CLOSE ? &event->data.socket_close : NULL;
}

const dcc_rate_limited_event_t *dcc_event_rate_limited(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_RATE_LIMITED ? &event->data.rate_limited : NULL;
}
