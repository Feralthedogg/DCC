#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_stage_instance_list_object(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    char *topic,
    size_t topic_cap
) {
    if (parser == NULL || stage_instance == NULL || topic == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_gateway_stage_instance_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_stage_instance_buffers(
        &buffers,
        topic,
        topic_cap
    );
    if (status != DCC_OK) {
        return status;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(stage_instance, 0, sizeof(*stage_instance));
    parser->cur++;
    return dcc_json_gateway_parse_stage_instance_object_fields(
        parser,
        stage_instance,
        &buffers
    );
}
