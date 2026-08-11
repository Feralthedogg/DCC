#include "internal/rest/dcc_rest_sensitive_internal.h"
#include "internal/rest/dcc_rest_task7_probe_internal.h"

#include <stdlib.h>

void dcc_endpoint_secure_zero(void *bytes, size_t byte_count) {
    dcc_endpoint_task7_probe_secure_wipe(byte_count);
    volatile unsigned char *cursor = (volatile unsigned char *)bytes;
    while (cursor != NULL && byte_count != 0U) {
        *cursor++ = 0U;
        --byte_count;
    }
}

void dcc_rest_sensitive_free(char *bytes, size_t byte_count) {
    if (bytes != NULL) {
        dcc_endpoint_secure_zero(bytes, byte_count);
        free(bytes);
    }
}
