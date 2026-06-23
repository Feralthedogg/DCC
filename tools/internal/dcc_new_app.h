#ifndef DCC_NEW_APP_TOOL_H
#define DCC_NEW_APP_TOOL_H

#include <stdio.h>
#include <stdint.h>

typedef enum dcc_new_app_mode {
    DCC_NEW_APP_MODE_CREATE = 1,
    DCC_NEW_APP_MODE_ADD_COG,
    DCC_NEW_APP_MODE_ADD_COMMAND,
    DCC_NEW_APP_MODE_ADD_SUBCOMMAND,
    DCC_NEW_APP_MODE_ADD_BUTTON,
    DCC_NEW_APP_MODE_ADD_SELECT,
    DCC_NEW_APP_MODE_ADD_MODAL,
    DCC_NEW_APP_MODE_ADD_AUTOCOMPLETE,
    DCC_NEW_APP_MODE_ADD_EVENT,
    DCC_NEW_APP_MODE_ADD_VIEW,
    DCC_NEW_APP_MODE_ADD_CONFIG,
    DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE,
    DCC_NEW_APP_MODE_ADD_MESSAGE_COMMAND,
    DCC_NEW_APP_MODE_ADD_TASK,
    DCC_NEW_APP_MODE_ADD_ACTION,
    DCC_NEW_APP_MODE_ADD_CONTEXT_MENU,
    DCC_NEW_APP_MODE_ADD_MIDDLEWARE,
    DCC_NEW_APP_MODE_ADD_ERROR_HANDLER,
    DCC_NEW_APP_MODE_ADD_PRESET
} dcc_new_app_mode_t;

typedef enum dcc_new_app_task_schedule {
    DCC_NEW_APP_TASK_SECONDS = 1,
    DCC_NEW_APP_TASK_MS,
    DCC_NEW_APP_TASK_MINUTES,
    DCC_NEW_APP_TASK_HOURS,
    DCC_NEW_APP_TASK_DAILY_KST
} dcc_new_app_task_schedule_t;

typedef struct dcc_new_app_options {
    dcc_new_app_mode_t mode;
    const char *path;
    const char *name_arg;
    const char *cog_arg;
    const char *command_arg;
    const char *subcommand_arg;
    const char *config_type_arg;
    const char *config_env_arg;
    const char *prefix_arg;
    const char *task_interval_arg;
    const char *guard_owner_arg;
    const char *guard_role_arg;
    const char *guard_any_role_arg;
    const char *guard_permission_arg;
    const char *guard_cooldown_user_arg;
    const char *guard_cooldown_global_arg;
    const char *guard_cooldown_channel_arg;
    const char *guard_cooldown_guild_arg;
    dcc_new_app_task_schedule_t task_schedule;
    char name[128];
    char cog_name[128];
    char command_name[128];
    char subcommand_name[128];
    char subcommand_path[160];
    uint8_t force;
    uint8_t help;
    uint8_t guard_guild_only;
    uint8_t guard_dm_only;
    uint8_t guard_nsfw_only;
} dcc_new_app_options_t;

void dcc_new_app_usage(FILE *stream, const char *argv0);
int dcc_new_app_parse_options(int argc, char **argv, dcc_new_app_options_t *options);
int dcc_new_app_validate_options(dcc_new_app_options_t *options);
int dcc_new_app_generate(const dcc_new_app_options_t *options);
int dcc_new_app_generate_cog(const dcc_new_app_options_t *options);
int dcc_new_app_generate_command(const dcc_new_app_options_t *options);
int dcc_new_app_generate_subcommand(const dcc_new_app_options_t *options);
int dcc_new_app_generate_button(const dcc_new_app_options_t *options);
int dcc_new_app_generate_select(const dcc_new_app_options_t *options);
int dcc_new_app_generate_modal(const dcc_new_app_options_t *options);
int dcc_new_app_generate_autocomplete(const dcc_new_app_options_t *options);
int dcc_new_app_generate_event(const dcc_new_app_options_t *options);
int dcc_new_app_generate_view(const dcc_new_app_options_t *options);
int dcc_new_app_generate_config(const dcc_new_app_options_t *options);
int dcc_new_app_generate_latest_message(const dcc_new_app_options_t *options);
int dcc_new_app_generate_message_command(const dcc_new_app_options_t *options);
int dcc_new_app_generate_task(const dcc_new_app_options_t *options);
int dcc_new_app_generate_action(const dcc_new_app_options_t *options);
int dcc_new_app_generate_context_menu(const dcc_new_app_options_t *options);
int dcc_new_app_generate_middleware(const dcc_new_app_options_t *options);
int dcc_new_app_generate_error_handler(const dcc_new_app_options_t *options);
int dcc_new_app_generate_known_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_announcement_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_counter_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_crud_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_flow_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_help_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_menu_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_poll_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_profile_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_roles_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_settings_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_ticket_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_welcome_preset(const dcc_new_app_options_t *options);
int dcc_new_app_generate_wizard_preset(const dcc_new_app_options_t *options);
int dcc_new_app_write_default_cogs(const dcc_new_app_options_t *options);

int dcc_new_app_path_exists(const char *path);
int dcc_new_app_mkdirs(const char *path);
char *dcc_new_app_join(const char *lhs, const char *rhs);
void dcc_new_app_make_guard(const char *name, char *out, size_t out_len);
void dcc_new_app_make_macro_name(const char *name, const char *suffix, char *out, size_t out_len);
void dcc_new_app_make_subcommand_symbol_base(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
);
FILE *dcc_new_app_open_output(const char *path, uint8_t force);
char *dcc_new_app_read_file(const char *path, size_t *out_len);
int dcc_new_app_write_file_all(const char *path, const char *data, size_t len);
int dcc_new_app_replace_range(
    const char *path,
    const char *data,
    size_t len,
    const char *start,
    size_t remove_len,
    const char *replacement
);
int dcc_new_app_insert_before_marker(
    const char *path,
    const char *marker,
    const char *insertion,
    const char *duplicate_needle
);
int dcc_new_app_insert_before_marker_compat(
    const char *path,
    const char *marker,
    const char *legacy_marker,
    const char *insertion,
    const char *duplicate_needle
);
int dcc_new_app_update_command_builder_subcommands(
    const char *path,
    const dcc_new_app_options_t *options
);
int dcc_new_app_update_command_json_subcommands(const dcc_new_app_options_t *options);
int dcc_new_app_guard_options_validate(const dcc_new_app_options_t *options);
int dcc_new_app_guard_policy_literal(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
);
int dcc_new_app_guard_items_literal(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
);

#endif
