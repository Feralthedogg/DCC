#ifndef DCC_VOICE_BASE_H
#define DCC_VOICE_BASE_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/objects.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_client dcc_client_t;
#ifndef DCC_VOICE_CLIENT_TYPEDEF
#define DCC_VOICE_CLIENT_TYPEDEF
typedef struct dcc_voice_client dcc_voice_client_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
