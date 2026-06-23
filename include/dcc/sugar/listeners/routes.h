#ifndef DCC_SUGAR_LISTENERS_ROUTES_H
#define DCC_SUGAR_LISTENERS_ROUTES_H

#include <dcc/sugar/listeners/decorated.h>

#define DCC_ROUTES(...) DCC_LISTENERS(__VA_ARGS__)
#define DCC_ROUTE_TABLE(...) DCC_ROUTES(__VA_ARGS__)
#define DCC_ROUTE_LIST(...) DCC_ROUTES(__VA_ARGS__)

#define DCC_ROUTES_REGISTER(app_, ...) DCC_LISTENERS_REGISTER((app_), __VA_ARGS__)
#define DCC_USE_ROUTES(app_, ...) DCC_ROUTES_REGISTER((app_), __VA_ARGS__)

#define DCC_DECLARE_ROUTES(name_) DCC_DECLARE_LISTENERS(name_)
#define DCC_DEFINE_ROUTES(name_, ...) DCC_DEFINE_LISTENERS(name_, __VA_ARGS__)
#define DCC_DEFINE_PUBLIC_ROUTES(name_, ...) DCC_DEFINE_PUBLIC_LISTENERS(name_, __VA_ARGS__)
#define DCC_LOAD_ROUTES(app_, name_) DCC_LOAD_LISTENERS((app_), name_)
#define DCC_LOAD_ROUTES_DATA(app_, name_, user_data_) DCC_LOAD_LISTENERS_DATA((app_), name_, (user_data_))
#define DCC_ROUTES_MODULE(name_, user_data_) DCC_LISTENERS_MODULE(name_, (user_data_))
#define DCC_ROUTE_MODULE(name_, user_data_) DCC_ROUTES_MODULE(name_, (user_data_))

#define DCC_ROUTE_DECORATED(handler_) DCC_LISTEN_DECORATED_SLASH(handler_)
#define DCC_ROUTE_DECORATED_COMMAND(handler_) DCC_LISTEN_DECORATED_COMMAND(handler_)
#define DCC_ROUTE_DECORATED_SLASH(handler_) DCC_LISTEN_DECORATED_SLASH(handler_)
#define DCC_ROUTE_DECORATED_SUBCOMMAND(handler_) DCC_LISTEN_DECORATED_SUBCOMMAND(handler_)
#define DCC_ROUTE_DECORATED_AUTOCOMPLETE(handler_) DCC_LISTEN_DECORATED_AUTOCOMPLETE(handler_)
#define DCC_ROUTE_DECORATED_USER_CONTEXT_MENU(handler_) DCC_LISTEN_DECORATED_USER_CONTEXT_MENU(handler_)
#define DCC_ROUTE_DECORATED_MESSAGE_CONTEXT_MENU(handler_) DCC_LISTEN_DECORATED_MESSAGE_CONTEXT_MENU(handler_)
#define DCC_ROUTE_DECORATED_COMPONENT(handler_) DCC_LISTEN_DECORATED_COMPONENT(handler_)
#define DCC_ROUTE_DECORATED_BUTTON(handler_) DCC_LISTEN_DECORATED_BUTTON(handler_)
#define DCC_ROUTE_DECORATED_SELECT(handler_) DCC_LISTEN_DECORATED_SELECT(handler_)
#define DCC_ROUTE_DECORATED_MODAL(handler_) DCC_LISTEN_DECORATED_MODAL(handler_)
#define DCC_ROUTE_DECORATED_EVENT(handler_) DCC_LISTEN_DECORATED_EVENT(handler_)
#define DCC_ROUTE_DECORATED_READY(handler_) DCC_LISTEN_DECORATED_READY(handler_)
#define DCC_ROUTE_DECORATED_PREFIX_COMMAND(handler_) DCC_LISTEN_DECORATED_PREFIX_COMMAND(handler_)
#define DCC_ROUTE_DECORATED_TASK(handler_) DCC_LISTEN_DECORATED_TASK(handler_)
#define DCC_ROUTE_DECORATED_TYPED_COMMAND(handler_) DCC_LISTEN_DECORATED_TYPED_COMMAND(handler_)
#define DCC_ROUTE_DECORATED_TYPED_SUBCOMMAND(handler_) DCC_LISTEN_DECORATED_TYPED_SUBCOMMAND(handler_)
#define DCC_ROUTE_DECORATED_TYPED_AUTOCOMPLETE(handler_) DCC_LISTEN_DECORATED_TYPED_AUTOCOMPLETE(handler_)
#define DCC_ROUTE_DECORATED_TYPED_MODAL(handler_) DCC_LISTEN_DECORATED_TYPED_MODAL(handler_)
#define DCC_ROUTE_DECORATED_TYPED_BUTTON(handler_) DCC_LISTEN_DECORATED_TYPED_BUTTON(handler_)
#define DCC_ROUTE_DECORATED_TYPED_SELECT(handler_) DCC_LISTEN_DECORATED_TYPED_SELECT(handler_)

