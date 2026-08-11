#include "internal/objects/dcc_builder_abi_internal.h"
#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int dcc_message_utf8_scalars(const char *text, size_t *out_count) {
    if (text == NULL || out_count == NULL) {
        return 0;
    }
    size_t count = 0U;
    const unsigned char *p = (const unsigned char *)text;
    while (*p != 0U) {
        uint32_t value;
        size_t width;
        if (*p < 0x80U) {
            value = *p;
            width = 1U;
        } else if (*p >= 0xC2U && *p <= 0xDFU) {
            value = (uint32_t)(*p & 0x1FU);
            width = 2U;
        } else if (*p >= 0xE0U && *p <= 0xEFU) {
            value = (uint32_t)(*p & 0x0FU);
            width = 3U;
        } else if (*p >= 0xF0U && *p <= 0xF4U) {
            value = (uint32_t)(*p & 0x07U);
            width = 4U;
        } else {
            return 0;
        }
        for (size_t i = 1U; i < width; ++i) {
            if ((p[i] & 0xC0U) != 0x80U) {
                return 0;
            }
            value = (value << 6U) | (uint32_t)(p[i] & 0x3FU);
        }
        if ((width == 2U && value < 0x80U) ||
            (width == 3U && value < 0x800U) ||
            (width == 4U && value < 0x10000U) ||
            (value >= 0xD800U && value <= 0xDFFFU) ||
            value > 0x10FFFFU) {
            return 0;
        }
        p += width;
        ++count;
    }
    *out_count = count;
    return 1;
}

static int dcc_message_base64_value(unsigned char c) {
    if (c >= 'A' && c <= 'Z') return (int)(c - 'A');
    if (c >= 'a' && c <= 'z') return (int)(c - 'a') + 26;
    if (c >= '0' && c <= '9') return (int)(c - '0') + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

static int dcc_message_waveform_valid(const char *value) {
    if (value == NULL) {
        return 0;
    }
    size_t len = strlen(value);
    if ((len & 3U) != 0U) {
        return 0;
    }
    size_t padding = 0U;
    if (len != 0U && value[len - 1U] == '=') ++padding;
    if (len > 1U && value[len - 2U] == '=') ++padding;
    if (padding > 2U || len / 4U > (SIZE_MAX - 2U) / 3U ||
        len / 4U * 3U - padding > 256U) {
        return 0;
    }
    for (size_t i = 0U; i < len - padding; ++i) {
        if (dcc_message_base64_value((unsigned char)value[i]) < 0) return 0;
    }
    for (size_t i = len - padding; i < len; ++i) {
        if (value[i] != '=') return 0;
    }
    if (padding == 1U) {
        int final = dcc_message_base64_value((unsigned char)value[len - 2U]);
        if (final < 0 || (final & 3) != 0) return 0;
    } else if (padding == 2U) {
        int final = dcc_message_base64_value((unsigned char)value[len - 3U]);
        if (final < 0 || (final & 15) != 0) return 0;
    }
    return 1;
}

dcc_status_t dcc_message_reference_validate(
    const dcc_message_reference_t *reference,
    dcc_message_reference_type_t *out_type
) {
    dcc_builder_abi_view_t view;
    if (dcc_message_reference_abi_validate(reference, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    dcc_message_reference_type_t type = HAS(DCC_MESSAGE_REFERENCE_PRESENT_TYPE)
        ? reference->type : DCC_MESSAGE_REFERENCE_DEFAULT;
    if ((type != DCC_MESSAGE_REFERENCE_DEFAULT &&
         type != DCC_MESSAGE_REFERENCE_FORWARD) ||
        !HAS(DCC_MESSAGE_REFERENCE_PRESENT_MESSAGE_ID) ||
        reference->message_id == 0U ||
        (HAS(DCC_MESSAGE_REFERENCE_PRESENT_CHANNEL_ID) &&
         reference->channel_id == 0U) ||
        (HAS(DCC_MESSAGE_REFERENCE_PRESENT_GUILD_ID) &&
         reference->guild_id == 0U) ||
        (HAS(DCC_MESSAGE_REFERENCE_PRESENT_FAIL_IF_NOT_EXISTS) &&
         reference->fail_if_not_exists > 1U) ||
        (type == DCC_MESSAGE_REFERENCE_FORWARD &&
         (!HAS(DCC_MESSAGE_REFERENCE_PRESENT_TYPE) ||
          !HAS(DCC_MESSAGE_REFERENCE_PRESENT_CHANNEL_ID) ||
          HAS(DCC_MESSAGE_REFERENCE_PRESENT_FAIL_IF_NOT_EXISTS)))) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_type != NULL) {
        *out_type = type;
    }
#undef HAS
    return DCC_OK;
}

static dcc_status_t dcc_message_attachment_validate(
    const dcc_message_attachment_t *attachment,
    size_t expected_stride
) {
    dcc_builder_abi_view_t view;
    if (dcc_message_attachment_abi_validate(attachment, &view) != DCC_OK ||
        view.size != expected_stride) {
        return DCC_ERR_INVALID_ARG;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    size_t scalars = 0U;
    if ((HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME) &&
         (!dcc_message_utf8_scalars(attachment->filename, &scalars) || scalars == 0U)) ||
        (HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_TITLE) &&
         (!dcc_message_utf8_scalars(attachment->title, &scalars) || scalars == 0U)) ||
        (HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_DESCRIPTION) &&
         (!dcc_message_utf8_scalars(attachment->description, &scalars) || scalars > 1024U)) ||
        (HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_DURATION_SECS) &&
         (!isfinite(attachment->duration_secs) || attachment->duration_secs < 0.0)) ||
        (HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_WAVEFORM) &&
         !dcc_message_waveform_valid(attachment->waveform)) ||
        (HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_IS_SPOILER) &&
         attachment->is_spoiler > 1U) ||
        (HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_DURATION_SECS) !=
         HAS(DCC_MESSAGE_ATTACHMENT_PRESENT_WAVEFORM))) {
        return DCC_ERR_INVALID_ARG;
    }
