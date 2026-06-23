#include "internal/dcc_new_app.h"

#include <string.h>

static int dcc_new_app_preset_is(const dcc_new_app_options_t *options, const char *name) {
    return options->command_arg != NULL && strcmp(options->command_arg, name) == 0;
}

int dcc_new_app_generate_known_preset(const dcc_new_app_options_t *options) {
    if (dcc_new_app_preset_is(options, "settings") ||
        dcc_new_app_preset_is(options, "settings-panel") ||
        dcc_new_app_preset_is(options, "panel")) {
        return dcc_new_app_generate_settings_preset(options);
    }
    if (dcc_new_app_preset_is(options, "announcement") ||
        dcc_new_app_preset_is(options, "announce") ||
        dcc_new_app_preset_is(options, "broadcast")) {
        return dcc_new_app_generate_announcement_preset(options);
    }
    if (dcc_new_app_preset_is(options, "wizard") ||
        dcc_new_app_preset_is(options, "setup-wizard") ||
        dcc_new_app_preset_is(options, "setup_wizard")) {
        return dcc_new_app_generate_wizard_preset(options);
    }
    if (dcc_new_app_preset_is(options, "counter") ||
        dcc_new_app_preset_is(options, "store-counter") ||
        dcc_new_app_preset_is(options, "store_counter")) {
        return dcc_new_app_generate_counter_preset(options);
    }
    if (dcc_new_app_preset_is(options, "crud") ||
        dcc_new_app_preset_is(options, "store-crud") ||
        dcc_new_app_preset_is(options, "store_crud")) {
        return dcc_new_app_generate_crud_preset(options);
    }
    if (dcc_new_app_preset_is(options, "help") ||
        dcc_new_app_preset_is(options, "help-center") ||
        dcc_new_app_preset_is(options, "help_center") ||
        dcc_new_app_preset_is(options, "docs")) {
        return dcc_new_app_generate_help_preset(options);
    }
    if (dcc_new_app_preset_is(options, "menu") ||
        dcc_new_app_preset_is(options, "select-menu") ||
        dcc_new_app_preset_is(options, "select_menu") ||
        dcc_new_app_preset_is(options, "navigation") ||
        dcc_new_app_preset_is(options, "nav")) {
        return dcc_new_app_generate_menu_preset(options);
    }
    if (dcc_new_app_preset_is(options, "poll") ||
        dcc_new_app_preset_is(options, "vote") ||
        dcc_new_app_preset_is(options, "voting")) {
        return dcc_new_app_generate_poll_preset(options);
    }
    if (dcc_new_app_preset_is(options, "profile") ||
        dcc_new_app_preset_is(options, "profile-card") ||
        dcc_new_app_preset_is(options, "profile_card") ||
        dcc_new_app_preset_is(options, "whoami") ||
        dcc_new_app_preset_is(options, "me")) {
        return dcc_new_app_generate_profile_preset(options);
    }
    if (dcc_new_app_preset_is(options, "roles") ||
        dcc_new_app_preset_is(options, "role-menu") ||
        dcc_new_app_preset_is(options, "role_menu") ||
        dcc_new_app_preset_is(options, "self-role") ||
        dcc_new_app_preset_is(options, "self_role")) {
        return dcc_new_app_generate_roles_preset(options);
    }
    if (dcc_new_app_preset_is(options, "welcome") ||
        dcc_new_app_preset_is(options, "member-welcome") ||
        dcc_new_app_preset_is(options, "member_welcome") ||
        dcc_new_app_preset_is(options, "greet-member") ||
        dcc_new_app_preset_is(options, "greet_member")) {
        return dcc_new_app_generate_welcome_preset(options);
    }
    if (dcc_new_app_preset_is(options, "ticket") ||
        dcc_new_app_preset_is(options, "support-ticket") ||
        dcc_new_app_preset_is(options, "support_ticket")) {
        return dcc_new_app_generate_ticket_preset(options);
    }
    if (dcc_new_app_preset_is(options, "flow") ||
        dcc_new_app_preset_is(options, "interaction-flow") ||
        dcc_new_app_preset_is(options, "interaction_flow")) {
        return dcc_new_app_generate_flow_preset(options);
    }
    return dcc_new_app_generate_preset(options);
}
