#include <dcc/sugar.h>

static int bot_main_state;

DCC_SLASH_FN(bot_main_ping) {
    (void)ctx;
    (void)user_data;
}

DCC_READY_FN(bot_main_ready) {
    (void)app;
    (void)ready;
    (void)user_data;
}

DCC_DEFINE_FEATURE(
    bot_main_feature,
    "bot_main_feature",
    DCC_FEATURE_EVENTS(DCC_ON_READY_DATA(bot_main_ready, user_data))
)

#define main dcc_bot_main_compile_app
DCC_BOT_MAIN(
    "main.compile.app",
    DCC_APP_EVENTS(DCC_ON_READY_DATA(bot_main_ready, &bot_main_state))
)
#undef main

#define main dcc_bot_main_compile_app_token
DCC_BOT_MAIN_TOKEN(
    "DCC_MAIN_TOKEN",
    "main.compile.app.token",
    DCC_APP_EVENTS(DCC_ON_READY_DATA(bot_main_ready, &bot_main_state))
)
#undef main

#define main dcc_bot_main_compile_app_env
DCC_BOT_MAIN_ENV(
    "DCC_MAIN_TOKEN",
    "main.compile.app.env",
    DCC_APP_EVENTS(DCC_ON_READY_DATA(bot_main_ready, &bot_main_state))
)
#undef main

#define main dcc_bot_main_compile_simple
DCC_SIMPLE_BOT_MAIN(
    "main.compile.simple",
    DCC_LISTEN_SLASH("ping", "Reply with pong", bot_main_ping),
    DCC_LISTEN_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_simple_token
DCC_SIMPLE_BOT_MAIN_TOKEN(
    "DCC_MAIN_TOKEN",
    "main.compile.simple.token",
    DCC_LISTEN_SLASH("ping", "Reply with pong", bot_main_ping),
    DCC_LISTEN_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_simple_env
DCC_SIMPLE_BOT_MAIN_ENV(
    "DCC_MAIN_TOKEN",
    "main.compile.simple.env",
    DCC_LISTEN_SLASH("ping", "Reply with pong", bot_main_ping),
    DCC_LISTEN_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_guild
DCC_GUILD_BOT_MAIN(
    "main.compile.guild",
    123ULL,
    DCC_APP_EVENTS(DCC_ON_READY_DATA(bot_main_ready, &bot_main_state))
)
#undef main

#define main dcc_bot_main_compile_guild_simple
DCC_SIMPLE_GUILD_BOT_MAIN(
    "main.compile.guild.simple",
    123ULL,
    DCC_LISTEN_SLASH("ping", "Reply with pong", bot_main_ping),
    DCC_LISTEN_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_listeners
DCC_BOT_LISTENERS_MAIN(
    "main.compile.listeners",
    DCC_LISTEN_SLASH("ping", "Reply with pong", bot_main_ping),
    DCC_LISTEN_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_routes
DCC_BOT_ROUTES_MAIN(
    "main.compile.routes",
    DCC_ROUTE_COMMAND("ping", "Reply with pong", bot_main_ping),
    DCC_ROUTE_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_guild_listeners
DCC_GUILD_BOT_LISTENERS_MAIN(
    "main.compile.guild.listeners",
    123ULL,
    DCC_LISTEN_SLASH("ping", "Reply with pong", bot_main_ping),
    DCC_LISTEN_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_guild_routes
DCC_GUILD_BOT_ROUTES_MAIN(
    "main.compile.guild.routes",
    123ULL,
    DCC_ROUTE_COMMAND("ping", "Reply with pong", bot_main_ping),
    DCC_ROUTE_READY_DATA(bot_main_ready, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_features
DCC_BOT_FEATURES_MAIN(
    "main.compile.features",
    DCC_USE_FEATURE(bot_main_feature, &bot_main_state)
)
#undef main

#define main dcc_bot_main_compile_guild_features
DCC_GUILD_BOT_FEATURES_MAIN(
    "main.compile.guild.features",
    123ULL,
    DCC_USE_FEATURE(bot_main_feature, &bot_main_state)
)
#undef main

int main(void) {
    return DCC_STATUS_EXIT_CODE(DCC_OK);
}
