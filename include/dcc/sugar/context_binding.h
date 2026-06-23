#ifndef DCC_SUGAR_CONTEXT_BINDING_H
#define DCC_SUGAR_CONTEXT_BINDING_H

#include <dcc/app.h>
#include <stddef.h>

#define DCC_CTX_OPTION_STRING(name_, out_, fallback_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_STRING, \
        .out = (out_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_OPTION_REQUIRED_STRING(name_, out_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_STRING, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_INT(name_, out_, fallback_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_INTEGER, \
        .out = (out_), \
        .fallback_integer = (fallback_) \
    })
#define DCC_CTX_OPTION_REQUIRED_INT(name_, out_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_INTEGER, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_NUMBER(name_, out_, fallback_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_NUMBER, \
        .out = (out_), \
        .fallback_number = (fallback_) \
    })
#define DCC_CTX_OPTION_REQUIRED_NUMBER(name_, out_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_NUMBER, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_BOOL(name_, out_, fallback_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_BOOLEAN, \
        .out = (out_), \
        .fallback_boolean = (fallback_) ? 1U : 0U \
    })
#define DCC_CTX_OPTION_REQUIRED_BOOL(name_, out_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_BOOLEAN, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_SNOWFLAKE(name_, out_, fallback_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_SNOWFLAKE, \
        .out = (out_), \
        .fallback_snowflake = (fallback_) \
    })
#define DCC_CTX_OPTION_REQUIRED_SNOWFLAKE(name_, out_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_SNOWFLAKE, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_BIND_OPTION_OBJECT(name_, out_, bind_type_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = (bind_type_), \
        .out = (out_) \
    })
#define DCC_CTX_BIND_REQUIRED_OPTION_OBJECT(name_, out_, bind_type_) \
    ((dcc_ctx_option_binding_t){ \
        .size = sizeof(dcc_ctx_option_binding_t), \
        .name = (name_), \
        .type = (bind_type_), \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_BIND_USER(name_, out_) \
    DCC_CTX_BIND_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_USER)
#define DCC_CTX_BIND_REQUIRED_USER(name_, out_) \
    DCC_CTX_BIND_REQUIRED_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_USER)
#define DCC_CTX_BIND_MEMBER(name_, out_) \
    DCC_CTX_BIND_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_MEMBER)
#define DCC_CTX_BIND_REQUIRED_MEMBER(name_, out_) \
    DCC_CTX_BIND_REQUIRED_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_MEMBER)
#define DCC_CTX_BIND_ROLE(name_, out_) \
    DCC_CTX_BIND_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_ROLE)
#define DCC_CTX_BIND_REQUIRED_ROLE(name_, out_) \
    DCC_CTX_BIND_REQUIRED_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_ROLE)
#define DCC_CTX_BIND_CHANNEL(name_, out_) \
    DCC_CTX_BIND_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_CHANNEL)
#define DCC_CTX_BIND_REQUIRED_CHANNEL(name_, out_) \
    DCC_CTX_BIND_REQUIRED_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_CHANNEL)
#define DCC_CTX_BIND_MESSAGE(name_, out_) \
    DCC_CTX_BIND_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_MESSAGE)
#define DCC_CTX_BIND_REQUIRED_MESSAGE(name_, out_) \
    DCC_CTX_BIND_REQUIRED_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_MESSAGE)
#define DCC_CTX_BIND_ATTACHMENT(name_, out_) \
    DCC_CTX_BIND_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_ATTACHMENT)
#define DCC_CTX_BIND_REQUIRED_ATTACHMENT(name_, out_) \
    DCC_CTX_BIND_REQUIRED_OPTION_OBJECT((name_), (out_), DCC_CTX_OPTION_BIND_ATTACHMENT)