#define DCC_ROUTE_COMMAND(name_, description_, handler_) DCC_LISTEN_COMMAND((name_), (description_), (handler_))
#define DCC_ROUTE_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_COMMAND_DATA((name_), (description_), (handler_), (user_data_))
#define DCC_ROUTE_COMMAND_GUARDED(name_, description_, handler_, ...) \
    DCC_LISTEN_SLASH_GUARDED((name_), (description_), (handler_), __VA_ARGS__)
#define DCC_ROUTE_COMMAND_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_LISTEN_SLASH_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_ROUTE_COMMAND_BUILDER_VALUE(builder_, handler_) \
    DCC_LISTEN_COMMAND_BUILDER_VALUE((builder_), (handler_))
#define DCC_ROUTE_COMMAND_BUILDER_VALUE_DATA(builder_, handler_, user_data_) \
    DCC_LISTEN_COMMAND_BUILDER_VALUE_DATA((builder_), (handler_), (user_data_))
#define DCC_ROUTE_COMMAND_BUILDER_VALUE_POLICY(builder_, handler_, policy_) \
    DCC_LISTEN_COMMAND_BUILDER_VALUE_POLICY((builder_), (handler_), (policy_))
#define DCC_ROUTE_COMMAND_BUILDER_VALUE_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_LISTEN_COMMAND_BUILDER_VALUE_DATA_POLICY((builder_), (handler_), (user_data_), (policy_))
#define DCC_ROUTE_SLASH(name_, description_, handler_) DCC_LISTEN_SLASH((name_), (description_), (handler_))
#define DCC_ROUTE_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_SLASH_DATA((name_), (description_), (handler_), (user_data_))
#define DCC_ROUTE_SLASH_GUARDED(name_, description_, handler_, ...) \
    DCC_LISTEN_SLASH_GUARDED((name_), (description_), (handler_), __VA_ARGS__)
#define DCC_ROUTE_SLASH_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_LISTEN_SLASH_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_ROUTE_COMMAND_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_COMMAND_PARAMS((name_), (description_), args_type_, (handler_), params_)
#define DCC_ROUTE_COMMAND_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_COMMAND_PARAMS_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_COMMAND_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_COMMAND_PARAMS_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_COMMAND_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_COMMAND_PARAMS_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ROUTE_COMMAND_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_COMMAND_PARAMS_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_COMMAND_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_COMMAND_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_COMMAND_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_COMMAND_FN(handler_, (description_), args_type_, params_)
#define DCC_ROUTE_COMMAND_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_COMMAND_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_ROUTE_COMMAND_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_COMMAND_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_ROUTE_COMMAND_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_LISTEN_COMMAND_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_))
#define DCC_ROUTE_COMMAND_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_ROUTE_COMMAND_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)

