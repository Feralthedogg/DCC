#include "internal/voice/dcc_voice_internal.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <errno.h>
#include <time.h>
#endif

#include <string.h>

dcc_status_t dcc_voice_copy_text(char *dst, size_t cap, const char *src) {
    if (dst == NULL || cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (src == NULL) {
        dst[0] = '\0';
        return DCC_OK;
    }

    size_t len = strlen(src);
    if (len >= cap) {
        return DCC_ERR_INVALID_ARG;
    }
    memmove(dst, src, len + 1U);
    return DCC_OK;
}


uint16_t dcc_voice_read_be16(const uint8_t *data) {
    return (uint16_t)(((uint16_t)data[0] << 8U) | (uint16_t)data[1]);
}

uint32_t dcc_voice_read_be32(const uint8_t *data) {
    return ((uint32_t)data[0] << 24U) |
        ((uint32_t)data[1] << 16U) |
        ((uint32_t)data[2] << 8U) |
        (uint32_t)data[3];
}

void dcc_voice_write_be16(uint8_t *data, uint16_t value) {
    data[0] = (uint8_t)(value >> 8U);
    data[1] = (uint8_t)value;
}

void dcc_voice_write_be32(uint8_t *data, uint32_t value) {
    data[0] = (uint8_t)(value >> 24U);
    data[1] = (uint8_t)(value >> 16U);
    data[2] = (uint8_t)(value >> 8U);
    data[3] = (uint8_t)value;
}

uint64_t dcc_voice_now_ms(void) {
    return llam_now_ns() / UINT64_C(1000000);
}

void dcc_voice_sleep_ms(uint32_t ms) {
    if (ms == 0) {
        return;
    }

    uint64_t ns = (uint64_t)ms * UINT64_C(1000000);
    if (llam_sleep_ns(ns) == 0) {
        return;
    }

#if defined(_WIN32)
    Sleep((DWORD)ms);
#else
    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000U);
    req.tv_nsec = (long)((ms % 1000U) * 1000000U);
    while (nanosleep(&req, &req) != 0 && errno == EINTR) {
    }
#endif
}
