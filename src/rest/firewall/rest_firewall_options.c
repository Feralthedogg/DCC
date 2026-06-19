#include <dcc/rest/firewall.h>

#include <stddef.h>
#include <string.h>

void dcc_rest_firewall_options_init(dcc_rest_firewall_options_t *options) {
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->invalid_request_soft_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_SOFT_LIMIT;
    options->invalid_request_hard_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT;
    options->invalid_request_window_ms = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_WINDOW_MS;
    options->soft_limit_delay_ms = DCC_REST_FIREWALL_DEFAULT_SOFT_LIMIT_DELAY_MS;
    options->on_hard_limit = DCC_REST_FIREWALL_HARD_LIMIT_REJECT_NONCRITICAL;
}

dcc_status_t dcc_rest_firewall_options_validate(const dcc_rest_firewall_options_t *options) {
    if (options == NULL ||
        options->size < offsetof(dcc_rest_firewall_options_t, on_hard_limit) + sizeof(options->on_hard_limit) ||
        options->invalid_request_window_ms == 0 ||
        options->invalid_request_hard_limit == 0 ||
        (options->invalid_request_soft_limit != 0 &&
            options->invalid_request_soft_limit > options->invalid_request_hard_limit)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options->on_hard_limit != DCC_REST_FIREWALL_HARD_LIMIT_REJECT_NONCRITICAL &&
        options->on_hard_limit != DCC_REST_FIREWALL_HARD_LIMIT_REJECT_ALL &&
        options->on_hard_limit != DCC_REST_FIREWALL_HARD_LIMIT_DELAY) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

uint8_t dcc_rest_firewall_status_is_invalid(uint16_t status) {
    return status == 401U || status == 403U || status == 429U;
}

static uint8_t dcc_firewall_path_has_prefix(const char *path, const char *prefix) {
    size_t prefix_len = strlen(prefix);
    return path != NULL && strncmp(path, prefix, prefix_len) == 0;
}

uint8_t dcc_rest_firewall_request_is_critical(const char *method, const char *path) {
    (void)method;
    return dcc_firewall_path_has_prefix(path, "/interactions/") ||
        dcc_firewall_path_has_prefix(path, "/webhooks/");
}

const char *dcc_rest_firewall_decision_string(dcc_rest_firewall_decision_t decision) {
    switch (decision) {
        case DCC_REST_FIREWALL_ALLOW:
            return "allow";
        case DCC_REST_FIREWALL_DELAY:
            return "delay";
        case DCC_REST_FIREWALL_REJECT:
            return "reject";
        default:
            return "unknown";
    }
}

const char *dcc_rest_firewall_reason_string(dcc_rest_firewall_reason_t reason) {
    switch (reason) {
        case DCC_REST_FIREWALL_REASON_NONE:
            return "none";
        case DCC_REST_FIREWALL_REASON_SOFT_INVALID_REQUEST_LIMIT:
            return "soft_invalid_request_limit";
        case DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT:
            return "hard_invalid_request_limit";
        default:
            return "unknown";
    }
}

const char *dcc_rest_firewall_hard_limit_action_string(dcc_rest_firewall_hard_limit_action_t action) {
    switch (action) {
        case DCC_REST_FIREWALL_HARD_LIMIT_REJECT_NONCRITICAL:
            return "reject_noncritical";
        case DCC_REST_FIREWALL_HARD_LIMIT_REJECT_ALL:
            return "reject_all";
        case DCC_REST_FIREWALL_HARD_LIMIT_DELAY:
            return "delay";
        default:
            return "unknown";
    }
}
