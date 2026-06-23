#ifndef DCC_SUGAR_COMPONENT_BINDING_H
#define DCC_SUGAR_COMPONENT_BINDING_H

#include <dcc/app.h>
#include <stddef.h>

#define DCC_CTX_COMPONENT_CUSTOM_ID(out_, fallback_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID, \
        .out = (out_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_COMPONENT_REQUIRED_CUSTOM_ID(out_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_CUSTOM_ID_SUFFIX(out_, custom_id_prefix_, fallback_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX, \
        .custom_id_prefix = (custom_id_prefix_), \
        .out = (out_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_COMPONENT_REQUIRED_CUSTOM_ID_SUFFIX(out_, custom_id_prefix_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX, \
        .custom_id_prefix = (custom_id_prefix_), \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_VALUES(out_, out_count_, fallback_values_, fallback_count_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_VALUES, \
        .out = (out_), \
        .out_count = (out_count_), \
        .fallback_values = (fallback_values_), \
        .fallback_values_count = (fallback_count_) \
    })
#define DCC_CTX_COMPONENT_REQUIRED_VALUES(out_, out_count_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_VALUES, \
        .out = (out_), \
        .out_count = (out_count_), \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_FIRST_VALUE(out_, fallback_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_FIRST_VALUE, \
        .out = (out_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_COMPONENT_REQUIRED_FIRST_VALUE(out_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_FIRST_VALUE, \
        .out = (out_), \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_TYPE(out_) \
    ((dcc_ctx_component_binding_t){ \
        .size = sizeof(dcc_ctx_component_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE, \
        .out = (out_) \
    })
#define DCC_CTX_BIND_COMPONENT(ctx_, ...) \
    dcc_ctx_bind_component( \
        (ctx_), \
        (dcc_ctx_component_binding_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_ctx_component_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_component_binding_t) \
    )

#define DCC_CTX_COMPONENT_FIELD_CUSTOM_ID(type_, field_, fallback_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID, \
        .field_offset = offsetof(type_, field_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID(type_, field_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_FIELD_CUSTOM_ID_SUFFIX(type_, field_, custom_id_prefix_, fallback_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX, \
        .custom_id_prefix = (custom_id_prefix_), \
        .field_offset = offsetof(type_, field_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, custom_id_prefix_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX, \
        .custom_id_prefix = (custom_id_prefix_), \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_FIELD_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_VALUES, \
        .field_offset = offsetof(type_, field_), \
        .count_offset = offsetof(type_, count_field_), \
        .has_count_offset = 1U, \
        .fallback_values = (fallback_values_), \
        .fallback_values_count = (fallback_count_) \
    })
#define DCC_CTX_COMPONENT_FIELD_REQUIRED_VALUES(type_, field_, count_field_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_VALUES, \
        .field_offset = offsetof(type_, field_), \
        .count_offset = offsetof(type_, count_field_), \
        .has_count_offset = 1U, \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_FIELD_FIRST_VALUE(type_, field_, fallback_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_FIRST_VALUE, \
        .field_offset = offsetof(type_, field_), \
        .fallback_string = (fallback_) \
    })
#define DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(type_, field_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_FIRST_VALUE, \
        .field_offset = offsetof(type_, field_), \
        .required = 1U \
    })
#define DCC_CTX_COMPONENT_FIELD_TYPE(type_, field_) \
    ((dcc_ctx_component_field_binding_t){ \
        .size = sizeof(dcc_ctx_component_field_binding_t), \
        .type = DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE, \
        .field_offset = offsetof(type_, field_) \
    })
#define DCC_CTX_BIND_COMPONENT_FIELDS(ctx_, base_, ...) \
    dcc_ctx_bind_component_fields( \
        (ctx_), \
        (base_), \
        (dcc_ctx_component_field_binding_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_ctx_component_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_component_field_binding_t) \
    )

#endif
