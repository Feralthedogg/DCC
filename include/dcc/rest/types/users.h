#ifndef DCC_REST_TYPES_USERS_H
#define DCC_REST_TYPES_USERS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_current_user_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *username;
  const char *avatar;
  const char *banner;
} dcc_current_user_params_t;

#define DCC_CURRENT_USER_PARAMS_VERSION 1U
enum {
  DCC_CURRENT_USER_PRESENT_USERNAME = UINT64_C(1),
  DCC_CURRENT_USER_PRESENT_AVATAR = UINT64_C(1) << 1U,
  DCC_CURRENT_USER_PRESENT_BANNER = UINT64_C(1) << 2U
};
#define DCC_CURRENT_USER_PARAMS_INIT                                           \
  {sizeof(dcc_current_user_params_t),                                          \
   DCC_CURRENT_USER_PARAMS_VERSION,                                            \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL}
static inline void
dcc_current_user_params_init(dcc_current_user_params_t *value) {
  if (value != NULL) {
    const dcc_current_user_params_t initialized = DCC_CURRENT_USER_PARAMS_INIT;
    *value = initialized;
  }
}

#ifdef __cplusplus
}
#endif

#endif
