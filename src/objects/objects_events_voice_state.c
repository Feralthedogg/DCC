#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_voice_state_t *dcc_voice_state_clone(const dcc_voice_state_t *voice_state) {
    if (voice_state == NULL) {
        return NULL;
    }

    dcc_voice_state_t *copy = (dcc_voice_state_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *voice_state;
    copy->session_id = dcc_clone_string_or_null(voice_state->session_id);
    copy->request_to_speak_timestamp = dcc_clone_string_or_null(voice_state->request_to_speak_timestamp);
    if ((voice_state->session_id != NULL && copy->session_id == NULL) ||
        (voice_state->request_to_speak_timestamp != NULL && copy->request_to_speak_timestamp == NULL)) {
        dcc_voice_state_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_voice_state_free(dcc_voice_state_t *voice_state) {
    if (voice_state == NULL) {
        return;
    }
    free((void *)voice_state->session_id);
    free((void *)voice_state->request_to_speak_timestamp);
    free(voice_state);
}
