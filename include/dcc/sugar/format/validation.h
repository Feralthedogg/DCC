#ifndef DCC_SUGAR_FORMAT_VALIDATION_H
#define DCC_SUGAR_FORMAT_VALIDATION_H

#include <dcc/sugar/format/base.h>

static inline uint8_t dcc_sugar_format_timestamp_style_valid(char style) {
    return style == '\0' ||
        style == 't' ||
        style == 'T' ||
        style == 'd' ||
        style == 'D' ||
        style == 'f' ||
        style == 'F' ||
        style == 'R'
            ? 1U
            : 0U;
}

static inline uint8_t dcc_sugar_format_emoji_name_valid_len(
    const char *name,
    size_t name_len
) {
    if (name == NULL || name_len < 2U || name_len > 32U) {
        return 0U;
    }

    for (size_t i = 0U; i < name_len; ++i) {
        unsigned char ch = (unsigned char)name[i];
        if (!((ch >= 'A' && ch <= 'Z') ||
              (ch >= 'a' && ch <= 'z') ||
              (ch >= '0' && ch <= '9') ||
              ch == '_')) {
            return 0U;
        }
    }
    return 1U;
}

static inline uint8_t dcc_sugar_format_emoji_name_valid(const char *name) {
    return name != NULL ? dcc_sugar_format_emoji_name_valid_len(name, strlen(name)) : 0U;
}

static inline uint8_t dcc_sugar_format_command_name_valid(const char *name) {
    if (name == NULL || name[0] == '\0') {
        return 0U;
    }

    size_t len = 0U;
    for (; name[len] != '\0'; ++len) {
        unsigned char ch = (unsigned char)name[len];
        if (len >= 32U || ch <= 0x20U || ch == ':' || ch == '<' || ch == '>') {
            return 0U;
        }
    }
    return len <= 32U ? 1U : 0U;
}

#endif /* DCC_SUGAR_FORMAT_VALIDATION_H */
