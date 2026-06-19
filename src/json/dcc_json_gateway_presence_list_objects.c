#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_list_object(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap,
    char *status_text,
    size_t status_text_cap,
    char *desktop_status,
    size_t desktop_status_cap,
    char *mobile_status,
    size_t mobile_status_cap,
    char *web_status,
    size_t web_status_cap,
    dcc_gateway_activity_t *activities,
    char *activity_names,
    size_t activity_name_stride,
    char *activity_states,
    size_t activity_state_stride,
    char *activity_details,
    size_t activity_detail_stride,
    char *activity_urls,
    size_t activity_url_stride,
    char *activity_emoji_names,
    size_t activity_emoji_name_stride
) {
    if (parser == NULL || presence == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_gateway_presence_list_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_presence_list_buffers(
        &buffers,
        username,
        username_cap,
        global_name,
        global_name_cap,
        avatar,
        avatar_cap,
        status_text,
        status_text_cap,
        desktop_status,
        desktop_status_cap,
        mobile_status,
        mobile_status_cap,
        web_status,
        web_status_cap,
        activities,
        activity_names,
        activity_name_stride,
        activity_states,
        activity_state_stride,
        activity_details,
        activity_detail_stride,
        activity_urls,
        activity_url_stride,
        activity_emoji_names,
        activity_emoji_name_stride
    );
    if (status != DCC_OK) {
        return status;
    }

    return dcc_json_gateway_parse_presence_list_object_with_buffers(parser, presence, &buffers);
}
