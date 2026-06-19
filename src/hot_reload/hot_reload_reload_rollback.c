#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#endif

dcc_hot_reload_rollback_result_t dcc_hot_reload_restore_previous_module_on_disk(
    dcc_hot_reload_t *hot_reload,
    uint64_t next_generation
) {
    dcc_hot_reload_rollback_result_t result = {0};
    if (hot_reload == NULL ||
        hot_reload->module.handle == NULL ||
        hot_reload->module.loaded_path == NULL) {
        return result;
    }

    result.attempted = 1U;
    char *rollback_path = dcc_hot_reload_temp_path(
        hot_reload->path,
        next_generation | (UINT64_C(1) << 63)
    );
    if (rollback_path == NULL) {
        return result;
    }

    dcc_status_t rollback_status = dcc_hot_reload_copy_file(hot_reload->module.loaded_path, rollback_path);
#if defined(_WIN32)
    if (rollback_status == DCC_OK &&
        !MoveFileExA(rollback_path, hot_reload->path, MOVEFILE_REPLACE_EXISTING)) {
        rollback_status = DCC_ERR_STATE;
    }
#else
    if (rollback_status == DCC_OK && rename(rollback_path, hot_reload->path) != 0) {
        rollback_status = DCC_ERR_STATE;
    }
#endif
    if (rollback_status != DCC_OK) {
        remove(rollback_path);
    } else {
        result.has_file_sig =
            dcc_hot_reload_file_sig(hot_reload->path, &result.file_sig) == DCC_OK ? 1U : 0U;
    }
    free(rollback_path);
    return result;
}
