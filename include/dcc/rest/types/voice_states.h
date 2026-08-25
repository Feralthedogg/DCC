#ifndef DCC_REST_TYPES_VOICE_STATES_H
#define DCC_REST_TYPES_VOICE_STATES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_current_user_voice_state_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t channel_id;
  uint8_t suppress;
  const char *request_to_speak_timestamp;
} dcc_current_user_voice_state_params_t;

typedef struct dcc_user_voice_state_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t channel_id;
  uint8_t suppress;
} dcc_user_voice_state_params_t;

/* Frozen App compatibility record; canonical REST uses the two split records
 * above. This type is removed with the App mirror surface in Task 14. */
typedef struct dcc_voice_state_params {
  size_t size;
  dcc_snowflake_t guild_id;
  dcc_snowflake_t user_id;
  dcc_snowflake_t channel_id;
  uint8_t suppress;
  const char *request_to_speak_timestamp;
} dcc_voice_state_params_t;

#define DCC_VOICE_STATE_PARAMS_VERSION 1U
enum {
  DCC_VOICE_STATE_PRESENT_CHANNEL_ID = UINT64_C(1),
  DCC_VOICE_STATE_PRESENT_SUPPRESS = UINT64_C(1) << 1U,
  DCC_VOICE_STATE_PRESENT_REQUEST_TO_SPEAK_TIMESTAMP = UINT64_C(1) << 2U
};
#define DCC_CURRENT_USER_VOICE_STATE_PARAMS_INIT                               \
  {sizeof(dcc_current_user_voice_state_params_t),                              \
   DCC_VOICE_STATE_PARAMS_VERSION,                                             \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   NULL}
#define DCC_USER_VOICE_STATE_PARAMS_INIT                                       \
  {sizeof(dcc_user_voice_state_params_t), DCC_VOICE_STATE_PARAMS_VERSION,      \
   UINT64_C(0), 0U, 0U}
static inline void dcc_current_user_voice_state_params_init(
    dcc_current_user_voice_state_params_t *value) {
  if (value != NULL) {
    const dcc_current_user_voice_state_params_t initialized =
        DCC_CURRENT_USER_VOICE_STATE_PARAMS_INIT;
    *value = initialized;
  }
}
static inline void
dcc_user_voice_state_params_init(dcc_user_voice_state_params_t *value) {
  if (value != NULL) {
    const dcc_user_voice_state_params_t initialized =
        DCC_USER_VOICE_STATE_PARAMS_INIT;
    *value = initialized;
  }
}

#ifdef __cplusplus
}
#endif

#endif
