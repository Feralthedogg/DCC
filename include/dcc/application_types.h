#ifndef DCC_APPLICATION_TYPES_H
#define DCC_APPLICATION_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_localization {
  const char *locale;
  const char *value;
} dcc_localization_t;

typedef enum dcc_application_integration_type {
  DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL = 0,
  DCC_APPLICATION_INTEGRATION_TYPE_USER_INSTALL = 1
} dcc_application_integration_type_t;

typedef enum dcc_interaction_context_type {
  DCC_INTERACTION_CONTEXT_GUILD = 0,
  DCC_INTERACTION_CONTEXT_BOT_DM = 1,
  DCC_INTERACTION_CONTEXT_PRIVATE_CHANNEL = 2
} dcc_interaction_context_type_t;

#ifdef __cplusplus
}
#endif

#endif
