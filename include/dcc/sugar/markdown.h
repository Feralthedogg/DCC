#ifndef DCC_SUGAR_MARKDOWN_H
#define DCC_SUGAR_MARKDOWN_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DCC_MARKDOWN_SHORT_MAX 512U

static inline dcc_status_t dcc_sugar_markdown_done(
    char *buffer,
    size_t buffer_size,
    int written
) {
    if (buffer == NULL || buffer_size == 0U || written < 0 || (size_t)written >= buffer_size) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static inline uint8_t dcc_sugar_markdown_has_substring(
    const char *text,
    const char *needle
) {
    return text != NULL && needle != NULL && strstr(text, needle) != NULL ? 1U : 0U;
}

static inline dcc_status_t dcc_sugar_markdown_wrap(
    char *buffer,
    size_t buffer_size,
    const char *prefix,
    const char *text,
    const char *suffix
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        prefix == NULL ||
        text == NULL ||
        suffix == NULL) {
        return dcc_sugar_markdown_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_markdown_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "%s%s%s", prefix, text, suffix)
    );
}

static inline dcc_status_t dcc_sugar_markdown_bold(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "**", text, "**");
}

static inline dcc_status_t dcc_sugar_markdown_italic(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "*", text, "*");
}

static inline dcc_status_t dcc_sugar_markdown_underline(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "__", text, "__");
}

static inline dcc_status_t dcc_sugar_markdown_strikethrough(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "~~", text, "~~");
}

static inline dcc_status_t dcc_sugar_markdown_spoiler(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "||", text, "||");
}

static inline dcc_status_t dcc_sugar_markdown_inline_code(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    if (dcc_sugar_markdown_has_substring(text, "`")) {
        return dcc_sugar_markdown_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "`", text, "`");
}

static inline dcc_status_t dcc_sugar_markdown_quote(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "> ", text, "");
}

static inline dcc_status_t dcc_sugar_markdown_hide_link_embed(
    char *buffer,
    size_t buffer_size,
    const char *url
) {
    if (url == NULL ||
        dcc_sugar_markdown_has_substring(url, "\n") ||
        dcc_sugar_markdown_has_substring(url, "<") ||
        dcc_sugar_markdown_has_substring(url, ">")) {
        return dcc_sugar_markdown_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_markdown_wrap(buffer, buffer_size, "<", url, ">");
}

static inline uint8_t dcc_sugar_markdown_code_language_valid(const char *language) {
    if (language == NULL || language[0] == '\0') {
        return 1U;
    }
    for (size_t i = 0U; language[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)language[i];
        if (i >= 32U ||
            !((ch >= 'A' && ch <= 'Z') ||
              (ch >= 'a' && ch <= 'z') ||
              (ch >= '0' && ch <= '9') ||
              ch == '_' ||
              ch == '-' ||
              ch == '+' ||
              ch == '#' ||
              ch == '.')) {
            return 0U;
        }
    }
    return 1U;
}

static inline dcc_status_t dcc_sugar_markdown_code_block(
    char *buffer,
    size_t buffer_size,
    const char *language,
    const char *text
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        text == NULL ||
        !dcc_sugar_markdown_code_language_valid(language) ||
        dcc_sugar_markdown_has_substring(text, "```")) {
        return dcc_sugar_markdown_done(buffer, buffer_size, -1);
    }

    if (language == NULL || language[0] == '\0') {
        return dcc_sugar_markdown_done(
            buffer,
            buffer_size,
            snprintf(buffer, buffer_size, "```\n%s\n```", text)
        );
    }

    return dcc_sugar_markdown_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "```%s\n%s\n```", language, text)
    );
}

static inline const char *dcc_sugar_markdown_bold_short(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_bold(buffer, buffer_size, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_italic_short(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_italic(buffer, buffer_size, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_underline_short(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_underline(buffer, buffer_size, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_strikethrough_short(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_strikethrough(buffer, buffer_size, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_spoiler_short(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_spoiler(buffer, buffer_size, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_inline_code_short(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_inline_code(buffer, buffer_size, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_quote_short(
    char *buffer,
    size_t buffer_size,
    const char *text
) {
    return dcc_sugar_markdown_quote(buffer, buffer_size, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_code_block_short(
    char *buffer,
    size_t buffer_size,
    const char *language,
    const char *text
) {
    return dcc_sugar_markdown_code_block(buffer, buffer_size, language, text) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_markdown_hide_link_embed_short(
    char *buffer,
    size_t buffer_size,
    const char *url
) {
    return dcc_sugar_markdown_hide_link_embed(buffer, buffer_size, url) == DCC_OK ? buffer : "";
}

#define DCC_MD_BOLD(text_) \
    dcc_sugar_markdown_bold_short((char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, DCC_MARKDOWN_SHORT_MAX, (text_))

#define DCC_MD_ITALIC(text_) \
    dcc_sugar_markdown_italic_short((char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, DCC_MARKDOWN_SHORT_MAX, (text_))

#define DCC_MD_UNDERLINE(text_) \
    dcc_sugar_markdown_underline_short((char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, DCC_MARKDOWN_SHORT_MAX, (text_))

#define DCC_MD_STRIKETHROUGH(text_) \
    dcc_sugar_markdown_strikethrough_short( \
        (char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, \
        DCC_MARKDOWN_SHORT_MAX, \
        (text_) \
    )

#define DCC_MD_SPOILER(text_) \
    dcc_sugar_markdown_spoiler_short((char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, DCC_MARKDOWN_SHORT_MAX, (text_))

#define DCC_MD_CODE(text_) \
    dcc_sugar_markdown_inline_code_short((char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, DCC_MARKDOWN_SHORT_MAX, (text_))

#define DCC_MD_QUOTE(text_) \
    dcc_sugar_markdown_quote_short((char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, DCC_MARKDOWN_SHORT_MAX, (text_))

#define DCC_MD_CODE_BLOCK(language_, text_) \
    dcc_sugar_markdown_code_block_short( \
        (char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, \
        DCC_MARKDOWN_SHORT_MAX, \
        (language_), \
        (text_) \
    )

#define DCC_MD_HIDE_LINK_EMBED(url_) \
    dcc_sugar_markdown_hide_link_embed_short( \
        (char[DCC_MARKDOWN_SHORT_MAX]){ 0 }, \
        DCC_MARKDOWN_SHORT_MAX, \
        (url_) \
    )

#endif
