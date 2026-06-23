#include <dcc/app.h>

#include <string.h>

void dcc_app_command_sync_options_init(dcc_app_command_sync_options_t *options) {
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->once = 1U;
    dcc_command_registry_options_init(&options->command_registry);
}

void dcc_app_options_init(dcc_app_options_t *options) {
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->client.size = sizeof(options->client);
    options->client.shard_count = 1U;
    options->client.rest_concurrency = 4U;
    dcc_command_registry_options_init(&options->command_registry);
    dcc_app_command_sync_options_init(&options->command_sync);
}

void dcc_app_cooldown_options_init(dcc_app_cooldown_options_t *options) {
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->bucket = DCC_APP_COOLDOWN_USER;
}
