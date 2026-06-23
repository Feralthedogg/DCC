#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_BASE_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_BASE_H

#include <dcc/app.h>
#include <dcc/application_command.h>
#include <dcc/sugar/context_binding.h>
#include <dcc/sugar/route_policy.h>
#include <stddef.h>
#include <stdint.h>

/* Included by <dcc/sugar/app_typed.h>. */
typedef struct dcc_no_args {
    unsigned char reserved;
} dcc_no_args_t;

typedef dcc_app_typed_slash_command_t dcc_command_route_t;
typedef dcc_app_typed_slash_command_t dcc_slash_route_t;
typedef dcc_app_typed_subcommand_t dcc_subcommand_route_t;
typedef dcc_app_typed_autocomplete_t dcc_autocomplete_route_t;
typedef dcc_app_typed_modal_t dcc_modal_route_t;
typedef dcc_app_typed_component_t dcc_component_route_t;
typedef dcc_app_typed_component_t dcc_button_route_t;
typedef dcc_app_typed_component_t dcc_select_route_t;
typedef dcc_app_modal_flow_t dcc_modal_flow_t;

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_BASE_H */
