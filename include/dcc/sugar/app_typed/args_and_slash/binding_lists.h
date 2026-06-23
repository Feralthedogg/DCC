#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_BINDING_LISTS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_BINDING_LISTS_H

#include <dcc/sugar/app_typed/args_and_slash/base.h>

#define DCC_CTX_OPTION_FIELD_BINDINGS(...) \
    ((dcc_ctx_option_field_binding_list_t){ \
        .items = (dcc_ctx_option_field_binding_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_option_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_option_field_binding_t) \
    })

#define DCC_CTX_FORM_FIELD_BINDINGS(...) \
    ((dcc_ctx_form_field_binding_list_t){ \
        .items = (dcc_ctx_form_field_binding_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_form_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_form_field_binding_t) \
    })

#define DCC_CTX_FIELD_VALIDATORS(...) \
    ((dcc_ctx_field_validator_list_t){ \
        .items = (dcc_ctx_field_validator_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_field_validator_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_field_validator_t) \
    })

#define DCC_CTX_COMPONENT_FIELD_BINDINGS(...) \
    ((dcc_ctx_component_field_binding_list_t){ \
        .items = (dcc_ctx_component_field_binding_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_component_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_component_field_binding_t) \
    })

#define DCC_CTX_NO_OPTION_FIELD_BINDINGS() \
    ((dcc_ctx_option_field_binding_list_t){0})

#define DCC_CTX_NO_FORM_FIELD_BINDINGS() \
    ((dcc_ctx_form_field_binding_list_t){0})

#define DCC_CTX_NO_COMPONENT_FIELD_BINDINGS() \
    ((dcc_ctx_component_field_binding_list_t){0})

#define DCC_CTX_NO_FIELD_VALIDATORS() \
    ((dcc_ctx_field_validator_list_t){0})

#define DCC_ARGS(...) DCC_CTX_OPTION_FIELD_BINDINGS(__VA_ARGS__)
#define DCC_FORM_ARGS(...) DCC_CTX_FORM_FIELD_BINDINGS(__VA_ARGS__)
#define DCC_COMPONENT_ARGS(...) DCC_CTX_COMPONENT_FIELD_BINDINGS(__VA_ARGS__)
#define DCC_VALIDATORS(...) DCC_CTX_FIELD_VALIDATORS(__VA_ARGS__)
#define DCC_NO_ARGS() DCC_CTX_NO_OPTION_FIELD_BINDINGS()
#define DCC_NO_FORM_ARGS() DCC_CTX_NO_FORM_FIELD_BINDINGS()
#define DCC_NO_COMPONENT_ARGS() DCC_CTX_NO_COMPONENT_FIELD_BINDINGS()
#define DCC_NO_VALIDATORS() DCC_CTX_NO_FIELD_VALIDATORS()

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_BINDING_LISTS_H */
