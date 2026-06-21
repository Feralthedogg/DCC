#ifndef DCC_HOT_RELOAD_WORKER_PROTOCOL_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_PROTOCOL_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#define DCC_HOT_RELOAD_WORKER_FD_IN 3
#define DCC_HOT_RELOAD_WORKER_FD_OUT 4
#define DCC_HOT_RELOAD_WORKER_MAGIC UINT32_C(0x44434357)
#define DCC_HOT_RELOAD_WORKER_VERSION UINT16_C(1)
#define DCC_HOT_RELOAD_WORKER_MAX_REST_COUNT UINT32_C(128)
#define DCC_HOT_RELOAD_WORKER_MAX_METHOD_LEN UINT32_C(32)
#define DCC_HOT_RELOAD_WORKER_MAX_PATH_LEN UINT32_C(8192)
#define DCC_HOT_RELOAD_WORKER_MAX_CONTENT_TYPE_LEN UINT32_C(128)
#define DCC_HOT_RELOAD_WORKER_MAX_BODY_LEN UINT64_C(16777216)
#define DCC_HOT_RELOAD_WORKER_MAX_EVENT_JSON_LEN UINT64_C(16777216)
#define DCC_HOT_RELOAD_WORKER_MAX_ERROR_LEN 256U
#define DCC_HOT_RELOAD_WORKER_CHILD_READ_TIMEOUT_MS UINT32_C(5000)
#define DCC_HOT_RELOAD_WORKER_CHILD_WRITE_TIMEOUT_MS UINT32_C(5000)

typedef enum dcc_hot_reload_worker_msg {
    DCC_HOT_RELOAD_WORKER_MSG_READY = 1,
    DCC_HOT_RELOAD_WORKER_MSG_EVENT = 2,
    DCC_HOT_RELOAD_WORKER_MSG_EVENT_RESULT = 3,
    DCC_HOT_RELOAD_WORKER_MSG_STOP = 4,
    DCC_HOT_RELOAD_WORKER_MSG_HEALTH = 5,
    DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT = 6
} dcc_hot_reload_worker_msg_t;

typedef struct dcc_hot_reload_worker_header {
    uint32_t magic;
    uint16_t version;
    uint16_t kind;
    uint32_t size;
} dcc_hot_reload_worker_header_t;

typedef struct dcc_hot_reload_worker_ready {
    uint64_t generation;
    uint32_t status;
    uint32_t handler_count;
    char last_error[DCC_HOT_RELOAD_WORKER_MAX_ERROR_LEN];
} dcc_hot_reload_worker_ready_t;

typedef struct dcc_hot_reload_worker_event {
    int32_t type;
    uint32_t shard_id;
    uint64_t json_len;
} dcc_hot_reload_worker_event_t;

typedef struct dcc_hot_reload_worker_result {
    uint32_t status;
    uint32_t rest_count;
} dcc_hot_reload_worker_result_t;

typedef struct dcc_hot_reload_worker_health {
    uint64_t generation;
    uint32_t status;
    uint32_t reserved;
} dcc_hot_reload_worker_health_t;

typedef struct dcc_hot_reload_worker_rest {
    uint32_t method_len;
    uint32_t path_len;
    uint32_t content_type_len;
    uint64_t body_len;
} dcc_hot_reload_worker_rest_t;

int dcc_hot_reload_worker_write_all(int fd, const void *data, size_t len);
int dcc_hot_reload_worker_write_all_timeout(int fd, const void *data, size_t len, uint32_t timeout_ms);
int dcc_hot_reload_worker_read_all(int fd, void *data, size_t len);
int dcc_hot_reload_worker_read_all_timeout(int fd, void *data, size_t len, uint32_t timeout_ms);
int dcc_hot_reload_worker_send_header(int fd, dcc_hot_reload_worker_msg_t kind, uint32_t size);
int dcc_hot_reload_worker_send_header_timeout(
    int fd,
    dcc_hot_reload_worker_msg_t kind,
    uint32_t size,
    uint32_t timeout_ms
);
int dcc_hot_reload_worker_read_header(
    int fd,
    dcc_hot_reload_worker_header_t *out,
    uint32_t timeout_ms
);

#endif
