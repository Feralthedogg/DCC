#ifndef DCC_REST_TASK7_PROBE_INTERNAL_H
#define DCC_REST_TASK7_PROBE_INTERNAL_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

void dcc_endpoint_task7_probe_prepared(
    const char *operation,
    const char *audit_header,
    const char *authorization
);
dcc_status_t dcc_endpoint_task7_probe_consume_failure(void);
void dcc_endpoint_task7_probe_secure_wipe(size_t byte_count);

#endif
