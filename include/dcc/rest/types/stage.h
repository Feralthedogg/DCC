#ifndef DCC_REST_TYPES_STAGE_H
#define DCC_REST_TYPES_STAGE_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_stage_privacy_level {
    DCC_STAGE_PRIVACY_PUBLIC = 1,
    DCC_STAGE_PRIVACY_GUILD_ONLY = 2
} dcc_stage_privacy_level_t;

typedef struct dcc_stage_instance_params {
    size_t size;
    dcc_snowflake_t channel_id;
    const char *topic;
    dcc_stage_privacy_level_t privacy_level;
} dcc_stage_instance_params_t;

#ifdef __cplusplus
}
#endif

#endif
