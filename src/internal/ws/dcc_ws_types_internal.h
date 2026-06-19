#ifndef DCC_WS_TYPES_INTERNAL_H
#define DCC_WS_TYPES_INTERNAL_H

#include <stddef.h>

typedef struct dcc_ws dcc_ws_t;

typedef enum dcc_ws_opcode {
    DCC_WS_CONTINUATION = 0x0,
    DCC_WS_TEXT = 0x1,
    DCC_WS_BINARY = 0x2,
    DCC_WS_CLOSE = 0x8,
    DCC_WS_PING = 0x9,
    DCC_WS_PONG = 0xA
} dcc_ws_opcode_t;

typedef struct dcc_ws_message {
    dcc_ws_opcode_t opcode;
    unsigned char *data;
    size_t len;
} dcc_ws_message_t;

#define DCC_WS_MAX_MESSAGE_BYTES (16U * 1024U * 1024U)

#endif
