#include "gateway_smoke_state.h"

#if !defined(_WIN32)

#include <string.h>

void gateway_state_mark_raw_event(gateway_state_t *state, dcc_event_type_t type) {
    if (state == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return;
    }
    if ((unsigned)type < 64U) {
        state->raw_seen_low |= 1ULL << (unsigned)type;
    } else if ((unsigned)type < 128U) {
        state->raw_seen_high |= 1ULL << ((unsigned)type - 64U);
    }
}

int gateway_state_raw_event_seen(const gateway_state_t *state, dcc_event_type_t type) {
    if (state == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return 0;
    }
    if ((unsigned)type < 64U) {
        return (state->raw_seen_low & (1ULL << (unsigned)type)) != 0;
    }
    if ((unsigned)type < 128U) {
        return (state->raw_seen_high & (1ULL << ((unsigned)type - 64U))) != 0;
    }
    return 0;
}

int gateway_smoke_event_is_not_gateway_dispatch(dcc_event_type_t type) {
    switch (type) {
        case DCC_EVENT_LOG:
        case DCC_EVENT_RAW:
        case DCC_EVENT_SOCKET_CLOSE:
        case DCC_EVENT_VOICE_BUFFER_SEND:
        case DCC_EVENT_VOICE_READY:
        case DCC_EVENT_VOICE_RECEIVE:
        case DCC_EVENT_VOICE_RECEIVE_COMBINED:
        case DCC_EVENT_VOICE_TRACK_MARKER:
        case DCC_EVENT_VOICE_CLIENT_SPEAKING:
        case DCC_EVENT_VOICE_CLIENT_DISCONNECT:
        case DCC_EVENT_VOICE_CLIENT_PLATFORM:
            return 1;
        default:
            return 0;
    }
}

int gateway_smoke_event_is_interaction_subtype(dcc_event_type_t type) {
    switch (type) {
        case DCC_EVENT_SLASH_COMMAND:
        case DCC_EVENT_BUTTON_CLICK:
        case DCC_EVENT_AUTOCOMPLETE:
        case DCC_EVENT_SELECT_CLICK:
        case DCC_EVENT_MESSAGE_CONTEXT_MENU:
        case DCC_EVENT_USER_CONTEXT_MENU:
        case DCC_EVENT_FORM_SUBMIT:
            return 1;
        default:
            return 0;
    }
}

const char *gateway_smoke_expected_dispatch_name(dcc_event_type_t type) {
    return gateway_smoke_event_is_interaction_subtype(type) ? "INTERACTION_CREATE" : dcc_event_type_name(type);
}

int gateway_smoke_typed_metadata_ok(const dcc_event_t *event, dcc_event_type_t type) {
    if (event == NULL || gateway_smoke_event_is_not_gateway_dispatch(type)) {
        return 0;
    }

    const char *expected_name = gateway_smoke_expected_dispatch_name(type);
    const char *event_name = dcc_event_name(event);
    const dcc_raw_event_t *raw = dcc_event_raw(event);
    return expected_name != NULL &&
        event_name != NULL &&
        raw != NULL &&
        raw->type == type &&
        raw->name != NULL &&
        strcmp(event_name, expected_name) == 0 &&
        strcmp(raw->name, expected_name) == 0 &&
        raw->sequence > 0 &&
        raw->json != NULL &&
        raw->json_len > 0;
}

int gateway_state_required_raw_events_seen(const gateway_state_t *state) {
    for (int type = 0; type < (int)DCC_EVENT_MAX; type++) {
        if (gateway_smoke_event_is_not_gateway_dispatch((dcc_event_type_t)type)) {
            continue;
        }
        if (!gateway_state_raw_event_seen(state, (dcc_event_type_t)type)) {
            return 0;
        }
    }
    return 1;
}

void gateway_state_mark_typed_event(gateway_state_t *state, dcc_event_type_t type) {
    if (state == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return;
    }
    if ((unsigned)type < 64U) {
        state->typed_seen_low |= 1ULL << (unsigned)type;
    } else if ((unsigned)type < 128U) {
        state->typed_seen_high |= 1ULL << ((unsigned)type - 64U);
    }
}

int gateway_state_typed_event_seen(const gateway_state_t *state, dcc_event_type_t type) {
    if (state == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return 0;
    }
    if ((unsigned)type < 64U) {
        return (state->typed_seen_low & (1ULL << (unsigned)type)) != 0;
    }
    if ((unsigned)type < 128U) {
        return (state->typed_seen_high & (1ULL << ((unsigned)type - 64U))) != 0;
    }
    return 0;
}

void gateway_state_mark_typed_accessor_event(gateway_state_t *state, dcc_event_type_t type) {
    if (state == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return;
    }
    if ((unsigned)type < 64U) {
        state->typed_accessor_seen_low |= 1ULL << (unsigned)type;
    } else if ((unsigned)type < 128U) {
        state->typed_accessor_seen_high |= 1ULL << ((unsigned)type - 64U);
    }
}

int gateway_state_typed_accessor_event_seen(const gateway_state_t *state, dcc_event_type_t type) {
    if (state == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return 0;
    }
    if ((unsigned)type < 64U) {
        return (state->typed_accessor_seen_low & (1ULL << (unsigned)type)) != 0;
    }
    if ((unsigned)type < 128U) {
        return (state->typed_accessor_seen_high & (1ULL << ((unsigned)type - 64U))) != 0;
    }
    return 0;
}

int gateway_state_required_typed_events_seen(const gateway_state_t *state) {
    for (int type = 0; type < (int)DCC_EVENT_MAX; type++) {
        if (gateway_smoke_event_is_not_gateway_dispatch((dcc_event_type_t)type)) {
            continue;
        }
        if (!gateway_state_typed_event_seen(state, (dcc_event_type_t)type)) {
            return 0;
        }
    }
    return 1;
}

int gateway_state_required_typed_accessors_seen(const gateway_state_t *state) {
    for (int type = 0; type < (int)DCC_EVENT_MAX; type++) {
        if (gateway_smoke_event_is_not_gateway_dispatch((dcc_event_type_t)type)) {
            continue;
        }
        if (!gateway_state_typed_accessor_event_seen(state, (dcc_event_type_t)type)) {
            return 0;
        }
    }
    return 1;
}

#endif
