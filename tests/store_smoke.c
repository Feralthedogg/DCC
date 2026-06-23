#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

int main(void) {
    const char *path = "dcc_store_smoke.kv";
    remove(path);
    remove("dcc_store_smoke.kv.tmp");

    dcc_store_t store = {0};
    dcc_store_t reopened = {0};
    char value[64];
    size_t value_len = 0;
    uint64_t count = 0;
    dcc_managed_message_ref_t ref = {
        .channel_id = 111ULL,
        .message_id = 222ULL,
    };
    dcc_managed_message_ref_t loaded_ref = {0};
    dcc_store_managed_message_binding_t binding =
        DCC_STORE_MANAGED_MESSAGE("latest.registration", &store);
    dcc_message_builder_t message = DCC_MESSAGE_TEXT("managed");
    dcc_managed_message_options_t managed =
        DCC_MANAGED_MESSAGE_STORE_OPTIONS(111ULL, &message, &binding);
    dcc_managed_message_options_t managed_keep =
        DCC_MANAGED_MESSAGE_STORE_KEEP_PREVIOUS_OPTIONS(111ULL, &message, &binding);
    dcc_store_file_options_t file_options = DCC_STORE_FILE_OPTIONS(path);

    if (dcc_store_init(NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_store_get(NULL, "key", value, sizeof(value), &value_len) != DCC_ERR_INVALID_ARG ||
        dcc_store_set(NULL, "key", "value") != DCC_ERR_INVALID_ARG ||
        dcc_store_delete(NULL, "key") != DCC_ERR_INVALID_ARG ||
        dcc_store_file_open(NULL, &store) != DCC_ERR_INVALID_ARG ||
        DCC_STORE_FILE(path, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "store invalid argument contract failed\n");
        return 1;
    }

    if (dcc_store_file_open_options(&file_options, &store) != DCC_OK ||
        store.size != sizeof(store) ||
        dcc_store_get(&store, "missing", value, sizeof(value), &value_len) != DCC_ERR_NOT_FOUND ||
        dcc_store_set(&store, "name", "dcc") != DCC_OK ||
        dcc_store_get(&store, "name", value, sizeof(value), &value_len) != DCC_OK ||
        strcmp(value, "dcc") != 0 ||
        value_len != 3U ||
        dcc_store_get(&store, "name", NULL, 0U, &value_len) != DCC_OK ||
        value_len != 3U ||
        dcc_store_get(&store, "name", value, 2U, &value_len) != DCC_ERR_STATE ||
        DCC_STORE_SET_U64(&store, "count", 42U) != DCC_OK ||
        DCC_STORE_GET_U64(&store, "count", &count) != DCC_OK ||
        count != 42U ||
        dcc_store_set(&store, "bad=key", "value") != DCC_ERR_INVALID_ARG ||
        dcc_store_set(&store, "bad", "line\nbreak") != DCC_ERR_INVALID_ARG ||
        dcc_store_save_managed_message_ref(&ref, &binding) != DCC_OK ||
        dcc_store_load_managed_message_ref(&loaded_ref, &binding) != DCC_OK ||
        loaded_ref.channel_id != 111ULL ||
        loaded_ref.message_id != 222ULL ||
        managed.load != dcc_store_load_managed_message_ref ||
        managed.save != dcc_store_save_managed_message_ref ||
        managed.storage_user_data != &binding ||
        managed_keep.keep_previous != 1U ||
        dcc_store_delete(&store, "name") != DCC_OK ||
        dcc_store_get(&store, "name", value, sizeof(value), NULL) != DCC_ERR_NOT_FOUND) {
        fprintf(stderr, "store file contract failed\n");
        dcc_store_deinit(&store);
        remove(path);
        remove("dcc_store_smoke.kv.tmp");
        return 1;
    }

    dcc_store_deinit(&store);

    if (dcc_store_file_open(path, &reopened) != DCC_OK ||
        dcc_store_load_managed_message_ref(&loaded_ref, &(dcc_store_managed_message_binding_t){
            .size = sizeof(dcc_store_managed_message_binding_t),
            .store = &reopened,
            .key = "latest.registration",
        }) != DCC_OK ||
        loaded_ref.channel_id != 111ULL ||
        loaded_ref.message_id != 222ULL) {
        fprintf(stderr, "store persistence contract failed\n");
        dcc_store_deinit(&reopened);
        remove(path);
        remove("dcc_store_smoke.kv.tmp");
        return 1;
    }

    dcc_store_deinit(&reopened);
    remove(path);
    remove("dcc_store_smoke.kv.tmp");
    return 0;
}
