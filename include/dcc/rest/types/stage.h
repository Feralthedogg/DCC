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
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t channel_id;
    const char *topic;
    dcc_stage_privacy_level_t privacy_level;
    uint8_t send_start_notification;
    dcc_snowflake_t guild_scheduled_event_id;
} dcc_stage_instance_params_t;

enum {
  DCC_STAGE_INSTANCE_PARAMS_VERSION = 1U,
  DCC_STAGE_INSTANCE_PARAMS_PRESENT_CHANNEL_ID = UINT64_C(1),
  DCC_STAGE_INSTANCE_PARAMS_PRESENT_TOPIC = UINT64_C(1) << 1U,
  DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL = UINT64_C(1) << 2U,
  DCC_STAGE_INSTANCE_PARAMS_PRESENT_SEND_START_NOTIFICATION = UINT64_C(1) << 3U,
  DCC_STAGE_INSTANCE_PARAMS_PRESENT_GUILD_SCHEDULED_EVENT_ID = UINT64_C(1) << 4U
};

#define DCC_STAGE_INSTANCE_PARAMS_INIT                                         \
  {sizeof(dcc_stage_instance_params_t), DCC_STAGE_INSTANCE_PARAMS_VERSION,     \
   UINT64_C(0), UINT64_C(0), NULL, DCC_STAGE_PRIVACY_GUILD_ONLY, 0U,           \
   UINT64_C(0)}

static inline void dcc_stage_instance_params_init(
    dcc_stage_instance_params_t *params) {
  if (params != NULL) {
    dcc_stage_instance_params_t value = DCC_STAGE_INSTANCE_PARAMS_INIT;
    *params = value;
  }
}

#ifdef __cplusplus
}
#endif

#endif
