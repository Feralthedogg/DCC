#ifndef DCC_EVENTS_BASE_H
#define DCC_EVENTS_BASE_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/objects.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_client dcc_client_t;
typedef struct dcc_event dcc_event_t;
#ifndef DCC_VOICE_CLIENT_TYPEDEF
#define DCC_VOICE_CLIENT_TYPEDEF
typedef struct dcc_voice_client dcc_voice_client_t;
#endif
typedef uint64_t dcc_listener_id_t;

#ifdef __cplusplus
}
#endif

#endif
