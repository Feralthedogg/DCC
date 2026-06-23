#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_NUMBERS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_NUMBERS_H

#include <dcc/sugar/app_typed/args_and_slash/slash_param_string.h>

#define DCC_SLASH_PARAM_BIND_INT(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_INTEGER, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_integer = (fallback_) \
    })
#define DCC_SLASH_PARAM_OPTION_INT(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_INTEGER(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_BIND_INT(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_INTEGER(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_OPTION_INT(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_INT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_INT(type_, field_, (name_), (description_), 1U, 0)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_INT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_INT(type_, field_, (name_), (description_), 1U, 0)
#define DCC_SLASH_PARAM_BIND_REQUIRED_INTEGER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_REQUIRED_INT(type_, field_, (name_), (description_))
#define DCC_SLASH_PARAM_OPTION_REQUIRED_INTEGER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_REQUIRED_INT(type_, field_, (name_), (description_))
#define DCC_SLASH_PARAM_BIND_INT_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_BIND_INT(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_INT_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .min_integer_value = (min_), \
        .max_integer_value = (max_), \
        .has_min_integer_value = 1U, \
        .has_max_integer_value = 1U \
    })
#define DCC_SLASH_PARAM_BIND_INTEGER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_BIND_INT_RANGE(type_, field_, (name_), (description_), (required_), (fallback_), (min_), (max_))
#define DCC_SLASH_PARAM_OPTION_INTEGER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_OPTION_INT_RANGE(type_, field_, (name_), (description_), (required_), (fallback_), (min_), (max_))

#define DCC_SLASH_PARAM_BIND_NUMBER(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_NUMBER, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_number = (fallback_) \
    })
#define DCC_SLASH_PARAM_OPTION_NUMBER(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_REQUIRED_NUMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_NUMBER(type_, field_, (name_), (description_), 1U, 0.0)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_NUMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_NUMBER(type_, field_, (name_), (description_), 1U, 0.0)
#define DCC_SLASH_PARAM_BIND_NUMBER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_BIND_NUMBER(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_NUMBER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .min_number_value = (min_), \
        .max_number_value = (max_), \
        .has_min_number_value = 1U, \
        .has_max_number_value = 1U \
    })

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_NUMBERS_H */
