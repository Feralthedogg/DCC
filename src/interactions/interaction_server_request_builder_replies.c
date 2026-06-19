#include "internal/interactions/dcc_interaction_server_internal.h"

dcc_status_t dcc_interaction_request_reply_modal(
    dcc_interaction_request_t *request,
    const dcc_modal_builder_t *modal
) {
    if (request == NULL || modal == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_modal_builder_build_json(modal, &body);
    if (status == DCC_OK) {
        status = dcc_interaction_request_reply_json(request, body);
    }
    dcc_modal_builder_json_free(body);
    return status;
}

dcc_status_t dcc_interaction_request_reply_autocomplete(
    dcc_interaction_request_t *request,
    const dcc_autocomplete_builder_t *autocomplete
) {
    if (request == NULL || autocomplete == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_autocomplete_builder_build_json(autocomplete, &body);
    if (status == DCC_OK) {
        status = dcc_interaction_request_reply_json(request, body);
    }
    dcc_autocomplete_builder_json_free(body);
    return status;
}
