#include "internal/client/dcc_client_state_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"

void dcc_rest_set_interceptor(
    dcc_client_t *client,
    dcc_rest_intercept_fn intercept,
    void *user_data
) {
    if (client == NULL) {
        return;
    }
    client->rest_intercept = intercept;
    client->rest_intercept_user_data = user_data;
}
