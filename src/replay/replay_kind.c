#include "internal/replay/dcc_replay_internal.h"

#include <string.h>

const char *dcc_replay_kind_string(dcc_replay_kind_t kind) {
    switch (kind) {
        case DCC_REPLAY_GATEWAY:
            return "gateway";
        case DCC_REPLAY_INTERACTION:
            return "interaction";
        default:
            return NULL;
    }
}

dcc_status_t dcc_replay_kind_from_string(const char *text, dcc_replay_kind_t *out) {
    if (text == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (strcmp(text, "gateway") == 0) {
        *out = DCC_REPLAY_GATEWAY;
        return DCC_OK;
    }
    if (strcmp(text, "interaction") == 0) {
        *out = DCC_REPLAY_INTERACTION;
        return DCC_OK;
    }
    return DCC_ERR_JSON;
}
