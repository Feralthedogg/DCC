#include <dcc/sugar.h>

DCC_SLASH_FN(on_ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_SIMPLE_BOT_MAIN(
    "ping-bot",
    DCC_LISTEN_SLASH("ping", "Reply with pong", on_ping)
)
