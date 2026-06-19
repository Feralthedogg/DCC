#include "internal/replay/dcc_replay_internal.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_replay_copy_field(
    const char *begin,
    const char *end,
    char **out
) {
    if (begin == NULL || end == NULL || end < begin || out == NULL) {
        return DCC_ERR_JSON;
    }
    size_t len = (size_t)(end - begin);
    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(copy, begin, len);
    copy[len] = '\0';
    *out = copy;
    return DCC_OK;
}

static const char *dcc_replay_field_string_begin(const char *line, const char *key) {
    const char *found = strstr(line, key);
    return found != NULL ? found + strlen(key) : NULL;
}

static dcc_status_t dcc_replay_parse_u64(const char *begin, uint64_t *out) {
    if (begin == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }
    errno = 0;
    char *end = NULL;
    unsigned long long value = strtoull(begin, &end, 10);
    if (errno != 0 || end == begin) {
        return DCC_ERR_JSON;
    }
    *out = (uint64_t)value;
    return DCC_OK;
}

dcc_status_t dcc_replay_parse_line(const char *line, size_t line_len, dcc_replay_record_t *out) {
    if (line == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);

    const char *kind_begin = dcc_replay_field_string_begin(line, "\"kind\":\"");
    const char *kind_end = kind_begin != NULL ? strchr(kind_begin, '"') : NULL;
    char *kind_text = NULL;
    dcc_status_t status = dcc_replay_copy_field(kind_begin, kind_end, &kind_text);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_replay_kind_from_string(kind_text, &out->kind);
    free(kind_text);
    if (status != DCC_OK) {
        return status;
    }

    const char *ts_begin = strstr(line, "\"ts\":");
    status = dcc_replay_parse_u64(ts_begin != NULL ? ts_begin + sizeof("\"ts\":") - 1U : NULL, &out->ts_ms);
    if (status != DCC_OK) {
        return status;
    }

    const char *payload_begin = strstr(line, "\"payload\":");
    if (payload_begin == NULL) {
        return DCC_ERR_JSON;
    }
    payload_begin += sizeof("\"payload\":") - 1U;
    while (*payload_begin == ' ' || *payload_begin == '\t') {
        ++payload_begin;
    }

    const char *line_end = line + line_len;
    while (line_end > line &&
           (line_end[-1] == '\n' || line_end[-1] == '\r' ||
            line_end[-1] == ' ' || line_end[-1] == '\t')) {
        --line_end;
    }
    if (line_end <= payload_begin || line_end[-1] != '}') {
        return DCC_ERR_JSON;
    }
    const char *payload_end = line_end - 1U;
    while (payload_end > payload_begin &&
           (payload_end[-1] == ' ' || payload_end[-1] == '\t')) {
        --payload_end;
    }
    size_t payload_len = (size_t)(payload_end - payload_begin);
    if (!dcc_replay_payload_shape_ok(payload_begin, payload_len)) {
        return DCC_ERR_JSON;
    }

    char *payload = (char *)malloc(payload_len + 1U);
    if (payload == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(payload, payload_begin, payload_len);
    payload[payload_len] = '\0';
    out->payload = payload;
    out->payload_len = payload_len;

    const char *event_begin = dcc_replay_field_string_begin(line, "\"event\":\"");
    const char *event_end = event_begin != NULL ? strchr(event_begin, '"') : NULL;
    if (event_begin != NULL && event_end != NULL && event_begin < payload_begin) {
        char *event = NULL;
        status = dcc_replay_copy_field(event_begin, event_end, &event);
        if (status != DCC_OK) {
            dcc_replay_record_deinit(out);
            return status;
        }
        out->event = event;
    }
    if (out->kind == DCC_REPLAY_GATEWAY && (out->event == NULL || out->event[0] == '\0')) {
        dcc_replay_record_deinit(out);
        return DCC_ERR_JSON;
    }
    return DCC_OK;
}
