#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if !defined(_WIN32)
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

void dcc_hot_reload_worker_close_fd_posix(int *fd) {
    if (fd != NULL && *fd >= 0) {
        close(*fd);
        *fd = -1;
    }
}

int dcc_hot_reload_worker_socketpair_posix(int fds[2]) {
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0) {
        return -1;
    }
#if defined(SO_NOSIGPIPE)
    int one = 1;
    (void)setsockopt(fds[0], SOL_SOCKET, SO_NOSIGPIPE, &one, (socklen_t)sizeof(one));
    (void)setsockopt(fds[1], SOL_SOCKET, SO_NOSIGPIPE, &one, (socklen_t)sizeof(one));
#endif
    return 0;
}

void dcc_hot_reload_worker_child_exec_posix(
    dcc_hot_reload_t *hot_reload,
    const char *exec_path,
    int in_fd,
    int out_fd,
    uint64_t generation
) {
    char generation_text[32];
    snprintf(generation_text, sizeof(generation_text), "%llu", (unsigned long long)generation);
    if (dup2(in_fd, DCC_HOT_RELOAD_WORKER_FD_IN) < 0 ||
        dup2(out_fd, DCC_HOT_RELOAD_WORKER_FD_OUT) < 0) {
        _exit(126);
    }
    execl(
        exec_path,
        hot_reload->worker_path,
        hot_reload->path,
        hot_reload->init_symbol,
        hot_reload->deinit_symbol,
        generation_text,
        (char *)NULL
    );
    _exit(127);
}
#endif
