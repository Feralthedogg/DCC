#ifndef DCC_VOICE_OWNER_REGISTRY_INTERNAL_H
#define DCC_VOICE_OWNER_REGISTRY_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_voice_client_register_owner(dcc_client_t *client, dcc_voice_client_t *voice_client);
void dcc_voice_client_unregister_owner(dcc_voice_client_t *voice_client);
void dcc_voice_client_stop_owned(dcc_client_t *client);
void dcc_voice_client_unbind_owner(dcc_client_t *client);
void dcc_voice_client_set_self_user_id(dcc_client_t *client, dcc_snowflake_t user_id);
void dcc_voice_client_apply_gateway_event(dcc_client_t *client, const dcc_event_t *event);
void dcc_voice_client_registry_lock(dcc_client_t *client);
void dcc_voice_client_registry_unlock(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
