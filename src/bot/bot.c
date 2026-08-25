#include <dcc/bot/main.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"

#include <llam/runtime.h>

#include <stddef.h>
#include <string.h>

static uint8_t dcc_bot_has_field(size_t size, size_t offset, size_t width) {
    return offset <= size && width <= size - offset;
}

static dcc_status_t dcc_bot_normalize_config(
    const dcc_bot_config_t *input,
    dcc_bot_config_t *output
) {
    if (input == NULL || output == NULL ||
        !dcc_bot_has_field(input->size, offsetof(dcc_bot_config_t, version),
                           sizeof(input->version)) ||
        input->version != DCC_BOT_CONFIG_VERSION) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_bot_config_t value = DCC_BOT_CONFIG();
    size_t copy_size = input->size < sizeof(value) ? input->size : sizeof(value);
    memcpy(&value, input, copy_size);
    value.size = sizeof(value);
    value.version = DCC_BOT_CONFIG_VERSION;
    if (!dcc_bot_has_field(input->size,
                           offsetof(dcc_bot_config_t, app_options),
                           sizeof(input->app_options))) {
        value.app_options = NULL;
    }
    if (!dcc_bot_has_field(input->size, offsetof(dcc_bot_config_t, token_env),
                           sizeof(input->token_env)) ||
        value.token_env == NULL || value.token_env[0] == '\0') {
        value.token_env = "DISCORD_TOKEN";
    }
    if (!dcc_bot_has_field(input->size,
                           offsetof(dcc_bot_config_t,
                                    install_default_error_handler),
                           sizeof(input->install_default_error_handler))) {
        value.install_default_error_handler = 1U;
    }
    if (!dcc_bot_has_field(input->size,
                           offsetof(dcc_bot_config_t, handle_signals),
                           sizeof(input->handle_signals))) {
        value.handle_signals = 1U;
    }
    if (value.install_default_error_handler > 1U || value.handle_signals > 1U) {
        return DCC_ERR_INVALID_ARG;
    }
    *output = value;
    return DCC_OK;
}

static dcc_status_t dcc_bot_normalize_app_options(
    const dcc_app_options_t *input,
    dcc_app_options_t *output
) {
    dcc_app_options_init(output);
    if (input == NULL) {
        return DCC_OK;
    }
    const size_t client_size_end =
        offsetof(dcc_app_options_t, client) +
        offsetof(dcc_client_options_t, size) + sizeof(input->client.size);
    if (input->size < client_size_end) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_options_t client_defaults = output->client;
    size_t outer_copy = input->size < sizeof(*output)
                            ? input->size
                            : sizeof(*output);
    memcpy(output, input, outer_copy);
    output->size = sizeof(*output);

    size_t client_available = input->size - offsetof(dcc_app_options_t, client);
    size_t client_copy = input->client.size;
    if (client_copy > client_available) {
        client_copy = client_available;
    }
    if (client_copy > sizeof(client_defaults)) {
        client_copy = sizeof(client_defaults);
    }
    if (client_copy < offsetof(dcc_client_options_t, token) +
                          sizeof(input->client.token)) {
        return DCC_ERR_INVALID_ARG;
    }
    output->client = client_defaults;
    memcpy(&output->client, &input->client, client_copy);
    output->client.size = sizeof(output->client);
    return DCC_OK;
}

static dcc_status_t dcc_bot_destroy_owned_app(
    dcc_app_t *app,
    dcc_status_t first_status
) {
    for (;;) {
        dcc_status_t destroy_status = dcc_app_destroy(app);
        if (destroy_status == DCC_OK) {
            return first_status;
        }
        if (first_status == DCC_OK) {
            first_status = destroy_status;
        }
        (void)dcc_app_stop(app);
        (void)dcc_app_wait(app);
    }
}

dcc_status_t dcc_bot_run(
    const dcc_bot_config_t *config,
    const dcc_listener_t *listeners,
    size_t listener_count,
    uint8_t development
) {
    if (llam_current_task() != NULL || dcc_app_callback_frame_active(NULL) ||
        dcc_rest_terminal_callback_active(NULL)) {
        return DCC_ERR_STATE;
    }
    if (development > 1U ||
        ((listeners == NULL) != (listener_count == 0U))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_bot_config_t normalized_config;
    dcc_status_t status = dcc_bot_normalize_config(config, &normalized_config);
    if (status != DCC_OK) {
        return status;
    }
    if (development != 0U) {
        status = dcc_app_env_load_dotenv();
        if (status != DCC_OK && status != DCC_ERR_NOT_FOUND) {
            return status;
        }
    }

    dcc_app_options_t options;
    status = dcc_bot_normalize_app_options(normalized_config.app_options,
                                           &options);
    if (status != DCC_OK) {
        return status;
    }
    if (options.client.token == NULL || options.client.token[0] == '\0') {
        const char *token = NULL;
        status = dcc_app_env_get_token(normalized_config.token_env, &token);
        if (status != DCC_OK) {
            return status;
        }
        options.client.token = token;
    }

    if (development != 0U) {
        options.command_sync.apply = 1U;
        options.command_sync.once = 1U;
        options.command_sync.infer_application_id = 1U;
        options.command_sync_on_ready = 1U;
    } else {
        options.command_sync_on_ready = 0U;
    }

    dcc_app_t *app = NULL;
    status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return status;
    }
    if (normalized_config.install_default_error_handler != 0U) {
        status = dcc_app_use_default_error_responses(app);
    }
    for (size_t i = 0U; status == DCC_OK && i < listener_count; ++i) {
        status = dcc_app_listen(app, &listeners[i], NULL);
    }
    if (status == DCC_OK) {
        status = normalized_config.handle_signals != 0U
                     ? dcc_app_run_with_signals(app)
                     : dcc_app_run(app);
    }
    return dcc_bot_destroy_owned_app(app, status);
}
