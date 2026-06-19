#include <dcc/hot_reload.h>

static void dcc_package_consumer_on_ready(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    (void)client;
    (void)event;
    (void)user_data;
}

static const dcc_bot_module_handler_t dcc_package_consumer_handlers[] = {
    DCC_BOT_MODULE_ON(DCC_EVENT_READY, dcc_package_consumer_on_ready),
};

DCC_BOT_MODULE(dcc_package_consumer_handlers)
