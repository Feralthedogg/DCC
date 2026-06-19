#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_subscription_time_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_CURRENT_PERIOD_START) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_current_period_start,
            sizeof(out->gateway_current_period_start),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.current_period_start = has_value ? out->gateway_current_period_start : NULL;
        out->subscription.current_period_start = has_value ? out->gateway_current_period_start : NULL;
        out->has_subscription = 1;
    }
    else if (key_id == DCC_JSON_KEY_CURRENT_PERIOD_END) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_current_period_end,
            sizeof(out->gateway_current_period_end),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.current_period_end = has_value ? out->gateway_current_period_end : NULL;
        out->subscription.current_period_end = has_value ? out->gateway_current_period_end : NULL;
        out->has_subscription = 1;
    }
    else if (key_id == DCC_JSON_KEY_CANCELED_AT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_canceled_at,
            sizeof(out->gateway_canceled_at),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.canceled_at = has_value ? out->gateway_canceled_at : NULL;
        out->subscription.canceled_at = has_value ? out->gateway_canceled_at : NULL;
        out->has_subscription = 1;
    }
    else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
