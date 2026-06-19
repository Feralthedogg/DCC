#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <stdio.h>
#include <windows.h>

void dcc_hot_reload_worker_close_fd_windows(int *fd) {
    if (fd != NULL && *fd >= 0) {
        _close(*fd);
        *fd = -1;
    }
}

static int dcc_worker_set_fd_inherit(int fd, int inherit) {
    intptr_t raw = _get_osfhandle(fd);
    if (raw == -1) {
        return -1;
    }
    return SetHandleInformation(
        (HANDLE)raw,
        HANDLE_FLAG_INHERIT,
        inherit ? HANDLE_FLAG_INHERIT : 0
    ) ? 0 : -1;
}

int dcc_hot_reload_worker_pipe_windows(int fds[2]) {
    return _pipe(fds, 4096U, _O_BINARY | _O_NOINHERIT);
}

static int dcc_worker_install_child_fd(int source_fd, int target_fd) {
    if (_dup2(source_fd, target_fd) != 0) {
        return -1;
    }
    return dcc_worker_set_fd_inherit(target_fd, 1);
}

static int dcc_worker_restore_fd(int target_fd, int saved_fd, int *installed_fd) {
    int rc = 0;
    if (saved_fd >= 0) {
        rc = _dup2(saved_fd, target_fd);
        _close(saved_fd);
    } else {
        _close(target_fd);
    }
    if (installed_fd != NULL && *installed_fd == target_fd) {
        *installed_fd = -1;
    }
    return rc == 0 ? 0 : -1;
}

intptr_t dcc_hot_reload_worker_spawn_child_windows(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    int *child_in_fd,
    int *child_out_fd
) {
    char generation_text[32];
    snprintf(generation_text, sizeof(generation_text), "%llu", (unsigned long long)generation);

    int saved_in = _dup(DCC_HOT_RELOAD_WORKER_FD_IN);
    int saved_out = _dup(DCC_HOT_RELOAD_WORKER_FD_OUT);
    if (child_in_fd == NULL || child_out_fd == NULL ||
        dcc_worker_install_child_fd(*child_in_fd, DCC_HOT_RELOAD_WORKER_FD_IN) != 0 ||
        dcc_worker_install_child_fd(*child_out_fd, DCC_HOT_RELOAD_WORKER_FD_OUT) != 0) {
        (void)dcc_worker_restore_fd(DCC_HOT_RELOAD_WORKER_FD_IN, saved_in, child_in_fd);
        (void)dcc_worker_restore_fd(DCC_HOT_RELOAD_WORKER_FD_OUT, saved_out, child_out_fd);
        return -1;
    }

    const char *argv[] = {
        hot_reload->worker_path,
        hot_reload->path,
        hot_reload->init_symbol,
        hot_reload->deinit_symbol,
        generation_text,
        NULL
    };
    intptr_t process = _spawnvp(_P_NOWAIT, hot_reload->worker_path, argv);
    (void)dcc_worker_restore_fd(DCC_HOT_RELOAD_WORKER_FD_IN, saved_in, child_in_fd);
    (void)dcc_worker_restore_fd(DCC_HOT_RELOAD_WORKER_FD_OUT, saved_out, child_out_fd);
    return process;
}
#endif
