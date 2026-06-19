#ifndef DCC_HOT_RELOAD_FILE_SIG_INTERNAL_H
#define DCC_HOT_RELOAD_FILE_SIG_INTERNAL_H

#include <dcc/error.h>

#include <stdint.h>

typedef struct dcc_hot_reload_file_sig {
    uint64_t mtime_ns;
    uint64_t size;
} dcc_hot_reload_file_sig_t;

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_file_sig(const char *path, dcc_hot_reload_file_sig_t *out);
int dcc_hot_reload_same_sig(
    const dcc_hot_reload_file_sig_t *left,
    const dcc_hot_reload_file_sig_t *right
);

#ifdef __cplusplus
}
#endif

#endif
