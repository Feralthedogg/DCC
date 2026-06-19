#include "internal/hot_reload/dcc_hot_reload_worker_internal.h"

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>

static int test_pipe(int fds[2]) {
    return _pipe(fds, 4096, _O_BINARY);
}

static void test_close(int fd) {
    (void)_close(fd);
}
#else
#include <sys/socket.h>
#include <unistd.h>

static int test_pipe(int fds[2]) {
    return socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
}

static void test_close(int fd) {
    (void)close(fd);
}
#endif

static int test_write_result(
    int fd,
    dcc_status_t status,
    uint32_t rest_count
) {
    dcc_hot_reload_worker_result_t result = {
        .status = (uint32_t)status,
        .rest_count = rest_count,
    };
    return dcc_hot_reload_worker_send_header(
               fd,
               DCC_HOT_RELOAD_WORKER_MSG_EVENT_RESULT,
               sizeof(result)
           ) != 0 ||
           dcc_hot_reload_worker_write_all(fd, &result, sizeof(result)) != 0
        ? -1
        : 0;
}

static int test_read_result(
    int write_fd,
    int read_fd,
    dcc_hot_reload_worker_result_set_t *out
) {
    (void)write_fd;
    dcc_hot_reload_worker_process_t worker;
    memset(&worker, 0, sizeof(worker));
    worker.in_fd = -1;
    worker.out_fd = read_fd;
    worker.loaded = 1U;
    return dcc_hot_reload_worker_result_set_read(&worker, 50U, out);
}

static int test_healthcheck_frame(void) {
    int to_worker[2];
    int from_worker[2];
    if (test_pipe(to_worker) != 0 || test_pipe(from_worker) != 0) {
        perror("pipe");
        return -1;
    }
    dcc_hot_reload_worker_health_t health = {
        .generation = 7U,
        .status = DCC_OK,
        .reserved = 0U,
    };
    if (dcc_hot_reload_worker_send_header(
            from_worker[1],
            DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT,
            sizeof(health)
        ) != 0 ||
        dcc_hot_reload_worker_write_all(from_worker[1], &health, sizeof(health)) != 0) {
        test_close(to_worker[0]);
        test_close(to_worker[1]);
        test_close(from_worker[0]);
        test_close(from_worker[1]);
        return -1;
    }

    dcc_hot_reload_worker_process_t worker;
    memset(&worker, 0, sizeof(worker));
    worker.in_fd = to_worker[0];
    worker.out_fd = from_worker[0];
    worker.loaded = 1U;
    dcc_status_t status = dcc_hot_reload_worker_process_healthcheck(&worker, 50U);

    dcc_hot_reload_worker_header_t request;
    int request_status = dcc_hot_reload_worker_read_header(to_worker[1], &request, 50U);
    test_close(to_worker[0]);
    test_close(to_worker[1]);
    test_close(from_worker[0]);
    test_close(from_worker[1]);
    if (status != DCC_OK ||
        worker.generation != 7U ||
        request_status != 0 ||
        request.kind != DCC_HOT_RELOAD_WORKER_MSG_HEALTH ||
        request.size != 0U) {
        fprintf(
            stderr,
            "healthcheck frame failed: status=%s generation=%llu request=%d kind=%u size=%u\n",
            dcc_status_string(status),
            (unsigned long long)worker.generation,
            request_status,
            (unsigned)request.kind,
            (unsigned)request.size
        );
        return -1;
    }
    return 0;
}

