#include <dcc/hot_reload.h>

#include "hot_reload_test_state.h"

DCC_MODULE_API dcc_status_t dcc_bot_module_init(dcc_bot_module_ctx_t *ctx) {
    void *state = dcc_bot_module_state(ctx, sizeof(hot_reload_shared_state_t) + 16U);
    return state == NULL ? DCC_ERR_STATE : DCC_ERR_RUNTIME;
}
