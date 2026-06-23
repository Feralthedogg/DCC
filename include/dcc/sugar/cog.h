#ifndef DCC_SUGAR_COG_H
#define DCC_SUGAR_COG_H

#include <dcc/app.h>
#include <dcc/sugar/app_extension.h>
#include <dcc/sugar/app_module.h>

#define DCC_COG_SETUP_NAME(name_) dcc_cog_setup_##name_
#define DCC_COG_SETUP(name_) DCC_COG_SETUP_NAME(name_)

#define DCC_DECLARE_COG(name_) \
    dcc_status_t DCC_COG_SETUP_NAME(name_)(dcc_app_t *app, void *user_data)

#define DCC_DEFINE_COG(name_, display_name_, ...) \
    static dcc_status_t DCC_COG_SETUP_NAME(name_)(dcc_app_t *app, void *user_data) { \
        (void)user_data; \
        dcc_app_extension_t dcc_sugar_cog_extension = \
            DCC_COG((display_name_), __VA_ARGS__); \
        return dcc_app_extension_register((app), &dcc_sugar_cog_extension); \
    }

#define DCC_DEFINE_PUBLIC_COG(name_, display_name_, ...) \
    dcc_status_t DCC_COG_SETUP_NAME(name_)(dcc_app_t *app, void *user_data) { \
        (void)user_data; \
        dcc_app_extension_t dcc_sugar_cog_extension = \
            DCC_COG((display_name_), __VA_ARGS__); \
        return dcc_app_extension_register((app), &dcc_sugar_cog_extension); \
    }

#define DCC_LOAD_COG(app_, name_) \
    DCC_COG_SETUP_NAME(name_)((app_), NULL)

#define DCC_LOAD_COG_DATA(app_, name_, user_data_) \
    DCC_COG_SETUP_NAME(name_)((app_), (user_data_))

#define DCC_COG_MODULE(name_, user_data_) \
    DCC_APP_MODULE(#name_, DCC_COG_SETUP_NAME(name_), (user_data_))

#define DCC_COG_MODULE_NAMED(module_name_, name_, user_data_) \
    DCC_APP_MODULE((module_name_), DCC_COG_SETUP_NAME(name_), (user_data_))

#define DCC_FEATURE_SETUP_NAME(name_) DCC_COG_SETUP_NAME(name_)
#define DCC_FEATURE_SETUP(name_) DCC_COG_SETUP(name_)

#define DCC_FEATURE(name_, ...) DCC_COG((name_), __VA_ARGS__)
#define DCC_DECLARE_FEATURE(name_) DCC_DECLARE_COG(name_)
#define DCC_DEFINE_FEATURE(name_, display_name_, ...) DCC_DEFINE_COG(name_, display_name_, __VA_ARGS__)
#define DCC_DEFINE_PUBLIC_FEATURE(name_, display_name_, ...) DCC_DEFINE_PUBLIC_COG(name_, display_name_, __VA_ARGS__)

#define DCC_LOAD_FEATURE(app_, name_) DCC_LOAD_COG((app_), name_)
#define DCC_LOAD_FEATURE_DATA(app_, name_, user_data_) DCC_LOAD_COG_DATA((app_), name_, (user_data_))
#define DCC_FEATURE_MODULE(name_, user_data_) DCC_COG_MODULE(name_, (user_data_))
#define DCC_FEATURE_MODULE_NAMED(module_name_, name_, user_data_) \
    DCC_COG_MODULE_NAMED((module_name_), name_, (user_data_))

#define DCC_USE_FEATURE(name_, user_data_) DCC_FEATURE_MODULE(name_, (user_data_))
#define DCC_USE_FEATURE_NAMED(module_name_, name_, user_data_) \
    DCC_FEATURE_MODULE_NAMED((module_name_), name_, (user_data_))

#define DCC_FEATURE_MIDDLEWARE(...) DCC_COG_MIDDLEWARE(__VA_ARGS__)
#define DCC_FEATURE_SLASHES(...) DCC_COG_SLASHES(__VA_ARGS__)
#define DCC_FEATURE_COMMANDS(...) DCC_COG_COMMANDS(__VA_ARGS__)
#define DCC_FEATURE_SUBCOMMANDS(...) DCC_COG_SUBCOMMANDS(__VA_ARGS__)
#define DCC_FEATURE_AUTOCOMPLETES(...) DCC_COG_AUTOCOMPLETES(__VA_ARGS__)
#define DCC_FEATURE_CONTEXT_MENUS(...) DCC_COG_CONTEXT_MENUS(__VA_ARGS__)
#define DCC_FEATURE_COMPONENTS(...) DCC_COG_COMPONENTS(__VA_ARGS__)
#define DCC_FEATURE_TYPED_SLASHES(...) DCC_COG_TYPED_SLASHES(__VA_ARGS__)
#define DCC_FEATURE_TYPED_COMMANDS(...) DCC_COG_TYPED_COMMANDS(__VA_ARGS__)
#define DCC_FEATURE_COMMAND_ROUTES(...) DCC_COG_COMMAND_ROUTES(__VA_ARGS__)
#define DCC_FEATURE_TYPED_SUBCOMMANDS(...) DCC_COG_TYPED_SUBCOMMANDS(__VA_ARGS__)
#define DCC_FEATURE_SUBCOMMAND_ROUTES(...) DCC_COG_SUBCOMMAND_ROUTES(__VA_ARGS__)
#define DCC_FEATURE_TYPED_AUTOCOMPLETES(...) DCC_COG_TYPED_AUTOCOMPLETES(__VA_ARGS__)
#define DCC_FEATURE_AUTOCOMPLETE_ROUTES(...) DCC_COG_AUTOCOMPLETE_ROUTES(__VA_ARGS__)
#define DCC_FEATURE_TYPED_MODALS(...) DCC_COG_TYPED_MODALS(__VA_ARGS__)
#define DCC_FEATURE_MODAL_ROUTES(...) DCC_COG_MODAL_ROUTES(__VA_ARGS__)
#define DCC_FEATURE_TYPED_BUTTONS(...) DCC_COG_TYPED_BUTTONS(__VA_ARGS__)
#define DCC_FEATURE_BUTTON_ROUTES(...) DCC_COG_BUTTON_ROUTES(__VA_ARGS__)
#define DCC_FEATURE_TYPED_SELECTS(...) DCC_COG_TYPED_SELECTS(__VA_ARGS__)
#define DCC_FEATURE_SELECT_ROUTES(...) DCC_COG_SELECT_ROUTES(__VA_ARGS__)
#define DCC_FEATURE_VIEWS(...) DCC_COG_VIEWS(__VA_ARGS__)
#define DCC_FEATURE_EVENTS(...) DCC_COG_EVENTS(__VA_ARGS__)
#define DCC_FEATURE_MESSAGE_COMMANDS(...) DCC_COG_MESSAGE_COMMANDS(__VA_ARGS__)
#define DCC_FEATURE_TASKS(...) DCC_COG_TASKS(__VA_ARGS__)

#endif
