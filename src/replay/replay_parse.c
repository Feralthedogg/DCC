#include "internal/replay/dcc_replay_internal.h"
#include "internal/json/dcc_json.h"

#include <stdlib.h>
#include <string.h>

static const char *dcc_replay_skip_ws(const char *p, const char *end) {
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) ++p;
    return p;
}

static const char *dcc_replay_skip_string(const char *p, const char *end) {
    if (p >= end || *p != '"') return NULL;
    for (++p; p < end; ++p) {
        if (*p == '\\') {
            if (++p >= end) return NULL;
            continue;
        }
        if (*p == '"') return p + 1U;
    }
    return NULL;
}

static const char *dcc_replay_skip_value(const char *p, const char *end) {
    p = dcc_replay_skip_ws(p, end);
    if (p >= end) return NULL;
    if (*p == '"') return dcc_replay_skip_string(p, end);
    if (*p != '{' && *p != '[') {
        while (p < end && *p != ',' && *p != '}') ++p;
        return p;
    }
    char stack[128];
    size_t depth = 0U;
    stack[depth++] = *p++;
    while (p < end && depth != 0U) {
        if (*p == '"') {
            p = dcc_replay_skip_string(p, end);
            if (p == NULL) return NULL;
            continue;
        }
        if (*p == '{' || *p == '[') {
            if (depth >= sizeof(stack)) return NULL;
            stack[depth++] = *p++;
            continue;
        }
        if (*p == '}' || *p == ']') {
            char open = stack[depth - 1U];
            if ((*p == '}' && open != '{') || (*p == ']' && open != '[')) return NULL;
            --depth;
        }
        ++p;
    }
    return depth == 0U ? p : NULL;
}

static int dcc_replay_key_is_payload(const char *begin, const char *end) {
    static const char key[] = "payload";
    return end > begin && (size_t)(end - begin) == sizeof(key) - 1U &&
        memcmp(begin, key, sizeof(key) - 1U) == 0;
}

static dcc_status_t dcc_replay_payload_span(
    const char *line,
    size_t line_len,
    const char **out_begin,
    const char **out_end
) {
    const char *end = line + line_len;
    const char *p = dcc_replay_skip_ws(line, end);
    if (p >= end || *p++ != '{') return DCC_ERR_JSON;
    for (;;) {
        p = dcc_replay_skip_ws(p, end);
        if (p >= end || *p == '}') return DCC_ERR_JSON;
        if (*p != '"') return DCC_ERR_JSON;
        const char *key_begin = p + 1U;
        const char *after_key = dcc_replay_skip_string(p, end);
        if (after_key == NULL) return DCC_ERR_JSON;
        const char *key_end = after_key - 1U;
        p = dcc_replay_skip_ws(after_key, end);
        if (p >= end || *p++ != ':') return DCC_ERR_JSON;
        p = dcc_replay_skip_ws(p, end);
        const char *value_begin = p;
        const char *value_end = dcc_replay_skip_value(p, end);
        if (value_end == NULL) return DCC_ERR_JSON;
        if (dcc_replay_key_is_payload(key_begin, key_end)) {
            *out_begin = value_begin;
            *out_end = value_end;
            return DCC_OK;
        }
        p = dcc_replay_skip_ws(value_end, end);
        if (p >= end || *p != ',') return DCC_ERR_JSON;
        ++p;
    }
}

static char *dcc_replay_strdup(const char *text) {
    if (text == NULL) return NULL;
    size_t len = strlen(text);
    char *copy = (char *)malloc(len + 1U);
    if (copy != NULL) memcpy(copy, text, len + 1U);
    return copy;
}

dcc_status_t dcc_replay_parse_line(const char *line, size_t line_len, dcc_replay_record_t *out) {
    if (line == NULL || out == NULL) return DCC_ERR_INVALID_ARG;
    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);

    dcc_json_t *root = NULL;
    dcc_status_t status = dcc_json_parse(line, line_len, &root);
    if (status != DCC_OK || dcc_json_typeof(root) != DCC_JSON_OBJECT) {
        dcc_json_free(root);
        return DCC_ERR_JSON;
    }
    const char *kind = dcc_json_string(dcc_json_object_get(root, "kind"));
    const dcc_json_t *timestamp = dcc_json_object_get(root, "ts");
    const dcc_json_t *payload = dcc_json_object_get(root, "payload");
    const dcc_json_t *event_value = dcc_json_object_get(root, "event");
    if (kind == NULL || dcc_json_u64(timestamp, &out->ts_ms) != 0 ||
        (dcc_json_typeof(payload) != DCC_JSON_OBJECT && dcc_json_typeof(payload) != DCC_JSON_ARRAY)) {
        dcc_json_free(root);
        return DCC_ERR_JSON;
    }
    status = dcc_replay_kind_from_string(kind, &out->kind);
    const char *event = event_value != NULL ? dcc_json_string(event_value) : NULL;
    if (status != DCC_OK ||
        (event_value != NULL && event == NULL) ||
        (out->kind == DCC_REPLAY_GATEWAY && (event == NULL || event[0] == '\0'))) {
        dcc_json_free(root);
        return DCC_ERR_JSON;
    }
    if (event != NULL) {
        out->event = dcc_replay_strdup(event);
        if (out->event == NULL) {
            dcc_json_free(root);
            return DCC_ERR_NOMEM;
        }
    }

    const char *payload_begin = NULL;
    const char *payload_end = NULL;
    status = dcc_replay_payload_span(line, line_len, &payload_begin, &payload_end);
    if (status != DCC_OK || payload_end < payload_begin) {
        dcc_json_free(root);
        dcc_replay_record_deinit(out);
        return DCC_ERR_JSON;
    }
    out->payload_len = (size_t)(payload_end - payload_begin);
    char *payload_copy = (char *)malloc(out->payload_len + 1U);
    if (payload_copy == NULL) {
        dcc_json_free(root);
        dcc_replay_record_deinit(out);
        return DCC_ERR_NOMEM;
    }
    memcpy(payload_copy, payload_begin, out->payload_len);
    payload_copy[out->payload_len] = '\0';
    out->payload = payload_copy;
    dcc_json_free(root);
    return DCC_OK;
}
