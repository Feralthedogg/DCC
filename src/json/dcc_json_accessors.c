#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_json_type_t dcc_json_typeof(const dcc_json_t *json) {
    return json != NULL ? json->type : DCC_JSON_NULL;
}

const dcc_json_t *dcc_json_object_get(const dcc_json_t *json, const char *key) {
    if (json == NULL || key == NULL || json->type != DCC_JSON_OBJECT) {
        return NULL;
    }
    for (size_t i = 0; i < json->as.object.len; ++i) {
        if (strcmp(json->as.object.entries[i].key, key) == 0) {
            return json->as.object.entries[i].value;
        }
    }
    return NULL;
}

const dcc_json_t *dcc_json_array_get(const dcc_json_t *json, size_t index) {
    if (json == NULL || json->type != DCC_JSON_ARRAY || index >= json->as.array.len) {
        return NULL;
    }
    return json->as.array.items[index];
}

size_t dcc_json_array_size(const dcc_json_t *json) {
    return json != NULL && json->type == DCC_JSON_ARRAY ? json->as.array.len : 0;
}

const char *dcc_json_string(const dcc_json_t *json) {
    return json != NULL && json->type == DCC_JSON_STRING ? json->as.string : NULL;
}

int dcc_json_bool(const dcc_json_t *json, int *out) {
    if (json == NULL || out == NULL || json->type != DCC_JSON_BOOL) {
        return -1;
    }
    *out = json->as.boolean;
    return 0;
}

int dcc_json_i64(const dcc_json_t *json, int64_t *out) {
    if (json == NULL || out == NULL || json->type != DCC_JSON_NUMBER) {
        return -1;
    }
    return dcc_json_parse_i64_ascii(json->as.number, strlen(json->as.number), out);
}

int dcc_json_u64(const dcc_json_t *json, uint64_t *out) {
    if (json == NULL || out == NULL || json->type != DCC_JSON_NUMBER || json->as.number[0] == '-') {
        return -1;
    }
    return dcc_json_parse_u64_ascii(json->as.number, strlen(json->as.number), out);
}
