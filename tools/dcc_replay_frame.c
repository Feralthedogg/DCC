#include "internal/dcc_replay_tool.h"
#include "internal/json/dcc_json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int dcc_replay_tool_payload_is_gateway_frame(
    const char *payload,
    size_t payload_len
) {
    dcc_json_t *root = NULL;
    dcc_status_t status = dcc_json_parse(payload, payload_len, &root);
    if (status != DCC_OK) {
        return 0;
    }
    int is_frame = dcc_json_typeof(root) == DCC_JSON_OBJECT &&
        dcc_json_object_get(root, "op") != NULL &&
        dcc_json_object_get(root, "t") != NULL;
    dcc_json_free(root);
    return is_frame;
}

static int dcc_replay_tool_event_name_safe(const char *event) {
    if (event == NULL || event[0] == '\0') {
        return 0;
    }
    for (const unsigned char *p = (const unsigned char *)event; *p != '\0'; ++p) {
        if ((*p >= 'A' && *p <= 'Z') ||
            (*p >= '0' && *p <= '9') ||
            *p == '_') {
            continue;
        }
        return 0;
    }
    return 1;
}

static dcc_status_t dcc_replay_tool_wrap_dispatch(
    const char *event,
    uint64_t sequence,
    const char *payload,
    size_t payload_len,
    char **out_json,
    size_t *out_len
) {
    if (!dcc_replay_tool_event_name_safe(event) ||
        payload == NULL ||
        out_json == NULL ||
        out_len == NULL ||
        payload_len > SIZE_MAX - 96U ||
        strlen(event) > SIZE_MAX - payload_len - 96U) {
        return DCC_ERR_INVALID_ARG;
    }

    int prefix = snprintf(
        NULL,
        0,
        "{\"op\":0,\"s\":%llu,\"t\":\"%s\",\"d\":",
        (unsigned long long)sequence,
        event
    );
    if (prefix < 0) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t total = (size_t)prefix + payload_len + 1U;
    char *frame = (char *)malloc(total + 1U);
    if (frame == NULL) {
        return DCC_ERR_NOMEM;
    }
    int written = snprintf(
        frame,
        (size_t)prefix + 1U,
        "{\"op\":0,\"s\":%llu,\"t\":\"%s\",\"d\":",
        (unsigned long long)sequence,
        event
    );
    if (written != prefix) {
        free(frame);
        return DCC_ERR_INVALID_ARG;
    }
    memcpy(frame + prefix, payload, payload_len);
    frame[prefix + payload_len] = '}';
    frame[prefix + payload_len + 1U] = '\0';
    *out_json = frame;
    *out_len = prefix + payload_len + 1U;
    return DCC_OK;
}

dcc_status_t dcc_replay_tool_frame_for_record(
    const dcc_replay_record_t *record,
    char **out_json,
    size_t *out_len
) {
    if (record == NULL || out_json == NULL || out_len == NULL ||
        record->payload == NULL || record->payload_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    *out_len = 0;
    if (dcc_replay_tool_payload_is_gateway_frame(record->payload, record->payload_len)) {
        char *copy = (char *)malloc(record->payload_len + 1U);
        if (copy == NULL) {
            return DCC_ERR_NOMEM;
        }
        memcpy(copy, record->payload, record->payload_len);
        copy[record->payload_len] = '\0';
        *out_json = copy;
        *out_len = record->payload_len;
        return DCC_OK;
    }

    const char *event = record->kind == DCC_REPLAY_INTERACTION
        ? "INTERACTION_CREATE"
        : record->event;
    return dcc_replay_tool_wrap_dispatch(
        event,
        record->ts_ms,
        record->payload,
        record->payload_len,
        out_json,
        out_len
    );
}
