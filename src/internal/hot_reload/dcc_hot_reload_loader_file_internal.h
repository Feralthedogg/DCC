#ifndef DCC_HOT_RELOAD_LOADER_FILE_INTERNAL_H
#define DCC_HOT_RELOAD_LOADER_FILE_INTERNAL_H

#include <dcc/error.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_copy_file(const char *source, const char *target);
char *dcc_hot_reload_temp_path(const char *path, uint64_t generation);

#ifdef __cplusplus
}
#endif

#endif
