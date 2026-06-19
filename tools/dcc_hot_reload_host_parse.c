#include "internal/dcc_hot_reload_host.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static int dcc_hot_reload_host_ascii_equal(char lhs, char rhs) {
    return tolower((unsigned char)lhs) == tolower((unsigned char)rhs);
}

static int dcc_hot_reload_host_is_sep(char c) {
    return c == '_' || c == '-' || c == ' ';
}

int dcc_hot_reload_host_token_matches(const char *token, const char *name) {
    const char *prefix = "dcc_intent_";
    size_t prefix_len = strlen(prefix);
    if (strlen(token) > prefix_len) {
        size_t i;
        for (i = 0; i < prefix_len; ++i) {
            if (!dcc_hot_reload_host_ascii_equal(token[i], prefix[i])) {
                break;
            }
        }
        if (i == prefix_len) {
            token += prefix_len;
        }
    }

    while (*token != '\0' || *name != '\0') {
        while (dcc_hot_reload_host_is_sep(*token)) {
            ++token;
        }
        while (dcc_hot_reload_host_is_sep(*name)) {
            ++name;
        }
        if (*token == '\0' || *name == '\0') {
            return *token == '\0' && *name == '\0';
        }
        if (!dcc_hot_reload_host_ascii_equal(*token, *name)) {
            return 0;
        }
        ++token;
        ++name;
    }
    return 1;
}

int dcc_hot_reload_host_parse_snowflake(const char *text, dcc_snowflake_t *out) {
    char *end = NULL;
    unsigned long long value;
    if (text == NULL || text[0] == '\0') {
        return -1;
    }
    errno = 0;
    value = strtoull(text, &end, 10);
    if (text == end || errno != 0 || end == NULL || *end != '\0' || value == 0ULL) {
        return -1;
    }
    *out = (dcc_snowflake_t)value;
    return 0;
}
