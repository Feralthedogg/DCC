#include "internal/objects/dcc_builder_abi_internal.h"
#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_multipart_build_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"
#include "internal/rest/dcc_rest_threads_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_FORUM_THREAD_MAX_BODY (25U * 1024U * 1024U)

static int duration_valid(dcc_channel_auto_archive_duration_t value) {
    return value == DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR ||
        value == DCC_CHANNEL_AUTO_ARCHIVE_1_DAY ||
        value == DCC_CHANNEL_AUTO_ARCHIVE_3_DAYS ||
        value == DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK;
}

static int thread_type_valid(dcc_channel_type_t value) {
    return value == DCC_CHANNEL_ANNOUNCEMENT_THREAD ||
        value == DCC_CHANNEL_PUBLIC_THREAD ||
        value == DCC_CHANNEL_PRIVATE_THREAD;
}

static int thread_present_field(
    const dcc_endpoint_record_view_t *view,
    uint64_t bit,
    size_t offset,
    size_t width
) {
    return dcc_endpoint_present_field_covered(view, bit, offset, width);
}

#define THREAD_FIELD(view_, bit_, field_) \
    thread_present_field( \
        (view_), (bit_), offsetof(dcc_thread_params_t, field_), \
        sizeof(((dcc_thread_params_t *)0)->field_) \
    )

#define THREAD_PARTIAL(view_, field_) \
    dcc_endpoint_field_partially_covered( \
        (view_)->size, offsetof(dcc_thread_params_t, field_), \
        sizeof(((dcc_thread_params_t *)0)->field_) \
    )

