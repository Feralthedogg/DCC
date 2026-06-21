#include "internal/hot_reload/dcc_hot_reload_worker_internal.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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

static int test_write_result(int fd, dcc_status_t status, uint32_t rest_count);
static int test_read_result(
    int write_fd,
    int read_fd,
    dcc_hot_reload_worker_result_set_t *out
);

#if !defined(_WIN32)
static int test_write_timeout(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }

    size_t len = 1024U * 1024U;
    char *payload = (char *)malloc(len);
    if (payload == NULL) {
        test_close(fds[0]);
        test_close(fds[1]);
        return -1;
    }
    memset(payload, 'x', len);

    errno = 0;
    int status = dcc_hot_reload_worker_write_all_timeout(fds[0], payload, len, 10U);
    int saved_errno = errno;
    free(payload);
    test_close(fds[0]);
    test_close(fds[1]);

    if (status == 0 || saved_errno != ETIMEDOUT) {
        fprintf(stderr, "write timeout failed: status=%d errno=%d\n", status, saved_errno);
        return -1;
    }
    return 0;
}

static int test_read_complete_frame_after_writer_close(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }
    if (test_write_result(fds[1], DCC_OK, 0U) != 0) {
        test_close(fds[0]);
        test_close(fds[1]);
        return -1;
    }
    test_close(fds[1]);

    dcc_hot_reload_worker_result_set_t result;
    memset(&result, 0, sizeof(result));
    dcc_status_t status = test_read_result(-1, fds[0], &result);
    test_close(fds[0]);
    dcc_hot_reload_worker_result_set_deinit(&result);
    if (status != DCC_OK || result.status != DCC_OK || result.count != 0U) {
        fprintf(
            stderr,
            "complete frame after writer close failed: status=%s count=%zu\n",
            dcc_status_string(status),
            result.count
        );
        return -1;
    }
    return 0;
}

static int test_child_loop_rejects_invalid_event_type(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }
    int saved_in = dup(DCC_HOT_RELOAD_WORKER_FD_IN);
    if (dup2(fds[0], DCC_HOT_RELOAD_WORKER_FD_IN) < 0) {
        perror("dup2");
        if (saved_in >= 0) {
            test_close(saved_in);
        }
        test_close(fds[0]);
        test_close(fds[1]);
        return -1;
    }
    test_close(fds[0]);

    dcc_hot_reload_worker_event_t event = {
        .type = (int32_t)DCC_EVENT_MAX,
        .shard_id = 0U,
        .json_len = 0U,
    };
    int write_status =
        dcc_hot_reload_worker_send_header(
            fds[1],
            DCC_HOT_RELOAD_WORKER_MSG_EVENT,
            sizeof(event)
        ) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], &event, sizeof(event)) != 0
            ? -1
            : 0;
    int loop_status = write_status == 0
        ? dcc_hot_reload_worker_child_event_loop(NULL)
        : 1;
    test_close(fds[1]);
    test_close(DCC_HOT_RELOAD_WORKER_FD_IN);
    if (saved_in >= 0) {
        (void)dup2(saved_in, DCC_HOT_RELOAD_WORKER_FD_IN);
        test_close(saved_in);
    }

    if (write_status != 0 || loop_status != 1) {
        fprintf(stderr, "child loop invalid event type failed: write=%d loop=%d\n", write_status, loop_status);
        return -1;
    }
    return 0;
}
#endif

