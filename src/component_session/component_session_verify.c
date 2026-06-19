#include "internal/component_session/dcc_component_session_internal.h"

#include <string.h>

dcc_component_session_verify_status_t dcc_component_session_verify_parts(
    const dcc_component_session_state_t *state,
    const dcc_component_session_parts_t *parts,
    const dcc_component_session_check_t *check
) {
    if (state == NULL || parts == NULL || check == NULL ||
        strcmp(state->session_id, parts->session_id) != 0) {
        return DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND;
    }

    char expected[DCC_COMPONENT_SESSION_SIG_HEX_LEN + 1U];
    dcc_status_t status = dcc_component_session_sign(state, parts->action, parts->nonce, expected);
    if (status != DCC_OK || !dcc_component_session_sig_equal(expected, parts->sig)) {
        return DCC_COMPONENT_SESSION_VERIFY_BAD_SIGNATURE;
    }
    if (check->now_ms > state->expires_at_ms) {
        return DCC_COMPONENT_SESSION_VERIFY_EXPIRED;
    }
    if (state->lock_user && check->user_id != state->user_id) {
        return DCC_COMPONENT_SESSION_VERIFY_WRONG_USER;
    }
    if (state->lock_channel && check->channel_id != state->channel_id) {
        return DCC_COMPONENT_SESSION_VERIFY_WRONG_CHANNEL;
    }
    if (state->lock_guild && check->guild_id != state->guild_id) {
        return DCC_COMPONENT_SESSION_VERIFY_WRONG_GUILD;
    }
    return DCC_COMPONENT_SESSION_VERIFY_OK;
}

dcc_status_t dcc_component_session_verify(
    const dcc_component_session_t *session,
    const dcc_component_session_check_t *check,
    dcc_component_session_result_t *result
) {
    if (session == NULL || check == NULL || check->custom_id == NULL || result == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(result, 0, sizeof(*result));
    result->size = sizeof(*result);

    dcc_component_session_parts_t parts;
    dcc_status_t status = dcc_component_session_parse_custom_id(check->custom_id, &parts);
    if (status != DCC_OK) {
        result->status = DCC_COMPONENT_SESSION_VERIFY_BAD_FORMAT;
        return DCC_OK;
    }

    dcc_component_session_state_t *state = dcc_component_session_state(session);
    result->status = dcc_component_session_verify_parts(state, &parts, check);
    memcpy(result->session_id, parts.session_id, sizeof(result->session_id));
    memcpy(result->action, parts.action, sizeof(result->action));
    memcpy(result->nonce, parts.nonce, sizeof(result->nonce));
    result->expires_at_ms = state != NULL ? state->expires_at_ms : 0U;
    return DCC_OK;
}

const char *dcc_component_session_verify_status_string(
    dcc_component_session_verify_status_t status
) {
    switch (status) {
        case DCC_COMPONENT_SESSION_VERIFY_OK:
            return "ok";
        case DCC_COMPONENT_SESSION_VERIFY_BAD_FORMAT:
            return "bad_format";
        case DCC_COMPONENT_SESSION_VERIFY_BAD_SIGNATURE:
            return "bad_signature";
        case DCC_COMPONENT_SESSION_VERIFY_EXPIRED:
            return "expired";
        case DCC_COMPONENT_SESSION_VERIFY_WRONG_USER:
            return "wrong_user";
        case DCC_COMPONENT_SESSION_VERIFY_WRONG_CHANNEL:
            return "wrong_channel";
        case DCC_COMPONENT_SESSION_VERIFY_WRONG_GUILD:
            return "wrong_guild";
        case DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND:
            return "not_found";
        default:
            return "unknown";
    }
}
