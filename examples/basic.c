#include <dcc/sugar.h>

#include <stdio.h>

DCC_SLASH_FN(on_ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_READY_FN(on_ready) {
    (void)app;
    (void)user_data;
    printf("DCC ready: shard %u/%u\n", ready->shard_id, ready->shard_count);
}

DCC_SIMPLE_BOT_MAIN(
    "basic",
    DCC_LISTEN_SLASH("ping", "Reply with pong", on_ping),
    DCC_LISTEN_READY(on_ready)
)
