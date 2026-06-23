#ifndef DCC_PACKAGE_CONSUMER_CHECKS_H
#define DCC_PACKAGE_CONSUMER_CHECKS_H

#include "package_consumer_symbols.h"

int dcc_package_consumer_check_wait_api(void);
int dcc_package_consumer_check_app_api(void);
int dcc_package_consumer_check_voice_api(void);
int dcc_package_consumer_check_cluster_api(void);
int dcc_package_consumer_check_interaction_api(void);
int dcc_package_consumer_check_interaction_server_api(void);
int dcc_package_consumer_check_hot_reload_api(void);

#endif
