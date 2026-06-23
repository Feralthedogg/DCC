#ifndef DCC_STORE_H
#define DCC_STORE_H

#include <dcc/managed_message.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef dcc_status_t (*dcc_store_get_fn)(
    void *user_data,
    const char *key,
    char *out_value,
    size_t out_value_size,
    size_t *out_value_len
);
typedef dcc_status_t (*dcc_store_set_fn)(
    void *user_data,
    const char *key,
    const char *value
);
typedef dcc_status_t (*dcc_store_delete_fn)(void *user_data, const char *key);
typedef void (*dcc_store_cleanup_fn)(void *user_data);

typedef struct dcc_store {
    size_t size;
    dcc_store_get_fn get;
    dcc_store_set_fn set;
    dcc_store_delete_fn delete_key;
    dcc_store_cleanup_fn cleanup;
    void *user_data;
} dcc_store_t;

typedef struct dcc_store_options {
    size_t size;
    dcc_store_get_fn get;
    dcc_store_set_fn set;
    dcc_store_delete_fn delete_key;
    dcc_store_cleanup_fn cleanup;
    void *user_data;
} dcc_store_options_t;

typedef struct dcc_store_file_options {
    size_t size;
    const char *path;
} dcc_store_file_options_t;

typedef struct dcc_store_managed_message_binding {
    size_t size;
    dcc_store_t *store;
    const char *key;
} dcc_store_managed_message_binding_t;

DCC_API dcc_status_t dcc_store_init(
    dcc_store_t *store,
    const dcc_store_options_t *options
);
DCC_API void dcc_store_deinit(dcc_store_t *store);
DCC_API dcc_status_t dcc_store_get(
    const dcc_store_t *store,
    const char *key,
    char *out_value,
    size_t out_value_size,
    size_t *out_value_len
);
DCC_API dcc_status_t dcc_store_set(
    dcc_store_t *store,
    const char *key,
    const char *value
);
DCC_API dcc_status_t dcc_store_delete(dcc_store_t *store, const char *key);
DCC_API dcc_status_t dcc_store_get_u64(
    const dcc_store_t *store,
    const char *key,
    uint64_t *out_value
);
DCC_API dcc_status_t dcc_store_set_u64(
    dcc_store_t *store,
    const char *key,
    uint64_t value
);
DCC_API dcc_status_t dcc_store_file_open(const char *path, dcc_store_t *out_store);
DCC_API dcc_status_t dcc_store_file_open_options(
    const dcc_store_file_options_t *options,
    dcc_store_t *out_store
);
DCC_API void dcc_store_managed_message_binding_init(
    dcc_store_managed_message_binding_t *binding,
    dcc_store_t *store,
    const char *key
);
DCC_API dcc_status_t dcc_store_load_managed_message_ref(
    dcc_managed_message_ref_t *out_ref,
    void *user_data
);
DCC_API dcc_status_t dcc_store_save_managed_message_ref(
    const dcc_managed_message_ref_t *ref,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
