#include "json_smoke_support.h"

int dcc_json_smoke_invalid_inputs(void) {
    dcc_json_t *root = NULL;
    dcc_json_gateway_payload_t *payload = dcc_json_smoke_payload();
    if (payload == NULL) {
        fprintf(stderr, "json smoke payload allocation failed\n");
        return 1;
    }

    const char bad_utf8[] = "{\"x\":\"" "\xC0" "\xAF" "\"}";
    root = NULL;
    if (dcc_json_parse(bad_utf8, sizeof(bad_utf8) - 1, &root) != DCC_ERR_JSON) {
        dcc_json_free(root);
        fprintf(stderr, "invalid utf8 was accepted\n");
        return 1;
    }

    const char bad_surrogate[] = "{\"x\":\"\\uD800\"}";
    root = NULL;
    if (dcc_json_parse(bad_surrogate, sizeof(bad_surrogate) - 1, &root) != DCC_ERR_JSON) {
        dcc_json_free(root);
        fprintf(stderr, "invalid surrogate was accepted\n");
        return 1;
    }

    const char gateway_bad_control[] =
        "{\"t\":\"MESSAGE_CREATE\",\"s\":1,\"op\":0,\"d\":{\"content\":\"bad\nraw\"}}";
    if (dcc_json_stage1_validate(gateway_bad_control, sizeof(gateway_bad_control) - 1) != DCC_ERR_JSON) {
        fprintf(stderr, "stage1 accepted raw control string\n");
        return 1;
    }
    if (dcc_json_parse_gateway_payload(gateway_bad_control, sizeof(gateway_bad_control) - 1, payload) != DCC_ERR_JSON) {
        fprintf(stderr, "gateway raw control string was accepted\n");
        return 1;
    }

    const char gateway_bad_utf8[] =
        "{\"t\":\"MESSAGE_CREATE\",\"s\":1,\"op\":0,\"d\":{\"content\":\"" "\xC0" "\xAF" "\"}}";
    if (dcc_json_parse_gateway_payload(gateway_bad_utf8, sizeof(gateway_bad_utf8) - 1, payload) != DCC_ERR_JSON) {
        fprintf(stderr, "gateway invalid utf8 string was accepted\n");
        return 1;
    }

    const char gateway_unclosed_string[] =
        "{\"t\":\"MESSAGE_CREATE\",\"s\":1,\"op\":0,\"d\":{\"content\":\"unterminated}}";
    if (dcc_json_stage1_validate(gateway_unclosed_string, sizeof(gateway_unclosed_string) - 1) != DCC_ERR_JSON) {
        fprintf(stderr, "stage1 accepted unterminated string\n");
        return 1;
    }
    if (dcc_json_parse_gateway_payload(gateway_unclosed_string, sizeof(gateway_unclosed_string) - 1, payload) != DCC_ERR_JSON) {
        fprintf(stderr, "gateway unterminated string was accepted\n");
        return 1;
    }

    return 0;
}
