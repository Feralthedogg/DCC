#include <dcc/rest/result.h>

#include <dcc/rest/response_helpers.h>

#include <stddef.h>

static dcc_status_t dcc_rest_result_response_view(
    const dcc_rest_result_t *result,
    dcc_rest_response_t *response
) {
    if (result == NULL || response == NULL ||
        result->size < offsetof(dcc_rest_result_t, body_len) +
                           sizeof(result->body_len) ||
        result->version != DCC_REST_RESULT_VERSION ||
        ((result->body == NULL) != (result->body_len == 0U))) {
        return DCC_ERR_INVALID_ARG;
    }
    *response = (dcc_rest_response_t){
        sizeof(*response), result->http_status, result->transport_status,
        result->body, result->body_len};
    return DCC_OK;
}

#define DCC_RESULT_FIELD_WRAPPER(name_, legacy_, type_) \
dcc_status_t name_(const dcc_rest_result_t *result, const char *field, type_ *out) { \
    dcc_rest_response_t response; \
    dcc_status_t status = dcc_rest_result_response_view(result, &response); \
    return status == DCC_OK ? legacy_(&response, field, out) : status; \
}

DCC_RESULT_FIELD_WRAPPER(dcc_rest_result_snowflake_field,
    dcc_rest_response_snowflake_field, dcc_snowflake_t)
DCC_RESULT_FIELD_WRAPPER(dcc_rest_result_u64_field,
    dcc_rest_response_u64_field, uint64_t)
DCC_RESULT_FIELD_WRAPPER(dcc_rest_result_i64_field,
    dcc_rest_response_i64_field, int64_t)
DCC_RESULT_FIELD_WRAPPER(dcc_rest_result_bool_field,
    dcc_rest_response_bool_field, int)

dcc_status_t dcc_rest_result_string_field_len(
    const dcc_rest_result_t *result, const char *field,
    char *out, size_t out_cap, size_t *out_len
) {
    dcc_rest_response_t response;
    dcc_status_t status = dcc_rest_result_response_view(result, &response);
    return status == DCC_OK
               ? dcc_rest_response_string_field_len(
                     &response, field, out, out_cap, out_len)
               : status;
}

dcc_status_t dcc_rest_result_string_field(
    const dcc_rest_result_t *result, const char *field,
    char *out, size_t out_cap
) {
    return dcc_rest_result_string_field_len(
        result, field, out, out_cap, NULL);
}

dcc_status_t dcc_rest_result_message_id(
    const dcc_rest_result_t *result, dcc_snowflake_t *out
) { return dcc_rest_result_snowflake_field(result, "id", out); }
dcc_status_t dcc_rest_result_channel_id(
    const dcc_rest_result_t *result, dcc_snowflake_t *out
) { return dcc_rest_result_snowflake_field(result, "channel_id", out); }
dcc_status_t dcc_rest_result_guild_id(
    const dcc_rest_result_t *result, dcc_snowflake_t *out
) { return dcc_rest_result_snowflake_field(result, "guild_id", out); }

#undef DCC_RESULT_FIELD_WRAPPER
