#include <dcc/webhook_events.h>

#include <ctype.h>
#include <stdint.h>
#include <string.h>

typedef struct dcc_webhook_event_name_map {
    dcc_webhook_event_type_t type;
    const char *name;
} dcc_webhook_event_name_map_t;

static const dcc_webhook_event_name_map_t dcc_webhook_event_names[] = {
    {DCC_WEBHOOK_EVENT_APPLICATION_AUTHORIZED, "APPLICATION_AUTHORIZED"},
    {DCC_WEBHOOK_EVENT_APPLICATION_DEAUTHORIZED, "APPLICATION_DEAUTHORIZED"},
    {DCC_WEBHOOK_EVENT_ENTITLEMENT_CREATE, "ENTITLEMENT_CREATE"},
    {DCC_WEBHOOK_EVENT_ENTITLEMENT_UPDATE, "ENTITLEMENT_UPDATE"},
    {DCC_WEBHOOK_EVENT_ENTITLEMENT_DELETE, "ENTITLEMENT_DELETE"},
    {DCC_WEBHOOK_EVENT_QUEST_USER_ENROLLMENT, "QUEST_USER_ENROLLMENT"},
    {DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_CREATE, "LOBBY_MESSAGE_CREATE"},
    {DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_UPDATE, "LOBBY_MESSAGE_UPDATE"},
    {DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_DELETE, "LOBBY_MESSAGE_DELETE"},
    {DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_CREATE, "GAME_DIRECT_MESSAGE_CREATE"},
    {DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_UPDATE, "GAME_DIRECT_MESSAGE_UPDATE"},
    {DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_DELETE, "GAME_DIRECT_MESSAGE_DELETE"},
};

const char *dcc_webhook_event_type_name(dcc_webhook_event_type_t type) {
    if (type == DCC_WEBHOOK_EVENT_PING) {
        return "PING";
    }
    for (size_t i = 0; i < sizeof(dcc_webhook_event_names) / sizeof(dcc_webhook_event_names[0]); ++i) {
        if (dcc_webhook_event_names[i].type == type) {
            return dcc_webhook_event_names[i].name;
        }
    }
    return NULL;
}

dcc_webhook_event_type_t dcc_webhook_event_type_from_string(const char *name) {
    if (name == NULL) {
        return DCC_WEBHOOK_EVENT_UNKNOWN;
    }
    if (strcmp(name, "PING") == 0) {
        return DCC_WEBHOOK_EVENT_PING;
    }
    for (size_t i = 0; i < sizeof(dcc_webhook_event_names) / sizeof(dcc_webhook_event_names[0]); ++i) {
        if (strcmp(name, dcc_webhook_event_names[i].name) == 0) {
            return dcc_webhook_event_names[i].type;
        }
    }
    return DCC_WEBHOOK_EVENT_UNKNOWN;
}

static const char *dcc_skip_json_ws(const char *p, const char *end) {
    while (p < end && isspace((unsigned char)*p)) {
        ++p;
    }
    return p;
}

static const char *dcc_trim_json_ws_end(const char *start, const char *end) {
    while (end > start && isspace((unsigned char)*(end - 1))) {
        --end;
    }
    return end;
}

