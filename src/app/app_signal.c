#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include "internal/app/dcc_app_internal.h"

#if defined(_WIN32)
#include <windows.h>

static dcc_app_t *dcc_signal_app;

static BOOL WINAPI dcc_app_console_handler(DWORD signal) {
    if ((signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT ||
            signal == CTRL_SHUTDOWN_EVENT) && dcc_signal_app != NULL) {
        (void)dcc_app_stop(dcc_signal_app);
        return TRUE;
    }
    return FALSE;
}

dcc_status_t dcc_app_run_with_signals(dcc_app_t *app) {
    if (app == NULL || dcc_signal_app != NULL) return DCC_ERR_INVALID_ARG;
    dcc_signal_app = app;
    if (!SetConsoleCtrlHandler(dcc_app_console_handler, TRUE)) {
        dcc_signal_app = NULL;
        return DCC_ERR_RUNTIME;
    }
    dcc_status_t status = dcc_app_run(app);
    (void)SetConsoleCtrlHandler(dcc_app_console_handler, FALSE);
    dcc_signal_app = NULL;
    return status;
}

#else

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

typedef struct dcc_app_signal_monitor {
    dcc_app_t *app;
    int read_fd;
} dcc_app_signal_monitor_t;

static volatile sig_atomic_t dcc_app_signal_write_fd = -1;

static void dcc_app_signal_handler(int signal_number) {
    (void)signal_number;
    int saved_errno = errno;
    if (dcc_app_signal_write_fd >= 0) {
        const unsigned char byte = 's';
        (void)write((int)dcc_app_signal_write_fd, &byte, 1U);
    }
    errno = saved_errno;
}

static void *dcc_app_signal_monitor_main(void *arg) {
    dcc_app_signal_monitor_t *monitor = (dcc_app_signal_monitor_t *)arg;
    unsigned char byte = 0U;
    while (read(monitor->read_fd, &byte, 1U) < 0 && errno == EINTR) {
    }
    if (byte == 's') (void)dcc_app_stop(monitor->app);
    return NULL;
}

dcc_status_t dcc_app_run_with_signals(dcc_app_t *app) {
    if (app == NULL || dcc_app_signal_write_fd >= 0) return DCC_ERR_INVALID_ARG;
    int pipe_fds[2];
    if (pipe(pipe_fds) != 0) return DCC_ERR_RUNTIME;
    int flags = fcntl(pipe_fds[1], F_GETFL, 0);
    if (flags >= 0) (void)fcntl(pipe_fds[1], F_SETFL, flags | O_NONBLOCK);

    struct sigaction action;
    struct sigaction old_int;
    struct sigaction old_term;
    sigemptyset(&action.sa_mask);
    action.sa_handler = dcc_app_signal_handler;
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, &old_int) != 0) {
        (void)close(pipe_fds[0]); (void)close(pipe_fds[1]);
        return DCC_ERR_RUNTIME;
    }
    if (sigaction(SIGTERM, &action, &old_term) != 0) {
        (void)sigaction(SIGINT, &old_int, NULL);
        (void)close(pipe_fds[0]); (void)close(pipe_fds[1]);
        return DCC_ERR_RUNTIME;
    }

    dcc_app_signal_monitor_t monitor = {.app = app, .read_fd = pipe_fds[0]};
    pthread_t thread;
    dcc_app_signal_write_fd = pipe_fds[1];
    if (pthread_create(&thread, NULL, dcc_app_signal_monitor_main, &monitor) != 0) {
        dcc_app_signal_write_fd = -1;
        (void)sigaction(SIGINT, &old_int, NULL); (void)sigaction(SIGTERM, &old_term, NULL);
        (void)close(pipe_fds[0]); (void)close(pipe_fds[1]);
        return DCC_ERR_RUNTIME;
    }

    dcc_status_t status = dcc_app_run(app);
    const unsigned char stop_byte = 'x';
    (void)write(pipe_fds[1], &stop_byte, 1U);
    (void)pthread_join(thread, NULL);
    dcc_app_signal_write_fd = -1;
    (void)sigaction(SIGINT, &old_int, NULL);
    (void)sigaction(SIGTERM, &old_term, NULL);
    (void)close(pipe_fds[0]); (void)close(pipe_fds[1]);
    return status;
}
#endif

dcc_status_t dcc_app_run_defined_with_signals(
    const dcc_app_options_t *options,
    const dcc_app_definition_t *definition
) {
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create_defined(options, definition, &app);
    if (status != DCC_OK) return status;
    status = dcc_app_run_with_signals(app);
    dcc_app_destroy(app);
    return status;
}

dcc_status_t dcc_app_run_from_env_defined_with_signals(
    const char *token_env,
    const dcc_app_definition_t *definition
) {
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create_from_env_defined(token_env, definition, &app);
    if (status != DCC_OK) return status;
    status = dcc_app_run_with_signals(app);
    dcc_app_destroy(app);
    return status;
}

dcc_status_t dcc_app_run_dotenv_defined_with_signals(
    const char *token_env,
    const dcc_app_definition_t *definition
) {
    dcc_status_t status = dcc_app_load_dotenv();
    if (status != DCC_OK && status != DCC_ERR_NOT_FOUND) return status;
    return dcc_app_run_from_env_defined_with_signals(token_env, definition);
}
