#ifndef DCC_VERSION_H
#define DCC_VERSION_H

#include <dcc/export.h>

#define DCC_VERSION_MAJOR 1
#define DCC_VERSION_MINOR 2
#define DCC_VERSION_PATCH 0

#ifdef __cplusplus
extern "C" {
#endif

DCC_API const char *dcc_version_string(void);

#ifdef __cplusplus
}
#endif

#endif
