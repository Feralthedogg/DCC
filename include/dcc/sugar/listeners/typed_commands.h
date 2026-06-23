#ifndef DCC_SUGAR_LISTENERS_TYPED_COMMANDS_H
#define DCC_SUGAR_LISTENERS_TYPED_COMMANDS_H

#include <dcc/sugar/listeners/types.h>

#define DCC_LISTEN_TYPED_SLASH(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_SLASH, \
        .typed_slash = (route_) \
    })
#define DCC_LISTEN_TYPED_SLASH_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SLASH(DCC_SLASH_ROUTE((name_), (description_), args_type_, (handler_), params_))
#define DCC_LISTEN_TYPED_SLASH_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_DATA_VALIDATED( \
            (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_DATA_GUARDED( \
            (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_TYPED_SLASH(DCC_SLASH_ROUTE_FN(handler_, (description_), args_type_, params_))
#define DCC_LISTEN_TYPED_SLASH_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_DATA(handler_, (description_), args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_)) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_DATA_VALIDATED( \
    handler_, description_, args_type_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_DATA_VALIDATED( \
            handler_, (description_), args_type_, (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_SLASH_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SLASH_PARAMS((name_), (description_), args_type_, (handler_), params_)
#define DCC_LISTEN_SLASH_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_LISTEN_SLASH_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_LISTEN_SLASH_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_LISTEN_SLASH_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_LISTEN_SLASH_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SLASH_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_TYPED_SLASH_FN(handler_, (description_), args_type_, params_)
#define DCC_LISTEN_SLASH_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_LISTEN_SLASH_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_LISTEN_SLASH_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_))
#define DCC_LISTEN_SLASH_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_SLASH_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_SLASH_PARAMS((name_), (description_), args_type_, (handler_), params_)
#define DCC_LISTEN_COMMAND_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SLASH_PARAMS_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_LISTEN_COMMAND_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_SLASH_PARAMS_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_LISTEN_COMMAND_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_SLASH_PARAMS_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_LISTEN_COMMAND_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_SLASH_PARAMS_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SLASH_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_COMMAND_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_SLASH_FN(handler_, (description_), args_type_, params_)
#define DCC_LISTEN_COMMAND_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_SLASH_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_LISTEN_COMMAND_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_SLASH_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_LISTEN_COMMAND_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_LISTEN_SLASH_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_))
#define DCC_LISTEN_COMMAND_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_SLASH_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_SLASH_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_COMMAND_FN(handler_, (description_), args_type_, params_)
#define DCC_LISTEN_COMMAND_PARAMS_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_COMMAND_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_LISTEN_COMMAND_PARAMS_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)
#define DCC_LISTEN_TYPED_SUBCOMMAND(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_SUBCOMMAND, \
        .typed_subcommand = (route_) \
    })
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE((command_name_), (subcommand_path_), args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_DATA( \
            (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_GUARDED( \
            (command_name_), (subcommand_path_), args_type_, (handler_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_DATA_GUARDED( \
            (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN((command_name_), group_, subcommand_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_DATA( \
            (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_GUARDED( \
            (command_name_), group_, subcommand_, args_type_, (handler_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_DATA_GUARDED( \
            (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN(command_name_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND(DCC_SUBCOMMAND_ROUTE_FN((command_name_), handler_, args_type_, params_))
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN_GUARDED(command_name_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_FN_GUARDED((command_name_), handler_, args_type_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_FN_DATA_GUARDED( \
            (command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN(command_name_, group_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND(DCC_SUBCOMMAND_ROUTE_IN_FN((command_name_), group_, handler_, args_type_, params_))
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA(command_name_, group_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_GUARDED(command_name_, group_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_FN_GUARDED((command_name_), group_, handler_, args_type_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA_GUARDED( \
    command_name_, group_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_FN_DATA_GUARDED( \
            (command_name_), group_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS((command_name_), (subcommand_path_), args_type_, (handler_), params_)
#define DCC_LISTEN_SUBCOMMAND_PARAMS_DATA(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_PARAMS_GUARDED(command_name_, subcommand_path_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_PARAMS_DATA_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS((command_name_), group_, subcommand_, args_type_, (handler_), params_)
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_FN(command_name_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN((command_name_), handler_, args_type_, params_)
#define DCC_LISTEN_SUBCOMMAND_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_)
#define DCC_LISTEN_SUBCOMMAND_FN_GUARDED(command_name_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN_GUARDED((command_name_), handler_, args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_SUBCOMMAND_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA_GUARDED( \
        (command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_FN(command_name_, group_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN((command_name_), group_, handler_, args_type_, params_)
#define DCC_LISTEN_SUBCOMMAND_IN_FN_DATA(command_name_, group_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_)
#define DCC_LISTEN_SUBCOMMAND_IN_FN_GUARDED(command_name_, group_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_GUARDED((command_name_), group_, handler_, args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_SUBCOMMAND_IN_FN_DATA_GUARDED( \
    command_name_, group_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA_GUARDED( \
        (command_name_), group_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_TYPED_AUTOCOMPLETE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_AUTOCOMPLETE, \
        .typed_autocomplete = (route_) \
    })
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS( \
    command_name_, subcommand_path_, args_type_, handler_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS((command_name_), (subcommand_path_), args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
            (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS( \
    command_name_, group_, subcommand_, args_type_, handler_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN( \
            (command_name_), group_, subcommand_, args_type_, (handler_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN_DATA( \
            (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS( \
        (command_name_), (subcommand_path_), args_type_, (handler_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS( \
    command_name_, group_, subcommand_, args_type_, handler_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS( \
        (command_name_), group_, subcommand_, args_type_, (handler_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS_DATA( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
    )

#endif
