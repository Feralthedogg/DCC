#include "internal/hot_reload/dcc_hot_reload_worker_protocol_internal.h"

#include <errno.h>

int dcc_hot_reload_worker_send_header(int fd, dcc_hot_reload_worker_msg_t kind, uint32_t size) {
    dcc_hot_reload_worker_header_t header = {
        .magic = DCC_HOT_RELOAD_WORKER_MAGIC,
        .version = DCC_HOT_RELOAD_WORKER_VERSION,
        .kind = (uint16_t)kind,
        .size = size,
    };
    return dcc_hot_reload_worker_write_all(fd, &header, sizeof(header));
}

int dcc_hot_reload_worker_send_header_timeout(
    int fd,
    dcc_hot_reload_worker_msg_t kind,
    uint32_t size,
    uint32_t timeout_ms
) {
    dcc_hot_reload_worker_header_t header = {
        .magic = DCC_HOT_RELOAD_WORKER_MAGIC,
        .version = DCC_HOT_RELOAD_WORKER_VERSION,
        .kind = (uint16_t)kind,
        .size = size,
    };
    return dcc_hot_reload_worker_write_all_timeout(fd, &header, sizeof(header), timeout_ms);
}

int dcc_hot_reload_worker_read_header(
    int fd,
    dcc_hot_reload_worker_header_t *out,
    uint32_t timeout_ms
) {
    if (out == NULL) {
        return -1;
    }
    int rc = timeout_ms != 0U
        ? dcc_hot_reload_worker_read_all_timeout(fd, out, sizeof(*out), timeout_ms)
        : dcc_hot_reload_worker_read_all(fd, out, sizeof(*out));
    if (rc != 0) {
        return -1;
    }
    if (out->magic != DCC_HOT_RELOAD_WORKER_MAGIC ||
        out->version != DCC_HOT_RELOAD_WORKER_VERSION) {
        errno = EPROTO;
        return -1;
    }
    return 0;
}
