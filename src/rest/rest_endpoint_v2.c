#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/objects/dcc_autocomplete_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"
#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"
#include "internal/objects/dcc_modal_builder_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_multipart_build_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"

#include <stdlib.h>
#include <string.h>

#define DCC_ENDPOINT_MAX_FILES 10U
#define DCC_ENDPOINT_MAX_APPLIED_TAGS 5U

static dcc_status_t dcc_endpoint_multipart_preflight(
    size_t payload_json_len,
    const dcc_rest_multipart_file_t *files,
    size_t file_count
) {
    if (file_count == 0U) {
        return DCC_OK;
    }
    size_t body_len = 0U;
    return dcc_rest_multipart_measure_field_value_length(
        "payload_json",
        payload_json_len,
        files,
        file_count,
        &body_len
    );
}

static _Thread_local size_t dcc_endpoint_probe_limit = SIZE_MAX;
static _Thread_local size_t dcc_endpoint_probe_calls;

void dcc_endpoint_test_allocation_probe_begin(size_t successful_allocations) {
    dcc_endpoint_probe_calls = 0U;
    dcc_endpoint_probe_limit = successful_allocations;
}

size_t dcc_endpoint_test_allocation_probe_end(void) {
    size_t calls = dcc_endpoint_probe_calls;
    dcc_endpoint_probe_calls = 0U;
    dcc_endpoint_probe_limit = SIZE_MAX;
    return calls;
}

dcc_status_t dcc_endpoint_allocation_probe(void) {
    if (dcc_endpoint_probe_limit == SIZE_MAX) {
        return DCC_OK;
    }
    size_t call = dcc_endpoint_probe_calls++;
    return call < dcc_endpoint_probe_limit ? DCC_OK : DCC_ERR_NOMEM;
}

typedef struct dcc_endpoint_legacy_bridge {
    dcc_rest_cb callback;
    void *user_data;
} dcc_endpoint_legacy_bridge_t;

static void dcc_endpoint_legacy_result(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
) {
    dcc_endpoint_legacy_bridge_t *bridge =
        (dcc_endpoint_legacy_bridge_t *)user_data;
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = result != NULL ? result->http_status : 0U,
        .error = result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME,
        .body = result != NULL ? result->body : NULL,
        .body_len = result != NULL ? result->body_len : 0U,
    };
    if (bridge != NULL && bridge->callback != NULL) {
        bridge->callback(client, &response, bridge->user_data);
    }
    free(bridge);
}

static int dcc_endpoint_options_valid(const dcc_rest_call_options_t *options) {
    return options != NULL && options->size >= sizeof(*options) &&
        options->version == DCC_REST_CALL_OPTIONS_VERSION &&
        options->priority >= DCC_REST_PRIORITY_LOW &&
        options->priority <= DCC_REST_PRIORITY_HIGH &&
        (options->callback != NULL || options->user_data == NULL);
}

int dcc_endpoint_field_covered(size_t size, size_t offset, size_t width) {
    return size >= offset && width <= size - offset;
}

