#ifndef DCC_REST_TYPES_INTERACTIONS_H
#define DCC_REST_TYPES_INTERACTIONS_H

#include <dcc/rest/base.h>

#define DCC_REST_INTERACTION_RESPONSE_VERSION 1U

#define DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE UINT64_C(1)
#define DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL (UINT64_C(1) << 1U)
#define DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE (UINT64_C(1) << 2U)
#define DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE (UINT64_C(1) << 3U)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_interaction_response_type {
    DCC_INTERACTION_RESPONSE_PONG = 1,
    DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE = 4,
    DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE = 5,
    DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE = 6,
    DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE = 7,
    DCC_INTERACTION_RESPONSE_AUTOCOMPLETE = 8,
    DCC_INTERACTION_RESPONSE_MODAL = 9,
    DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED = 10,
    DCC_INTERACTION_RESPONSE_LAUNCH_ACTIVITY = 12
} dcc_interaction_response_type_t;

typedef union dcc_rest_interaction_response_data {
    const dcc_message_builder_t *message;
    const dcc_modal_builder_t *modal;
    const dcc_autocomplete_builder_t *autocomplete;
} dcc_rest_interaction_response_data_t;

typedef struct dcc_rest_interaction_response {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_interaction_response_type_t type;
    uint8_t with_response;
    dcc_rest_interaction_response_data_t data;
    const dcc_rest_multipart_file_t *files;
    size_t file_count;
} dcc_rest_interaction_response_t;

#define DCC_REST_INTERACTION_RESPONSE_INIT \
    { sizeof(dcc_rest_interaction_response_t), \
      DCC_REST_INTERACTION_RESPONSE_VERSION, UINT64_C(0), \
      (dcc_interaction_response_type_t)0, 0U, { NULL }, NULL, 0U }

static inline void dcc_rest_interaction_response_init(
    dcc_rest_interaction_response_t *response
) {
    if (response != NULL) {
        dcc_rest_interaction_response_t value =
            DCC_REST_INTERACTION_RESPONSE_INIT;
        *response = value;
    }
}

static inline dcc_status_t dcc_rest_interaction_response_set_pong(
    dcc_rest_interaction_response_t *response
) {
    if (response == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_PONG;
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_message(
    dcc_rest_interaction_response_t *response,
    const dcc_message_builder_t *message
) {
    if (response == NULL || message == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE;
    response->present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response->data.message = message;
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_deferred_message(
    dcc_rest_interaction_response_t *response,
    const dcc_message_builder_t *message
) {
    if (response == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type =
        DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE;
    if (message != NULL) {
        response->present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
        response->data.message = message;
    }
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_deferred_update(
    dcc_rest_interaction_response_t *response
) {
    if (response == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE;
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_update_message(
    dcc_rest_interaction_response_t *response,
    const dcc_message_builder_t *message
) {
    if (response == NULL || message == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE;
    response->present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response->data.message = message;
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_autocomplete(
    dcc_rest_interaction_response_t *response,
    const dcc_autocomplete_builder_t *autocomplete
) {
    if (response == NULL || autocomplete == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_AUTOCOMPLETE;
    response->present = DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE;
    response->data.autocomplete = autocomplete;
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_modal(
    dcc_rest_interaction_response_t *response,
    const dcc_modal_builder_t *modal
) {
    if (response == NULL || modal == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_MODAL;
    response->present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL;
    response->data.modal = modal;
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_premium_required(
    dcc_rest_interaction_response_t *response
) {
    if (response == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED;
    return DCC_OK;
}

static inline dcc_status_t dcc_rest_interaction_response_set_launch_activity(
    dcc_rest_interaction_response_t *response
) {
    if (response == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_interaction_response_init(response);
    response->type = DCC_INTERACTION_RESPONSE_LAUNCH_ACTIVITY;
    return DCC_OK;
}

#ifdef __cplusplus
}
#endif

#endif
