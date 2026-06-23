#include <dcc/store.h>

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static int dcc_store_text_has_newline(const char *value) {
    return value != NULL && (strchr(value, '\n') != NULL || strchr(value, '\r') != NULL);
}

static int dcc_store_key_valid(const char *key) {
    return key != NULL &&
        key[0] != '\0' &&
        strchr(key, '=') == NULL &&
        !dcc_store_text_has_newline(key);
}

static int dcc_store_value_valid(const char *value) {
    return value != NULL && !dcc_store_text_has_newline(value);
}

static dcc_status_t dcc_store_parse_u64(const char *value, uint64_t *out) {
    if (value == NULL || value[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t parsed = 0;
    for (const char *cursor = value; *cursor != '\0'; ++cursor) {
        if (*cursor < '0' || *cursor > '9') {
            return DCC_ERR_INVALID_ARG;
        }
        uint64_t digit = (uint64_t)(*cursor - '0');
        if (parsed > (UINT64_MAX - digit) / 10U) {
            return DCC_ERR_INVALID_ARG;
        }
        parsed = parsed * 10U + digit;
    }

    *out = parsed;
    return DCC_OK;
}

dcc_status_t dcc_store_init(
    dcc_store_t *store,
    const dcc_store_options_t *options
) {
    if (store == NULL || options == NULL || options->get == NULL || options->set == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    memset(store, 0, sizeof(*store));
    store->size = sizeof(*store);
    store->get = options->get;
    store->set = options->set;
    store->delete_key = options->delete_key;
    store->cleanup = options->cleanup;
    store->user_data = options->user_data;
    return DCC_OK;
}

void dcc_store_deinit(dcc_store_t *store) {
    if (store == NULL) {
        return;
    }
    if (store->cleanup != NULL) {
        store->cleanup(store->user_data);
    }
    memset(store, 0, sizeof(*store));
}

dcc_status_t dcc_store_get(
    const dcc_store_t *store,
    const char *key,
    char *out_value,
    size_t out_value_size,
    size_t *out_value_len
) {
    if (store == NULL || store->get == NULL || !dcc_store_key_valid(key)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_value == NULL && out_value_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return store->get(store->user_data, key, out_value, out_value_size, out_value_len);
}

dcc_status_t dcc_store_set(
    dcc_store_t *store,
    const char *key,
    const char *value
) {
    if (store == NULL ||
        store->set == NULL ||
        !dcc_store_key_valid(key) ||
        !dcc_store_value_valid(value)) {
        return DCC_ERR_INVALID_ARG;
    }
    return store->set(store->user_data, key, value);
}

dcc_status_t dcc_store_delete(dcc_store_t *store, const char *key) {
    if (store == NULL || store->delete_key == NULL || !dcc_store_key_valid(key)) {
        return DCC_ERR_INVALID_ARG;
    }
    return store->delete_key(store->user_data, key);
}

dcc_status_t dcc_store_get_u64(
    const dcc_store_t *store,
    const char *key,
    uint64_t *out_value
) {
    char value[32];
    size_t value_len = 0;
    if (out_value == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_store_get(store, key, value, sizeof(value), &value_len);
    if (status != DCC_OK) {
        return status;
    }
    if (value_len >= sizeof(value)) {
        return DCC_ERR_STATE;
    }
    return dcc_store_parse_u64(value, out_value);
}

dcc_status_t dcc_store_set_u64(
    dcc_store_t *store,
    const char *key,
    uint64_t value
) {
    char buffer[32];
    int written = snprintf(buffer, sizeof(buffer), "%" PRIu64, value);
    if (written <= 0 || (size_t)written >= sizeof(buffer)) {
        return DCC_ERR_RUNTIME;
    }
    return dcc_store_set(store, key, buffer);
}
