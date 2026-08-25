#include <dcc/bot/main.h>

int main(void) {
    dcc_bot_config_t config = DCC_BOT_CONFIG();
    if (config.size != sizeof(config) || config.version != DCC_BOT_CONFIG_VERSION) {
        return 1;
    }
    return dcc_bot_run(NULL, NULL, 0U, 0U) == DCC_ERR_INVALID_ARG ? 0 : 1;
}