static dcc_status_t thread_message_files(
    const dcc_rest_message_payload_t *payload,
    const dcc_message_builder_t **out_message,
    const dcc_rest_multipart_file_t **out_files,
    size_t *out_file_count
) {
    if (out_message == NULL || out_files == NULL || out_file_count == NULL ||
        dcc_endpoint_message_payload_preflight(payload) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_message = payload->message;
    *out_files = NULL;
    *out_file_count = 0U;
    if (dcc_endpoint_field_covered(
            payload->size, offsetof(dcc_rest_message_payload_t, files),
            sizeof(payload->files)
        ) && dcc_endpoint_field_covered(
            payload->size, offsetof(dcc_rest_message_payload_t, file_count),
            sizeof(payload->file_count)
        )) {
        *out_files = payload->files;
        *out_file_count = payload->file_count;
    }
    return DCC_OK;
}

static dcc_status_t thread_forum_message_validate(
    const dcc_message_builder_t *message,
    const dcc_rest_multipart_file_t *files,
    size_t file_count
) {
    if (message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_builder_abi_view_t view;
    if (dcc_message_builder_abi_validate(message, &view) != DCC_OK ||
        dcc_message_builder_validate_for_json(message) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    const uint64_t allowed = DCC_MESSAGE_BUILDER_PRESENT_CONTENT |
        DCC_MESSAGE_BUILDER_PRESENT_EMBEDS |
        DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS |
        DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS |
        DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS |
        DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS |
        DCC_MESSAGE_BUILDER_PRESENT_FLAGS;
    if ((view.present & ~allowed) != 0U ||
        ((view.present & DCC_MESSAGE_BUILDER_PRESENT_FLAGS) != 0U &&
         (message->flags & ~(DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS |
             DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS)) != 0U) ||
        ((view.present & DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS) != 0U &&
         message->sticker_ids_count > 3U)) {
        return DCC_ERR_INVALID_ARG;
    }
    const int has_content =
        (view.present & DCC_MESSAGE_BUILDER_PRESENT_CONTENT) != 0U &&
        message->content != NULL && message->content[0] != '\0';
    const int has_embeds =
        (view.present & DCC_MESSAGE_BUILDER_PRESENT_EMBEDS) != 0U &&
        message->embeds_count != 0U;
    const int has_stickers =
        (view.present & DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS) != 0U &&
        message->sticker_ids_count != 0U;
    const int has_components =
        (view.present & DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS) != 0U &&
        message->components_count != 0U;
    if (!has_content && !has_embeds && !has_stickers && !has_components &&
        file_count == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t attachment_stride = 0U;
    size_t attachment_count =
        (view.present & DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS) != 0U
        ? message->attachment_count : 0U;
    if (attachment_count != 0U &&
        dcc_message_attachment_array_validate(
            message->attachments, attachment_count, &attachment_stride
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (attachment_count != file_count) {
        return DCC_ERR_INVALID_ARG;
    }
    if (file_count != 0U && files == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < file_count; ++i) {
        char expected_field[64];
        int length = snprintf(
            expected_field, sizeof(expected_field), "files[%zu]", i
        );
        if (length <= 0 || (size_t)length >= sizeof(expected_field) ||
            files[i].field_name == NULL ||
            strcmp(files[i].field_name, expected_field) != 0) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    int voice_metadata = 0;
    for (size_t i = 0U; i < attachment_count; ++i) {
        const dcc_message_attachment_t *attachment =
            (const dcc_message_attachment_t *)dcc_builder_abi_array_at(
                message->attachments, attachment_stride, i
            );
        if (attachment->id >= file_count) {
            return DCC_ERR_INVALID_ARG;
        }
        if ((attachment->present &
                (DCC_MESSAGE_ATTACHMENT_PRESENT_DURATION_SECS |
                 DCC_MESSAGE_ATTACHMENT_PRESENT_WAVEFORM)) != 0U) {
            voice_metadata = 1;
        }
    }
    if (voice_metadata &&
        (file_count != 1U || files == NULL || files[0].content_type == NULL ||
         strncmp(files[0].content_type, "audio/", 6U) != 0)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static dcc_status_t thread_preflight(
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    dcc_endpoint_record_view_t *out_view,
    const dcc_message_builder_t **out_message,
    const dcc_rest_multipart_file_t **out_files,
    size_t *out_file_count
) {
    const uint64_t known = (UINT64_C(1) << 7U) - 1U;
    if (out_view == NULL || out_message == NULL || out_files == NULL ||
        out_file_count == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_message = NULL;
    *out_files = NULL;
    *out_file_count = 0U;
    dcc_status_t status = dcc_endpoint_record_read(
        params, offsetof(dcc_thread_params_t, version),
        offsetof(dcc_thread_params_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_thread_params_t, present),
        DCC_THREAD_PARAMS_VERSION, known, out_view
    );
    if (status != DCC_OK ||
        THREAD_PARTIAL(out_view, name) ||
        THREAD_PARTIAL(out_view, auto_archive_duration) ||
        THREAD_PARTIAL(out_view, type) ||
        THREAD_PARTIAL(out_view, invitable) ||
        THREAD_PARTIAL(out_view, rate_limit_per_user) ||
        THREAD_PARTIAL(out_view, message) ||
        THREAD_PARTIAL(out_view, applied_tags) ||
        THREAD_PARTIAL(out_view, applied_tag_count) ||
        !THREAD_FIELD(out_view, DCC_THREAD_PARAMS_PRESENT_NAME, name) ||
        !THREAD_FIELD(out_view, DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION,
            auto_archive_duration) ||
        !THREAD_FIELD(out_view, DCC_THREAD_PARAMS_PRESENT_TYPE, type) ||
        !THREAD_FIELD(out_view, DCC_THREAD_PARAMS_PRESENT_INVITABLE, invitable) ||
        !THREAD_FIELD(out_view, DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER,
            rate_limit_per_user) ||
        !THREAD_FIELD(out_view, DCC_THREAD_PARAMS_PRESENT_MESSAGE, message) ||
        !THREAD_FIELD(out_view, DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS,
            applied_tags) ||
        ((out_view->present & DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS) != 0U &&
         !dcc_endpoint_field_covered(
             out_view->size, offsetof(dcc_thread_params_t, applied_tag_count),
             sizeof(params->applied_tag_count)
         ))) {
        return DCC_ERR_INVALID_ARG;
    }
    uint64_t allowed = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
        DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER;
    if (mode == DCC_REST_THREAD_BODY_CREATE) {
        allowed |= DCC_THREAD_PARAMS_PRESENT_TYPE |
            DCC_THREAD_PARAMS_PRESENT_INVITABLE;
    } else if (mode == DCC_REST_THREAD_BODY_CREATE_FORUM) {
        allowed |= DCC_THREAD_PARAMS_PRESENT_MESSAGE |
            DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS;
    }
    size_t name_scalars = 0U;
    if ((out_view->present & DCC_THREAD_PARAMS_PRESENT_NAME) == 0U ||
        (out_view->present & ~allowed) != 0U ||
        !dcc_endpoint_utf8_scalar_count(params->name, &name_scalars) ||
        name_scalars == 0U || name_scalars > 100U ||
        ((out_view->present & DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION) != 0U &&
         !duration_valid(params->auto_archive_duration)) ||
        ((out_view->present & DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER) != 0U &&
         params->rate_limit_per_user > 21600U) ||
        ((out_view->present & DCC_THREAD_PARAMS_PRESENT_TYPE) != 0U &&
         !thread_type_valid(params->type)) ||
        ((out_view->present & DCC_THREAD_PARAMS_PRESENT_INVITABLE) != 0U &&
         (params->invitable > 1U ||
          (out_view->present & DCC_THREAD_PARAMS_PRESENT_TYPE) == 0U ||
          params->type != DCC_CHANNEL_PRIVATE_THREAD))) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((out_view->present & DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS) != 0U) {
        if (params->applied_tag_count > 5U ||
            (params->applied_tag_count != 0U && params->applied_tags == NULL) ||
            params->applied_tag_count > SIZE_MAX / sizeof(*params->applied_tags)) {
            return DCC_ERR_INVALID_ARG;
        }
        for (size_t i = 0U; i < params->applied_tag_count; ++i) {
            if (params->applied_tags[i] == 0U) return DCC_ERR_INVALID_ARG;
            for (size_t j = 0U; j < i; ++j) {
                if (params->applied_tags[j] == params->applied_tags[i])
                    return DCC_ERR_INVALID_ARG;
            }
        }
    }
    if (mode == DCC_REST_THREAD_BODY_CREATE_FORUM) {
        if ((out_view->present & DCC_THREAD_PARAMS_PRESENT_MESSAGE) == 0U ||
            params->message == NULL ||
            thread_message_files(
                params->message, out_message, out_files, out_file_count
            ) != DCC_OK ||
            thread_forum_message_validate(
                *out_message, *out_files, *out_file_count
            ) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_build_thread_body(
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    dcc_endpoint_body_t *out
) {
    if (out == NULL) return DCC_ERR_INVALID_ARG;
    *out = (dcc_endpoint_body_t){0};
    dcc_endpoint_record_view_t view;
    const dcc_message_builder_t *message = NULL;
    const dcc_rest_multipart_file_t *files = NULL;
    size_t file_count = 0U;
    dcc_status_t status = thread_preflight(
        params, mode, &view, &message, &files, &file_count
    );
    if (status != DCC_OK) return status;

    char *message_json = NULL;
    if (message != NULL) {
        status = dcc_message_builder_build_json(message, &message_json);
    }
    dcc_rest_buffer_t body = {0};
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) status = dcc_rest_json_append_string_member(
        &body, &first, "name", params->name
    );
    if (status == DCC_OK && (view.present & DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION) != 0U)
        status = dcc_rest_json_append_u64_member(&body, &first, "auto_archive_duration", params->auto_archive_duration);
    if (status == DCC_OK && (view.present & DCC_THREAD_PARAMS_PRESENT_TYPE) != 0U)
        status = dcc_rest_json_append_u64_member(&body, &first, "type", params->type);
    if (status == DCC_OK && (view.present & DCC_THREAD_PARAMS_PRESENT_INVITABLE) != 0U)
        status = dcc_rest_json_append_bool_member(&body, &first, "invitable", params->invitable);
    if (status == DCC_OK && (view.present & DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER) != 0U)
        status = dcc_rest_json_append_u64_member(&body, &first, "rate_limit_per_user", params->rate_limit_per_user);
    if (status == DCC_OK && (view.present & DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS) != 0U)
        status = dcc_rest_json_append_snowflake_string_array_member(
            &body, &first, "applied_tags", params->applied_tags, params->applied_tag_count
        );
    if (status == DCC_OK && message_json != NULL)
        status = dcc_rest_json_append_raw_member(&body, &first, "message", message_json);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "}");
    free(message_json);
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&body);
        return status;
    }
    if (file_count == 0U) {
        out->data = body.data;
        out->len = body.len;
        out->content_type = "application/json";
        return DCC_OK;
    }
    size_t measured = 0U;
    status = dcc_rest_multipart_measure_field_value_length(
        "payload_json", body.len, files, file_count, &measured
    );
    if (status != DCC_OK || measured > DCC_FORUM_THREAD_MAX_BODY) {
        dcc_rest_buffer_deinit(&body);
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_multipart_field_t field = {"payload_json", body.data};
    char *multipart = NULL;
    size_t multipart_len = 0U;
    status = dcc_rest_build_multipart_body(
        &field, 1U, files, file_count, &multipart, &multipart_len
    );
    dcc_rest_buffer_deinit(&body);
    if (status != DCC_OK || multipart_len != measured) {
        free(multipart);
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }
    out->data = multipart;
    out->len = multipart_len;
    out->content_type = DCC_REST_MULTIPART_CONTENT_TYPE;
    return DCC_OK;
}
