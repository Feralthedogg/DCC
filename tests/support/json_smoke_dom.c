#include "json_smoke_support.h"

int dcc_json_smoke_dom_core(void) {
    if (!dcc_json_smoke_arena_allocator_ok()) {
        fprintf(stderr, "json arena allocator guard failed\n");
        return 1;
    }

    const char input[] =
        "{\"op\":10,\"s\":42,\"d\":{\"heartbeat_interval\":45000,"
        "\"session_id\":\"abc\\n\\uD83D\\uDE00\"},\"a\":[true,false,null,-7]}";

    dcc_json_t *root = NULL;
    dcc_status_t st = dcc_json_parse(input, sizeof(input) - 1, &root);
    if (st != DCC_OK) {
        fprintf(stderr, "parse failed: %s\n", dcc_status_string(st));
        return 1;
    }
    if (dcc_json_stage1_validate(input, sizeof(input) - 1) != DCC_OK) {
        fprintf(stderr, "stage1 validation rejected valid json\n");
        dcc_json_free(root);
        return 1;
    }

    int64_t op = 0;
    uint64_t seq = 0;
    int64_t interval = 0;
    int bool_value = 0;
    int64_t neg = 0;

    const dcc_json_t *data = dcc_json_object_get(root, "d");
    const dcc_json_t *array = dcc_json_object_get(root, "a");
    const char *session_id = dcc_json_string(dcc_json_object_get(data, "session_id"));

    int ok = dcc_json_i64(dcc_json_object_get(root, "op"), &op) == 0 &&
             dcc_json_u64(dcc_json_object_get(root, "s"), &seq) == 0 &&
             dcc_json_i64(dcc_json_object_get(data, "heartbeat_interval"), &interval) == 0 &&
             dcc_json_array_size(array) == 4 &&
             dcc_json_bool(dcc_json_array_get(array, 0), &bool_value) == 0 &&
             bool_value == 1 &&
             dcc_json_i64(dcc_json_array_get(array, 3), &neg) == 0 &&
             op == 10 &&
             seq == 42 &&
             interval == 45000 &&
             neg == -7 &&
             session_id != NULL &&
             strcmp(session_id, "abc\n" "\xF0" "\x9F" "\x98" "\x80") == 0;

    dcc_json_free(root);

    if (!ok) {
        fprintf(stderr, "json smoke validation failed\n");
        return 1;
    }

    return 0;
}
