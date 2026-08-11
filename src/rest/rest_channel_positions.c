#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdint.h>

static dcc_status_t channel_position_validate(
    const dcc_channel_position_t *position,
    size_t expected_size,
    dcc_endpoint_record_view_t *out_view
) {
    const uint64_t known = DCC_CHANNEL_POSITION_PRESENT_POSITION |
        DCC_CHANNEL_POSITION_PRESENT_LOCK_PERMISSIONS |
        DCC_CHANNEL_POSITION_PRESENT_PARENT_ID |
        DCC_CHANNEL_POSITION_PRESENT_FLAGS;
    dcc_status_t status = dcc_endpoint_record_read(
        position, offsetof(dcc_channel_position_t, version),
        offsetof(dcc_channel_position_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_channel_position_t, channel_id),
        DCC_CHANNEL_POSITION_VERSION, known, out_view
    );
    uint64_t nulls = status == DCC_OK && dcc_endpoint_field_covered(
        out_view->size, offsetof(dcc_channel_position_t, nulls),
        sizeof(position->nulls)
    ) ? position->nulls : 0U;
    if (status != DCC_OK ||
        (expected_size != 0U && out_view->size != expected_size) ||
        dcc_endpoint_field_partially_covered(
            out_view->size, offsetof(dcc_channel_position_t, position),
            sizeof(position->position)
        ) ||
        dcc_endpoint_field_partially_covered(
            out_view->size,
            offsetof(dcc_channel_position_t, lock_permissions),
            sizeof(position->lock_permissions)
        ) ||
        dcc_endpoint_field_partially_covered(
            out_view->size, offsetof(dcc_channel_position_t, parent_id),
            sizeof(position->parent_id)
        ) ||
        dcc_endpoint_field_partially_covered(
            out_view->size, offsetof(dcc_channel_position_t, flags),
            sizeof(position->flags)
        ) ||
        dcc_endpoint_field_partially_covered(
            out_view->size, offsetof(dcc_channel_position_t, nulls),
            sizeof(position->nulls)
        ) ||
        position->channel_id == 0U ||
        !dcc_endpoint_present_field_covered(
            out_view, DCC_CHANNEL_POSITION_PRESENT_POSITION,
            offsetof(dcc_channel_position_t, position),
            sizeof(position->position)
        ) ||
        !dcc_endpoint_present_field_covered(
            out_view, DCC_CHANNEL_POSITION_PRESENT_LOCK_PERMISSIONS,
            offsetof(dcc_channel_position_t, lock_permissions),
            sizeof(position->lock_permissions)
        ) ||
        !dcc_endpoint_present_field_covered(
            out_view, DCC_CHANNEL_POSITION_PRESENT_PARENT_ID,
            offsetof(dcc_channel_position_t, parent_id),
            sizeof(position->parent_id)
        ) ||
        !dcc_endpoint_present_field_covered(
            out_view, DCC_CHANNEL_POSITION_PRESENT_FLAGS,
            offsetof(dcc_channel_position_t, flags),
            sizeof(position->flags)
        ) ||
        ((out_view->present & DCC_CHANNEL_POSITION_PRESENT_POSITION) != 0U &&
         position->position < 0) ||
        ((out_view->present & DCC_CHANNEL_POSITION_PRESENT_LOCK_PERMISSIONS) != 0U &&
         position->lock_permissions > 1U) ||
        ((out_view->present & DCC_CHANNEL_POSITION_PRESENT_FLAGS) != 0U &&
         (position->flags & ~(DCC_CHANNEL_FLAG_PINNED |
             DCC_CHANNEL_FLAG_REQUIRE_TAG |
             DCC_CHANNEL_FLAG_HIDE_MEDIA_DOWNLOAD_OPTIONS)) != 0U) ||
        (nulls & ~DCC_CHANNEL_POSITION_NULL_PARENT_ID) != 0U ||
        ((nulls & DCC_CHANNEL_POSITION_NULL_PARENT_ID) != 0U &&
         ((out_view->present & DCC_CHANNEL_POSITION_PRESENT_PARENT_ID) == 0U ||
          position->parent_id != 0U)) ||
        ((out_view->present & DCC_CHANNEL_POSITION_PRESENT_PARENT_ID) != 0U &&
         position->parent_id == 0U &&
         (nulls & DCC_CHANNEL_POSITION_NULL_PARENT_ID) == 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_modify_guild_channel_positions(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_channel_positions_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CHANNEL_EDIT_POSITIONS, DCC_REST_PATCH);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_DENIED
    );
    dcc_endpoint_record_view_t params_view;
    if (status != DCC_OK || client == NULL || guild_id == 0U ||
        dcc_endpoint_record_read(
            params, offsetof(dcc_channel_positions_params_t, version),
            SIZE_MAX,
            DCC_ENDPOINT_FIELD_END(dcc_channel_positions_params_t, position_count),
            DCC_CHANNEL_POSITIONS_PARAMS_VERSION, 0U, &params_view
        ) != DCC_OK || params->positions == NULL || params->position_count == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;

    size_t parent_moves = 0U;
    uintptr_t positions_address = (uintptr_t)params->positions;
    dcc_endpoint_record_view_t first_position;
    if (positions_address % _Alignof(dcc_channel_position_t) != 0U ||
        channel_position_validate(
            params->positions, 0U, &first_position
        ) != DCC_OK ||
        first_position.size % _Alignof(dcc_channel_position_t) != 0U ||
        params->position_count > SIZE_MAX / first_position.size ||
        positions_address > UINTPTR_MAX -
            (params->position_count * first_position.size - 1U)) {
        return DCC_ERR_INVALID_ARG;
    }
    const size_t stride = first_position.size;
    for (size_t i = 0U; i < params->position_count; ++i) {
        const dcc_channel_position_t *position =
            (const dcc_channel_position_t *)
                ((const unsigned char *)params->positions + i * stride);
        dcc_endpoint_record_view_t view;
        if (channel_position_validate(position, stride, &view) != DCC_OK)
            return DCC_ERR_INVALID_ARG;
        for (size_t j = 0U; j < i; ++j) {
            const dcc_channel_position_t *prior =
                (const dcc_channel_position_t *)
                    ((const unsigned char *)params->positions + j * stride);
            if (prior->channel_id == position->channel_id)
                return DCC_ERR_INVALID_ARG;
        }
        if ((view.present & DCC_CHANNEL_POSITION_PRESENT_PARENT_ID) != 0U &&
            ++parent_moves > 1U) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    dcc_rest_buffer_t body = {0};
    status = dcc_rest_buffer_append_cstr(&body, "[");
    for (size_t i = 0U; i < params->position_count && status == DCC_OK; ++i) {
        const dcc_channel_position_t *position =
            (const dcc_channel_position_t *)
                ((const unsigned char *)params->positions + i * stride);
        if (i != 0U) status = dcc_rest_buffer_append_cstr(&body, ",");
        if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "{");
        int first = 1;
        if (status == DCC_OK) status = dcc_rest_json_append_u64_string_member(
            &body, &first, "id", position->channel_id
        );
        if (status == DCC_OK && (position->present & DCC_CHANNEL_POSITION_PRESENT_POSITION) != 0U)
            status = dcc_rest_json_append_u64_member(&body, &first, "position", (uint64_t)position->position);
        if (status == DCC_OK && (position->present & DCC_CHANNEL_POSITION_PRESENT_LOCK_PERMISSIONS) != 0U)
            status = dcc_rest_json_append_bool_member(&body, &first, "lock_permissions", position->lock_permissions);
        if (status == DCC_OK && (position->present & DCC_CHANNEL_POSITION_PRESENT_PARENT_ID) != 0U) {
            uint64_t nulls = dcc_endpoint_field_covered(
                position->size, offsetof(dcc_channel_position_t, nulls),
                sizeof(position->nulls)
            ) ? position->nulls : 0U;
            if ((nulls & DCC_CHANNEL_POSITION_NULL_PARENT_ID) != 0U)
                status = dcc_rest_json_append_raw_member(&body, &first, "parent_id", "null");
            else
                status = dcc_rest_json_append_u64_string_member(&body, &first, "parent_id", position->parent_id);
        }
        if (status == DCC_OK && (position->present & DCC_CHANNEL_POSITION_PRESENT_FLAGS) != 0U)
            status = dcc_rest_json_append_u64_member(&body, &first, "flags", position->flags);
        if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "]");
    char path[80];
    if (status == DCC_OK) status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/channels", (unsigned long long)guild_id
    );
    dcc_endpoint_body_t request_body = { body.data, body.len, "application/json" };
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_modify_guild_channel_positions", DCC_REST_PATCH,
        path, &request_body, &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    dcc_rest_buffer_deinit(&body);
    return status;
}
