#ifndef DCC_SUGAR_APP_EXTENSION_COMPONENTS_H
#define DCC_SUGAR_APP_EXTENSION_COMPONENTS_H

#include <dcc/sugar/app_extension/base.h>

#define DCC_EXTENSION_COMPONENT_ROUTE(type_, custom_id_, handler_, user_data_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_COMPONENT_ROUTE_ID(type_, custom_id_, handler_, user_data_, out_route_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_EXTENSION_COMPONENT_ROUTE_POLICY(type_, custom_id_, handler_, user_data_, policy_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_COMPONENT_ROUTE_ID_POLICY(type_, custom_id_, handler_, user_data_, out_route_, policy_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_BUTTON(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_BUTTON, (custom_id_), (handler_), (user_data_))
#define DCC_EXTENSION_BUTTON_PREFIX(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_BUTTON_PREFIX, (custom_id_prefix_), (handler_), (user_data_))
#define DCC_EXTENSION_SELECT(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_SELECT, (custom_id_), (handler_), (user_data_))
#define DCC_EXTENSION_SELECT_PREFIX(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_SELECT_PREFIX, (custom_id_prefix_), (handler_), (user_data_))
#define DCC_EXTENSION_MODAL(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_MODAL, (custom_id_), (handler_), (user_data_))
#define DCC_EXTENSION_MODAL_PREFIX(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_MODAL_PREFIX, (custom_id_prefix_), (handler_), (user_data_))

#define DCC_EXTENSION_BUTTON_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY(DCC_APP_EXTENSION_BUTTON, (custom_id_), (handler_), (user_data_), (policy_))
#define DCC_EXTENSION_BUTTON_PREFIX_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY( \
        DCC_APP_EXTENSION_BUTTON_PREFIX, \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_EXTENSION_SELECT_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY(DCC_APP_EXTENSION_SELECT, (custom_id_), (handler_), (user_data_), (policy_))
#define DCC_EXTENSION_SELECT_PREFIX_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY( \
        DCC_APP_EXTENSION_SELECT_PREFIX, \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_EXTENSION_MODAL_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY(DCC_APP_EXTENSION_MODAL, (custom_id_), (handler_), (user_data_), (policy_))
#define DCC_EXTENSION_MODAL_PREFIX_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY( \
        DCC_APP_EXTENSION_MODAL_PREFIX, \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_BUTTON_HANDLER(custom_id_, handler_) \
    DCC_EXTENSION_BUTTON((custom_id_), (handler_), NULL)
#define DCC_BUTTON_HANDLER_DATA(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_BUTTON((custom_id_), (handler_), (user_data_))
#define DCC_BUTTON_HANDLER_POLICY(custom_id_, handler_, policy_) \
    DCC_EXTENSION_BUTTON_POLICY((custom_id_), (handler_), NULL, (policy_))
#define DCC_BUTTON_HANDLER_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_BUTTON_POLICY((custom_id_), (handler_), (user_data_), (policy_))
#define DCC_BUTTON_PREFIX_HANDLER(custom_id_prefix_, handler_) \
    DCC_EXTENSION_BUTTON_PREFIX((custom_id_prefix_), (handler_), NULL)
#define DCC_BUTTON_PREFIX_HANDLER_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_BUTTON_PREFIX((custom_id_prefix_), (handler_), (user_data_))
#define DCC_BUTTON_PREFIX_HANDLER_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_EXTENSION_BUTTON_PREFIX_POLICY((custom_id_prefix_), (handler_), NULL, (policy_))
#define DCC_BUTTON_PREFIX_HANDLER_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_BUTTON_PREFIX_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))
#define DCC_SELECT_HANDLER(custom_id_, handler_) \
    DCC_EXTENSION_SELECT((custom_id_), (handler_), NULL)
#define DCC_SELECT_HANDLER_DATA(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_SELECT((custom_id_), (handler_), (user_data_))
#define DCC_SELECT_HANDLER_POLICY(custom_id_, handler_, policy_) \
    DCC_EXTENSION_SELECT_POLICY((custom_id_), (handler_), NULL, (policy_))
#define DCC_SELECT_HANDLER_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SELECT_POLICY((custom_id_), (handler_), (user_data_), (policy_))
#define DCC_SELECT_PREFIX_HANDLER(custom_id_prefix_, handler_) \
    DCC_EXTENSION_SELECT_PREFIX((custom_id_prefix_), (handler_), NULL)
