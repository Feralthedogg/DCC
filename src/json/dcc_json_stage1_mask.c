#include "internal/json/dcc_json_internal.h"

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define DCC_JSON_HAVE_NEON 1
#elif defined(__SSE2__)
#include <emmintrin.h>
#define DCC_JSON_HAVE_SSE2 1
#endif

unsigned dcc_json_stage1_ctz32(unsigned value) {
#if defined(__GNUC__) || defined(__clang__)
    return (unsigned)__builtin_ctz(value);
#else
    unsigned count = 0;
    while (((value >> count) & 1U) == 0U) {
        count++;
    }
    return count;
#endif
}

int dcc_json_stage1_is_structural(unsigned char c) {
    return c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',';
}

#if defined(DCC_JSON_HAVE_SSE2)
unsigned dcc_json_stage1_interesting_mask_16(const unsigned char *data) {
    const __m128i v = _mm_loadu_si128((const __m128i *)(const void *)data);
    __m128i m = _mm_cmpeq_epi8(v, _mm_set1_epi8('{'));
    m = _mm_or_si128(m, _mm_cmpeq_epi8(v, _mm_set1_epi8('}')));
    m = _mm_or_si128(m, _mm_cmpeq_epi8(v, _mm_set1_epi8('[')));
    m = _mm_or_si128(m, _mm_cmpeq_epi8(v, _mm_set1_epi8(']')));
    m = _mm_or_si128(m, _mm_cmpeq_epi8(v, _mm_set1_epi8(':')));
    m = _mm_or_si128(m, _mm_cmpeq_epi8(v, _mm_set1_epi8(',')));
    m = _mm_or_si128(m, _mm_cmpeq_epi8(v, _mm_set1_epi8('"')));
    m = _mm_or_si128(m, _mm_cmpeq_epi8(v, _mm_set1_epi8('\\')));
    return (unsigned)_mm_movemask_epi8(m);
}
#elif defined(DCC_JSON_HAVE_NEON)
unsigned dcc_json_stage1_interesting_mask_16(const unsigned char *data) {
    const uint8x16_t v = vld1q_u8(data);
    uint8x16_t m = vceqq_u8(v, vdupq_n_u8((uint8_t)'{'));
    m = vorrq_u8(m, vceqq_u8(v, vdupq_n_u8((uint8_t)'}')));
    m = vorrq_u8(m, vceqq_u8(v, vdupq_n_u8((uint8_t)'[')));
    m = vorrq_u8(m, vceqq_u8(v, vdupq_n_u8((uint8_t)']')));
    m = vorrq_u8(m, vceqq_u8(v, vdupq_n_u8((uint8_t)':')));
    m = vorrq_u8(m, vceqq_u8(v, vdupq_n_u8((uint8_t)',')));
    m = vorrq_u8(m, vceqq_u8(v, vdupq_n_u8((uint8_t)'"')));
    m = vorrq_u8(m, vceqq_u8(v, vdupq_n_u8((uint8_t)'\\')));

    unsigned char lanes[16];
    vst1q_u8(lanes, m);
    unsigned mask = 0;
    for (unsigned i = 0; i < 16U; ++i) {
        if (lanes[i] != 0) {
            mask |= 1U << i;
        }
    }
    return mask;
}
#else
unsigned dcc_json_stage1_interesting_mask_16(const unsigned char *data) {
    unsigned mask = 0;
    for (unsigned i = 0; i < 16U; ++i) {
        unsigned char c = data[i];
        if (dcc_json_stage1_is_structural(c) || c == '"' || c == '\\') {
            mask |= 1U << i;
        }
    }
    return mask;
}
#endif
