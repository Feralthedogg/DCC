#include "internal/objects/dcc_message_json_members_internal.h"
#include "internal/objects/dcc_message_poll_internal.h"

#include <stdlib.h>

dcc_status_t dcc_message_poll_builder_build_json(const dcc_poll_builder_t *poll, char **out_json) {
    if (poll == NULL || out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    dcc_status_t status = dcc_message_poll_validate(poll);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_json_buffer_t buffer = {0};
    status = dcc_message_json_append_cstr(&buffer, "{");
    int first = 1;

    if (status == DCC_OK) {
        status = dcc_message_json_member_prefix(&buffer, &first, "question");
    }
    if (status == DCC_OK) {
        status = dcc_message_poll_append_media(&poll->question, &buffer);
    }
    if (status == DCC_OK) {
        status = dcc_message_json_member_prefix(&buffer, &first, "answers");
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(&buffer, "[");
    }
    for (size_t i = 0; status == DCC_OK && i < poll->answer_count; ++i) {
        if (i != 0U) {
            status = dcc_message_json_append_cstr(&buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_message_json_append_cstr(&buffer, "{\"poll_media\":");
        }
        if (status == DCC_OK) {
            status = dcc_message_poll_append_media(&poll->answers[i].media, &buffer);
        }
        if (status == DCC_OK) {
            status = dcc_message_json_append_cstr(&buffer, "}");
        }
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(&buffer, "]");
    }
    if (status == DCC_OK) {
        uint32_t duration = poll->has_duration ? poll->duration_hours : 24U;
        status = dcc_message_json_append_u64_member(&buffer, &first, "duration", duration);
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_bool_member(
            &buffer,
            &first,
            "allow_multiselect",
            poll->has_allow_multiselect ? poll->allow_multiselect : 0U
        );
    }
    if (status == DCC_OK) {
        dcc_poll_layout_type_t layout = poll->has_layout_type ? poll->layout_type : DCC_POLL_LAYOUT_DEFAULT;
        status = dcc_message_json_append_u64_member(&buffer, &first, "layout_type", (uint64_t)layout);
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(&buffer, "}");
    }

    if (status != DCC_OK) {
        dcc_message_json_buffer_deinit(&buffer);
        return status;
    }
    *out_json = buffer.data;
    return DCC_OK;
}

void dcc_message_poll_builder_json_free(char *json) {
    free(json);
}
