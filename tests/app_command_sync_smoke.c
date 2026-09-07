#include <dcc/app/legacy.h>
#include <dcc/app/lifecycle.h>

#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

typedef struct command_sync_callback_state {
    atomic_uint called;
    dcc_status_t error;
    uint16_t status;
} command_sync_callback_state_t;

static void command_sync_callback(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    command_sync_callback_state_t *state =
        (command_sync_callback_state_t *)user_data;
    if (state == NULL || response == NULL) {
        return;
    }
    state->error = response->error;
    state->status = response->status;
    atomic_store_explicit(&state->called, 1U, memory_order_release);
}

static void command_sync_sleep_ms(long milliseconds) {
    if (milliseconds <= 0L) return;
#if defined(_WIN32)
    Sleep((DWORD)milliseconds);
#else
    struct timespec delay = {
        .tv_sec = milliseconds / 1000L,
        .tv_nsec = (milliseconds % 1000L) * 1000000L,
    };
    (void)nanosleep(&delay, NULL);
#endif
}

int main(void) {
    dcc_app_options_t app_options;
    dcc_app_options_init(&app_options);
    app_options.client.token = "";

    dcc_app_t *app = NULL;
    if (dcc_app_create(&app_options, &app) != DCC_OK || app == NULL) {
        fprintf(stderr, "failed to create app\n");
        return 1;
    }

    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);
    if (dcc_application_command_builder_set_name(&command, "sync") != DCC_OK ||
        dcc_application_command_builder_set_description(&command, "sync") != DCC_OK ||
        dcc_application_command_builder_set_type(
            &command, DCC_APPLICATION_COMMAND_CHAT_INPUT
        ) != DCC_OK ||
        dcc_command_registry_add_builder(
            dcc_app_command_registry(app), &command
        ) != DCC_OK) {
        (void)dcc_app_destroy(app);
        fprintf(stderr, "failed to add command\n");
        return 1;
    }

    dcc_command_registry_options_t options = DCC_COMMAND_REGISTRY_OPTIONS_INIT;
    if (dcc_command_registry_options_set_dry_run(&options, 1U) != DCC_OK) {
        (void)dcc_app_destroy(app);
        return 1;
    }
    dcc_command_registry_plan_t plan;
    if (dcc_command_registry_build_plan(
            dcc_app_command_registry(app), &options, NULL, 0U, &plan
        ) != DCC_OK) {
        (void)dcc_app_destroy(app);
        fprintf(stderr, "failed to build command plan\n");
        return 1;
    }

    command_sync_callback_state_t callback_state;
    memset(&callback_state, 0, sizeof(callback_state));
    atomic_init(&callback_state.called, 0U);
    dcc_status_t status = dcc_app_apply_command_plan(
        app, 123U, &options, &plan, command_sync_callback, &callback_state
    );
    dcc_command_registry_plan_deinit(&plan);
    if (status != DCC_OK) {
        (void)dcc_app_destroy(app);
        fprintf(stderr, "apply returned %s\n", dcc_status_string(status));
        return 1;
    }

    for (int attempt = 0; attempt < 100 &&
         atomic_load_explicit(&callback_state.called, memory_order_acquire) == 0U;
         ++attempt) {
        command_sync_sleep_ms(5L);
    }
    unsigned callback_called =
        atomic_load_explicit(&callback_state.called, memory_order_acquire);
    int callback_error_ok = callback_state.error == DCC_OK;
    int callback_status_ok = callback_state.status != 0U;
    int passed = callback_called != 0U && callback_error_ok && callback_status_ok;
    dcc_status_t destroy_status = DCC_ERR_STATE;
    for (int attempt = 0; attempt < 100 && destroy_status == DCC_ERR_STATE; ++attempt) {
        destroy_status = dcc_app_destroy(app);
        if (destroy_status == DCC_ERR_STATE) {
            command_sync_sleep_ms(5L);
        }
    }
    if (destroy_status != DCC_OK || !passed) {
        fprintf(
            stderr,
            "command sync callback missing or failed: called=%u status=%u error=%s destroy=%s checks=%u/%d/%d\n",
            callback_called,
            callback_state.status,
            dcc_status_string(callback_state.error),
            dcc_status_string(destroy_status),
            callback_called,
            callback_error_ok,
            callback_status_ok
        );
        return 1;
    }
    return 0;
}
