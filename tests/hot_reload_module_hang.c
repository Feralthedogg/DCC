#include <dcc/hot_reload.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

DCC_MODULE_API dcc_status_t dcc_bot_module_init(dcc_bot_module_ctx_t *ctx) {
    (void)ctx;
#if defined(_WIN32)
    Sleep(10000);
#else
    sleep(10);
#endif
    return DCC_OK;
}
