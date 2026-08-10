#ifndef DCC_CLIENT_LIFECYCLE_INTERNAL_H
#define DCC_CLIENT_LIFECYCLE_INTERNAL_H

#include <dcc/client.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Stop every REST producer while retaining the client allocation so App-owned
 * callback data can remain valid until terminal delivery has drained. */
dcc_status_t dcc_client_prepare_destroy(dcc_client_t *client);

/* Permanently close terminal entry after every producer has quiesced. */
void dcc_client_close_rest(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