static int test_malformed_healthcheck_frame(void) {
    int to_worker[2];
    int from_worker[2];
    if (test_pipe(to_worker) != 0 || test_pipe(from_worker) != 0) {
        perror("pipe");
        return -1;
    }
    if (dcc_hot_reload_worker_send_header(
            from_worker[1],
            DCC_HOT_RELOAD_WORKER_MSG_EVENT_RESULT,
            0U
        ) != 0) {
        test_close(to_worker[0]);
        test_close(to_worker[1]);
        test_close(from_worker[0]);
        test_close(from_worker[1]);
        return -1;
    }

    dcc_hot_reload_worker_process_t worker;
    memset(&worker, 0, sizeof(worker));
    worker.in_fd = to_worker[0];
    worker.out_fd = from_worker[0];
    worker.loaded = 1U;
    dcc_status_t status = dcc_hot_reload_worker_process_healthcheck(&worker, 50U);

    dcc_hot_reload_worker_header_t request;
    int request_status = dcc_hot_reload_worker_read_header(to_worker[1], &request, 50U);
    test_close(to_worker[0]);
    test_close(to_worker[1]);
    test_close(from_worker[0]);
    test_close(from_worker[1]);
    if (status != DCC_ERR_RUNTIME ||
        request_status != 0 ||
        request.kind != DCC_HOT_RELOAD_WORKER_MSG_HEALTH ||
        request.size != 0U) {
        fprintf(
            stderr,
            "malformed healthcheck frame failed: status=%s request=%d kind=%u size=%u\n",
            dcc_status_string(status),
            request_status,
            (unsigned)request.kind,
            (unsigned)request.size
        );
        return -1;
    }
    return 0;
}

static int test_healthcheck_timeout(void) {
    int to_worker[2];
    int from_worker[2];
    if (test_pipe(to_worker) != 0 || test_pipe(from_worker) != 0) {
        perror("pipe");
        return -1;
    }

    dcc_hot_reload_worker_process_t worker;
    memset(&worker, 0, sizeof(worker));
    worker.in_fd = to_worker[0];
    worker.out_fd = from_worker[0];
    worker.loaded = 1U;
    dcc_status_t status = dcc_hot_reload_worker_process_healthcheck(&worker, 10U);

    dcc_hot_reload_worker_header_t request;
    int request_status = dcc_hot_reload_worker_read_header(to_worker[1], &request, 50U);
    test_close(to_worker[0]);
    test_close(to_worker[1]);
    test_close(from_worker[0]);
    test_close(from_worker[1]);
    if (status != DCC_ERR_TIMEOUT ||
        request_status != 0 ||
        request.kind != DCC_HOT_RELOAD_WORKER_MSG_HEALTH ||
        request.size != 0U) {
        fprintf(
            stderr,
            "healthcheck timeout failed: status=%s request=%d kind=%u size=%u\n",
            dcc_status_string(status),
            request_status,
            (unsigned)request.kind,
            (unsigned)request.size
        );
        return -1;
    }
    return 0;
}

static int test_ready_error_frame(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }
    const char error[] = "hot reload module init failed: runtime error";
    dcc_hot_reload_worker_ready_t ready = {
        .generation = 11U,
        .status = (uint32_t)DCC_ERR_RUNTIME,
        .handler_count = 0U,
    };
    snprintf(ready.last_error, sizeof(ready.last_error), "%s", error);
    if (dcc_hot_reload_worker_send_header(
            fds[1],
            DCC_HOT_RELOAD_WORKER_MSG_READY,
            sizeof(ready)
        ) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], &ready, sizeof(ready)) != 0) {
        test_close(fds[0]);
        test_close(fds[1]);
        return -1;
    }

    dcc_hot_reload_worker_process_t worker;
    memset(&worker, 0, sizeof(worker));
    worker.in_fd = -1;
    worker.out_fd = fds[0];
    worker.loaded = 1U;
    dcc_event_type_t handlers[1];
    size_t handler_count = 1U;
    dcc_status_t status =
        dcc_hot_reload_worker_process_read_ready(&worker, handlers, &handler_count, 1U, 50U);
    test_close(fds[0]);
    test_close(fds[1]);

    if (status != DCC_ERR_RUNTIME ||
        worker.generation != 11U ||
        strcmp(worker.last_error, error) != 0 ||
        handler_count != 0U) {
        fprintf(
            stderr,
            "ready error frame failed: status=%s generation=%llu handlers=%zu err=%s\n",
            dcc_status_string(status),
            (unsigned long long)worker.generation,
            handler_count,
            worker.last_error
        );
        return -1;
    }
    return 0;
}

