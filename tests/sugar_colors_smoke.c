#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static int check_color_macros(void) {
    if (DCC_COLOR_RGB(88U, 101U, 242U) != DCC_COLOR_BLURPLE) {
        return 1;
    }
    if (DCC_COLOR_GRAY(0x33U) != 0x333333U || DCC_COLOR_GREY(0x44U) != 0x444444U) {
        return 1;
    }
    if (DCC_COLOR_HEX(0x1FFAA00U) != 0xFFAA00U) {
        return 1;
    }
    if (!DCC_COLOR_IS_VALID(0xFFFFFFU) || DCC_COLOR_IS_VALID(0x1000000U)) {
        return 1;
    }
    return 0;
}

static int check_checked_rgb(void) {
    uint32_t color = 0U;
    if (DCC_COLOR_RGB_CHECKED(0x12U, 0x34U, 0x56U, &color) != DCC_OK ||
        color != 0x123456U) {
        return 1;
    }
    if (DCC_COLOR_RGB_CHECKED(256U, 0U, 0U, &color) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    if (DCC_COLOR_RGB_CHECKED(0U, 0U, 0U, NULL) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    return 0;
}

static int check_hex_parse_and_format(void) {
    uint32_t color = 0U;
    char hex[8];

    if (DCC_COLOR_FROM_HEX("#5865F2", &color) != DCC_OK || color != DCC_COLOR_BLURPLE) {
        return 1;
    }
    if (DCC_COLOR_FROM_HEX("57f287", &color) != DCC_OK || color != DCC_COLOR_SUCCESS) {
        return 1;
    }
    if (DCC_COLOR_FROM_HEX("0x2B2D31", &color) != DCC_OK || color != DCC_COLOR_NEUTRAL) {
        return 1;
    }
    if (DCC_COLOR_TO_HEX(hex, sizeof(hex), DCC_COLOR_WARNING) != DCC_OK ||
        strcmp(hex, "#FEE75C") != 0) {
        return 1;
    }
    if (DCC_COLOR_FROM_HEX("#12345", &color) != DCC_ERR_INVALID_ARG ||
        DCC_COLOR_FROM_HEX("#1234567", &color) != DCC_ERR_INVALID_ARG ||
        DCC_COLOR_FROM_HEX("#12345Z", &color) != DCC_ERR_INVALID_ARG ||
        DCC_COLOR_FROM_HEX(NULL, &color) != DCC_ERR_INVALID_ARG ||
        DCC_COLOR_FROM_HEX("#123456", NULL) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    if (DCC_COLOR_TO_HEX(hex, 7U, DCC_COLOR_SUCCESS) != DCC_ERR_INVALID_ARG ||
        DCC_COLOR_TO_HEX(hex, sizeof(hex), 0x1000000U) != DCC_ERR_INVALID_ARG ||
        DCC_COLOR_TO_HEX(NULL, 0U, DCC_COLOR_SUCCESS) != DCC_ERR_INVALID_ARG) {
        return 1;
    }
    return 0;
}

static int check_embed_serialization(void) {
    dcc_message_builder_t message =
        DCC_MESSAGE_EMBED(DCC_EMBED_COLOR("Theme", "RGB helper", DCC_COLOR_RGB(18U, 52U, 86U)));

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize color embed\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"title\":\"Theme\"") ||
        require_contains(json, "\"description\":\"RGB helper\"") ||
        require_contains(json, "\"color\":1193046");
    dcc_message_builder_json_free(json);
    return failed;
}

int main(void) {
    if (check_color_macros() != 0) {
        return 1;
    }
    if (check_checked_rgb() != 0) {
        return 1;
    }
    if (check_hex_parse_and_format() != 0) {
        return 1;
    }
    if (check_embed_serialization() != 0) {
        return 1;
    }
    return 0;
}
