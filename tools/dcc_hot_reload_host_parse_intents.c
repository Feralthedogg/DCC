#include "internal/dcc_hot_reload_host.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static int dcc_hot_reload_host_parse_numeric(const char *text, dcc_intents_t *out) {
    char *end = NULL;
    unsigned long long value;
    errno = 0;
    value = strtoull(text, &end, 0);
    if (text == end || errno != 0 || end == NULL || *end != '\0') {
        return -1;
    }
    *out = (dcc_intents_t)value;
    return 0;
}

static char *dcc_hot_reload_host_trim(char *text) {
    char *end;
    while (*text != '\0' && isspace((unsigned char)*text)) {
        ++text;
    }
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return text;
}

static int dcc_hot_reload_host_parse_intent_token(const char *token, dcc_intents_t *out) {
    dcc_intents_t value = 0;
    if (dcc_hot_reload_host_parse_numeric(token, &value) == 0) {
        *out = value;
        return 0;
    }
    if (dcc_hot_reload_host_token_matches(token, "all")) {
        *out = dcc_hot_reload_host_all_intents();
        return 0;
    }
    if (dcc_hot_reload_host_token_matches(token, "none")) {
        *out = 0;
        return 0;
    }
    return dcc_hot_reload_host_lookup_intent(token, out);
}

int dcc_hot_reload_host_parse_intents(const char *text, dcc_intents_t *out) {
    char *copy;
    char *cursor;
    dcc_intents_t intents = 0;
    int saw_token = 0;
    if (text == NULL || text[0] == '\0') {
        return -1;
    }
    if (dcc_hot_reload_host_parse_numeric(text, out) == 0) {
        return 0;
    }

    size_t text_len = strlen(text);
    copy = (char *)malloc(text_len + 1U);
    if (copy == NULL) {
        return -1;
    }
    memcpy(copy, text, text_len + 1U);

    cursor = copy;
    while (cursor != NULL) {
        char *next = strpbrk(cursor, ",+|");
        char *token;
        dcc_intents_t value = 0;
        if (next != NULL) {
            *next++ = '\0';
        }
        token = dcc_hot_reload_host_trim(cursor);
        if (token[0] == '\0' || dcc_hot_reload_host_parse_intent_token(token, &value) != 0) {
            free(copy);
            return -1;
        }
        intents |= value;
        saw_token = 1;
        cursor = next;
    }

    free(copy);
    if (!saw_token) {
        return -1;
    }
    *out = intents;
    return 0;
}