dcc_status_t dcc_endpoint_record_read(
    const void *value,
    size_t version_offset,
    size_t present_offset,
    size_t mandatory_size,
    uint32_t expected_version,
    uint64_t known_present,
    dcc_endpoint_record_view_t *out
) {
    if (value == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_endpoint_record_view_t){0};
    memcpy(&out->size, value, sizeof(out->size));
    if (out->size < sizeof(out->size) || out->size < mandatory_size ||
        !dcc_endpoint_field_covered(
            out->size, version_offset, sizeof(out->version)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    memcpy(
        &out->version,
        (const unsigned char *)value + version_offset,
        sizeof(out->version)
    );
    if (out->version != expected_version) {
        return DCC_ERR_INVALID_ARG;
    }
    if (present_offset != SIZE_MAX) {
        if (!dcc_endpoint_field_covered(
                out->size, present_offset, sizeof(out->present)
            )) {
            return DCC_ERR_INVALID_ARG;
        }
        memcpy(
            &out->present,
            (const unsigned char *)value + present_offset,
            sizeof(out->present)
        );
        if ((out->present & ~known_present) != 0U) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

int dcc_endpoint_present_field_covered(
    const dcc_endpoint_record_view_t *view,
    uint64_t bit,
    size_t offset,
    size_t width
) {
    return view != NULL && ((view->present & bit) == 0U ||
        dcc_endpoint_field_covered(view->size, offset, width));
}

#define DCC_ENDPOINT_PRESENT_FIELD(type_, view_, bit_, field_) \
    dcc_endpoint_present_field_covered( \
        (view_), (bit_), offsetof(type_, field_), \
        sizeof(((type_ *)0)->field_) \
    )

static int dcc_endpoint_file_pair_covered(
    size_t size,
    size_t files_offset,
    size_t files_width,
    size_t count_offset,
    size_t count_width
) {
    return dcc_endpoint_field_covered(size, files_offset, files_width) &&
        dcc_endpoint_field_covered(size, count_offset, count_width);
}

dcc_status_t dcc_endpoint_message_payload_preflight(
    const dcc_rest_message_payload_t *payload
) {
    dcc_endpoint_record_view_t view;
    dcc_status_t status = dcc_endpoint_record_read(
        payload,
        offsetof(dcc_rest_message_payload_t, version),
        SIZE_MAX,
        DCC_ENDPOINT_FIELD_END(dcc_rest_message_payload_t, message),
        DCC_REST_MESSAGE_PAYLOAD_VERSION,
        0U,
        &view
    );
    if (status != DCC_OK) {
        return status;
    }

    const dcc_message_builder_t *message = payload->message;
    const dcc_rest_multipart_file_t *files = NULL;
    size_t file_count = 0U;
    if (dcc_endpoint_file_pair_covered(
            view.size,
            offsetof(dcc_rest_message_payload_t, files),
            sizeof(payload->files),
            offsetof(dcc_rest_message_payload_t, file_count),
            sizeof(payload->file_count)
        )) {
        files = payload->files;
        file_count = payload->file_count;
    }
    if (file_count > DCC_ENDPOINT_MAX_FILES ||
        (file_count != 0U && files == NULL) ||
        (message == NULL && file_count == 0U) ||
        (message != NULL &&
            dcc_message_builder_validate_for_json(message) != DCC_OK)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (file_count != 0U) {
        size_t payload_json_len = 2U;
        if ((message != NULL &&
                dcc_message_builder_measure_json(
                    message, &payload_json_len
                ) != DCC_OK) ||
            dcc_endpoint_multipart_preflight(
                payload_json_len, files, file_count
            ) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_message_list_preflight(
    const dcc_rest_message_list_query_t *query,
    dcc_endpoint_record_view_t *out
) {
    const uint64_t known = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT;
    if (out == NULL) return DCC_ERR_INVALID_ARG;
    *out = (dcc_endpoint_record_view_t){0};
    if (query == NULL) return DCC_OK;
    dcc_status_t status = dcc_endpoint_record_read(
        query,
        offsetof(dcc_rest_message_list_query_t, version),
        offsetof(dcc_rest_message_list_query_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_message_list_query_t, present),
        DCC_REST_MESSAGE_LIST_QUERY_VERSION,
        known,
        out
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_message_list_query_t, out,
            DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND, around
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_message_list_query_t, out,
            DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE, before
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_message_list_query_t, out,
            DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER, after
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_message_list_query_t, out,
            DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT, limit
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    uint64_t cursors = out->present &
        (DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND |
         DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE |
         DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER);
    if ((cursors & (cursors - 1U)) != 0U ||
        ((out->present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND) != 0U &&
            query->around == 0U) ||
        ((out->present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE) != 0U &&
            query->before == 0U) ||
        ((out->present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER) != 0U &&
            query->after == 0U) ||
        ((out->present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT) != 0U &&
            (query->limit == 0U || query->limit > 100U))) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_id_page_preflight(
    const dcc_rest_id_page_t *page,
    uint64_t allowed_present,
    dcc_endpoint_record_view_t *out
) {
    const uint64_t known = DCC_REST_ID_PAGE_PRESENT_BEFORE |
        DCC_REST_ID_PAGE_PRESENT_AFTER |
        DCC_REST_ID_PAGE_PRESENT_LIMIT;
    if (out == NULL || (allowed_present & ~known) != 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_endpoint_record_view_t){0};
    if (page == NULL) return DCC_OK;
    dcc_status_t status = dcc_endpoint_record_read(
        page,
        offsetof(dcc_rest_id_page_t, version),
        offsetof(dcc_rest_id_page_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_id_page_t, present),
        DCC_REST_ID_PAGE_VERSION,
        allowed_present,
        out
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_id_page_t, out, DCC_REST_ID_PAGE_PRESENT_BEFORE, before
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_id_page_t, out, DCC_REST_ID_PAGE_PRESENT_AFTER, after
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_id_page_t, out, DCC_REST_ID_PAGE_PRESENT_LIMIT, limit
        ) ||
        ((out->present & DCC_REST_ID_PAGE_PRESENT_BEFORE) != 0U &&
            page->before == 0U) ||
        ((out->present & DCC_REST_ID_PAGE_PRESENT_AFTER) != 0U &&
            page->after == 0U) ||
        ((out->present & DCC_REST_ID_PAGE_PRESENT_LIMIT) != 0U &&
            (page->limit == 0U || page->limit > 100U))) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_pin_page_preflight(
    const dcc_rest_pin_page_t *page,
    dcc_endpoint_record_view_t *out
) {
    const uint64_t known = DCC_REST_PIN_PAGE_PRESENT_BEFORE |
        DCC_REST_PIN_PAGE_PRESENT_LIMIT;
    if (out == NULL) return DCC_ERR_INVALID_ARG;
    *out = (dcc_endpoint_record_view_t){0};
    if (page == NULL) return DCC_OK;
    dcc_status_t status = dcc_endpoint_record_read(
        page,
        offsetof(dcc_rest_pin_page_t, version),
        offsetof(dcc_rest_pin_page_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_pin_page_t, present),
        DCC_REST_PIN_PAGE_VERSION,
        known,
        out
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_pin_page_t, out, DCC_REST_PIN_PAGE_PRESENT_BEFORE, before
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_pin_page_t, out, DCC_REST_PIN_PAGE_PRESENT_LIMIT, limit
        ) ||
        ((out->present & DCC_REST_PIN_PAGE_PRESENT_BEFORE) != 0U &&
            (page->before == NULL || page->before[0] == '\0')) ||
        ((out->present & DCC_REST_PIN_PAGE_PRESENT_LIMIT) != 0U &&
            (page->limit == 0U || page->limit > 50U))) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_reaction_query_preflight(
    const dcc_rest_reaction_query_t *query,
    dcc_endpoint_record_view_t *out
) {
    const uint64_t known = DCC_REST_REACTION_QUERY_PRESENT_TYPE |
        DCC_REST_REACTION_QUERY_PRESENT_AFTER |
        DCC_REST_REACTION_QUERY_PRESENT_LIMIT;
    if (out == NULL) return DCC_ERR_INVALID_ARG;
    *out = (dcc_endpoint_record_view_t){0};
    if (query == NULL) return DCC_OK;
    dcc_status_t status = dcc_endpoint_record_read(
        query,
        offsetof(dcc_rest_reaction_query_t, version),
        offsetof(dcc_rest_reaction_query_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_reaction_query_t, present),
        DCC_REST_REACTION_QUERY_VERSION,
        known,
        out
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_reaction_query_t, out,
            DCC_REST_REACTION_QUERY_PRESENT_TYPE, type
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_reaction_query_t, out,
            DCC_REST_REACTION_QUERY_PRESENT_AFTER, after
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_reaction_query_t, out,
            DCC_REST_REACTION_QUERY_PRESENT_LIMIT, limit
        ) ||
        ((out->present & DCC_REST_REACTION_QUERY_PRESENT_TYPE) != 0U &&
            query->type != DCC_REST_REACTION_NORMAL &&
            query->type != DCC_REST_REACTION_BURST) ||
        ((out->present & DCC_REST_REACTION_QUERY_PRESENT_AFTER) != 0U &&
            query->after == 0U) ||
        ((out->present & DCC_REST_REACTION_QUERY_PRESENT_LIMIT) != 0U &&
            (query->limit == 0U || query->limit > 100U))) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_interaction_response_preflight(
    const dcc_rest_interaction_response_t *response,
    dcc_endpoint_interaction_view_t *out
) {
    const uint64_t data_mask = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE |
        DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL |
        DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE;
    const uint64_t known_mask = data_mask |
        DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE;
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_endpoint_interaction_view_t){0};
    dcc_status_t status = dcc_endpoint_record_read(
        response,
        offsetof(dcc_rest_interaction_response_t, version),
        offsetof(dcc_rest_interaction_response_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_interaction_response_t, type),
        DCC_REST_INTERACTION_RESPONSE_VERSION,
        known_mask,
        &out->record
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_interaction_response_t, &out->record,
            DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE,
            with_response
        ) ||
        !dcc_endpoint_present_field_covered(
            &out->record,
            out->record.present & data_mask,
            offsetof(dcc_rest_interaction_response_t, data),
            sizeof(response->data)
        )) {
        return DCC_ERR_INVALID_ARG;
    }

    out->type = response->type;
    if ((out->record.present &
            DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE) != 0U) {
        out->with_response = response->with_response;
        if (out->with_response > 1U) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    if ((out->record.present & data_mask) != 0U) {
        out->data = response->data;
    }
    if (dcc_endpoint_file_pair_covered(
            out->record.size,
            offsetof(dcc_rest_interaction_response_t, files),
            sizeof(response->files),
            offsetof(dcc_rest_interaction_response_t, file_count),
            sizeof(response->file_count)
        )) {
        out->files = response->files;
        out->file_count = response->file_count;
    }
    if (out->file_count > DCC_ENDPOINT_MAX_FILES ||
        (out->file_count != 0U && out->files == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t expected_data = 0U;
    int files_allowed = 0;
    switch (out->type) {
        case DCC_INTERACTION_RESPONSE_PONG:
        case DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE:
        case DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED:
        case DCC_INTERACTION_RESPONSE_LAUNCH_ACTIVITY:
            break;
        case DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE:
        case DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE:
            expected_data = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
            files_allowed = 1;
            if (out->data.message == NULL ||
                dcc_message_builder_validate_for_json(out->data.message) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
            break;
        case DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE:
            expected_data = out->record.present &
                DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
            if (expected_data != 0U) {
                dcc_builder_abi_view_t message_view;
                const dcc_message_builder_t *message = out->data.message;
                if (message == NULL ||
                    dcc_message_builder_validate_for_json(message) != DCC_OK ||
                    dcc_message_builder_abi_validate(message, &message_view) != DCC_OK ||
                    message_view.present != DCC_MESSAGE_BUILDER_PRESENT_FLAGS ||
                    (message->flags & ~DCC_MESSAGE_FLAG_EPHEMERAL) != 0U) {
                    return DCC_ERR_INVALID_ARG;
                }
            }
            break;
        case DCC_INTERACTION_RESPONSE_AUTOCOMPLETE: {
            expected_data = DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE;
            if (out->data.autocomplete == NULL ||
                dcc_autocomplete_builder_validate_for_json(
                    out->data.autocomplete
                ) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
            break;
        }
        case DCC_INTERACTION_RESPONSE_MODAL: {
            expected_data = DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL;
            if (out->data.modal == NULL ||
                dcc_modal_builder_validate_for_json(out->data.modal) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
            break;
        }
        default:
            return DCC_ERR_INVALID_ARG;
    }
    if ((out->record.present & data_mask) != expected_data ||
        (!files_allowed && out->file_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out->file_count != 0U) {
        size_t payload_json_len = 0U;
        if (dcc_endpoint_measure_interaction_message_json(
                out->type, out->data.message, &payload_json_len
            ) != DCC_OK ||
            dcc_endpoint_multipart_preflight(
                payload_json_len, out->files, out->file_count
            ) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_webhook_builder_preflight(
    const dcc_rest_webhook_builder_t *builder,
    int create,
    int token_authenticated,
    dcc_endpoint_record_view_t *out
) {
    const uint64_t known = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID;
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_endpoint_record_read(
        builder,
        offsetof(dcc_rest_webhook_builder_t, version),
        offsetof(dcc_rest_webhook_builder_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_webhook_builder_t, present),
        DCC_REST_WEBHOOK_BUILDER_VERSION,
        known,
        out
    );
    if (status != DCC_OK || out->present == 0U ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_builder_t, out,
            DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME, name
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_builder_t, out,
            DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR, avatar
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_builder_t, out,
            DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID, channel_id
        ) ||
        (create &&
            (out->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME) == 0U) ||
        ((create || token_authenticated) &&
            (out->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID) != 0U) ||
        ((out->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME) != 0U &&
            (builder->name == NULL || builder->name[0] == '\0')) ||
        ((out->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID) != 0U &&
            builder->channel_id == 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_webhook_execute_preflight(
    const dcc_rest_webhook_execute_t *execute,
    dcc_endpoint_webhook_execute_view_t *out
) {
    const uint64_t known = DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_endpoint_webhook_execute_view_t){0};
    dcc_status_t status = dcc_endpoint_record_read(
        execute,
        offsetof(dcc_rest_webhook_execute_t, version),
        offsetof(dcc_rest_webhook_execute_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_webhook_execute_t, message),
        DCC_REST_WEBHOOK_EXECUTE_VERSION,
        known,
        &out->record
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME, username
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL, avatar_url
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME, thread_name
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS, applied_tag_ids
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS, applied_tag_count
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT, wait
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID, thread_id
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_execute_t, &out->record,
            DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS, with_components
        )) {
        return DCC_ERR_INVALID_ARG;
    }

    out->message = execute->message;
    if (dcc_endpoint_file_pair_covered(
            out->record.size,
            offsetof(dcc_rest_webhook_execute_t, files),
            sizeof(execute->files),
            offsetof(dcc_rest_webhook_execute_t, file_count),
            sizeof(execute->file_count)
        )) {
        out->files = execute->files;
        out->file_count = execute->file_count;
    }
    if ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME) != 0U)
        out->username = execute->username;
    if ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL) != 0U)
        out->avatar_url = execute->avatar_url;
    if ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME) != 0U)
        out->thread_name = execute->thread_name;
    if ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS) != 0U) {
        out->applied_tag_ids = execute->applied_tag_ids;
        out->applied_tag_count = execute->applied_tag_count;
    }
    if ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT) != 0U)
        out->wait = execute->wait;
    if ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID) != 0U)
        out->thread_id = execute->thread_id;
    if ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS) != 0U)
        out->with_components = execute->with_components;

    if ((out->message != NULL &&
            dcc_message_builder_validate_for_json(out->message) != DCC_OK) ||
        out->file_count > DCC_ENDPOINT_MAX_FILES ||
        (out->file_count != 0U && out->files == NULL) ||
        (out->username == NULL &&
            (out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME) != 0U) ||
        (out->avatar_url == NULL &&
            (out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL) != 0U) ||
        ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME) != 0U &&
            (out->thread_name == NULL || out->thread_name[0] == '\0')) ||
        ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID) != 0U &&
            out->thread_id == 0U) ||
        ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT) != 0U &&
            out->wait > 1U) ||
        ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS) != 0U &&
            out->with_components > 1U) ||
        ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME) != 0U &&
            (out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID) != 0U) ||
        ((out->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS) != 0U &&
            (out->applied_tag_count > DCC_ENDPOINT_MAX_APPLIED_TAGS ||
             (out->applied_tag_count != 0U && out->applied_tag_ids == NULL)))) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < out->applied_tag_count; ++i) {
        if (out->applied_tag_ids[i] == 0U) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    if (out->file_count != 0U) {
        size_t payload_json_len = 0U;
        if (dcc_endpoint_measure_webhook_execute_json(
                out, &payload_json_len
            ) != DCC_OK ||
            dcc_endpoint_multipart_preflight(
                payload_json_len, out->files, out->file_count
            ) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_webhook_message_query_preflight(
    const dcc_rest_webhook_message_query_t *query,
    dcc_endpoint_record_view_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_endpoint_record_view_t){0};
    if (query == NULL) {
        return DCC_OK;
    }
    dcc_status_t status = dcc_endpoint_record_read(
        query,
        offsetof(dcc_rest_webhook_message_query_t, version),
        offsetof(dcc_rest_webhook_message_query_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_webhook_message_query_t, present),
        DCC_REST_WEBHOOK_MESSAGE_QUERY_VERSION,
        DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID,
        out
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_message_query_t, out,
            DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID, thread_id
        ) ||
        ((out->present & DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID) != 0U &&
            query->thread_id == 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_webhook_message_edit_preflight(
    const dcc_rest_webhook_message_edit_t *edit,
    dcc_endpoint_webhook_edit_view_t *out
) {
    const uint64_t known = DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID |
        DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS;
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_endpoint_webhook_edit_view_t){0};
    dcc_status_t status = dcc_endpoint_record_read(
        edit,
        offsetof(dcc_rest_webhook_message_edit_t, version),
        offsetof(dcc_rest_webhook_message_edit_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_webhook_message_edit_t, payload),
        DCC_REST_WEBHOOK_MESSAGE_EDIT_VERSION,
        known,
        &out->record
    );
    if (status != DCC_OK ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_message_edit_t, &out->record,
            DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID, thread_id
        ) ||
        !DCC_ENDPOINT_PRESENT_FIELD(
            dcc_rest_webhook_message_edit_t, &out->record,
            DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS,
            with_components
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    out->payload = edit->payload;
    if ((out->record.present & DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID) != 0U)
        out->thread_id = edit->thread_id;
    if ((out->record.present & DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS) != 0U)
        out->with_components = edit->with_components;
    if (out->payload == NULL ||
        dcc_endpoint_message_payload_preflight(out->payload) != DCC_OK ||
        ((out->record.present & DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID) != 0U &&
            out->thread_id == 0U) ||
        ((out->record.present & DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS) != 0U &&
            out->with_components > 1U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_endpoint_webhook_compat_preflight(
    const dcc_rest_webhook_compat_payload_t *payload,
    const void **out_body,
    size_t *out_body_len
) {
    if (out_body == NULL || out_body_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = NULL;
    *out_body_len = 0U;
    dcc_endpoint_record_view_t view;
    dcc_status_t status = dcc_endpoint_record_read(
        payload,
        offsetof(dcc_rest_webhook_compat_payload_t, version),
        SIZE_MAX,
        DCC_ENDPOINT_FIELD_END(dcc_rest_webhook_compat_payload_t, version),
        DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION,
        0U,
        &view
    );
    if (status != DCC_OK) {
        return status;
    }
    if (dcc_endpoint_file_pair_covered(
            view.size,
            offsetof(dcc_rest_webhook_compat_payload_t, body),
            sizeof(payload->body),
            offsetof(dcc_rest_webhook_compat_payload_t, body_len),
            sizeof(payload->body_len)
        )) {
        *out_body = payload->body;
        *out_body_len = payload->body_len;
    }
    return *out_body_len != 0U && *out_body == NULL
        ? DCC_ERR_INVALID_ARG
        : DCC_OK;
}

dcc_status_t dcc_endpoint_prepare(
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request,
    dcc_rest_call_options_t *out_options
) {
    if (out_request != NULL) {
        *out_request = NULL;
    }
    if (out_options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options == NULL) {
        *out_options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
        return DCC_OK;
    }
    if (!dcc_endpoint_options_valid(options)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_options = *options;
    return DCC_OK;
}

dcc_status_t dcc_endpoint_submit(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_endpoint_body_t *body,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.method = method;
    description.path = path;
    if (body != NULL) {
        description.content_type = body->content_type;
        description.body = body->data;
        description.body_len = body->len;
    }
    description.options = *options;
    return dcc_rest_submit(client, &description, out_request);
}

dcc_status_t dcc_endpoint_submit_legacy_raw(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const char *content_type,
    const void *body,
    size_t body_len,
    dcc_rest_cb callback,
    void *user_data
) {
    if (client == NULL || dcc_rest_method_name(method) == NULL ||
        path == NULL || path[0] == '\0' ||
        (body == NULL && (body_len != 0U || content_type != NULL)) ||
        (body != NULL && (content_type == NULL || content_type[0] == '\0'))) {
        return DCC_ERR_INVALID_ARG;
    }

    char *owned_path = NULL;
    const char *submit_path = path;
    dcc_status_t status = DCC_OK;
    if (query != NULL && query[0] != '\0') {
        const char *query_data = query[0] == '?' ? query + 1 : query;
        size_t query_len = strlen(query_data);
        dcc_rest_buffer_t query_view = {
            .data = (char *)(uintptr_t)query_data,
            .len = query_len,
            .cap = query_len,
        };
        if (query_view.len != 0U) {
            status = dcc_endpoint_path_with_query(path, &query_view, &owned_path);
            submit_path = owned_path;
        }
    }

    dcc_rest_call_options_t options;
    void *bridge = NULL;
    if (status == DCC_OK) {
        status = dcc_endpoint_legacy_options(
            callback, user_data, &options, &bridge
        );
    }
    if (status == DCC_OK) {
        dcc_endpoint_body_t request_body = {
            .data = (char *)(uintptr_t)body,
            .len = body_len,
            .content_type = content_type,
        };
        status = dcc_endpoint_submit(
            client, method, submit_path, body != NULL ? &request_body : NULL,
            &options, NULL
        );
    }
    if (status != DCC_OK) {
        dcc_endpoint_legacy_bridge_release(bridge);
    }
    free(owned_path);
    return status;
}

dcc_status_t dcc_endpoint_path_with_query(
    const char *path,
    const dcc_rest_buffer_t *query,
    char **out_path
) {
    if (out_path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_path = NULL;
    if (path == NULL || path[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    size_t path_len = strlen(path);
    size_t query_len = query != NULL ? query->len : 0U;
    if (path_len > SIZE_MAX - query_len - 2U) {
        return DCC_ERR_NOMEM;
    }
    char *combined = (char *)malloc(path_len + (query_len != 0U ? query_len + 1U : 0U) + 1U);
    if (combined == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(combined, path, path_len);
    size_t used = path_len;
    if (query_len != 0U) {
        combined[used++] = '?';
        memcpy(combined + used, query->data, query_len);
        used += query_len;
    }
    combined[used] = '\0';
    *out_path = combined;
    return DCC_OK;
}

static dcc_status_t dcc_endpoint_body_from_json_files(
    char *json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_endpoint_body_t *out_body
) {
    if (out_body == NULL || json == NULL || file_count > DCC_ENDPOINT_MAX_FILES ||
        (file_count != 0U && files == NULL)) {
        free(json);
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = (dcc_endpoint_body_t){0};
    if (file_count == 0U) {
        out_body->data = json;
        out_body->len = strlen(json);
        out_body->content_type = "application/json";
        return DCC_OK;
    }
    dcc_rest_multipart_field_t field = {"payload_json", json};
    char *multipart = NULL;
    size_t multipart_len = 0U;
    dcc_status_t status = dcc_rest_build_multipart_body(
        &field, 1U, files, file_count, &multipart, &multipart_len
    );
    free(json);
    if (status != DCC_OK) {
        return status;
    }
    out_body->data = multipart;
    out_body->len = multipart_len;
    out_body->content_type = DCC_REST_MULTIPART_CONTENT_TYPE;
    return DCC_OK;
}

dcc_status_t dcc_endpoint_build_message_body(
    const dcc_rest_message_payload_t *payload,
    dcc_endpoint_body_t *out_body
) {
    if (out_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = (dcc_endpoint_body_t){0};
    dcc_status_t status = dcc_endpoint_message_payload_preflight(payload);
    if (status != DCC_OK) return status;
    const dcc_message_builder_t *message = payload->message;
    const dcc_rest_multipart_file_t *files = NULL;
    size_t file_count = 0U;
    if (dcc_endpoint_file_pair_covered(
            payload->size,
            offsetof(dcc_rest_message_payload_t, files),
            sizeof(payload->files),
            offsetof(dcc_rest_message_payload_t, file_count),
            sizeof(payload->file_count)
        )) {
        files = payload->files;
        file_count = payload->file_count;
    }
    char *json = NULL;
    status = message != NULL
        ? dcc_message_builder_build_json(message, &json)
        : DCC_OK;
    if (status == DCC_OK && json == NULL) {
        json = (char *)malloc(3U);
        if (json == NULL) {
            status = DCC_ERR_NOMEM;
        } else {
            memcpy(json, "{}", 3U);
        }
    }
    return status == DCC_OK
        ? dcc_endpoint_body_from_json_files(json, files, file_count, out_body)
        : status;
}

static dcc_status_t dcc_endpoint_append_interaction_message_json(
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    dcc_message_json_buffer_t *body
) {
    if (message == NULL || body == NULL ||
        dcc_message_builder_validate_for_json(message) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_message_json_append_cstr(body, "{\"type\":");
    if (status == DCC_OK) {
        status = dcc_message_json_append_u64(body, (uint64_t)type);
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(body, ",\"data\":");
    }
    if (status == DCC_OK) {
        status = dcc_message_builder_append_json(body, message);
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(body, "}");
    }
    return status;
}

static dcc_status_t dcc_endpoint_interaction_message_json(
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    dcc_message_json_buffer_t body = {0};
    dcc_status_t status = dcc_endpoint_append_interaction_message_json(
        type, message, &body
    );
    if (status != DCC_OK) {
        dcc_message_json_buffer_deinit(&body);
        return status;
    }
    *out_json = body.data;
    return DCC_OK;
}

dcc_status_t dcc_endpoint_measure_interaction_message_json(
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    size_t *out_json_len
) {
    if (out_json_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json_len = 0U;
    dcc_message_json_buffer_t body;
    dcc_message_json_buffer_init_count(&body);
    dcc_status_t status = dcc_endpoint_append_interaction_message_json(
        type, message, &body
    );
    if (status == DCC_OK) {
        *out_json_len = body.len;
    }
    dcc_message_json_buffer_deinit(&body);
    return status;
}

static dcc_status_t dcc_endpoint_interaction_type_json(
    dcc_interaction_response_type_t type,
    char **out_json
) {
    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"type\":");
    if (status == DCC_OK) status = dcc_rest_buffer_append_u64_text(&body, (uint64_t)type);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "}");
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&body);
        return status;
    }
    *out_json = body.data;
    return DCC_OK;
}

dcc_status_t dcc_endpoint_build_interaction_body(
    const dcc_rest_interaction_response_t *response,
    dcc_endpoint_body_t *out_body
) {
    if (out_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = (dcc_endpoint_body_t){0};
    dcc_endpoint_interaction_view_t view;
    dcc_status_t status = dcc_endpoint_interaction_response_preflight(
        response, &view
    );
    if (status != DCC_OK) return status;
    char *json = NULL;
    switch (view.type) {
        case DCC_INTERACTION_RESPONSE_PONG:
        case DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE:
        case DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED:
        case DCC_INTERACTION_RESPONSE_LAUNCH_ACTIVITY:
            status = dcc_endpoint_interaction_type_json(view.type, &json);
            break;
        case DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE:
        case DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE:
            status = dcc_endpoint_interaction_message_json(
                view.type, view.data.message, &json
            );
            break;
        case DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE:
            if ((view.record.present &
                    DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE) != 0U) {
                status = dcc_endpoint_interaction_message_json(
                    view.type, view.data.message, &json
                );
            } else {
                status = dcc_endpoint_interaction_type_json(view.type, &json);
            }
            break;
        case DCC_INTERACTION_RESPONSE_AUTOCOMPLETE:
            status = dcc_autocomplete_builder_build_json(
                view.data.autocomplete, &json
            );
            break;
        case DCC_INTERACTION_RESPONSE_MODAL:
            status = dcc_modal_builder_build_json(view.data.modal, &json);
            break;
        default:
            return DCC_ERR_INVALID_ARG;
    }
    return status == DCC_OK
        ? dcc_endpoint_body_from_json_files(
            json, view.files, view.file_count, out_body
        )
        : status;
}

dcc_status_t dcc_endpoint_build_webhook_builder_body(
    const dcc_rest_webhook_builder_t *builder,
    int create,
    int token_authenticated,
    dcc_endpoint_body_t *out_body
) {
    if (out_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_endpoint_record_view_t view;
    dcc_status_t status = dcc_endpoint_webhook_builder_preflight(
        builder, create, token_authenticated, &view
    );
    if (status != DCC_OK) return status;
    dcc_rest_buffer_t json = {0};
    int first = 1;
    status = dcc_rest_buffer_append_cstr(&json, "{");
    if (status == DCC_OK && (view.present & DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME) != 0U) {
        status = dcc_rest_json_append_string_member(&json, &first, "name", builder->name);
    }
    if (status == DCC_OK && (view.present & DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR) != 0U) {
        status = dcc_rest_json_append_nullable_string_member(&json, &first, "avatar", builder->avatar);
    }
    if (status == DCC_OK && (view.present & DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID) != 0U) {
        status = dcc_rest_json_append_u64_string_member(
            &json, &first, "channel_id", builder->channel_id
        );
    }
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "}");
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&json);
        return status;
    }
    *out_body = (dcc_endpoint_body_t){json.data, json.len, "application/json"};
    return DCC_OK;
}

static dcc_status_t dcc_endpoint_message_append_snowflake_array_member(
    dcc_message_json_buffer_t *json,
    int *first,
    const char *name,
    const dcc_snowflake_t *values,
    size_t count
) {
    if (json == NULL || first == NULL || name == NULL ||
        (count != 0U && values == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_message_json_member_prefix(json, first, name);
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(json, "[");
    }
    for (size_t i = 0U; status == DCC_OK && i < count; ++i) {
        if (i != 0U) {
            status = dcc_message_json_append_cstr(json, ",");
        }
        if (status == DCC_OK) {
            status = dcc_message_json_append_snowflake_string(json, values[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(json, "]");
    }
    return status;
}

static dcc_status_t dcc_endpoint_append_webhook_execute_json(
    const dcc_endpoint_webhook_execute_view_t *view,
    dcc_message_json_buffer_t *json
) {
    if (view == NULL || json == NULL ||
        (view->message != NULL &&
            dcc_message_builder_validate_for_json(view->message) != DCC_OK)) {
        return DCC_ERR_INVALID_ARG;
    }

    int first = 1;
    dcc_status_t status = dcc_message_json_append_cstr(json, "{");
    if (status == DCC_OK && view->message != NULL) {
        status = dcc_message_builder_append_members_json(
            json, &first, view->message
        );
    }
    if (status == DCC_OK &&
        (view->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME) != 0U) {
        status = dcc_message_json_append_string_member(
            json, &first, "username", view->username
        );
    }
    if (status == DCC_OK &&
        (view->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL) != 0U) {
        status = dcc_message_json_append_string_member(
            json, &first, "avatar_url", view->avatar_url
        );
    }
    if (status == DCC_OK &&
        (view->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME) != 0U) {
        status = dcc_message_json_append_string_member(
            json, &first, "thread_name", view->thread_name
        );
    }
    if (status == DCC_OK &&
        (view->record.present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS) != 0U) {
        status = dcc_endpoint_message_append_snowflake_array_member(
            json, &first, "applied_tags", view->applied_tag_ids,
            view->applied_tag_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(json, "}");
    }
    return status;
}

dcc_status_t dcc_endpoint_measure_webhook_execute_json(
    const dcc_endpoint_webhook_execute_view_t *view,
    size_t *out_json_len
) {
    if (view == NULL || out_json_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json_len = 0U;
    dcc_message_json_buffer_t json;
    dcc_message_json_buffer_init_count(&json);
    dcc_status_t status = dcc_endpoint_append_webhook_execute_json(
        view, &json
    );
    if (status == DCC_OK) {
        *out_json_len = json.len;
    }
    dcc_message_json_buffer_deinit(&json);
    return status;
}

dcc_status_t dcc_endpoint_build_webhook_execute_body(
    const dcc_rest_webhook_execute_t *execute,
    dcc_endpoint_body_t *out_body
) {
    if (out_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_endpoint_webhook_execute_view_t view;
    dcc_status_t status = dcc_endpoint_webhook_execute_preflight(execute, &view);
    if (status != DCC_OK) {
        return status;
    }
    dcc_message_json_buffer_t json = {0};
    status = dcc_endpoint_append_webhook_execute_json(&view, &json);
    if (status != DCC_OK) {
        dcc_message_json_buffer_deinit(&json);
        return status;
    }
    return dcc_endpoint_body_from_json_files(
        json.data, view.files, view.file_count, out_body
    );
}

void dcc_endpoint_body_deinit(dcc_endpoint_body_t *body) {
    if (body != NULL) {
        free(body->data);
        *body = (dcc_endpoint_body_t){0};
    }
}

dcc_status_t dcc_endpoint_legacy_options(
    dcc_rest_cb callback,
    void *user_data,
    dcc_rest_call_options_t *out_options,
    void **out_bridge
) {
    if (out_options == NULL || out_bridge == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    *out_bridge = NULL;
    if (callback == NULL) {
        return DCC_OK;
    }
    dcc_endpoint_legacy_bridge_t *bridge =
        (dcc_endpoint_legacy_bridge_t *)malloc(sizeof(*bridge));
    if (bridge == NULL) {
        return DCC_ERR_NOMEM;
    }
    bridge->callback = callback;
    bridge->user_data = user_data;
    out_options->callback = dcc_endpoint_legacy_result;
    out_options->user_data = bridge;
    *out_bridge = bridge;
    return DCC_OK;
}

void dcc_endpoint_legacy_bridge_release(void *bridge) {
    free(bridge);
}
