#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int check_text(const char *actual, const char *expected) {
    if (actual == NULL || strcmp(actual, expected) != 0) {
        fprintf(stderr, "expected '%s', got '%s'\n", expected, actual != NULL ? actual : "(null)");
        return 1;
    }
    return 0;
}

static int check_checked_functions(void) {
    char buffer[DCC_MARKDOWN_SHORT_MAX];

    if (dcc_sugar_markdown_bold(buffer, sizeof(buffer), "Saved") != DCC_OK ||
        check_text(buffer, "**Saved**") != 0 ||
        dcc_sugar_markdown_italic(buffer, sizeof(buffer), "hint") != DCC_OK ||
        check_text(buffer, "*hint*") != 0 ||
        dcc_sugar_markdown_underline(buffer, sizeof(buffer), "title") != DCC_OK ||
        check_text(buffer, "__title__") != 0 ||
        dcc_sugar_markdown_strikethrough(buffer, sizeof(buffer), "old") != DCC_OK ||
        check_text(buffer, "~~old~~") != 0 ||
        dcc_sugar_markdown_spoiler(buffer, sizeof(buffer), "secret") != DCC_OK ||
        check_text(buffer, "||secret||") != 0 ||
        dcc_sugar_markdown_inline_code(buffer, sizeof(buffer), "id=1") != DCC_OK ||
        check_text(buffer, "`id=1`") != 0 ||
        dcc_sugar_markdown_quote(buffer, sizeof(buffer), "quoted") != DCC_OK ||
        check_text(buffer, "> quoted") != 0 ||
        dcc_sugar_markdown_hide_link_embed(buffer, sizeof(buffer), "https://example.com") != DCC_OK ||
        check_text(buffer, "<https://example.com>") != 0 ||
        dcc_sugar_markdown_code_block(buffer, sizeof(buffer), "c", "return 0;") != DCC_OK ||
        check_text(buffer, "```c\nreturn 0;\n```") != 0 ||
        dcc_sugar_markdown_code_block(buffer, sizeof(buffer), NULL, "plain") != DCC_OK ||
        check_text(buffer, "```\nplain\n```") != 0) {
        return 1;
    }

    return 0;
}

static int check_short_macros(void) {
    char line[512];
    int written = snprintf(
        line,
        sizeof(line),
        "%s %s %s %s %s",
        DCC_MD_BOLD("Ready"),
        DCC_MD_CODE("ok"),
        DCC_MD_SPOILER("hidden"),
        DCC_MD_HIDE_LINK_EMBED("https://example.com"),
        DCC_MD_CODE_BLOCK("json", "{\"ok\":true}")
    );

    if (written < 0 || (size_t)written >= sizeof(line)) {
        return 1;
    }

    return check_text(
        line,
        "**Ready** `ok` ||hidden|| <https://example.com> ```json\n{\"ok\":true}\n```"
    );
}

static int check_invalid_inputs(void) {
    char tiny[6];
    char buffer[DCC_MARKDOWN_SHORT_MAX];

    return
        dcc_sugar_markdown_bold(NULL, 0U, "text") == DCC_ERR_INVALID_ARG &&
        dcc_sugar_markdown_bold(tiny, sizeof(tiny), "long text") == DCC_ERR_INVALID_ARG &&
        dcc_sugar_markdown_bold(buffer, sizeof(buffer), NULL) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_markdown_inline_code(buffer, sizeof(buffer), "bad`code") == DCC_ERR_INVALID_ARG &&
        dcc_sugar_markdown_code_block(buffer, sizeof(buffer), "bad lang", "text") ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_markdown_code_block(buffer, sizeof(buffer), "c", "bad```fence") ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_markdown_hide_link_embed(buffer, sizeof(buffer), "https://example.com/a\nb") ==
            DCC_ERR_INVALID_ARG &&
        strcmp(DCC_MD_CODE("bad`code"), "") == 0
            ? 0
            : 1;
}

int main(void) {
    if (check_checked_functions() != 0) {
        return 1;
    }
    if (check_short_macros() != 0) {
        return 1;
    }
    if (check_invalid_inputs() != 0) {
        return 1;
    }
    return 0;
}
