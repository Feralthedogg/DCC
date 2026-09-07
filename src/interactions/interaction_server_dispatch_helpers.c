#include "internal/interactions/dcc_interaction_server_internal.h"

#include <llam/runtime.h>

#include <openssl/sha.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

static int dcc_interaction_parse_unix_timestamp(
    const char *timestamp,
    uint64_t *out_timestamp
) {
    if (timestamp == NULL || out_timestamp == NULL || timestamp[0] == '\0') {
        return 0;
    }
    uint64_t value = 0U;
    for (const unsigned char *cursor = (const unsigned char *)timestamp;
         *cursor != '\0'; ++cursor) {
        if (*cursor < (unsigned char)'0' || *cursor > (unsigned char)'9') {
            return 0;
        }
        uint64_t digit = (uint64_t)(*cursor - (unsigned char)'0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10)) {
            return 0;
        }
        value = value * UINT64_C(10) + digit;
    }
    *out_timestamp = value;
    return 1;
}

int dcc_interaction_server_timestamp_fresh(
    const dcc_interaction_server_t *server,
    const char *timestamp
) {
    if (server == NULL || timestamp == NULL || server->replay_window_ms == 0U) {
        return 0;
    }
    uint64_t signed_at = 0U;
    if (!dcc_interaction_parse_unix_timestamp(timestamp, &signed_at)) {
        return 0;
    }
    time_t current_time = time(NULL);
    if (current_time < (time_t)0) {
        return 0;
    }
    uint64_t current = (uint64_t)current_time;
    uint64_t delta_seconds = signed_at > current
        ? signed_at - current
        : current - signed_at;
    if (delta_seconds > UINT64_MAX / UINT64_C(1000)) {
        return 0;
    }
    return delta_seconds * UINT64_C(1000) <= (uint64_t)server->replay_window_ms;
}

static int dcc_interaction_replay_digest(
    const char *timestamp,
    const char *signature,
    unsigned char out_digest[32]
) {
    if (timestamp == NULL || signature == NULL || out_digest == NULL) {
        return 0;
    }
    size_t timestamp_len = strlen(timestamp);
    size_t signature_len = strlen(signature);
    if (timestamp_len > 32U || signature_len > 128U ||
        timestamp_len > SIZE_MAX - signature_len - 1U) {
        return 0;
    }
    unsigned char material[32U + 1U + 128U];
    memcpy(material, timestamp, timestamp_len);
    material[timestamp_len] = 0U;
    memcpy(material + timestamp_len + 1U, signature, signature_len);
    (void)SHA256(material, timestamp_len + 1U + signature_len, out_digest);
    return 1;
}

int dcc_interaction_server_replay_seen(
    dcc_interaction_server_t *server,
    const char *timestamp,
    const char *signature
) {
    if (server == NULL || timestamp == NULL || signature == NULL || server->replay_window_ms == 0U) {
        return DCC_INTERACTION_REPLAY_NEW;
    }
    unsigned char digest[32];
    if (!dcc_interaction_replay_digest(timestamp, signature, digest)) {
        return DCC_INTERACTION_REPLAY_NEW;
    }
    uint64_t now_ns = llam_now_ns();
    uint64_t window_ns = (uint64_t)server->replay_window_ms * UINT64_C(1000000);
    while (atomic_flag_test_and_set_explicit(&server->replay_lock, memory_order_acquire)) {
    }
    dcc_interaction_replay_entry_t *free_entry = NULL;
    for (size_t i = 0; i < DCC_INTERACTION_REPLAY_CAP; ++i) {
        dcc_interaction_replay_entry_t *entry = &server->replay_entries[i];
        if (entry->occupied == 0U) {
            if (free_entry == NULL) {
                free_entry = entry;
            }
            continue;
        }
        if (now_ns >= entry->seen_at_ns &&
            now_ns - entry->seen_at_ns > window_ns) {
            entry->occupied = 0U;
            if (free_entry == NULL) {
                free_entry = entry;
            }
            continue;
        }
        if (memcmp(entry->digest, digest, sizeof(digest)) == 0) {
            atomic_flag_clear_explicit(&server->replay_lock, memory_order_release);
            return DCC_INTERACTION_REPLAY_SEEN;
        }
    }
    if (free_entry == NULL) {
        atomic_flag_clear_explicit(&server->replay_lock, memory_order_release);
        return DCC_INTERACTION_REPLAY_CAPACITY;
    }
    memcpy(free_entry->digest, digest, sizeof(digest));
    free_entry->seen_at_ns = now_ns;
    free_entry->occupied = 1U;
    atomic_flag_clear_explicit(&server->replay_lock, memory_order_release);
    return DCC_INTERACTION_REPLAY_NEW;
}

static int dcc_interaction_component_is_select(uint32_t component_type) {
    return component_type == 3U || component_type == 5U ||
        component_type == 6U || component_type == 7U ||
        component_type == 8U;
}

dcc_event_type_t dcc_interaction_event_type_from_payload(const dcc_interaction_t *interaction) {
    if (interaction == NULL) {
        return DCC_EVENT_INTERACTION_CREATE;
    }
    if (interaction->type == 2U) {
        if (interaction->command_type == 2U) {
            return DCC_EVENT_USER_CONTEXT_MENU;
        }
        if (interaction->command_type == 3U) {
            return DCC_EVENT_MESSAGE_CONTEXT_MENU;
        }
        return DCC_EVENT_SLASH_COMMAND;
    }
    if (interaction->type == 3U) {
        if (interaction->component_type == 2U) {
            return DCC_EVENT_BUTTON_CLICK;
        }
        if (dcc_interaction_component_is_select(interaction->component_type)) {
            return DCC_EVENT_SELECT_CLICK;
        }
    }
    if (interaction->type == 4U) {
        return DCC_EVENT_AUTOCOMPLETE;
    }
    if (interaction->type == 5U) {
        return DCC_EVENT_FORM_SUBMIT;
    }
    return DCC_EVENT_INTERACTION_CREATE;
}

dcc_status_t dcc_interaction_call_route(
    const dcc_interaction_route_entry_t *route,
    dcc_interaction_request_t *request
) {
    if (route == NULL || request == NULL || route->callback == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    route->callback(request->server, request, route->user_data);
    if (!request->response_set) {
        return dcc_interaction_request_set_response_copy(
            request,
            204,
            "text/plain; charset=utf-8",
            "",
            0U
        );
    }
    return DCC_OK;
}

dcc_status_t dcc_interaction_set_default_response(dcc_interaction_request_t *request) {
    const dcc_interaction_t *interaction = dcc_interaction_request_interaction(request);
    if (interaction != NULL && interaction->type == 1U) {
        return dcc_interaction_request_reply_pong(request);
    }
    return dcc_interaction_request_reply_deferred_message(request);
}
