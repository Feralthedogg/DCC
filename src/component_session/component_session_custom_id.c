#include "internal/component_session/dcc_component_session_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int dcc_component_session_hex_string(const char *value) {
    if (value == NULL || strlen(value) != DCC_COMPONENT_SESSION_SIG_HEX_LEN) {
        return 0;
    }
    for (size_t i = 0; i < DCC_COMPONENT_SESSION_SIG_HEX_LEN; ++i) {
        char ch = value[i];
        if (!((ch >= '0' && ch <= '9') ||
              (ch >= 'a' && ch <= 'f') ||
              (ch >= 'A' && ch <= 'F'))) {
            return 0;
        }
    }
    return 1;
}

static dcc_status_t dcc_component_session_copy_part(
    char *dst,
    size_t dst_len,
    const char *begin,
    const char *end,
    size_t max_token_len
) {
    if (dst == NULL || begin == NULL || end == NULL || end < begin) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t len = (size_t)(end - begin);
    if (len == 0U || len > max_token_len || len + 1U > dst_len) {
        return DCC_ERR_JSON;
    }
    memcpy(dst, begin, len);
    dst[len] = '\0';
    return dcc_component_session_token_valid(dst, max_token_len) ? DCC_OK : DCC_ERR_JSON;
}

dcc_status_t dcc_component_session_parse_custom_id(
    const char *custom_id,
    dcc_component_session_parts_t *parts
) {
    static const char prefix[] = "dcc:v1:";
    if (custom_id == NULL || parts == NULL || strncmp(custom_id, prefix, sizeof(prefix) - 1U) != 0 ||
        strlen(custom_id) > DCC_COMPONENT_SESSION_CUSTOM_ID_MAX) {
        return DCC_ERR_JSON;
    }
    memset(parts, 0, sizeof(*parts));
    const char *session_begin = custom_id + sizeof(prefix) - 1U;
    const char *session_end = strchr(session_begin, ':');
    const char *action_begin = session_end != NULL ? session_end + 1 : NULL;
    const char *action_end = action_begin != NULL ? strchr(action_begin, ':') : NULL;
    const char *nonce_begin = action_end != NULL ? action_end + 1 : NULL;
    const char *nonce_end = nonce_begin != NULL ? strchr(nonce_begin, ':') : NULL;
    const char *sig_begin = nonce_end != NULL ? nonce_end + 1 : NULL;
    if (session_end == NULL || action_end == NULL || nonce_end == NULL || sig_begin == NULL ||
        strchr(sig_begin, ':') != NULL) {
        return DCC_ERR_JSON;
    }
    dcc_status_t status = dcc_component_session_copy_part(
        parts->session_id,
        sizeof(parts->session_id),
        session_begin,
        session_end,
        DCC_COMPONENT_SESSION_ID_MAX
    );
    if (status == DCC_OK) {
        status = dcc_component_session_copy_part(
            parts->action,
            sizeof(parts->action),
            action_begin,
            action_end,
            DCC_COMPONENT_SESSION_ACTION_MAX
        );
    }
    if (status == DCC_OK) {
        status = dcc_component_session_copy_part(
            parts->nonce,
            sizeof(parts->nonce),
            nonce_begin,
            nonce_end,
            DCC_COMPONENT_SESSION_NONCE_MAX
        );
    }
    if (status != DCC_OK) {
        return status;
    }
    if (strlen(sig_begin) != DCC_COMPONENT_SESSION_SIG_HEX_LEN ||
        !dcc_component_session_hex_string(sig_begin)) {
        return DCC_ERR_JSON;
    }
    memcpy(parts->sig, sig_begin, DCC_COMPONENT_SESSION_SIG_HEX_LEN + 1U);
    return DCC_OK;
}

dcc_status_t dcc_component_session_build_custom_id_heap(
    const dcc_component_session_state_t *state,
    const char *action,
    char **out
) {
    if (state == NULL || action == NULL || out == NULL ||
        !dcc_component_session_token_valid(action, DCC_COMPONENT_SESSION_ACTION_MAX)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    char sig[DCC_COMPONENT_SESSION_SIG_HEX_LEN + 1U];
    dcc_status_t status = dcc_component_session_sign(state, action, state->nonce, sig);
    if (status != DCC_OK) {
        return status;
    }
    int len = snprintf(
        NULL,
        0,
        "dcc:v1:%s:%s:%s:%s",
        state->session_id,
        action,
        state->nonce,
        sig
    );
    if (len <= 0 || (size_t)len > DCC_COMPONENT_SESSION_CUSTOM_ID_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    char *custom_id = (char *)malloc((size_t)len + 1U);
    if (custom_id == NULL) {
        return DCC_ERR_NOMEM;
    }
    (void)snprintf(
        custom_id,
        (size_t)len + 1U,
        "dcc:v1:%s:%s:%s:%s",
        state->session_id,
        action,
        state->nonce,
        sig
    );
    *out = custom_id;
    return DCC_OK;
}

dcc_status_t dcc_component_session_custom_id(
    dcc_component_session_t *session,
    const char *action,
    char *out,
    size_t out_len
) {
    dcc_component_session_state_t *state = dcc_component_session_state(session);
    if (state == NULL || out == NULL || out_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    char *custom_id = NULL;
    dcc_status_t status = dcc_component_session_build_custom_id_heap(state, action, &custom_id);
    if (status != DCC_OK) {
        return status;
    }
    size_t len = strlen(custom_id);
    if (len + 1U > out_len) {
        free(custom_id);
        return DCC_ERR_INVALID_ARG;
    }
    memcpy(out, custom_id, len + 1U);
    free(custom_id);
    return DCC_OK;
}
