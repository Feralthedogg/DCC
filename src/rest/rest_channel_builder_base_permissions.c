#include "internal/rest/dcc_rest_channel_builder_json_internal.h"
#include "internal/rest/dcc_rest_channel_permission_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_channel_append_permission_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
) {
    if (params->permission_overwrite_count == 0) {
        return DCC_OK;
    }

    dcc_status_t status = dcc_rest_json_member_sep(body, first);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"permission_overwrites\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_build_channel_permission_overwrites_array(
            params->permission_overwrites,
            params->permission_overwrite_count,
            body
        );
    }
    return status;
}
