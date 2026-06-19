#ifndef DCC_CLIENT_LIFECYCLE_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_LIFECYCLE_STATE_FIELDS_INTERNAL_H

#include <dcc/client.h>

#include <stdatomic.h>

#define DCC_CLIENT_LIFECYCLE_STATE_FIELDS \
    atomic_bool started; \
    atomic_bool stopping; \
    dcc_log_fn log_fn; \
    void *log_user_data; \
    char last_error[256]

#endif
