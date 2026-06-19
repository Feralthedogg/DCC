#include "internal/rest/dcc_rest_async_signal_internal.h"

void dcc_rest_async_signal(dcc_client_t *client) {
#if !defined(_WIN32)
    if (client == NULL || !client->rest_wait_initialized) {
        return;
    }
    if (pthread_mutex_lock(&client->rest_wait_mutex) != 0) {
        return;
    }
    (void)pthread_cond_broadcast(&client->rest_wait_cond);
    (void)pthread_mutex_unlock(&client->rest_wait_mutex);
#else
    (void)client;
#endif
}
