#include <dcc/bot.h>

DCC_SLASH_FN(bot_v2_main_ping) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "pong");
}

#if defined(DCC_TEST_DEV_DEFAULT_MAIN)
DCC_DEV_BOT_MAIN(DCC_LISTEN_SLASH("ping", "Ping", bot_v2_main_ping))
#elif defined(DCC_TEST_PROD_DEFAULT_MAIN)
DCC_BOT_MAIN(DCC_LISTEN_SLASH("ping", "Ping", bot_v2_main_ping))
#elif defined(DCC_TEST_DEV_CONFIG_MAIN)
DCC_DEV_BOT_MAIN_WITH(
    DCC_BOT_CONFIG(),
    DCC_LISTEN_SLASH("ping", "Ping", bot_v2_main_ping)
)
#elif defined(DCC_TEST_PROD_CONFIG_MAIN)
DCC_BOT_MAIN_WITH(
    DCC_BOT_CONFIG(),
    DCC_LISTEN_SLASH("ping", "Ping", bot_v2_main_ping)
)
#else
#error "select one Bot main compile mode"
#endif
