#ifndef DCC_ERROR_H
#define DCC_ERROR_H

#include <dcc/export.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_status {
    DCC_OK = 0,
    DCC_ERR_INVALID_ARG,
    DCC_ERR_NOMEM,
    DCC_ERR_RUNTIME,
    DCC_ERR_NETWORK,
    DCC_ERR_TLS,
    DCC_ERR_JSON,
    DCC_ERR_DISCORD,
    DCC_ERR_RATE_LIMITED,
    DCC_ERR_CANCELED,
    DCC_ERR_STATE,
    DCC_ERR_TIMEOUT
} dcc_status_t;

DCC_API const char *dcc_status_string(dcc_status_t status);

#ifdef __cplusplus
}
#endif

#endif
