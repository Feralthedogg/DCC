#include <dcc/hot_reload.h>

#include <stdlib.h>

DCC_MODULE_API dcc_status_t dcc_bot_module_init(dcc_bot_module_ctx_t *ctx) {
    (void)ctx;
    exit(43);
}
