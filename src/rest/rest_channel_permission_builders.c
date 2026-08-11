#include "internal/rest/dcc_rest_channel_permission_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

#include <stdint.h>

static dcc_status_t permission_overwrite_validate(
    const dcc_channel_permission_overwrite_t *overwrite,
    uint8_t include_id,
    size_t expected_size,
    dcc_endpoint_record_view_t *out_view
) {
    const uint64_t known = DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_ALLOW |
        DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_DENY;
    dcc_status_t status = dcc_endpoint_record_read(
        overwrite, offsetof(dcc_channel_permission_overwrite_t, version),
        offsetof(dcc_channel_permission_overwrite_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_channel_permission_overwrite_t, type),
        DCC_CHANNEL_PERMISSION_OVERWRITE_VERSION, known, out_view
    );
    if (status != DCC_OK || (expected_size != 0U && out_view->size != expected_size) ||
        dcc_endpoint_field_partially_covered(
            out_view->size,
            offsetof(dcc_channel_permission_overwrite_t, allow),
            sizeof(overwrite->allow)
        ) ||
        dcc_endpoint_field_partially_covered(
            out_view->size,
            offsetof(dcc_channel_permission_overwrite_t, deny),
            sizeof(overwrite->deny)
        ) ||
        include_id > 1U || (include_id != 0U && overwrite->id == 0U) ||
        overwrite->type > DCC_CHANNEL_OVERWRITE_MEMBER ||
        !dcc_endpoint_present_field_covered(
            out_view, DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_ALLOW,
            offsetof(dcc_channel_permission_overwrite_t, allow),
            sizeof(overwrite->allow)
        ) ||
        !dcc_endpoint_present_field_covered(
            out_view, DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_DENY,
            offsetof(dcc_channel_permission_overwrite_t, deny),
            sizeof(overwrite->deny)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_build_channel_permission_overwrite_body(
    const dcc_channel_permission_overwrite_t *overwrite,
    uint8_t include_id,
    dcc_rest_buffer_t *body
) {
    dcc_endpoint_record_view_t view;
    if (body == NULL || permission_overwrite_validate(
            overwrite, include_id, 0U, &view
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK && include_id != 0) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "id", overwrite->id);
    }
    if (status == DCC_OK &&
        (view.present & DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_ALLOW) != 0U) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "allow", overwrite->allow);
    }
    if (status == DCC_OK &&
        (view.present & DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_DENY) != 0U) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "deny", overwrite->deny);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(body, &first, "type", overwrite->type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_build_channel_permission_overwrites_array(
    const dcc_channel_permission_overwrite_t *overwrites,
    size_t overwrite_count,
    dcc_rest_buffer_t *body
) {
    if (body == NULL || (overwrite_count != 0U && overwrites == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t stride = 0U;
    if (overwrite_count != 0U) {
        uintptr_t address = (uintptr_t)overwrites;
        dcc_endpoint_record_view_t first;
        if (address % _Alignof(dcc_channel_permission_overwrite_t) != 0U ||
            permission_overwrite_validate(overwrites, 1U, 0U, &first) != DCC_OK ||
            first.size % _Alignof(dcc_channel_permission_overwrite_t) != 0U ||
            overwrite_count > SIZE_MAX / first.size ||
            address > UINTPTR_MAX - (overwrite_count * first.size - 1U)) {
            return DCC_ERR_INVALID_ARG;
        }
        stride = first.size;
        for (size_t i = 0U; i < overwrite_count; ++i) {
            const dcc_channel_permission_overwrite_t *item =
                (const dcc_channel_permission_overwrite_t *)
                    ((const unsigned char *)overwrites + i * stride);
            dcc_endpoint_record_view_t view;
            if (permission_overwrite_validate(item, 1U, stride, &view) != DCC_OK)
                return DCC_ERR_INVALID_ARG;
            for (size_t j = 0U; j < i; ++j) {
                const dcc_channel_permission_overwrite_t *prior =
                    (const dcc_channel_permission_overwrite_t *)
                        ((const unsigned char *)overwrites + j * stride);
                if (prior->id == item->id) return DCC_ERR_INVALID_ARG;
            }
        }
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "[");
    for (size_t i = 0; i < overwrite_count && status == DCC_OK; ++i) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(body, ",");
        }
        if (status == DCC_OK) {
            const dcc_channel_permission_overwrite_t *item =
                (const dcc_channel_permission_overwrite_t *)
                    ((const unsigned char *)overwrites + i * stride);
            status = dcc_rest_build_channel_permission_overwrite_body(item, 1, body);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "]");
    }
    return status;
}
