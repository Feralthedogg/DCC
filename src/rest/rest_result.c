#include <dcc/rest/result.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DCC_REST_RESULT_STATUS_SIZE \
    (offsetof(dcc_rest_result_t, http_status) + sizeof(((dcc_rest_result_t *)0)->http_status))

static int dcc_rest_result_valid_prefix(
    const dcc_rest_result_t *result,
    size_t required_size
) {
    const size_t version_size = offsetof(dcc_rest_result_t, version) +
        sizeof(((dcc_rest_result_t *)0)->version);
    return result != NULL && result->size >= version_size &&
        result->version == DCC_REST_RESULT_VERSION && result->size >= required_size;
}

uint8_t dcc_rest_result_ok(const dcc_rest_result_t *result) {
    if (!dcc_rest_result_valid_prefix(result, DCC_REST_RESULT_STATUS_SIZE)) {
        return 0U;
    }
    return result->transport_status == DCC_OK &&
        result->http_status >= 200U && result->http_status <= 299U
        ? 1U
        : 0U;
}

dcc_status_t dcc_rest_result_status(const dcc_rest_result_t *result) {
    if (!dcc_rest_result_valid_prefix(result, DCC_REST_RESULT_STATUS_SIZE)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (result->transport_status != DCC_OK) {
        return result->transport_status;
    }
    return result->http_status >= 200U && result->http_status <= 299U
        ? DCC_OK
        : DCC_ERR_DISCORD;
}

dcc_status_t dcc_rest_result_clone(
    const dcc_rest_result_t *source,
    dcc_rest_result_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (!dcc_rest_result_valid_prefix(source, sizeof(*source)) ||
        (source->body_len != 0U && source->body == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t message_len = source->discord_message != NULL
        ? strlen(source->discord_message)
        : 0U;
    if (message_len == SIZE_MAX || source->body_len == SIZE_MAX ||
        message_len + 1U > SIZE_MAX - sizeof(*source) ||
        source->body_len + 1U >
            SIZE_MAX - sizeof(*source) - message_len - 1U) {
        return DCC_ERR_NOMEM;
    }
    size_t allocation_size = sizeof(*source) + message_len + 1U +
        source->body_len + 1U;
    dcc_rest_result_t *copy = (dcc_rest_result_t *)calloc(1U, allocation_size);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    *copy = *source;
    copy->size = sizeof(*copy);
    copy->version = DCC_REST_RESULT_VERSION;

    char *cursor = (char *)(copy + 1);
    if (source->discord_message != NULL) {
        memcpy(cursor, source->discord_message, message_len + 1U);
        copy->discord_message = cursor;
    } else {
        copy->discord_message = NULL;
    }
    cursor += message_len + 1U;
    if (source->body_len != 0U) {
        memcpy(cursor, source->body, source->body_len);
        copy->body = cursor;
        cursor[source->body_len] = '\0';
    } else {
        copy->body = NULL;
    }
    *out = copy;
    return DCC_OK;
}

void dcc_rest_result_free(dcc_rest_result_t *result) {
    free(result);
}
