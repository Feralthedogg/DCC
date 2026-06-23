#include <dcc/sugar/component_presets.h>

#include <stdio.h>
#include <string.h>

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static int check_confirm_message(void) {
    dcc_message_builder_t confirm =
        DCC_CONFIRM_MESSAGE_LABELS("Delete this message?", "Yes", "delete.yes", "No", "delete.no");
    char *json = NULL;
    if (dcc_message_builder_build_json(&confirm, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar confirm preset\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"Delete this message?\"") ||
        require_contains(json, "\"label\":\"Yes\"") ||
        require_contains(json, "\"style\":3") ||
        require_contains(json, "\"custom_id\":\"delete.yes\"") ||
        require_contains(json, "\"label\":\"No\"") ||
        require_contains(json, "\"style\":4") ||
        require_contains(json, "\"custom_id\":\"delete.no\"") ||
        require_contains(json, "\"disabled\":false");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_paginator_message(void) {
    dcc_message_builder_t page =
        DCC_PAGINATOR_MESSAGE("Page 1/3", "page.prev", "page.next", 1U, 0U);
    char *json = NULL;
    if (dcc_message_builder_build_json(&page, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar paginator preset\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"Page 1/3\"") ||
        require_contains(json, "\"custom_id\":\"page.prev\"") ||
        require_contains(json, "\"custom_id\":\"page.next\"") ||
        require_contains(json, "\"disabled\":true") ||
        require_contains(json, "\"disabled\":false");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_confirm_v2_message(void) {
    dcc_message_builder_t confirm =
        DCC_CONFIRM_V2("Continue?", "confirm.yes", "confirm.no");
    char *json = NULL;
    if (dcc_message_builder_build_json(&confirm, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar confirm v2 preset\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"flags\":32768") ||
        require_contains(json, "\"type\":17") ||
        require_contains(json, "\"content\":\"Continue?\"") ||
        require_contains(json, "\"custom_id\":\"confirm.yes\"") ||
        require_contains(json, "\"custom_id\":\"confirm.no\"") ||
        require_contains(json, "\"disabled\":false");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_paginator_v2_message(void) {
    dcc_message_builder_t page =
        DCC_PAGINATOR_V2_LABELS("Page 2/3", "Back", "page.back", "Forward", "page.forward", 0U, 1U);
    char *json = NULL;
    if (dcc_message_builder_build_json(&page, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar paginator v2 preset\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"flags\":32768") ||
        require_contains(json, "\"content\":\"Page 2/3\"") ||
        require_contains(json, "\"label\":\"Back\"") ||
        require_contains(json, "\"custom_id\":\"page.back\"") ||
        require_contains(json, "\"label\":\"Forward\"") ||
        require_contains(json, "\"custom_id\":\"page.forward\"") ||
        require_contains(json, "\"disabled\":true") ||
        require_contains(json, "\"disabled\":false");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_settings_panel_message(void) {
    dcc_message_builder_t panel =
        DCC_SETTINGS_PANEL_MESSAGE("Settings", "settings.edit", "settings.enable", "settings.disable", "settings.reset", 1U);
    char *json = NULL;
    if (dcc_message_builder_build_json(&panel, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar settings panel preset\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"content\":\"Settings\"") ||
        require_contains(json, "\"label\":\"Edit\"") ||
        require_contains(json, "\"style\":1") ||
        require_contains(json, "\"custom_id\":\"settings.edit\"") ||
        require_contains(json, "\"label\":\"Enable\"") ||
        require_contains(json, "\"custom_id\":\"settings.enable\"") ||
        require_contains(json, "\"label\":\"Disable\"") ||
        require_contains(json, "\"custom_id\":\"settings.disable\"") ||
        require_contains(json, "\"label\":\"Reset\"") ||
        require_contains(json, "\"custom_id\":\"settings.reset\"") ||
        require_contains(json, "\"disabled\":true") ||
        require_contains(json, "\"disabled\":false");
    dcc_message_builder_json_free(json);
    return failed;
}

static int check_settings_panel_v2_message(void) {
    dcc_message_builder_t panel =
        DCC_SETTINGS_PANEL_V2("## Settings", "settings.edit", "settings.enable", "settings.disable", "settings.reset", 0U);
    char *json = NULL;
    if (dcc_message_builder_build_json(&panel, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar settings panel v2 preset\n");
        return 1;
    }

    int failed =
        require_contains(json, "\"flags\":32768") ||
        require_contains(json, "\"type\":17") ||
        require_contains(json, "\"content\":\"## Settings\"") ||
        require_contains(json, "\"custom_id\":\"settings.edit\"") ||
        require_contains(json, "\"custom_id\":\"settings.enable\"") ||
        require_contains(json, "\"custom_id\":\"settings.disable\"") ||
        require_contains(json, "\"custom_id\":\"settings.reset\"") ||
        require_contains(json, "\"disabled\":true") ||
        require_contains(json, "\"disabled\":false");
    dcc_message_builder_json_free(json);
    return failed;
}

int main(void) {
    if (check_confirm_message() != 0) {
        return 1;
    }
    if (check_paginator_message() != 0) {
        return 1;
    }
    if (check_confirm_v2_message() != 0) {
        return 1;
    }
    if (check_paginator_v2_message() != 0) {
        return 1;
    }
    if (check_settings_panel_message() != 0) {
        return 1;
    }
    if (check_settings_panel_v2_message() != 0) {
        return 1;
    }
    return 0;
}