#define DCC_CTX_BIND_OPTIONS(ctx_, ...) \
    dcc_ctx_bind_options( \
        (ctx_), \
        (dcc_ctx_option_binding_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_ctx_option_binding_t[]){ __VA_ARGS__ }) / sizeof(dcc_ctx_option_binding_t) \
    )

#define DCC_CTX_OPTION_FIELD_STRING(type_, field_, name_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_STRING, \
        .field_offset = offsetof(type_, field_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_OPTION_FIELD_REQUIRED_STRING(type_, field_, name_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_STRING, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_FIELD_INT(type_, field_, name_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_INTEGER, \
        .field_offset = offsetof(type_, field_), \
        .fallback_integer = (fallback_) \
    })
#define DCC_CTX_OPTION_FIELD_REQUIRED_INT(type_, field_, name_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_INTEGER, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_FIELD_NUMBER(type_, field_, name_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_NUMBER, \
        .field_offset = offsetof(type_, field_), \
        .fallback_number = (fallback_) \
    })
#define DCC_CTX_OPTION_FIELD_REQUIRED_NUMBER(type_, field_, name_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_NUMBER, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_FIELD_BOOL(type_, field_, name_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_BOOLEAN, \
        .field_offset = offsetof(type_, field_), \
        .fallback_boolean = (fallback_) ? 1U : 0U \
    })
#define DCC_CTX_OPTION_FIELD_REQUIRED_BOOL(type_, field_, name_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_BOOLEAN, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_FIELD_SNOWFLAKE(type_, field_, name_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_SNOWFLAKE, \
        .field_offset = offsetof(type_, field_), \
        .fallback_snowflake = (fallback_) \
    })
#define DCC_CTX_OPTION_FIELD_REQUIRED_SNOWFLAKE(type_, field_, name_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_SNOWFLAKE, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_FIELD_OBJECT(type_, field_, name_, bind_type_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = (bind_type_), \
        .field_offset = offsetof(type_, field_) \
    })
#define DCC_CTX_OPTION_FIELD_REQUIRED_OBJECT(type_, field_, name_, bind_type_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = (bind_type_), \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_OPTION_FIELD_USER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_USER)
#define DCC_CTX_OPTION_FIELD_REQUIRED_USER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_USER)
#define DCC_CTX_OPTION_FIELD_MEMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_MEMBER)
#define DCC_CTX_OPTION_FIELD_REQUIRED_MEMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_MEMBER)
#define DCC_CTX_OPTION_FIELD_ROLE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_ROLE)
#define DCC_CTX_OPTION_FIELD_REQUIRED_ROLE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_ROLE)
#define DCC_CTX_OPTION_FIELD_CHANNEL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_CHANNEL)
#define DCC_CTX_OPTION_FIELD_REQUIRED_CHANNEL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_CHANNEL)
#define DCC_CTX_OPTION_FIELD_MESSAGE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_MESSAGE)
#define DCC_CTX_OPTION_FIELD_REQUIRED_MESSAGE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_MESSAGE)
#define DCC_CTX_OPTION_FIELD_ATTACHMENT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_ATTACHMENT)
#define DCC_CTX_OPTION_FIELD_REQUIRED_ATTACHMENT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_OBJECT(type_, field_, (name_), DCC_CTX_OPTION_BIND_ATTACHMENT)
#define DCC_CTX_BIND_OPTION_FIELDS(ctx_, base_, ...) \
    dcc_ctx_bind_option_fields( \
        (ctx_), \
        (base_), \
        (dcc_ctx_option_field_binding_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_ctx_option_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_option_field_binding_t) \
    )

