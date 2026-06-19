#ifndef DCC_REST_TYPES_INVITES_H
#define DCC_REST_TYPES_INVITES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_invite_target_type {
    DCC_INVITE_TARGET_NONE = 0,
    DCC_INVITE_TARGET_STREAM = 1,
    DCC_INVITE_TARGET_EMBEDDED_APPLICATION = 2
} dcc_invite_target_type_t;

typedef struct dcc_invite_params {
    size_t size;
    dcc_snowflake_t channel_id;
    uint32_t max_age;
    uint8_t max_uses;
    dcc_snowflake_t target_user_id;
    dcc_invite_target_type_t target_type;
    uint8_t temporary;
    uint8_t unique;
} dcc_invite_params_t;

#ifdef __cplusplus
}
#endif

#endif
