#include <dcc/dcc.h>
#include <dcc/hot_reload.h>

#include <stdlib.h>

static void isolated_crash_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    (void)user_data;
    exit(42);
}

static void isolated_crash_slash(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    (void)user_data;
    exit(42);
}

static const dcc_bot_module_handler_t isolated_crash_handlers[] = {
    DCC_BOT_MODULE_ON(DCC_EVENT_READY, isolated_crash_ready),
    DCC_BOT_MODULE_ON(DCC_EVENT_SLASH_COMMAND, isolated_crash_slash),
};

DCC_BOT_MODULE(isolated_crash_handlers)
