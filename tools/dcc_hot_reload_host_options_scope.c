#include "internal/dcc_hot_reload_host.h"

int dcc_hot_reload_host_apply_scope_env(const dcc_hot_reload_host_options_t *options) {
    if (options->guild_id_set > 0) {
        char guild_id_text[32];
        snprintf(guild_id_text, sizeof(guild_id_text), "%llu", (unsigned long long)options->guild_id);
        if (dcc_hot_reload_host_setenv("DCC_HOT_RELOAD_GUILD_ID", guild_id_text) != 0) {
            fprintf(stderr, "could not set DCC_HOT_RELOAD_GUILD_ID\n");
            return -1;
        }
    } else if (options->guild_id_set < 0 && dcc_hot_reload_host_unsetenv("DCC_HOT_RELOAD_GUILD_ID") != 0) {
        fprintf(stderr, "could not unset DCC_HOT_RELOAD_GUILD_ID\n");
        return -1;
    }
    return 0;
}
