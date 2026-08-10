#include <dcc/bot.h>

DCC_SLASH_FN(v2_ping) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "pong");
}

static const dcc_listener_t listeners[] = {
    DCC_LISTEN_SLASH("ping", "Reply with pong", v2_ping),
};

int main(void) {
    return listeners[0].kind == DCC_LISTENER_SLASH ? 0 : 1;
}
