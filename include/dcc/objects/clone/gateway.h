#ifndef DCC_OBJECTS_CLONE_GATEWAY_H
#define DCC_OBJECTS_CLONE_GATEWAY_H

#include <dcc/objects/gateway.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_voice_state_t *dcc_voice_state_clone(const dcc_voice_state_t *voice_state);
DCC_API void dcc_voice_state_free(dcc_voice_state_t *voice_state);
DCC_API dcc_gateway_event_data_t *dcc_gateway_event_data_clone(const dcc_gateway_event_data_t *data);
DCC_API void dcc_gateway_event_data_free(dcc_gateway_event_data_t *data);

#ifdef __cplusplus
}
#endif

#endif
