#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_stage_instance_object_field(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_stage_instance_buffers_t *buffers
) {
    if (parser == NULL || stage_instance == NULL || buffers == NULL) {
        return DCC_ERR_JSON;
    }

    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_stage_instance_id_field(
        parser,
        stage_instance,
        key_id,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_stage_instance_scalar_field(
            parser,
            stage_instance,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_stage_instance_text_field(
            parser,
            stage_instance,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