#define DCC_SELECT_PREFIX_HANDLER_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_SELECT_PREFIX((custom_id_prefix_), (handler_), (user_data_))
#define DCC_SELECT_PREFIX_HANDLER_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_EXTENSION_SELECT_PREFIX_POLICY((custom_id_prefix_), (handler_), NULL, (policy_))
#define DCC_SELECT_PREFIX_HANDLER_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SELECT_PREFIX_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))
#define DCC_MODAL_HANDLER(custom_id_, handler_) \
    DCC_EXTENSION_MODAL((custom_id_), (handler_), NULL)
#define DCC_MODAL_HANDLER_DATA(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_MODAL((custom_id_), (handler_), (user_data_))
#define DCC_MODAL_HANDLER_POLICY(custom_id_, handler_, policy_) \
    DCC_EXTENSION_MODAL_POLICY((custom_id_), (handler_), NULL, (policy_))
#define DCC_MODAL_HANDLER_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_MODAL_POLICY((custom_id_), (handler_), (user_data_), (policy_))
#define DCC_MODAL_PREFIX_HANDLER(custom_id_prefix_, handler_) \
    DCC_EXTENSION_MODAL_PREFIX((custom_id_prefix_), (handler_), NULL)
#define DCC_MODAL_PREFIX_HANDLER_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_MODAL_PREFIX((custom_id_prefix_), (handler_), (user_data_))
#define DCC_MODAL_PREFIX_HANDLER_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_EXTENSION_MODAL_PREFIX_POLICY((custom_id_prefix_), (handler_), NULL, (policy_))
#define DCC_MODAL_PREFIX_HANDLER_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_MODAL_PREFIX_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_BUTTON(custom_id_, handler_) \
    DCC_BUTTON_HANDLER((custom_id_), (handler_))

#define DCC_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_BUTTON_HANDLER_DATA((custom_id_), (handler_), (user_data_))

#define DCC_BUTTON_POLICY(custom_id_, handler_, policy_) \
    DCC_BUTTON_HANDLER_POLICY((custom_id_), (handler_), (policy_))

#define DCC_BUTTON_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_BUTTON_HANDLER_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_BUTTON_PREFIX(custom_id_prefix_, handler_) \
    DCC_BUTTON_PREFIX_HANDLER((custom_id_prefix_), (handler_))

#define DCC_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_BUTTON_PREFIX_HANDLER_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_BUTTON_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_BUTTON_PREFIX_HANDLER_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_BUTTON_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_BUTTON_PREFIX_HANDLER_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_SELECT(custom_id_, handler_) \
    DCC_SELECT_HANDLER((custom_id_), (handler_))

#define DCC_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_SELECT_HANDLER_DATA((custom_id_), (handler_), (user_data_))

#define DCC_SELECT_POLICY(custom_id_, handler_, policy_) \
    DCC_SELECT_HANDLER_POLICY((custom_id_), (handler_), (policy_))

#define DCC_SELECT_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_SELECT_HANDLER_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_SELECT_PREFIX(custom_id_prefix_, handler_) \
    DCC_SELECT_PREFIX_HANDLER((custom_id_prefix_), (handler_))

#define DCC_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_SELECT_PREFIX_HANDLER_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_SELECT_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_SELECT_PREFIX_HANDLER_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_SELECT_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_SELECT_PREFIX_HANDLER_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_MODAL_ROUTE(custom_id_, handler_) \
    DCC_MODAL_HANDLER((custom_id_), (handler_))

#define DCC_MODAL_ROUTE_DATA(custom_id_, handler_, user_data_) \
    DCC_MODAL_HANDLER_DATA((custom_id_), (handler_), (user_data_))

#define DCC_MODAL_ROUTE_POLICY(custom_id_, handler_, policy_) \
    DCC_MODAL_HANDLER_POLICY((custom_id_), (handler_), (policy_))

#define DCC_MODAL_ROUTE_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_MODAL_HANDLER_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_MODAL_PREFIX_ROUTE(custom_id_prefix_, handler_) \
    DCC_MODAL_PREFIX_HANDLER((custom_id_prefix_), (handler_))

#define DCC_MODAL_PREFIX_ROUTE_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_MODAL_PREFIX_HANDLER_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_MODAL_PREFIX_ROUTE_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_MODAL_PREFIX_HANDLER_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_MODAL_PREFIX_ROUTE_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_MODAL_PREFIX_HANDLER_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_ON_BUTTON(custom_id_, handler_) \
    DCC_BUTTON((custom_id_), (handler_))

#define DCC_ON_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_BUTTON_DATA((custom_id_), (handler_), (user_data_))

#define DCC_ON_BUTTON_POLICY(custom_id_, handler_, policy_) \
    DCC_BUTTON_POLICY((custom_id_), (handler_), (policy_))

#define DCC_ON_BUTTON_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_BUTTON_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_ON_BUTTON_PREFIX(custom_id_prefix_, handler_) \
    DCC_BUTTON_PREFIX((custom_id_prefix_), (handler_))

