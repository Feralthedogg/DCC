#ifndef DCC_APP_OPTIONS_H
#define DCC_APP_OPTIONS_H

#include <dcc/client.h>
#include <dcc/command_registry.h>
#include <dcc/export.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_app_command_sync_options {
    size_t size;
    dcc_snowflake_t application_id;
    dcc_command_registry_options_t command_registry;
    uint8_t apply;
    uint8_t once;
    uint8_t infer_application_id;
} dcc_app_command_sync_options_t;

typedef struct dcc_app_options {
    size_t size;
    dcc_client_options_t client;
    dcc_command_registry_options_t command_registry;
    uint64_t auto_defer_after_ms;
    uint8_t auto_defer_ephemeral;
    dcc_app_command_sync_options_t command_sync;
    uint8_t command_sync_on_ready;
    const char *store_file_path;
} dcc_app_options_t;

/** Initializes command synchronization options with stable defaults. */
DCC_API void dcc_app_command_sync_options_init(dcc_app_command_sync_options_t *options);

/** Initializes application options with stable defaults. */
DCC_API void dcc_app_options_init(dcc_app_options_t *options);

#ifdef __cplusplus
}
#endif

#endif
