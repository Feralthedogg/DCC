#include "internal/component_session/dcc_component_session_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_component_session_button(
    dcc_component_session_t *session,
    const char *action,
    const char *label,
    dcc_button_style_t style,
    dcc_component_builder_t *out
) {
    dcc_component_session_state_t *state = dcc_component_session_state(session);
    if (state == NULL || label == NULL || label[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *custom_id = NULL;
    dcc_status_t status = dcc_component_session_build_custom_id_heap(state, action, &custom_id);
    if (status != DCC_OK) {
        return status;
    }

    dcc_component_builder_t builder;
    dcc_component_builder_init(&builder, DCC_COMPONENT_BUTTON);
    status = dcc_component_builder_set_style(&builder, style);
    if (status == DCC_OK) {
        status = dcc_component_builder_set_label(&builder, label);
    }
    if (status == DCC_OK) {
        status = dcc_component_session_state_adopt_id(state, custom_id);
    }
    if (status == DCC_OK) {
        status = dcc_component_builder_set_custom_id(&builder, custom_id);
    }
    if (status != DCC_OK) {
        free(custom_id);
        memset(out, 0, sizeof(*out));
        return status;
    }
    *out = builder;
    return DCC_OK;
}