#define DCC_ROUTE_TYPED_COMMAND(route_) DCC_LISTEN_TYPED_SLASH(route_)
#define DCC_ROUTE_TYPED_SLASH(route_) DCC_LISTEN_TYPED_SLASH(route_)
#define DCC_ROUTE_TYPED_SUBCOMMAND(route_) DCC_LISTEN_TYPED_SUBCOMMAND(route_)
#define DCC_ROUTE_TYPED_AUTOCOMPLETE(route_) DCC_LISTEN_TYPED_AUTOCOMPLETE(route_)
#define DCC_ROUTE_TYPED_MODAL(route_) DCC_LISTEN_TYPED_MODAL(route_)
#define DCC_ROUTE_TYPED_BUTTON(route_) DCC_LISTEN_TYPED_BUTTON(route_)
#define DCC_ROUTE_TYPED_SELECT(route_) DCC_LISTEN_TYPED_SELECT(route_)

#define DCC_ROUTE_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_LISTEN_SUBCOMMAND((command_name_), (subcommand_path_), (handler_))
#define DCC_ROUTE_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_LISTEN_SUBCOMMAND_DATA((command_name_), (subcommand_path_), (handler_), (user_data_))
#define DCC_ROUTE_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_SUBCOMMAND_PARAMS((command_name_), (subcommand_path_), args_type_, (handler_), params_)
#define DCC_ROUTE_SUBCOMMAND_PARAMS_DATA(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SUBCOMMAND_PARAMS_DATA( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_ROUTE_SUBCOMMAND_PARAMS_GUARDED(command_name_, subcommand_path_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_PARAMS_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_PARAMS_DATA_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SUBCOMMAND_PARAMS_DATA_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS((command_name_), group_, subcommand_, args_type_, (handler_), params_)
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_FN(command_name_, handler_, args_type_, params_) \
    DCC_LISTEN_SUBCOMMAND_FN((command_name_), handler_, args_type_, params_)
