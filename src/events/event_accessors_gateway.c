#include "internal/events/dcc_event_state_internal.h"

const dcc_gateway_event_data_t *dcc_event_data(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_READY:
            return event->raw.json != NULL ? event->gateway : NULL;
        case DCC_EVENT_RESUMED:
        case DCC_EVENT_LOG:
        case DCC_EVENT_SOCKET_CLOSE:
            return NULL;
        case DCC_EVENT_RATE_LIMITED:
            return event->raw.json != NULL ? event->gateway : NULL;
        case DCC_EVENT_VOICE_BUFFER_SEND:
        case DCC_EVENT_VOICE_READY:
        case DCC_EVENT_VOICE_RECEIVE:
        case DCC_EVENT_VOICE_RECEIVE_COMBINED:
        case DCC_EVENT_VOICE_TRACK_MARKER:
        case DCC_EVENT_VOICE_CLIENT_SPEAKING:
        case DCC_EVENT_VOICE_CLIENT_DISCONNECT:
        case DCC_EVENT_VOICE_CLIENT_PLATFORM:
        case DCC_EVENT_MAX:
            return NULL;
        default:
            return event->gateway;
    }
}