static dcc_status_t dcc_json_skip_string(const char *p, const char *end, const char **out_end) {
    if (p >= end || *p != '"') {
        return DCC_ERR_INVALID_ARG;
    }
    ++p;
    while (p < end) {
        if (*p == '\\') {
            ++p;
            if (p >= end) {
                return DCC_ERR_INVALID_ARG;
            }
            ++p;
            continue;
        }
        if (*p == '"') {
            *out_end = p + 1;
            return DCC_OK;
        }
        ++p;
    }
    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_json_skip_compound(const char *p, const char *end, const char **out_end) {
    int depth = 0;
    int in_string = 0;
    int escaped = 0;

    for (const char *q = p; q < end; ++q) {
        if (in_string) {
            if (escaped) {
                escaped = 0;
            } else if (*q == '\\') {
                escaped = 1;
            } else if (*q == '"') {
                in_string = 0;
            }
            continue;
        }

        if (*q == '"') {
            in_string = 1;
        } else if (*q == '{' || *q == '[') {
            ++depth;
        } else if (*q == '}' || *q == ']') {
            --depth;
            if (depth == 0) {
                *out_end = q + 1;
                return DCC_OK;
            }
            if (depth < 0) {
                return DCC_ERR_INVALID_ARG;
            }
        }
    }
    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_json_skip_value(const char *p, const char *end, const char **out_end) {
    p = dcc_skip_json_ws(p, end);
    if (p >= end) {
        return DCC_ERR_INVALID_ARG;
    }
    if (*p == '"') {
        return dcc_json_skip_string(p, end, out_end);
    }
    if (*p == '{' || *p == '[') {
        return dcc_json_skip_compound(p, end, out_end);
    }

    const char *q = p;
    while (q < end && *q != ',' && *q != '}' && *q != ']') {
        ++q;
    }
    q = dcc_trim_json_ws_end(p, q);
    if (q == p) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_end = q;
    return DCC_OK;
}

static dcc_status_t dcc_webhook_find_root_object(
    const char *body,
    size_t body_len,
    const char **out_start,
    const char **out_end
) {
    const char *end = body + body_len;
    const char *start = dcc_skip_json_ws(body, end);
    if (start >= end || *start != '{') {
        return DCC_ERR_INVALID_ARG;
    }

    const char *root_end = NULL;
    dcc_status_t status = dcc_json_skip_compound(start, end, &root_end);
    if (status != DCC_OK) {
        return status;
    }
    if (dcc_skip_json_ws(root_end, end) != end) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_start = start;
    *out_end = root_end;
    return DCC_OK;
}

static dcc_status_t dcc_json_find_top_level_value(
    const char *object_start,
    const char *object_end,
    const char *key,
    const char **out_value,
    const char **out_value_end
) {
    if (object_start == NULL || object_end == NULL || key == NULL ||
        object_start >= object_end || *object_start != '{') {
        return DCC_ERR_INVALID_ARG;
    }

    size_t key_len = strlen(key);
    const char *p = dcc_skip_json_ws(object_start + 1, object_end);
    while (p < object_end) {
        p = dcc_skip_json_ws(p, object_end);
        if (p < object_end && *p == '}') {
            return DCC_ERR_NOT_FOUND;
        }
        if (p >= object_end || *p != '"') {
            return DCC_ERR_INVALID_ARG;
        }

        const char *name = p + 1;
        const char *name_end = name;
        int name_escaped = 0;
        while (name_end < object_end) {
            if (*name_end == '\\') {
                name_escaped = 1;
                name_end += 2;
                continue;
            }
            if (*name_end == '"') {
                break;
            }
            ++name_end;
        }
        if (name_end >= object_end) {
            return DCC_ERR_INVALID_ARG;
        }

        const char *colon = dcc_skip_json_ws(name_end + 1, object_end);
        if (colon >= object_end || *colon != ':') {
            return DCC_ERR_INVALID_ARG;
        }
        const char *value = dcc_skip_json_ws(colon + 1, object_end);
        const char *value_end = NULL;
        dcc_status_t status = dcc_json_skip_value(value, object_end, &value_end);
        if (status != DCC_OK) {
            return status;
        }

        if (!name_escaped && (size_t)(name_end - name) == key_len &&
            memcmp(name, key, key_len) == 0) {
            *out_value = value;
            *out_value_end = value_end;
            return DCC_OK;
        }

        p = dcc_skip_json_ws(value_end, object_end);
        if (p < object_end && *p == ',') {
            ++p;
            continue;
        }
        if (p < object_end && *p == '}') {
            return DCC_ERR_NOT_FOUND;
        }
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_ERR_INVALID_ARG;
}

static int dcc_webhook_payload_is_ping(const char *body, size_t body_len) {
    const char *root_start = NULL;
    const char *root_end = NULL;
    const char *value = NULL;
    const char *value_end = NULL;

    if (dcc_webhook_find_root_object(body, body_len, &root_start, &root_end) != DCC_OK ||
        dcc_json_find_top_level_value(root_start, root_end, "type", &value, &value_end) != DCC_OK) {
        return 0;
    }
    value_end = dcc_trim_json_ws_end(value, value_end);
    return value_end == value + 1 && *value == '0';
}

static dcc_status_t dcc_webhook_find_event_object(
    const char *body,
    size_t body_len,
    const char **out_start,
    const char **out_end
) {
    const char *root_start = NULL;
    const char *root_end = NULL;
    const char *value = NULL;
    const char *value_end = NULL;
    dcc_status_t status = dcc_webhook_find_root_object(body, body_len, &root_start, &root_end);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_json_find_top_level_value(root_start, root_end, "event", &value, &value_end);
    if (status != DCC_OK) {
        return status;
    }
    if (value >= value_end || *value != '{') {
        return DCC_ERR_INVALID_ARG;
    }
    *out_start = value;
    *out_end = value_end;
    return DCC_OK;
}

static dcc_status_t dcc_webhook_find_object_field(
    const char *object_start,
    const char *object_end,
    const char *key,
    const char **out_start,
    const char **out_end
) {
    const char *value = NULL;
    const char *value_end = NULL;
    dcc_status_t status = dcc_json_find_top_level_value(object_start, object_end, key, &value, &value_end);
    if (status != DCC_OK) {
        return status;
    }
    if (value >= value_end || *value != '{') {
        return DCC_ERR_INVALID_ARG;
    }
    *out_start = value;
    *out_end = value_end;
    return DCC_OK;
}

static dcc_status_t dcc_webhook_parse_event_string_field(
    const char *body,
    size_t body_len,
    const char *key,
    const char **out,
    size_t *out_len
) {
    const char *object_start = NULL;
    const char *object_end = NULL;
    dcc_status_t status = dcc_webhook_find_event_object(body, body_len, &object_start, &object_end);
    if (status != DCC_OK) {
        return status;
    }

    const char *value = NULL;
    const char *value_end = NULL;
    status = dcc_json_find_top_level_value(object_start, object_end, key, &value, &value_end);
    if (status != DCC_OK) {
        return status;
    }
    if (value >= value_end || *value != '"') {
        return DCC_ERR_INVALID_ARG;
    }
    ++value;
    const char *string_end = value;
    while (string_end < value_end && *string_end != '"') {
        if (*string_end == '\\') {
            return DCC_ERR_INVALID_ARG;
        }
        ++string_end;
    }
    if (string_end >= value_end) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = value;
    *out_len = (size_t)(string_end - value);
    return DCC_OK;
}

static dcc_status_t dcc_webhook_parse_event_type(
    const char *body,
    size_t body_len,
    char *out,
    size_t out_len
) {
    const char *value = NULL;
    size_t len = 0;
    dcc_status_t status = dcc_webhook_parse_event_string_field(body, body_len, "type", &value, &len);
    if (status != DCC_OK) {
        return status;
    }
    if (len == 0 || len >= out_len) {
        return DCC_ERR_INVALID_ARG;
    }
    memcpy(out, value, len);
    out[len] = '\0';
    return DCC_OK;
}

static dcc_status_t dcc_webhook_parse_snowflake_field(
    const char *body,
    size_t body_len,
    const char *key,
    dcc_snowflake_t *out
) {
    const char *root_start = NULL;
    const char *root_end = NULL;
    const char *value = NULL;
    const char *value_end = NULL;
    dcc_status_t status = dcc_webhook_find_root_object(body, body_len, &root_start, &root_end);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_json_find_top_level_value(root_start, root_end, key, &value, &value_end);
    if (status != DCC_OK) {
        return status;
    }

    int quoted = value < value_end && *value == '"';
    if (quoted) {
        if (value_end <= value + 1 || *(value_end - 1) != '"') {
            return DCC_ERR_INVALID_ARG;
        }
        ++value;
        --value_end;
    }
    value_end = dcc_trim_json_ws_end(value, value_end);
    if (value >= value_end || !isdigit((unsigned char)*value)) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t result = 0;
    while (value < value_end && isdigit((unsigned char)*value)) {
        uint64_t digit = (uint64_t)(*value - '0');
        if (result > (UINT64_MAX - digit) / 10U) {
            return DCC_ERR_INVALID_ARG;
        }
        result = result * 10U + digit;
        ++value;
    }
    if (value != value_end) {
        return DCC_ERR_INVALID_ARG;
    }

    *out = (dcc_snowflake_t)result;
    return DCC_OK;
}

dcc_status_t dcc_webhook_event_parse(
    const char *body,
    size_t body_len,
    dcc_webhook_event_t *out
) {
    if (body == NULL || body_len == 0 || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    out->type = DCC_WEBHOOK_EVENT_UNKNOWN;
    out->type_name = NULL;
    out->timestamp = NULL;
    out->timestamp_len = 0;
    out->application_id = 0;
    out->event_json = NULL;
    out->event_json_len = 0;
    out->data_json = NULL;
    out->data_json_len = 0;
    out->raw_body = body;
    out->raw_body_len = body_len;

    dcc_snowflake_t application_id = 0;
    if (dcc_webhook_parse_snowflake_field(body, body_len, "application_id", &application_id) == DCC_OK) {
        out->application_id = application_id;
    }

    if (dcc_webhook_payload_is_ping(body, body_len)) {
        out->type = DCC_WEBHOOK_EVENT_PING;
        out->type_name = dcc_webhook_event_type_name(out->type);
        return DCC_OK;
    }

    char type_name[80];
    dcc_status_t status = dcc_webhook_parse_event_type(body, body_len, type_name, sizeof(type_name));
    if (status != DCC_OK) {
        return status;
    }

    const char *event_start = NULL;
    const char *event_end = NULL;
    status = dcc_webhook_find_event_object(body, body_len, &event_start, &event_end);
    if (status != DCC_OK) {
        return status;
    }
    out->event_json = event_start;
    out->event_json_len = (size_t)(event_end - event_start);

    const char *data_start = NULL;
    const char *data_end = NULL;
    status = dcc_webhook_find_object_field(event_start, event_end, "data", &data_start, &data_end);
    if (status == DCC_OK) {
        out->data_json = data_start;
        out->data_json_len = (size_t)(data_end - data_start);
    } else if (status != DCC_ERR_NOT_FOUND) {
        return status;
    }

    out->type = dcc_webhook_event_type_from_string(type_name);
    out->type_name = dcc_webhook_event_type_name(out->type);

    const char *timestamp = NULL;
    size_t timestamp_len = 0;
    status = dcc_webhook_parse_event_string_field(body, body_len, "timestamp", &timestamp, &timestamp_len);
    if (status == DCC_OK) {
        out->timestamp = timestamp;
        out->timestamp_len = timestamp_len;
    } else if (status != DCC_ERR_NOT_FOUND) {
        return status;
    }
    return DCC_OK;
}

static void dcc_webhook_events_route_callback(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
) {
    dcc_webhook_event_route_t *route = (dcc_webhook_event_route_t *)user_data;
    dcc_webhook_event_t event = {.size = sizeof(event)};
    dcc_status_t status = dcc_webhook_event_parse(
        dcc_interaction_request_body(request),
        dcc_interaction_request_body_len(request),
        &event
    );
    if (status != DCC_OK) {
        (void)dcc_interaction_request_reply_text(request, 400, "invalid webhook event payload");
        return;
    }
    if (event.type == DCC_WEBHOOK_EVENT_PING) {
        (void)dcc_interaction_request_set_response(request, 204, "application/json", NULL, 0);
        return;
    }
    if (route != NULL && route->callback != NULL) {
        status = route->callback(server, request, &event, route->user_data);
        if (status != DCC_OK) {
            (void)dcc_interaction_request_reply_text(request, 500, "webhook event handler failed");
            return;
        }
    }
    (void)dcc_interaction_request_set_response(request, 204, "application/json", NULL, 0);
}

dcc_status_t dcc_webhook_events_add_route(
    dcc_interaction_server_t *server,
    const char *path,
    dcc_webhook_event_route_t *route
) {
    if (server == NULL || path == NULL || path[0] == '\0' ||
        route == NULL || route->size < sizeof(*route)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_interaction_route_t interaction_route = {
        .size = sizeof(interaction_route),
        .method = "POST",
        .path = path,
        .flags = route->flags,
        .callback = dcc_webhook_events_route_callback,
        .user_data = route,
    };
    return dcc_interaction_server_add_route(server, &interaction_route);
}
