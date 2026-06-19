#ifndef DCC_CACHE_VALUE_INTERNAL_H
#define DCC_CACHE_VALUE_INTERNAL_H

#include <dcc/dcc.h>

typedef void (*dcc_cache_value_free_fn)(void *value);

static inline void dcc_cache_free_guild_value(void *value) {
    dcc_guild_free((dcc_guild_t *)value);
}

static inline void dcc_cache_free_channel_value(void *value) {
    dcc_channel_free((dcc_channel_t *)value);
}

static inline void dcc_cache_free_role_value(void *value) {
    dcc_role_free((dcc_role_t *)value);
}

static inline void dcc_cache_free_member_value(void *value) {
    dcc_member_free((dcc_member_t *)value);
}

static inline void dcc_cache_free_user_value(void *value) {
    dcc_user_free((dcc_user_t *)value);
}

static inline void dcc_cache_free_message_value(void *value) {
    dcc_message_free((dcc_message_t *)value);
}

static inline void dcc_cache_free_voice_state_value(void *value) {
    dcc_voice_state_free((dcc_voice_state_t *)value);
}

#endif
