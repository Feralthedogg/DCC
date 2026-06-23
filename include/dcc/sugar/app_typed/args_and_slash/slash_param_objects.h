#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_OBJECTS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_OBJECTS_H

#include <dcc/sugar/app_typed/args_and_slash/slash_param_numbers.h>

#define DCC_SLASH_PARAM_BIND_BOOL(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_BOOLEAN, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_boolean = (fallback_) ? 1U : 0U \
    })
#define DCC_SLASH_PARAM_OPTION_BOOL(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_BOOLEAN, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_BOOLEAN(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_BIND_BOOL(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_BOOLEAN(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_OPTION_BOOL(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_BOOL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_BOOL(type_, field_, (name_), (description_), 1U, 0U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_BOOL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_BOOL(type_, field_, (name_), (description_), 1U, 0U)
#define DCC_SLASH_PARAM_BIND_REQUIRED_BOOLEAN(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_REQUIRED_BOOL(type_, field_, (name_), (description_))
#define DCC_SLASH_PARAM_OPTION_REQUIRED_BOOLEAN(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_REQUIRED_BOOL(type_, field_, (name_), (description_))

#define DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, name_, description_, required_, bind_type_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = (bind_type_), \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U \
    })
#define DCC_SLASH_PARAM_OPTION_OBJECT(option_type_, name_, description_, required_) \
    ((dcc_application_command_option_builder_t){ \
        .type = (option_type_), \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_USER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_USER)
#define DCC_SLASH_PARAM_OPTION_USER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_USER, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_USER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_USER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_USER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_USER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_MEMBER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_MEMBER)
#define DCC_SLASH_PARAM_OPTION_MEMBER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_USER, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_MEMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_MEMBER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_MEMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_MEMBER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_ROLE(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_ROLE)
#define DCC_SLASH_PARAM_OPTION_ROLE(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_ROLE, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_ROLE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_ROLE(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_ROLE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_ROLE(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_CHANNEL(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_CHANNEL)
#define DCC_SLASH_PARAM_OPTION_CHANNEL(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_CHANNEL, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_CHANNEL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_CHANNEL(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_CHANNEL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_CHANNEL(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_CHANNEL_TYPES(type_, field_, name_, description_, required_, ...) \
    DCC_SLASH_PARAM_BIND_CHANNEL(type_, field_, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_OPTION_CHANNEL_TYPES(type_, field_, name_, description_, required_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_CHANNEL, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .channel_types = (uint32_t[]){ __VA_ARGS__ }, \
        .channel_types_count = sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t) \
    })
#define DCC_SLASH_PARAM_BIND_ATTACHMENT(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_ATTACHMENT)
#define DCC_SLASH_PARAM_OPTION_ATTACHMENT(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_ATTACHMENT, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_ATTACHMENT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_ATTACHMENT(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_ATTACHMENT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_ATTACHMENT(type_, field_, (name_), (description_), 1U)

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_OBJECTS_H */
