#ifndef DCC_HOT_RELOAD_BASE_H
#define DCC_HOT_RELOAD_BASE_H

#include <dcc/client.h>
#include <dcc/error.h>
#include <dcc/events.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#  define DCC_MODULE_API __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#  define DCC_MODULE_API __attribute__((visibility("default")))
#else
#  define DCC_MODULE_API
#endif

#define DCC_BOT_MODULE_INIT_SYMBOL "dcc_bot_module_init"
#define DCC_BOT_MODULE_DEINIT_SYMBOL "dcc_bot_module_deinit"
#define DCC_HOT_RELOAD_NO_SETTLE UINT32_MAX

typedef struct dcc_hot_reload dcc_hot_reload_t;
typedef struct dcc_bot_module_ctx dcc_bot_module_ctx_t;

typedef dcc_status_t (*dcc_bot_module_init_fn)(dcc_bot_module_ctx_t *ctx);
typedef void (*dcc_bot_module_deinit_fn)(dcc_bot_module_ctx_t *ctx);
typedef dcc_status_t (*dcc_bot_module_load_fn)(dcc_bot_module_ctx_t *ctx, void *state);
typedef void (*dcc_bot_module_unload_fn)(dcc_bot_module_ctx_t *ctx, void *state);

#ifdef __cplusplus
}
#endif

#endif