#define DCC_CTX_FORM_STRING(custom_id_, out_, fallback_) \
    ((dcc_ctx_form_binding_t){ \
        .size = sizeof(dcc_ctx_form_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_STRING, \
        .out = (out_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_FORM_REQUIRED_STRING(custom_id_, out_) \
    ((dcc_ctx_form_binding_t){ \
        .size = sizeof(dcc_ctx_form_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_STRING, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_FORM_BOOL(custom_id_, out_, fallback_) \
    ((dcc_ctx_form_binding_t){ \
        .size = sizeof(dcc_ctx_form_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_BOOLEAN, \
        .out = (out_), \
        .fallback_boolean = (fallback_) ? 1U : 0U \
    })
#define DCC_CTX_FORM_REQUIRED_BOOL(custom_id_, out_) \
    ((dcc_ctx_form_binding_t){ \
        .size = sizeof(dcc_ctx_form_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_BOOLEAN, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_FORM_VALUES(custom_id_, out_, out_count_, fallback_values_, fallback_count_) \
    ((dcc_ctx_form_binding_t){ \
        .size = sizeof(dcc_ctx_form_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_VALUES, \
        .out = (out_), \
        .out_count = (out_count_), \
        .fallback_values = (fallback_values_), \
        .fallback_values_count = (fallback_count_) \
    })
#define DCC_CTX_FORM_REQUIRED_VALUES(custom_id_, out_, out_count_) \
    ((dcc_ctx_form_binding_t){ \
        .size = sizeof(dcc_ctx_form_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_VALUES, \
        .out = (out_), \
        .out_count = (out_count_), \
        .required = 1U \
    })
#define DCC_CTX_BIND_FORM(ctx_, ...) \
    dcc_ctx_bind_form( \
        (ctx_), \
        (dcc_ctx_form_binding_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_ctx_form_binding_t[]){ __VA_ARGS__ }) / sizeof(dcc_ctx_form_binding_t) \
    )

#define DCC_CTX_FORM_FIELD_STRING(type_, field_, custom_id_, fallback_) \
    ((dcc_ctx_form_field_binding_t){ \
        .size = sizeof(dcc_ctx_form_field_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_STRING, \
        .field_offset = offsetof(type_, field_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_FORM_FIELD_REQUIRED_STRING(type_, field_, custom_id_) \
    ((dcc_ctx_form_field_binding_t){ \
        .size = sizeof(dcc_ctx_form_field_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_STRING, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_FORM_FIELD_BOOL(type_, field_, custom_id_, fallback_) \
    ((dcc_ctx_form_field_binding_t){ \
        .size = sizeof(dcc_ctx_form_field_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_BOOLEAN, \
        .field_offset = offsetof(type_, field_), \
        .fallback_boolean = (fallback_) ? 1U : 0U \
    })
#define DCC_CTX_FORM_FIELD_REQUIRED_BOOL(type_, field_, custom_id_) \
    ((dcc_ctx_form_field_binding_t){ \
        .size = sizeof(dcc_ctx_form_field_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_BOOLEAN, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_FORM_FIELD_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_) \
    ((dcc_ctx_form_field_binding_t){ \
        .size = sizeof(dcc_ctx_form_field_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_VALUES, \
        .field_offset = offsetof(type_, field_), \
        .count_offset = offsetof(type_, count_field_), \
        .has_count_offset = 1U, \
        .fallback_values = (fallback_values_), \
        .fallback_values_count = (fallback_count_) \
    })
#define DCC_CTX_FORM_FIELD_REQUIRED_VALUES(type_, field_, count_field_, custom_id_) \
    ((dcc_ctx_form_field_binding_t){ \
        .size = sizeof(dcc_ctx_form_field_binding_t), \
        .custom_id = (custom_id_), \
        .type = DCC_CTX_FORM_BIND_VALUES, \
        .field_offset = offsetof(type_, field_), \
        .count_offset = offsetof(type_, count_field_), \
        .has_count_offset = 1U, \
        .required = 1U \
    })
#define DCC_CTX_BIND_FORM_FIELDS(ctx_, base_, ...) \
    dcc_ctx_bind_form_fields( \
        (ctx_), \
        (base_), \
        (dcc_ctx_form_field_binding_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_ctx_form_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_form_field_binding_t) \
    )

#endif
