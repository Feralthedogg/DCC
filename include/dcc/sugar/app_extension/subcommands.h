#ifndef DCC_SUGAR_APP_EXTENSION_SUBCOMMANDS_H
#define DCC_SUGAR_APP_EXTENSION_SUBCOMMANDS_H

#include <dcc/sugar/app_extension/base.h>

#define DCC_EXTENSION_SUBCOMMAND(command_name_, subcommand_path_, handler_, user_data_) \
    ((dcc_app_extension_subcommand_t){ \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SUBCOMMAND_POLICY(command_name_, subcommand_path_, handler_, user_data_, policy_) \
    ((dcc_app_extension_subcommand_t){ \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_EXTENSION_SUBCOMMAND((command_name_), (subcommand_path_), (handler_), NULL)

#define DCC_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_EXTENSION_SUBCOMMAND((command_name_), (subcommand_path_), (handler_), (user_data_))

#define DCC_SUBCOMMAND_POLICY(command_name_, subcommand_path_, handler_, policy_) \
    DCC_EXTENSION_SUBCOMMAND_POLICY((command_name_), (subcommand_path_), (handler_), NULL, (policy_))

#define DCC_SUBCOMMAND_DATA_POLICY(command_name_, subcommand_path_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SUBCOMMAND_POLICY((command_name_), (subcommand_path_), (handler_), (user_data_), (policy_))

#define DCC_ON_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_SUBCOMMAND((command_name_), (subcommand_path_), (handler_))

#define DCC_ON_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_SUBCOMMAND_DATA((command_name_), (subcommand_path_), (handler_), (user_data_))

#define DCC_ON_SUBCOMMAND_POLICY(command_name_, subcommand_path_, handler_, policy_) \
    DCC_SUBCOMMAND_POLICY((command_name_), (subcommand_path_), (handler_), (policy_))

#define DCC_ON_SUBCOMMAND_DATA_POLICY(command_name_, subcommand_path_, handler_, user_data_, policy_) \
    DCC_SUBCOMMAND_DATA_POLICY((command_name_), (subcommand_path_), (handler_), (user_data_), (policy_))

#endif /* DCC_SUGAR_APP_EXTENSION_SUBCOMMANDS_H */
