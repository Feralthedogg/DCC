# DCC 2.0

DCC is a C11 Discord runtime built on LLAM 2.2.0. DCC 2.0.0 Stable provides a
small Bot API for application code and focused headers for App, Gateway, REST,
voice, cluster, replay, hot reload, and interaction-server integration.

```c
#include <dcc/bot.h>

DCC_SLASH_FN(ping) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "pong");
}

DCC_DEV_BOT_MAIN(
    DCC_LISTEN_SLASH("ping", "Reply with pong", ping)
)
```

Use `DCC_DEV_BOT_MAIN` locally when `.env` loading and READY command sync are
wanted. Use `DCC_BOT_MAIN` in production and apply command changes explicitly.

- [Quickstart](getting-started.md)
- [Choose an API layer](concepts/api-layers.md)
- [Ownership and async](concepts/ownership-and-async.md)
- [Components v2](guides/components-v2.md)
- [Generated API reference](reference/api/index.md)
- [Migrate from DCC 1](migration-1-to-2.md)
