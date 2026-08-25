#ifndef DCC_BOT_MAIN_H
#define DCC_BOT_MAIN_H

#include <dcc/app/env.h>
#include <dcc/app/lifecycle.h>
#include <dcc/app/listeners.h>
#include <dcc/app/options.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_bot_config {
    size_t size;
    uint32_t version;
    const dcc_app_options_t *app_options;
    const char *token_env;
    uint8_t install_default_error_handler;
    uint8_t handle_signals;
} dcc_bot_config_t;

enum { DCC_BOT_CONFIG_VERSION = 1 };

/** Initializes the canonical blocking Bot runner configuration. */
#define DCC_BOT_CONFIG_INIT \
    {sizeof(dcc_bot_config_t), DCC_BOT_CONFIG_VERSION, NULL, \
     "DISCORD_TOKEN", 1U, 1U}

/** Returns the default Bot runner configuration by value. */
static inline dcc_bot_config_t DCC_BOT_CONFIG(void) {
    dcc_bot_config_t value = DCC_BOT_CONFIG_INIT;
    return value;
}

/**
 * Runs borrowed configuration/listener inputs and owns the App through cleanup.
 */
DCC_API dcc_status_t dcc_bot_run(
    const dcc_bot_config_t *config,
    const dcc_listener_t *listeners,
    size_t listener_count,
    uint8_t development
);

#ifdef __cplusplus
}
#endif

/** Defines a development main using the default config and local listeners. */
#define DCC_DEV_BOT_MAIN(...) \
    DCC_DEV_BOT_MAIN_WITH(DCC_BOT_CONFIG(), __VA_ARGS__)
/** Defines a production main using the default config and local listeners. */
#define DCC_BOT_MAIN(...) \
    DCC_BOT_MAIN_WITH(DCC_BOT_CONFIG(), __VA_ARGS__)
/** Defines a development main evaluating config/listeners exactly once. */
#define DCC_DEV_BOT_MAIN_WITH(config_, ...) \
    int main(void) { \
        dcc_bot_config_t dcc_bot_main_config = (config_); \
        dcc_listener_t dcc_bot_main_listeners[] = {__VA_ARGS__}; \
        return dcc_bot_run(&dcc_bot_main_config, dcc_bot_main_listeners, \
            sizeof(dcc_bot_main_listeners) / sizeof(dcc_bot_main_listeners[0]), \
            1U) == DCC_OK ? 0 : 1; \
    }
/** Defines a production main evaluating config/listeners exactly once. */
#define DCC_BOT_MAIN_WITH(config_, ...) \
    int main(void) { \
        dcc_bot_config_t dcc_bot_main_config = (config_); \
        dcc_listener_t dcc_bot_main_listeners[] = {__VA_ARGS__}; \
        return dcc_bot_run(&dcc_bot_main_config, dcc_bot_main_listeners, \
            sizeof(dcc_bot_main_listeners) / sizeof(dcc_bot_main_listeners[0]), \
            0U) == DCC_OK ? 0 : 1; \
    }

#endif
