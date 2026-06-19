#include "internal/hot_reload/dcc_hot_reload_worker_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

typedef enum fake_worker_mode {
    FAKE_WORKER_GOOD = 0,
    FAKE_WORKER_INVALID_READY,
    FAKE_WORKER_MALFORMED_EVENT,
    FAKE_WORKER_HUNG_EVENT,
    FAKE_WORKER_MALFORMED_HEALTH,
    FAKE_WORKER_HUNG_HEALTH,
} fake_worker_mode_t;

static fake_worker_mode_t marker_mode(const char *path) {
    char buffer[64];
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return FAKE_WORKER_INVALID_READY;
    }
    size_t n = fread(buffer, 1, sizeof(buffer) - 1U, file);
    fclose(file);
    buffer[n] = '\0';
    if (strstr(buffer, "invalid-ready") != NULL || strstr(buffer, "invalid") != NULL) {
        return FAKE_WORKER_INVALID_READY;
    }
    if (strstr(buffer, "malformed-event") != NULL) {
        return FAKE_WORKER_MALFORMED_EVENT;
    }
    if (strstr(buffer, "hung-event") != NULL) {
        return FAKE_WORKER_HUNG_EVENT;
    }
    if (strstr(buffer, "malformed-health") != NULL) {
        return FAKE_WORKER_MALFORMED_HEALTH;
    }
    if (strstr(buffer, "hung-health") != NULL) {
        return FAKE_WORKER_HUNG_HEALTH;
    }
    return FAKE_WORKER_GOOD;
}

static int send_ready(uint64_t generation) {
    dcc_hot_reload_worker_ready_t ready = {
        .generation = generation,
        .status = DCC_OK,
        .handler_count = 1U,
    };
    int32_t handler_type = (int32_t)DCC_EVENT_SLASH_COMMAND;
    return dcc_hot_reload_worker_send_header(
               DCC_HOT_RELOAD_WORKER_FD_OUT,
               DCC_HOT_RELOAD_WORKER_MSG_READY,
               sizeof(ready)
           ) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &ready, sizeof(ready)) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &handler_type, sizeof(handler_type)) != 0
        ? 1
        : 0;
}

static int send_invalid_ready(void) {
    return dcc_hot_reload_worker_send_header(
        DCC_HOT_RELOAD_WORKER_FD_OUT,
        DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT,
        0U
    ) != 0
        ? 1
        : 0;
}

static int send_health(uint64_t generation) {
    dcc_hot_reload_worker_health_t health = {
        .generation = generation,
        .status = DCC_OK,
        .reserved = 0U,
    };
    return dcc_hot_reload_worker_send_header(
               DCC_HOT_RELOAD_WORKER_FD_OUT,
               DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT,
               sizeof(health)
           ) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &health, sizeof(health)) != 0
        ? 1
        : 0;
}

static int send_malformed_health(void) {
    return dcc_hot_reload_worker_send_header(
        DCC_HOT_RELOAD_WORKER_FD_OUT,
        DCC_HOT_RELOAD_WORKER_MSG_EVENT_RESULT,
        0U
    ) != 0
        ? 1
        : 0;
}

static void hang_worker(void) {
#if defined(_WIN32)
    Sleep(10000);
#else
    sleep(10);
#endif
}

static int discard_event_json(uint64_t json_len) {
    char buffer[512];
    while (json_len > 0U) {
        size_t chunk = json_len > sizeof(buffer) ? sizeof(buffer) : (size_t)json_len;
        if (dcc_hot_reload_worker_read_all(DCC_HOT_RELOAD_WORKER_FD_IN, buffer, chunk) != 0) {
            return 1;
        }
        json_len -= (uint64_t)chunk;
    }
    return 0;
}

static int send_malformed_event_result(void) {
    return dcc_hot_reload_worker_send_header(
        DCC_HOT_RELOAD_WORKER_FD_OUT,
        DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT,
        0U
    ) != 0
        ? 1
        : 0;
}

static int send_good_event_result(void) {
    const char method[] = "POST";
    const char path[] = "/fake-worker";
    const char content_type[] = "application/json";
    const char body[] = "{\"content\":\"fake worker slash\"}";
    dcc_hot_reload_worker_result_t result = {
        .status = (uint32_t)DCC_OK,
        .rest_count = 1U,
    };
    dcc_hot_reload_worker_rest_t rest = {
        .method_len = sizeof(method) - 1U,
        .path_len = sizeof(path) - 1U,
        .content_type_len = sizeof(content_type) - 1U,
        .body_len = sizeof(body) - 1U,
    };
    return dcc_hot_reload_worker_send_header(
               DCC_HOT_RELOAD_WORKER_FD_OUT,
               DCC_HOT_RELOAD_WORKER_MSG_EVENT_RESULT,
               sizeof(result)
           ) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &result, sizeof(result)) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &rest, sizeof(rest)) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, method, sizeof(method) - 1U) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, path, sizeof(path) - 1U) != 0 ||
           dcc_hot_reload_worker_write_all(
               DCC_HOT_RELOAD_WORKER_FD_OUT,
               content_type,
               sizeof(content_type) - 1U
           ) != 0 ||
           dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, body, sizeof(body) - 1U) != 0
        ? 1
        : 0;
}

static int handle_event(fake_worker_mode_t mode, const dcc_hot_reload_worker_header_t *header) {
    if (header == NULL || header->size != sizeof(dcc_hot_reload_worker_event_t)) {
        return 1;
    }
    dcc_hot_reload_worker_event_t event;
    if (dcc_hot_reload_worker_read_all(DCC_HOT_RELOAD_WORKER_FD_IN, &event, sizeof(event)) != 0 ||
        discard_event_json(event.json_len) != 0) {
        return 1;
    }
    if (mode == FAKE_WORKER_MALFORMED_EVENT) {
        return send_malformed_event_result();
    }
    if (mode == FAKE_WORKER_HUNG_EVENT) {
        hang_worker();
        return 0;
    }
    return send_good_event_result();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 2;
    }
    uint64_t generation = argc > 4 ? (uint64_t)strtoull(argv[4], NULL, 10) : 1U;
    fake_worker_mode_t mode = marker_mode(argv[1]);
    if (mode == FAKE_WORKER_INVALID_READY) {
        return send_invalid_ready();
    }
    if (send_ready(generation) != 0) {
        return 1;
    }
    for (;;) {
        dcc_hot_reload_worker_header_t header;
        if (dcc_hot_reload_worker_read_header(DCC_HOT_RELOAD_WORKER_FD_IN, &header, 0U) != 0) {
            return 1;
        }
        if (header.kind == DCC_HOT_RELOAD_WORKER_MSG_STOP) {
            return 0;
        }
        if (header.kind == DCC_HOT_RELOAD_WORKER_MSG_HEALTH && header.size == 0U) {
            if (mode == FAKE_WORKER_MALFORMED_HEALTH) {
                if (send_malformed_health() != 0) {
                    return 1;
                }
                continue;
            }
            if (mode == FAKE_WORKER_HUNG_HEALTH) {
                hang_worker();
                continue;
            }
            if (send_health(generation) != 0) {
                return 1;
            }
            continue;
        }
        if (header.kind == DCC_HOT_RELOAD_WORKER_MSG_EVENT) {
            if (handle_event(mode, &header) != 0) {
                return 1;
            }
            continue;
        }
        return 1;
    }
}
