#ifndef DCC_REST_TYPES_ONBOARDING_H
#define DCC_REST_TYPES_ONBOARDING_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_onboarding_mode {
    DCC_ONBOARDING_MODE_DEFAULT = 0,
    DCC_ONBOARDING_MODE_ADVANCED = 1
} dcc_onboarding_mode_t;

typedef enum dcc_onboarding_prompt_type {
    DCC_ONBOARDING_PROMPT_MULTIPLE_CHOICE = 0,
    DCC_ONBOARDING_PROMPT_DROPDOWN = 1
} dcc_onboarding_prompt_type_t;

typedef enum dcc_onboarding_prompt_flag {
    DCC_ONBOARDING_PROMPT_SINGLE_SELECT = 1U << 0,
    DCC_ONBOARDING_PROMPT_REQUIRED = 1U << 1,
    DCC_ONBOARDING_PROMPT_IN_ONBOARDING = 1U << 2
} dcc_onboarding_prompt_flag_t;

typedef struct dcc_onboarding_emoji_params {
    size_t size;
    const char *name;
    const char *image;
    const dcc_snowflake_t *roles;
    size_t role_count;
} dcc_onboarding_emoji_params_t;

typedef struct dcc_onboarding_prompt_option_params {
    size_t size;
    const dcc_onboarding_emoji_params_t *emoji;
    const char *title;
    const char *description;
    const dcc_snowflake_t *channel_ids;
    size_t channel_id_count;
    const dcc_snowflake_t *role_ids;
    size_t role_id_count;
} dcc_onboarding_prompt_option_params_t;

typedef struct dcc_onboarding_prompt_params {
    size_t size;
    dcc_onboarding_prompt_type_t type;
    const char *title;
    const dcc_onboarding_prompt_option_params_t *options;
    size_t option_count;
    uint8_t flags;
} dcc_onboarding_prompt_params_t;

typedef struct dcc_onboarding_params {
    size_t size;
    dcc_snowflake_t guild_id;
    const dcc_onboarding_prompt_params_t *prompts;
    size_t prompt_count;
    const dcc_snowflake_t *default_channel_ids;
    size_t default_channel_id_count;
    uint8_t enabled;
    dcc_onboarding_mode_t mode;
} dcc_onboarding_params_t;

#ifdef __cplusplus
}
#endif

#endif
