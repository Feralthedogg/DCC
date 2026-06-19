#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_attachment_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t key,
    size_t index
) {
    dcc_status_t status = dcc_json_gateway_parse_attachment_object(
        parser,
        &out->interaction_resolved_attachments[index],
        out->interaction_resolved_attachment_filenames[index],
        sizeof(out->interaction_resolved_attachment_filenames[index]),
        out->interaction_resolved_attachment_descriptions[index],
        sizeof(out->interaction_resolved_attachment_descriptions[index]),
        out->interaction_resolved_attachment_urls[index],
        sizeof(out->interaction_resolved_attachment_urls[index]),
        out->interaction_resolved_attachment_proxy_urls[index],
        sizeof(out->interaction_resolved_attachment_proxy_urls[index]),
        out->interaction_resolved_attachment_content_types[index],
        sizeof(out->interaction_resolved_attachment_content_types[index]),
        out->interaction_resolved_attachment_waveforms[index],
        sizeof(out->interaction_resolved_attachment_waveforms[index])
    );
    if (status != DCC_OK) {
        return status;
    }
    if (out->interaction_resolved_attachments[index].id == 0) {
        out->interaction_resolved_attachments[index].id = key;
    }
    return DCC_OK;
}
