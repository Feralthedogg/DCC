#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_STRING_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_STRING_H

#include <dcc/sugar/app_typed/args_and_slash/slash_param_core.h>

#define DCC_SLASH_PARAM_BIND_STRING(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_STRING, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_string = (fallback_) \
    })
#define DCC_SLASH_PARAM_OPTION_STRING(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_REQUIRED_STRING(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_STRING(type_, field_, (name_), (description_), 1U, NULL)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_STRING(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_STRING(type_, field_, (name_), (description_), 1U, NULL)
#define DCC_SLASH_PARAM_BIND_STRING_AUTOCOMPLETE(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_BIND_STRING(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_STRING_AUTOCOMPLETE(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .autocomplete = 1U, \
        .has_autocomplete = 1U \
    })
#define DCC_SLASH_PARAM_BIND_REQUIRED_STRING_AUTOCOMPLETE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_STRING_AUTOCOMPLETE(type_, field_, (name_), (description_), 1U, NULL)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_STRING_AUTOCOMPLETE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_STRING_AUTOCOMPLETE(type_, field_, (name_), (description_), 1U, NULL)

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_STRING_H */
