#include <dcc/bot.h>

DCC_SLASH_FN(on_ping) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "pong");
}

DCC_DEV_BOT_MAIN(
    DCC_LISTEN_SLASH("ping", "Reply with pong", on_ping)
)
