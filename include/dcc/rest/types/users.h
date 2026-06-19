#ifndef DCC_REST_TYPES_USERS_H
#define DCC_REST_TYPES_USERS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_current_user_params {
    size_t size;
    const char *nickname;
    const char *avatar;
    const char *banner;
} dcc_current_user_params_t;

#ifdef __cplusplus
}
#endif

#endif
