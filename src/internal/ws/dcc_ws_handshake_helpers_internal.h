#ifndef DCC_WS_HANDSHAKE_HELPERS_INTERNAL_H
#define DCC_WS_HANDSHAKE_HELPERS_INTERNAL_H

#include <stddef.h>

#define DCC_WS_MAX_HEADER_BYTES 16384U

typedef struct dcc_ws_buffer {
    unsigned char *data;
    size_t len;
    size_t cap;
} dcc_ws_buffer_t;

#ifdef __cplusplus
extern "C" {
#endif

int dcc_ws_buffer_append(dcc_ws_buffer_t *buf, const void *data, size_t len);
void dcc_ws_buffer_deinit(dcc_ws_buffer_t *buf);
int dcc_ws_accept_for_key(const char *key, char out[64]);
char *dcc_ws_header_value(char *headers, const char *name);
char *dcc_ws_find_header_end(unsigned char *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