static int test_worker_main_rejects_invalid_generation(void) {
    char *argv_bad_text[] = { "dcc_hot_reload_worker", "module.so", "init", "deinit", "abc", NULL };
    char *argv_bad_suffix[] = { "dcc_hot_reload_worker", "module.so", "init", "deinit", "12x", NULL };
    char *argv_zero[] = { "dcc_hot_reload_worker", "module.so", "init", "deinit", "0", NULL };
    if (dcc_hot_reload_worker_main(5, argv_bad_text) != 2 ||
        dcc_hot_reload_worker_main(5, argv_bad_suffix) != 2 ||
        dcc_hot_reload_worker_main(5, argv_zero) != 2) {
        fprintf(stderr, "worker main accepted invalid generation argument\n");
        return -1;
    }
    return 0;
}

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
    worker.in_fd = to_worker[1];
    worker.out_fd = from_worker[0];
    worker.loaded = 1U;
    dcc_status_t status = dcc_hot_reload_worker_process_healthcheck(&worker, 50U);

    dcc_hot_reload_worker_header_t request;
    int request_status = dcc_hot_reload_worker_read_header(to_worker[0], &request, 50U);
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
    worker.in_fd = to_worker[1];
    worker.out_fd = from_worker[0];
    worker.loaded = 1U;
    dcc_status_t status = dcc_hot_reload_worker_process_healthcheck(&worker, 50U);

    dcc_hot_reload_worker_header_t request;
    int request_status = dcc_hot_reload_worker_read_header(to_worker[0], &request, 50U);
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
    worker.in_fd = to_worker[1];
    worker.out_fd = from_worker[0];
    worker.loaded = 1U;
    dcc_status_t status = dcc_hot_reload_worker_process_healthcheck(&worker, 10U);

    dcc_hot_reload_worker_header_t request;
    int request_status = dcc_hot_reload_worker_read_header(to_worker[0], &request, 50U);
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

static int test_healthcheck_body_timeout(void) {
    int to_worker[2];
    int from_worker[2];
    if (test_pipe(to_worker) != 0 || test_pipe(from_worker) != 0) {
        perror("pipe");
        return -1;
    }
    if (dcc_hot_reload_worker_send_header(
            from_worker[1],
            DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT,
            sizeof(dcc_hot_reload_worker_health_t)
        ) != 0) {
        test_close(to_worker[0]);
        test_close(to_worker[1]);
        test_close(from_worker[0]);
        test_close(from_worker[1]);
        return -1;
    }

    dcc_hot_reload_worker_process_t worker;
    memset(&worker, 0, sizeof(worker));
    worker.in_fd = to_worker[1];
    worker.out_fd = from_worker[0];
    worker.loaded = 1U;
    dcc_status_t status = dcc_hot_reload_worker_process_healthcheck(&worker, 10U);

    test_close(to_worker[0]);
    test_close(to_worker[1]);
    test_close(from_worker[0]);
    test_close(from_worker[1]);
    if (status != DCC_ERR_TIMEOUT) {
        fprintf(stderr, "healthcheck body timeout returned %s\n", dcc_status_string(status));
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

static int test_rest_body_timeout(void) {
    int fds[2];
    if (test_pipe(fds) != 0) {
        perror("pipe");
        return -1;
    }
    const char method[] = "POST";
    const char path[] = "/pong";
    dcc_hot_reload_worker_rest_t rest = {
        .method_len = sizeof(method) - 1U,
        .path_len = sizeof(path) - 1U,
        .content_type_len = 0U,
        .body_len = 2U,
    };
    if (test_write_result(fds[1], DCC_OK, 1U) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], &rest, sizeof(rest)) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], method, sizeof(method) - 1U) != 0 ||
        dcc_hot_reload_worker_write_all(fds[1], path, sizeof(path) - 1U) != 0) {
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
    if (status != DCC_ERR_TIMEOUT) {
        fprintf(stderr, "rest body timeout returned %s\n", dcc_status_string(status));
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
        test_healthcheck_body_timeout() != 0 ||
        test_ready_error_frame() != 0 ||
        test_worker_main_rejects_invalid_generation() != 0 ||
        test_rest_body_timeout() != 0 ||
        test_valid_rest_result() != 0 ||
        test_reject_rest_count_over_cap() != 0 ||
        test_reject_body_len_over_cap() != 0) {
        return 1;
    }
#if !defined(_WIN32)
    if (test_write_timeout() != 0 ||
        test_read_complete_frame_after_writer_close() != 0 ||
        test_child_loop_rejects_invalid_event_type() != 0) {
        return 1;
    }
#endif
    return 0;
}
