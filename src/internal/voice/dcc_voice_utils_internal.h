#ifndef DCC_VOICE_UTILS_INTERNAL_H
#define DCC_VOICE_UTILS_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_voice_copy_text(char *dst, size_t cap, const char *src);
uint16_t dcc_voice_read_be16(const uint8_t *data);
uint32_t dcc_voice_read_be32(const uint8_t *data);
void dcc_voice_write_be16(uint8_t *data, uint16_t value);
void dcc_voice_write_be32(uint8_t *data, uint32_t value);
uint64_t dcc_voice_now_ms(void);
void dcc_voice_sleep_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