#undef HAS
    return DCC_OK;
}

dcc_status_t dcc_message_attachment_array_validate(
    const dcc_message_attachment_t *attachments,
    size_t attachment_count,
    size_t *out_stride
) {
    size_t stride = 0U;
    if (dcc_message_attachment_array_begin(
            attachments, attachment_count, &stride
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < attachment_count; ++i) {
        const dcc_message_attachment_t *item =
            (const dcc_message_attachment_t *)dcc_builder_abi_array_at(
                attachments, stride, i
            );
        if (dcc_message_attachment_validate(item, stride) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
        for (size_t j = 0U; j < i; ++j) {
            const dcc_message_attachment_t *prior =
                (const dcc_message_attachment_t *)dcc_builder_abi_array_at(
                    attachments, stride, j
                );
            if (prior->id == item->id) {
                return DCC_ERR_INVALID_ARG;
            }
        }
    }
    if (out_stride != NULL) {
        *out_stride = stride;
    }
    return DCC_OK;
}

static dcc_status_t dcc_message_json_append_double(
    dcc_message_json_buffer_t *buffer,
    double value
) {
    char text[32];
    int written = snprintf(text, sizeof(text), "%.17g", value);
    return written > 0 && (size_t)written < sizeof(text)
        ? dcc_message_json_append(buffer, text, (size_t)written)
        : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_message_reference_append_json(
    const dcc_message_reference_t *reference,
    dcc_message_json_buffer_t *buffer
) {
    dcc_builder_abi_view_t view;
    dcc_message_reference_type_t type;
    if (dcc_message_reference_abi_validate(reference, &view) != DCC_OK ||
        dcc_message_reference_validate(reference, &type) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    dcc_status_t status = dcc_message_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK && HAS(DCC_MESSAGE_REFERENCE_PRESENT_TYPE)) {
        status = dcc_message_json_append_u64_member(
            buffer, &first, "type", (uint64_t)type
        );
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_REFERENCE_PRESENT_MESSAGE_ID)) {
        status = dcc_message_json_member_prefix(buffer, &first, "message_id");
        if (status == DCC_OK) status = dcc_message_json_append_snowflake_string(buffer, reference->message_id);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_REFERENCE_PRESENT_CHANNEL_ID)) {
        status = dcc_message_json_member_prefix(buffer, &first, "channel_id");
        if (status == DCC_OK) status = dcc_message_json_append_snowflake_string(buffer, reference->channel_id);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_REFERENCE_PRESENT_GUILD_ID)) {
        status = dcc_message_json_member_prefix(buffer, &first, "guild_id");
        if (status == DCC_OK) status = dcc_message_json_append_snowflake_string(buffer, reference->guild_id);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_REFERENCE_PRESENT_FAIL_IF_NOT_EXISTS)) {
        status = dcc_message_json_append_bool_member(
            buffer, &first, "fail_if_not_exists", reference->fail_if_not_exists
        );
    }
#undef HAS
    return status == DCC_OK ? dcc_message_json_append_cstr(buffer, "}") : status;
}

dcc_status_t dcc_message_attachments_append_json(
    const dcc_message_attachment_t *attachments,
    size_t attachment_count,
    dcc_message_json_buffer_t *buffer
) {
    size_t stride = 0U;
    if (dcc_message_attachment_array_validate(
            attachments, attachment_count, &stride
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_message_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < attachment_count; ++i) {
        const dcc_message_attachment_t *item =
            (const dcc_message_attachment_t *)dcc_builder_abi_array_at(
                attachments, stride, i
            );
        dcc_builder_abi_view_t view;
        status = dcc_message_attachment_abi_validate(item, &view);
        if (status == DCC_OK && i != 0U) status = dcc_message_json_append_cstr(buffer, ",");
        if (status == DCC_OK) status = dcc_message_json_append_cstr(buffer, "{");
        int first = 1;
        if (status == DCC_OK) {
            status = dcc_message_json_member_prefix(buffer, &first, "id");
            if (status == DCC_OK) status = dcc_message_json_append_snowflake_string(buffer, item->id);
        }
#define APPEND_STRING(bit_, name_, field_) \
        if (status == DCC_OK && dcc_builder_abi_view_has(&view, (bit_))) \
            status = dcc_message_json_append_string_member(buffer, &first, (name_), item->field_)
        APPEND_STRING(DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME, "filename", filename);
        APPEND_STRING(DCC_MESSAGE_ATTACHMENT_PRESENT_TITLE, "title", title);
        APPEND_STRING(DCC_MESSAGE_ATTACHMENT_PRESENT_DESCRIPTION, "description", description);
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_MESSAGE_ATTACHMENT_PRESENT_DURATION_SECS
            )) {
            status = dcc_message_json_member_prefix(buffer, &first, "duration_secs");
            if (status == DCC_OK) status = dcc_message_json_append_double(buffer, item->duration_secs);
        }
        APPEND_STRING(DCC_MESSAGE_ATTACHMENT_PRESENT_WAVEFORM, "waveform", waveform);
#undef APPEND_STRING
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_MESSAGE_ATTACHMENT_PRESENT_IS_SPOILER
            )) {
            status = dcc_message_json_append_bool_member(
                buffer, &first, "is_spoiler", item->is_spoiler
            );
        }
        if (status == DCC_OK) status = dcc_message_json_append_cstr(buffer, "}");
    }
    return status == DCC_OK ? dcc_message_json_append_cstr(buffer, "]") : status;
}