#define DCC_ROUTE_SUBCOMMAND_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_SUBCOMMAND_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_)
#define DCC_ROUTE_SUBCOMMAND_FN_GUARDED(command_name_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_FN_GUARDED((command_name_), handler_, args_type_, params_, __VA_ARGS__)
#define DCC_ROUTE_SUBCOMMAND_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_FN_DATA_GUARDED( \
        (command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_FN(command_name_, group_, handler_, args_type_, params_) \
    DCC_LISTEN_SUBCOMMAND_IN_FN((command_name_), group_, handler_, args_type_, params_)
#define DCC_ROUTE_SUBCOMMAND_IN_FN_DATA(command_name_, group_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_SUBCOMMAND_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_)
#define DCC_ROUTE_SUBCOMMAND_IN_FN_GUARDED(command_name_, group_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_IN_FN_GUARDED((command_name_), group_, handler_, args_type_, params_, __VA_ARGS__)
#define DCC_ROUTE_SUBCOMMAND_IN_FN_DATA_GUARDED(command_name_, group_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_IN_FN_DATA_GUARDED( \
        (command_name_), group_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ROUTE_AUTOCOMPLETE(command_name_, handler_) DCC_LISTEN_AUTOCOMPLETE((command_name_), (handler_))
#define DCC_ROUTE_AUTOCOMPLETE_DATA(command_name_, handler_, user_data_) \
    DCC_LISTEN_AUTOCOMPLETE_DATA((command_name_), (handler_), (user_data_))
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS(command_name_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE(DCC_AUTOCOMPLETE_PARAMS((command_name_), args_type_, (handler_), params_))
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS_DATA(command_name_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_AUTOCOMPLETE_PARAMS_DATA((command_name_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS_GUARDED(command_name_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED((command_name_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS_DATA_GUARDED(command_name_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED( \
            (command_name_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )

#define DCC_ROUTE_USER_CONTEXT_MENU(name_, handler_) DCC_LISTEN_USER_CONTEXT_MENU((name_), (handler_))
#define DCC_ROUTE_USER_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_USER_CONTEXT_MENU_DATA((name_), (handler_), (user_data_))
#define DCC_ROUTE_MESSAGE_CONTEXT_MENU(name_, handler_) DCC_LISTEN_MESSAGE_CONTEXT_MENU((name_), (handler_))
#define DCC_ROUTE_MESSAGE_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_CONTEXT_MENU_DATA((name_), (handler_), (user_data_))

#define DCC_ROUTE_BUTTON(custom_id_, handler_) DCC_LISTEN_BUTTON((custom_id_), (handler_))
#define DCC_ROUTE_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_DATA((custom_id_), (handler_), (user_data_))
#define DCC_ROUTE_BUTTON_PREFIX(custom_id_prefix_, handler_) DCC_LISTEN_BUTTON_PREFIX((custom_id_prefix_), (handler_))
#define DCC_ROUTE_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))
#define DCC_ROUTE_BUTTON_NS(namespace_, action_, handler_) DCC_LISTEN_BUTTON_NS(namespace_, action_, (handler_))
#define DCC_ROUTE_BUTTON_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_NS_DATA(namespace_, action_, (handler_), (user_data_))
#define DCC_ROUTE_BUTTON_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_BUTTON_NS_PREFIX(namespace_, action_prefix_, (handler_))
#define DCC_ROUTE_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_))

#define DCC_ROUTE_BUTTON_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_PARAMS((custom_id_), args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_BUTTON_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_BUTTON_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_BUTTON_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_LISTEN_BUTTON_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, (validators_))
#define DCC_ROUTE_BUTTON_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_BUTTON_PARAMS_GUARDED((custom_id_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_BUTTON_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_BUTTON_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__)
#define DCC_ROUTE_BUTTON_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA_GUARDED( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_BUTTON_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_BUTTON_NS_PARAMS_DATA_GUARDED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ROUTE_SELECT(custom_id_, handler_) DCC_LISTEN_SELECT((custom_id_), (handler_))
#define DCC_ROUTE_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_SELECT_DATA((custom_id_), (handler_), (user_data_))
#define DCC_ROUTE_SELECT_PREFIX(custom_id_prefix_, handler_) DCC_LISTEN_SELECT_PREFIX((custom_id_prefix_), (handler_))
#define DCC_ROUTE_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_SELECT_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))
#define DCC_ROUTE_SELECT_NS(namespace_, action_, handler_) DCC_LISTEN_SELECT_NS(namespace_, action_, (handler_))
#define DCC_ROUTE_SELECT_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_SELECT_NS_DATA(namespace_, action_, (handler_), (user_data_))
#define DCC_ROUTE_SELECT_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_SELECT_NS_PREFIX(namespace_, action_prefix_, (handler_))
#define DCC_ROUTE_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_))
#define DCC_ROUTE_SELECT_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_PARAMS((custom_id_), args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SELECT_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_SELECT_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_SELECT_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_LISTEN_SELECT_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, (validators_))
#define DCC_ROUTE_SELECT_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_SELECT_PARAMS_GUARDED((custom_id_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_SELECT_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_SELECT_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__)
#define DCC_ROUTE_SELECT_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA_GUARDED( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SELECT_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_SELECT_NS_PARAMS_DATA_GUARDED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ROUTE_MODAL(custom_id_, handler_) DCC_LISTEN_MODAL((custom_id_), (handler_))
#define DCC_ROUTE_MODAL_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_MODAL_DATA((custom_id_), (handler_), (user_data_))
#define DCC_ROUTE_MODAL_PREFIX(custom_id_prefix_, handler_) DCC_LISTEN_MODAL_PREFIX((custom_id_prefix_), (handler_))
#define DCC_ROUTE_MODAL_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_MODAL_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))
#define DCC_ROUTE_MODAL_NS(namespace_, action_, handler_) DCC_LISTEN_MODAL_NS(namespace_, action_, (handler_))
#define DCC_ROUTE_MODAL_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_MODAL_NS_DATA(namespace_, action_, (handler_), (user_data_))
#define DCC_ROUTE_MODAL_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_MODAL_NS_PREFIX(namespace_, action_prefix_, (handler_))
#define DCC_ROUTE_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_))
#define DCC_ROUTE_MODAL_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_PARAMS((custom_id_), args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_MODAL_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_MODAL_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_MODAL_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_LISTEN_MODAL_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, (validators_))
#define DCC_ROUTE_MODAL_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_PARAMS_GUARDED((custom_id_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_MODAL_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__)
#define DCC_ROUTE_MODAL_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA_GUARDED( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_MODAL_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_NS_PARAMS_DATA_GUARDED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_MODAL_FLOW(flow_) DCC_LISTEN_MODAL_FLOW(flow_)
#define DCC_ROUTE_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, (title_), args_type_, (handler_), params_, __VA_ARGS__)

#define DCC_ROUTE_EVENT(type_, handler_) DCC_LISTEN_EVENT((type_), (handler_))
#define DCC_ROUTE_EVENT_DATA(type_, handler_, user_data_) DCC_LISTEN_EVENT_DATA((type_), (handler_), (user_data_))
#define DCC_ROUTE_READY(handler_) DCC_LISTEN_READY(handler_)
#define DCC_ROUTE_READY_DATA(handler_, user_data_) DCC_LISTEN_READY_DATA(handler_, (user_data_))
#define DCC_ROUTE_READY_ONCE(handler_) DCC_LISTEN_READY_ONCE(handler_)
#define DCC_ROUTE_READY_ONCE_DATA(handler_, user_data_) DCC_LISTEN_READY_ONCE_DATA(handler_, (user_data_))
#define DCC_ROUTE_MESSAGE_CREATE(handler_) DCC_LISTEN_MESSAGE_CREATE(handler_)
#define DCC_ROUTE_MESSAGE_CREATE_DATA(handler_, user_data_) DCC_LISTEN_MESSAGE_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_JOIN(handler_) DCC_LISTEN_MEMBER_JOIN(handler_)
#define DCC_ROUTE_MEMBER_JOIN_DATA(handler_, user_data_) DCC_LISTEN_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_ADD(handler_) DCC_ROUTE_MEMBER_JOIN(handler_)
#define DCC_ROUTE_MEMBER_ADD_DATA(handler_, user_data_) DCC_ROUTE_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_UPDATE(handler_) DCC_LISTEN_MEMBER_UPDATE(handler_)
#define DCC_ROUTE_MEMBER_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_MEMBER_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_REMOVE(handler_) DCC_LISTEN_MEMBER_REMOVE(handler_)
#define DCC_ROUTE_MEMBER_REMOVE_DATA(handler_, user_data_) DCC_LISTEN_MEMBER_REMOVE_DATA(handler_, (user_data_))
#define DCC_ROUTE_GUILD_CREATE(handler_) DCC_LISTEN_GUILD_CREATE(handler_)
#define DCC_ROUTE_GUILD_CREATE_DATA(handler_, user_data_) DCC_LISTEN_GUILD_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_GUILD_UPDATE(handler_) DCC_LISTEN_GUILD_UPDATE(handler_)
#define DCC_ROUTE_GUILD_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_GUILD_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_GUILD_DELETE(handler_) DCC_LISTEN_GUILD_DELETE(handler_)
#define DCC_ROUTE_GUILD_DELETE_DATA(handler_, user_data_) DCC_LISTEN_GUILD_DELETE_DATA(handler_, (user_data_))
#define DCC_ROUTE_CHANNEL_CREATE(handler_) DCC_LISTEN_CHANNEL_CREATE(handler_)
#define DCC_ROUTE_CHANNEL_CREATE_DATA(handler_, user_data_) DCC_LISTEN_CHANNEL_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_CHANNEL_UPDATE(handler_) DCC_LISTEN_CHANNEL_UPDATE(handler_)
#define DCC_ROUTE_CHANNEL_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_CHANNEL_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_CHANNEL_DELETE(handler_) DCC_LISTEN_CHANNEL_DELETE(handler_)
#define DCC_ROUTE_CHANNEL_DELETE_DATA(handler_, user_data_) DCC_LISTEN_CHANNEL_DELETE_DATA(handler_, (user_data_))
#define DCC_ROUTE_THREAD_CREATE(handler_) DCC_LISTEN_THREAD_CREATE(handler_)
#define DCC_ROUTE_THREAD_CREATE_DATA(handler_, user_data_) DCC_LISTEN_THREAD_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_THREAD_UPDATE(handler_) DCC_LISTEN_THREAD_UPDATE(handler_)
#define DCC_ROUTE_THREAD_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_THREAD_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_THREAD_DELETE(handler_) DCC_LISTEN_THREAD_DELETE(handler_)
#define DCC_ROUTE_THREAD_DELETE_DATA(handler_, user_data_) DCC_LISTEN_THREAD_DELETE_DATA(handler_, (user_data_))
#define DCC_ROUTE_REACTION_ADD(handler_) DCC_LISTEN_REACTION_ADD(handler_)
#define DCC_ROUTE_REACTION_ADD_DATA(handler_, user_data_) DCC_LISTEN_REACTION_ADD_DATA(handler_, (user_data_))
#define DCC_ROUTE_REACTION_REMOVE(handler_) DCC_LISTEN_REACTION_REMOVE(handler_)
#define DCC_ROUTE_REACTION_REMOVE_DATA(handler_, user_data_) DCC_LISTEN_REACTION_REMOVE_DATA(handler_, (user_data_))
#define DCC_ROUTE_VOICE_STATE_UPDATE(handler_) DCC_LISTEN_VOICE_STATE_UPDATE(handler_)
#define DCC_ROUTE_VOICE_STATE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_VOICE_STATE_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_TYPING_START(handler_) DCC_LISTEN_TYPING_START(handler_)
#define DCC_ROUTE_TYPING_START_DATA(handler_, user_data_) DCC_LISTEN_TYPING_START_DATA(handler_, (user_data_))
#define DCC_ROUTE_PRESENCE_UPDATE(handler_) DCC_LISTEN_PRESENCE_UPDATE(handler_)
#define DCC_ROUTE_PRESENCE_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_PRESENCE_UPDATE_DATA(handler_, (user_data_))

#define DCC_ROUTE_PREFIX_COMMAND(prefix_, name_, handler_) DCC_LISTEN_PREFIX_COMMAND((prefix_), (name_), (handler_))
#define DCC_ROUTE_PREFIX_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_PREFIX_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))
#define DCC_ROUTE_MESSAGE_COMMAND(prefix_, name_, handler_) DCC_LISTEN_MESSAGE_COMMAND((prefix_), (name_), (handler_))
#define DCC_ROUTE_MESSAGE_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_ROUTE_TASK_MS(interval_ms_, handler_) DCC_LISTEN_TASK_MS((interval_ms_), (handler_))
#define DCC_ROUTE_TASK_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_LISTEN_TASK_MS_DATA((interval_ms_), (handler_), (user_data_))
#define DCC_ROUTE_TASK_SECONDS(interval_seconds_, handler_) DCC_LISTEN_TASK_SECONDS((interval_seconds_), (handler_))
#define DCC_ROUTE_TASK_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_LISTEN_TASK_SECONDS_DATA((interval_seconds_), (handler_), (user_data_))
#define DCC_ROUTE_TASK_DAILY_KST(hhmm_, handler_) DCC_LISTEN_TASK_DAILY_KST((hhmm_), (handler_))
#define DCC_ROUTE_TASK_DAILY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_LISTEN_TASK_DAILY_KST_DATA((hhmm_), (handler_), (user_data_))

#define DCC_ROUTE_COMMAND_SCHEMA(command_) DCC_LISTEN_COMMAND_SCHEMA(command_)
#define DCC_ROUTE_COMMAND_BUILDER(command_) DCC_LISTEN_COMMAND_BUILDER(command_)
#define DCC_ROUTE_SCHEMA(command_) DCC_LISTEN_SCHEMA(command_)

#endif