#define DCC_ON_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_BUTTON_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_ON_BUTTON_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_BUTTON_PREFIX_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_ON_BUTTON_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_BUTTON_PREFIX_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_ON_SELECT(custom_id_, handler_) \
    DCC_SELECT((custom_id_), (handler_))

#define DCC_ON_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_SELECT_DATA((custom_id_), (handler_), (user_data_))

#define DCC_ON_SELECT_POLICY(custom_id_, handler_, policy_) \
    DCC_SELECT_POLICY((custom_id_), (handler_), (policy_))

#define DCC_ON_SELECT_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_SELECT_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_ON_SELECT_PREFIX(custom_id_prefix_, handler_) \
    DCC_SELECT_PREFIX((custom_id_prefix_), (handler_))

#define DCC_ON_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_SELECT_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_ON_SELECT_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_SELECT_PREFIX_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_ON_SELECT_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_SELECT_PREFIX_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_ON_MODAL(custom_id_, handler_) \
    DCC_MODAL_ROUTE((custom_id_), (handler_))

#define DCC_ON_MODAL_DATA(custom_id_, handler_, user_data_) \
    DCC_MODAL_ROUTE_DATA((custom_id_), (handler_), (user_data_))

#define DCC_ON_MODAL_POLICY(custom_id_, handler_, policy_) \
    DCC_MODAL_ROUTE_POLICY((custom_id_), (handler_), (policy_))

#define DCC_ON_MODAL_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_MODAL_ROUTE_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_ON_MODAL_PREFIX(custom_id_prefix_, handler_) \
    DCC_MODAL_PREFIX_ROUTE((custom_id_prefix_), (handler_))

#define DCC_ON_MODAL_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_MODAL_PREFIX_ROUTE_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_ON_MODAL_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_MODAL_PREFIX_ROUTE_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_ON_MODAL_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_MODAL_PREFIX_ROUTE_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

#define DCC_ON_BUTTON_NS(namespace_, action_, handler_) \
    DCC_ON_BUTTON(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_ON_BUTTON_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_ON_BUTTON_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_ON_BUTTON_NS_POLICY(namespace_, action_, handler_, policy_) \
    DCC_ON_BUTTON_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_ON_BUTTON_NS_DATA_POLICY(namespace_, action_, handler_, user_data_, policy_) \
    DCC_ON_BUTTON_DATA_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_), (policy_))
#define DCC_ON_BUTTON_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_ON_BUTTON_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_ON_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_ON_BUTTON_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))
#define DCC_ON_BUTTON_NS_PREFIX_POLICY(namespace_, action_prefix_, handler_, policy_) \
    DCC_ON_BUTTON_PREFIX_POLICY(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (policy_))
#define DCC_ON_BUTTON_NS_PREFIX_DATA_POLICY(namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_ON_BUTTON_PREFIX_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_ON_SELECT_NS(namespace_, action_, handler_) \
    DCC_ON_SELECT(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_ON_SELECT_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_ON_SELECT_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_ON_SELECT_NS_POLICY(namespace_, action_, handler_, policy_) \
    DCC_ON_SELECT_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_ON_SELECT_NS_DATA_POLICY(namespace_, action_, handler_, user_data_, policy_) \
    DCC_ON_SELECT_DATA_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_), (policy_))
#define DCC_ON_SELECT_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_ON_SELECT_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_ON_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_ON_SELECT_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))
#define DCC_ON_SELECT_NS_PREFIX_POLICY(namespace_, action_prefix_, handler_, policy_) \
    DCC_ON_SELECT_PREFIX_POLICY(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (policy_))
#define DCC_ON_SELECT_NS_PREFIX_DATA_POLICY(namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_ON_SELECT_PREFIX_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_ON_MODAL_NS(namespace_, action_, handler_) \
    DCC_ON_MODAL(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_ON_MODAL_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_ON_MODAL_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_ON_MODAL_NS_POLICY(namespace_, action_, handler_, policy_) \
    DCC_ON_MODAL_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_ON_MODAL_NS_DATA_POLICY(namespace_, action_, handler_, user_data_, policy_) \
    DCC_ON_MODAL_DATA_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_), (policy_))
#define DCC_ON_MODAL_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_ON_MODAL_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_ON_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_ON_MODAL_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))
#define DCC_ON_MODAL_NS_PREFIX_POLICY(namespace_, action_prefix_, handler_, policy_) \
    DCC_ON_MODAL_PREFIX_POLICY(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (policy_))
#define DCC_ON_MODAL_NS_PREFIX_DATA_POLICY(namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_ON_MODAL_PREFIX_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#endif /* DCC_SUGAR_APP_EXTENSION_COMPONENTS_H */
