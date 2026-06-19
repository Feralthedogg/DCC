#include "internal/rest/dcc_rest_role_connection_builders_internal.h"

int dcc_rest_application_role_connection_key_valid(const char *key) {
    if (key == NULL || key[0] == '\0') {
        return 0;
    }
    size_t len = 0;
    for (const unsigned char *p = (const unsigned char *)key; *p != '\0'; ++p) {
        unsigned char ch = *p;
        if (!((ch >= (unsigned char)'a' && ch <= (unsigned char)'z') ||
              (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') ||
              ch == (unsigned char)'_')) {
            return 0;
        }
        if (++len > 50U) {
            return 0;
        }
    }
    return 1;
}
