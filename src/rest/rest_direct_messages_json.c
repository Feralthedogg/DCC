#include "internal/json/dcc_json.h"
#include "internal/rest/dcc_rest_direct_messages_internal.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

static int dcc_rest_parse_snowflake_digits(const char *data, size_t len, dcc_snowflake_t *out) {
    if (data == NULL || len == 0 || out == NULL) {
        return 0;
    }

    uint64_t value = 0;
    for (size_t i = 0; i < len; ++i) {
        unsigned char ch = (unsigned char)data[i];
        if (ch < '0' || ch > '9') {
            return 0;
        }
        uint64_t digit = (uint64_t)(ch - '0');
        if (value > (UINT64_MAX - digit) / 10U) {
            return 0;
        }
        value = value * 10U + digit;
    }
    if (value == 0) {
        return 0;
    }
    *out = (dcc_snowflake_t)value;
    return 1;
}

dcc_status_t dcc_rest_parse_channel_id_from_json(
    const char *body,
    size_t body_len,
    dcc_snowflake_t *out
) {
    if (body == NULL || body_len == 0 || out == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_t *root = NULL;
    dcc_status_t status = dcc_json_parse(body, body_len, &root);
    if (status != DCC_OK) {
        return status;
    }

    const dcc_json_t *id = dcc_json_object_get(root, "id");
    uint64_t channel_id = 0;
    if (dcc_json_u64(id, &channel_id) == 0 && channel_id != 0) {
        *out = (dcc_snowflake_t)channel_id;
        dcc_json_free(root);
        return DCC_OK;
    }

    const char *id_text = dcc_json_string(id);
    if (id_text != NULL &&
        dcc_rest_parse_snowflake_digits(id_text, strlen(id_text), out)) {
        dcc_json_free(root);
        return DCC_OK;
    }

    dcc_json_free(root);
    return DCC_ERR_JSON;
}
