#ifndef DCC_WS_IO_INTERNAL_H
#define DCC_WS_IO_INTERNAL_H

#include "internal/ws/dcc_ws_types_internal.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_ws_read_exact(dcc_ws_t *ws, void *buf, size_t len);
void dcc_ws_write_u16(unsigned char *out, uint16_t value);
void dcc_ws_write_u64(unsigned char *out, uint64_t value);
uint16_t dcc_ws_read_u16(const unsigned char *data);
uint64_t dcc_ws_read_u64(const unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif
