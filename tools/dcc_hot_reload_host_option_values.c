#include "internal/dcc_hot_reload_host.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

int dcc_hot_reload_host_parse_u16(const char *text, uint16_t *out) {
    char *end = NULL;
    unsigned long value;
    if (text == NULL || text[0] == '\0') {
        return -1;
    }
    errno = 0;
    value = strtoul(text, &end, 10);
    if (text == end || errno != 0 || end == NULL || *end != '\0' || value > 65535UL) {
        return -1;
    }
    *out = (uint16_t)value;
    return 0;
}

int dcc_hot_reload_host_parse_u32(const char *text, uint32_t *out) {
    char *end = NULL;
    unsigned long value;
    if (text == NULL || text[0] == '\0') {
        return -1;
    }
    errno = 0;
    value = strtoul(text, &end, 10);
    if (text == end || errno != 0 || end == NULL || *end != '\0' || value > UINT32_MAX) {
        return -1;
    }
    *out = (uint32_t)value;
    return 0;
}

int dcc_hot_reload_host_parse_positive_u32(const char *text, uint32_t *out) {
    if (dcc_hot_reload_host_parse_u32(text, out) != 0 || *out == 0U) {
        return -1;
    }
    return 0;
}

int dcc_hot_reload_host_parse_settle_ms(const char *text, uint32_t *out) {
    if (text != NULL &&
        (dcc_hot_reload_host_token_matches(text, "none") ||
         dcc_hot_reload_host_token_matches(text, "no_settle") ||
         dcc_hot_reload_host_token_matches(text, "immediate"))) {
        *out = DCC_HOT_RELOAD_NO_SETTLE;
        return 0;
    }
    return dcc_hot_reload_host_parse_positive_u32(text, out);
}
