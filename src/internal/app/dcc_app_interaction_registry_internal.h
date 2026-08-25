#ifndef DCC_APP_INTERACTION_REGISTRY_INTERNAL_H
#define DCC_APP_INTERACTION_REGISTRY_INTERNAL_H

#include <dcc/client.h>
#include <dcc/interaction_flow.h>

dcc_status_t dcc_app_interaction_registry_init(dcc_client_t *client);
void dcc_app_interaction_registry_deinit(dcc_client_t *client);
dcc_status_t
dcc_app_interaction_registry_register(dcc_client_t *client,
                                      dcc_interaction_flow_t *flow);
void dcc_app_interaction_registry_retire(dcc_client_t *client,
                                         dcc_interaction_flow_t *flow);

#endif
