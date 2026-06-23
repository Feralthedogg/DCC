#ifndef DCC_SUGAR_FORMAT_BASE_H
#define DCC_SUGAR_FORMAT_BASE_H

#include <dcc/app.h>
#include <dcc/error.h>
#include <dcc/sugar/flags.h>
#include <dcc/snowflake.h>

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_FORMAT_MENTION_MAX 25U
#define DCC_FORMAT_EMOJI_MAX 64U
#define DCC_FORMAT_COMMAND_MENTION_MAX 128U
#define DCC_FORMAT_TIMESTAMP_MAX 32U

typedef struct dcc_sugar_custom_emoji {
    const char *name;
    size_t name_len;
    dcc_snowflake_t id;
    uint8_t animated;
} dcc_sugar_custom_emoji_t;

static inline dcc_status_t dcc_sugar_format_done(char *buffer, size_t buffer_size, int written) {
    if (buffer == NULL || buffer_size == 0U || written < 0 || (size_t)written >= buffer_size) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_format_alloc_v(
    char **out_text,
    const char *format,
    va_list args
) {
    if (out_text == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_text = NULL;

    va_list count_args;
    va_copy(count_args, args);
    int needed = vsnprintf(NULL, 0U, format, count_args);
    va_end(count_args);
    if (needed < 0) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t size = (size_t)needed + 1U;
    if (size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    char *text = (char *)malloc(size);
    if (text == NULL) {
        return DCC_ERR_NOMEM;
    }

    va_list write_args;
    va_copy(write_args, args);
    int written = vsnprintf(text, size, format, write_args);
    va_end(write_args);
    if (written < 0 || written != needed) {
        free(text);
        return DCC_ERR_INVALID_ARG;
    }

    *out_text = text;
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_format_alloc(
    char **out_text,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_format_alloc_v(out_text, format, args);
    va_end(args);
    return status;
}

static inline void dcc_sugar_format_free(char *text) {
    free(text);
}

#endif /* DCC_SUGAR_FORMAT_BASE_H */
