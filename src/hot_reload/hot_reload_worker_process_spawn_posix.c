#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if !defined(_WIN32)
#include <errno.h>
#include <spawn.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

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

static int dcc_hot_reload_worker_env_allowed(const char *entry) {
    if (entry == NULL) {
        return 0;
    }
    const char *equals = strchr(entry, '=');
    size_t name_len = equals != NULL ? (size_t)(equals - entry) : strlen(entry);
    static const char *const exact_names[] = {
        "PATH", "HOME", "TMPDIR", "TMP", "TEMP", "LANG", "LC_ALL",
        "LC_CTYPE", "LC_MESSAGES", "TZ", "DCC_DISCORD_API_BASE",
        "DCC_ISOLATED_TEST_URL"
    };
    for (size_t i = 0U; i < sizeof(exact_names) / sizeof(exact_names[0]); ++i) {
        if (strlen(exact_names[i]) == name_len &&
            strncmp(entry, exact_names[i], name_len) == 0) {
            return 1;
        }
    }
    return 0;
}

static char **dcc_hot_reload_worker_sanitized_environment(void) {
    size_t count = 0U;
    if (environ != NULL) {
        for (char **entry = environ; *entry != NULL; ++entry) {
            if (dcc_hot_reload_worker_env_allowed(*entry)) {
                ++count;
            }
        }
    }
    if (count > (SIZE_MAX / sizeof(char *)) - 1U) {
        return NULL;
    }
    char **environment = (char **)calloc(count + 1U, sizeof(*environment));
    if (environment == NULL) {
        return NULL;
    }
    size_t index = 0U;
    if (environ != NULL) {
        for (char **entry = environ; *entry != NULL; ++entry) {
            if (dcc_hot_reload_worker_env_allowed(*entry)) {
                environment[index++] = *entry;
            }
        }
    }
    environment[index] = NULL;
    return environment;
}

int dcc_hot_reload_worker_spawn_posix(
    const dcc_hot_reload_t *hot_reload,
    const char *exec_path,
    int to_child[2],
    int from_child[2],
    uint64_t generation,
    pid_t *out_pid
) {
    if (hot_reload == NULL || exec_path == NULL || to_child == NULL ||
        from_child == NULL || out_pid == NULL) {
        return EINVAL;
    }
    char generation_text[32];
    int generation_len = snprintf(
        generation_text, sizeof(generation_text), "%llu",
        (unsigned long long)generation
    );
    if (generation_len < 0 || (size_t)generation_len >= sizeof(generation_text)) {
        return EOVERFLOW;
    }
    char *const argv[] = {
        (char *)hot_reload->worker_path,
        hot_reload->path,
        hot_reload->init_symbol,
        hot_reload->deinit_symbol,
        generation_text,
        NULL,
    };
    char **environment = dcc_hot_reload_worker_sanitized_environment();
    if (environment == NULL) {
        return ENOMEM;
    }
    posix_spawn_file_actions_t actions;
    int result = posix_spawn_file_actions_init(&actions);
    int actions_initialized = result == 0;
    if (result == 0) {
        /* Drop the parent's endpoints before dup2: descriptor 4 is commonly
         * the parent end of the first socketpair and is also the child's
         * protocol output descriptor. */
        result = posix_spawn_file_actions_addclose(&actions, to_child[1]);
    }
    if (result == 0) {
        result = posix_spawn_file_actions_addclose(&actions, from_child[0]);
    }
    if (result == 0) {
        result = posix_spawn_file_actions_adddup2(
            &actions, to_child[0], DCC_HOT_RELOAD_WORKER_FD_IN
        );
    }
    if (result == 0) {
        result = posix_spawn_file_actions_adddup2(
            &actions, from_child[1], DCC_HOT_RELOAD_WORKER_FD_OUT
        );
    }
    /* Keep the source descriptors open in the child.  They are the same two
     * IPC sockets (and may alias protocol descriptors 3/4); closing an alias
     * after dup2 would otherwise tear down the child's output endpoint. */
    if (result == 0) {
        result = posix_spawn(
            out_pid, exec_path, &actions, NULL, argv, environment
        );
    }
    if (actions_initialized) {
        (void)posix_spawn_file_actions_destroy(&actions);
    }
    free(environment);
    return result;
}
#endif
