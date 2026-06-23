#include <dcc/store.h>

#include <stdio.h>
#include <string.h>

static dcc_status_t dcc_store_managed_parse_snowflake(
    const char *value,
    size_t len,
    dcc_snowflake_t *out
) {
    if (value == NULL || len == 0U || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_snowflake_t parsed = 0;
    for (size_t i = 0; i < len; ++i) {
        if (value[i] < '0' || value[i] > '9') {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_snowflake_t digit = (dcc_snowflake_t)(value[i] - '0');
        if (parsed > (((dcc_snowflake_t)-1) - digit) / 10U) {
            return DCC_ERR_INVALID_ARG;
        }
        parsed = parsed * 10U + digit;
    }

    *out = parsed;
    return DCC_OK;
}

void dcc_store_managed_message_binding_init(
    dcc_store_managed_message_binding_t *binding,
    dcc_store_t *store,
    const char *key
) {
    if (binding == NULL) {
        return;
    }
    binding->size = sizeof(*binding);
    binding->store = store;
    binding->key = key;
}

dcc_status_t dcc_store_load_managed_message_ref(
    dcc_managed_message_ref_t *out_ref,
    void *user_data
) {
    dcc_store_managed_message_binding_t *binding =
        (dcc_store_managed_message_binding_t *)user_data;
    if (out_ref == NULL || binding == NULL || binding->store == NULL || binding->key == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char value[64];
    size_t value_len = 0;
    dcc_status_t status =
        dcc_store_get(binding->store, binding->key, value, sizeof(value), &value_len);
    if (status != DCC_OK) {
        return status;
    }
    if (value_len >= sizeof(value)) {
        return DCC_ERR_STATE;
    }

    const char *colon = strchr(value, ':');
    if (colon == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_managed_message_ref_t parsed = {0};
    status = dcc_store_managed_parse_snowflake(
        value,
        (size_t)(colon - value),
        &parsed.channel_id
    );
    if (status == DCC_OK) {
        status = dcc_store_managed_parse_snowflake(
            colon + 1,
            value_len - (size_t)(colon - value) - 1U,
            &parsed.message_id
        );
    }
    if (status != DCC_OK) {
        return status;
    }

    *out_ref = parsed;
    return DCC_OK;
}

dcc_status_t dcc_store_save_managed_message_ref(
    const dcc_managed_message_ref_t *ref,
    void *user_data
) {
    dcc_store_managed_message_binding_t *binding =
        (dcc_store_managed_message_binding_t *)user_data;
    if (ref == NULL || binding == NULL || binding->store == NULL || binding->key == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char value[64];
    int written = snprintf(
        value,
        sizeof(value),
        "%llu:%llu",
        (unsigned long long)ref->channel_id,
        (unsigned long long)ref->message_id
    );
    if (written <= 0 || (size_t)written >= sizeof(value)) {
        return DCC_ERR_RUNTIME;
    }

    return dcc_store_set(binding->store, binding->key, value);
}
