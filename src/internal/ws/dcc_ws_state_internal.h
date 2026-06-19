#ifndef DCC_WS_STATE_INTERNAL_H
#define DCC_WS_STATE_INTERNAL_H

#include "internal/net/dcc_conn.h"
#include "internal/ws/dcc_ws_types_internal.h"

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

struct dcc_ws {
    dcc_conn_t conn;
    char *host;
    char *path;
    unsigned char *rx_buf;
    size_t rx_len;
    size_t rx_pos;
    uint16_t close_code;
    char close_reason[124];
    atomic_uint ref_count;
    atomic_flag send_lock;
    atomic_int open;
};

#endif
