#ifndef DCC_SUGAR_COLORS_H
#define DCC_SUGAR_COLORS_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define DCC_COLOR_BLURPLE 0x5865F2U
#define DCC_COLOR_SUCCESS 0x57F287U
#define DCC_COLOR_WARNING 0xFEE75CU
#define DCC_COLOR_ERROR 0xED4245U
#define DCC_COLOR_INFO DCC_COLOR_BLURPLE
#define DCC_COLOR_NEUTRAL 0x2B2D31U
#define DCC_COLOR_WHITE 0xFFFFFFU
#define DCC_COLOR_BLACK 0x000000U

#define DCC_COLOR_HEX(color_) ((uint32_t)((color_) & 0xFFFFFFU))
#define DCC_COLOR_RGB(red_, green_, blue_) \
    ((uint32_t)((((uint32_t)(red_)) & 0xFFU) << 16U) | \
     (uint32_t)((((uint32_t)(green_)) & 0xFFU) << 8U) | \
     (uint32_t)(((uint32_t)(blue_)) & 0xFFU))
#define DCC_COLOR_GRAY(value_) DCC_COLOR_RGB((value_), (value_), (value_))
#define DCC_COLOR_GREY(value_) DCC_COLOR_GRAY(value_)
#define DCC_COLOR_IS_VALID(color_) (((uint32_t)(color_)) <= 0xFFFFFFU)

static inline int dcc_sugar_color_hex_digit(unsigned char ch) {
    if (ch >= '0' && ch <= '9') {
        return (int)(ch - '0');
    }
    if (ch >= 'A' && ch <= 'F') {
        return 10 + (int)(ch - 'A');
    }
    if (ch >= 'a' && ch <= 'f') {
        return 10 + (int)(ch - 'a');
    }
    return -1;
}

static inline dcc_status_t dcc_sugar_color_rgb_checked(
    uint32_t red,
    uint32_t green,
    uint32_t blue,
    uint32_t *out_color
) {
    if (out_color == NULL || red > 255U || green > 255U || blue > 255U) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_color = DCC_COLOR_RGB(red, green, blue);
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_color_from_hex(
    const char *text,
    uint32_t *out_color
) {
    if (text == NULL || out_color == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (text[0] == '#') {
        ++text;
    } else if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
        text += 2;
    }

    uint32_t color = 0U;
    for (size_t i = 0U; i < 6U; ++i) {
        int digit = dcc_sugar_color_hex_digit((unsigned char)text[i]);
        if (digit < 0) {
            return DCC_ERR_INVALID_ARG;
        }
        color = (color << 4U) | (uint32_t)digit;
    }
    if (text[6] != '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    *out_color = color;
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_color_to_hex(
    char *buffer,
    size_t buffer_size,
    uint32_t color
) {
    if (buffer == NULL || buffer_size < 8U || color > 0xFFFFFFU) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return DCC_ERR_INVALID_ARG;
    }

    int written = snprintf(
        buffer,
        buffer_size,
        "#%02X%02X%02X",
        (unsigned int)((color >> 16U) & 0xFFU),
        (unsigned int)((color >> 8U) & 0xFFU),
        (unsigned int)(color & 0xFFU)
    );
    if (written != 7) {
        buffer[0] = '\0';
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

#define DCC_COLOR_RGB_CHECKED(red_, green_, blue_, out_) \
    dcc_sugar_color_rgb_checked((red_), (green_), (blue_), (out_))
#define DCC_COLOR_FROM_HEX(text_, out_) dcc_sugar_color_from_hex((text_), (out_))
#define DCC_COLOR_TO_HEX(buffer_, buffer_size_, color_) \
    dcc_sugar_color_to_hex((buffer_), (buffer_size_), (color_))

#endif