static int test_valid_rest_result(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }
    const char method[] = "POST";
    const char path[] = "/pong";
    const char content_type[] = "application/json";
    const char body[] = "{}";
    dcc_hot_reload_worker_rest_t rest = {
        .method_len = sizeof(method) - 1U,
        .path_len = sizeof(path) - 1U,
        .content_type_len = sizeof(content_type) - 1U,
        .body_len = sizeof(body) - 1U,
    };
    if (test_write_result(fds[1], DCC_OK, 1U) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], &rest, sizeof(rest)) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], method, sizeof(method) - 1U) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], path, sizeof(path) - 1U) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], content_type, sizeof(content_type) - 1U) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], body, sizeof(body) - 1U) != 0) {
        test_close(fds[0]);
        test_close(fds[1]);
        return -1;
    }

    dcc_hot_reload_worker_result_set_t result;
    memset(&result, 0, sizeof(result));
    dcc_status_t status = test_read_result(fds[1], fds[0], &result);
    test_close(fds[1]);
    test_close(fds[0]);
    int ok = status == DCC_OK &&
             result.status == DCC_OK &&
             result.count == 1U &&
             result.items != NULL &&
             strcmp(result.items[0].method, method) == 0 &&
             strcmp(result.items[0].path, path) == 0 &&
             strcmp(result.items[0].content_type, content_type) == 0 &&
             result.items[0].body_len == sizeof(body) - 1U &&
             memcmp(result.items[0].body, body, sizeof(body) - 1U) == 0;
    dcc_hot_reload_worker_result_set_deinit(&result);
    if (!ok) {
        fprintf(stderr, "valid worker result frame was not decoded correctly\n");
        return -1;
    }
    return 0;
}

static int test_reject_rest_count_over_cap(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }
    if (test_write_result(fds[1], DCC_OK, DCC_HOT_RELOAD_WORKER_MAX_REST_COUNT + 1U) != 0) {
        test_close(fds[0]);
        test_close(fds[1]);
        return -1;
    }
    dcc_hot_reload_worker_result_set_t result;
    memset(&result, 0, sizeof(result));
    dcc_status_t status = test_read_result(fds[1], fds[0], &result);
    test_close(fds[1]);
    test_close(fds[0]);
    dcc_hot_reload_worker_result_set_deinit(&result);
    if (status != DCC_ERR_RUNTIME) {
        fprintf(stderr, "oversized rest_count returned %s\n", dcc_status_string(status));
        return -1;
    }
    return 0;
}

static int test_reject_body_len_over_cap(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }
    dcc_hot_reload_worker_rest_t rest = {
        .method_len = 4U,
        .path_len = 5U,
        .content_type_len = 0U,
        .body_len = DCC_HOT_RELOAD_WORKER_MAX_BODY_LEN + 1U,
    };
    if (test_write_result(fds[1], DCC_OK, 1U) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], &rest, sizeof(rest)) != 0) {
        test_close(fds[0]);
        test_close(fds[1]);
        return -1;
    }
    dcc_hot_reload_worker_result_set_t result;
    memset(&result, 0, sizeof(result));
    dcc_status_t status = test_read_result(fds[1], fds[0], &result);
    test_close(fds[1]);
    test_close(fds[0]);
    dcc_hot_reload_worker_result_set_deinit(&result);
    if (status != DCC_ERR_RUNTIME) {
        fprintf(stderr, "oversized body_len returned %s\n", dcc_status_string(status));
        return -1;
    }
    return 0;
}

int main(void) {
    if (test_healthcheck_frame() != 0 ||
        test_malformed_healthcheck_frame() != 0 ||
        test_healthcheck_timeout() != 0 ||
        test_ready_error_frame() != 0 ||
        test_valid_rest_result() != 0 ||
        test_reject_rest_count_over_cap() != 0 ||
        test_reject_body_len_over_cap() != 0) {
        return 1;
    }
    return 0;
}
