#include <dcc/dcc.h>

#include <stdio.h>
#include <string.h>

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static int run_component_v2_message_smoke(void) {
    dcc_component_v2_builder_t title;
    if (dcc_component_v2_builder_init_text_display(&title, "# Runtime status") != DCC_OK) {
        return 1;
    }

    dcc_component_v2_media_t gallery_items[] = {
        {
            .url = "attachment://graph.png",
            .description = "gateway latency graph",
        },
    };
    dcc_component_v2_builder_t gallery;
    if (dcc_component_v2_builder_init_media_gallery(&gallery, gallery_items, 1U) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t section_text;
    if (dcc_component_v2_builder_init_text_display(
            &section_text,
            "Gateway connected\nREST queue healthy"
        ) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t refresh;
    if (dcc_component_v2_builder_init_button(
            &refresh,
            DCC_BUTTON_PRIMARY,
            "Refresh",
            "status.refresh"
        ) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t section;
    if (dcc_component_v2_builder_init_section(&section, &section_text, 1U, &refresh) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t separator;
    if (dcc_component_v2_builder_init_separator(
            &separator,
            1U,
            DCC_COMPONENT_V2_SPACING_SMALL
        ) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t close_button;
    if (dcc_component_v2_builder_init_button(
            &close_button,
            DCC_BUTTON_SECONDARY,
            "Close",
            "status.close"
        ) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t action_row;
    if (dcc_component_v2_builder_init_action_row(&action_row, &close_button, 1U) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t container_children[] = {
        title,
        gallery,
        separator,
        section,
        action_row,
    };
    dcc_component_v2_builder_t container;
    if (dcc_component_v2_builder_init_container(
            &container,
            container_children,
            sizeof(container_children) / sizeof(container_children[0])
        ) != DCC_OK ||
        dcc_component_v2_builder_set_accent_color(&container, 0x5865F2U) != DCC_OK) {
        return 1;
    }

    char *component_json = NULL;
    if (dcc_component_v2_builder_build_json(&container, &component_json) != DCC_OK) {
        fprintf(stderr, "failed to serialize component v2 container\n");
        return 1;
    }
    int failed =
        require_contains(component_json, "\"type\":17") ||
        require_contains(component_json, "\"type\":10") ||
        require_contains(component_json, "\"type\":12") ||
        require_contains(component_json, "\"type\":9") ||
        require_contains(component_json, "\"type\":1") ||
        require_contains(component_json, "\"accent_color\":5793266") ||
        require_contains(component_json, "\"custom_id\":\"status.refresh\"");
    dcc_component_v2_builder_json_free(component_json);
    component_json = NULL;
    if (failed) {
        return 1;
    }

    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    if (dcc_message_builder_set_attachments_json(
            &message,
            "[{\"id\":\"0\",\"filename\":\"graph.png\"}]"
        ) != DCC_OK ||
        dcc_message_builder_set_components_v2(&message, &container, 1U) != DCC_OK) {
        return 1;
    }
    char *message_json = NULL;
    if (dcc_message_builder_build_json(&message, &message_json) != DCC_OK) {
        fprintf(stderr, "failed to serialize component v2 message\n");
        return 1;
    }
    failed =
        require_contains(message_json, "\"flags\":32768") ||
        require_contains(message_json, "\"components\":[{\"type\":17") ||
        require_contains(message_json, "\"attachments\":[{\"id\":\"0\"");
    dcc_message_builder_json_free(message_json);
    message_json = NULL;
    if (failed) {
        return 1;
    }

    dcc_message_builder_init(&message);
    if (dcc_message_builder_set_content(&message, "legacy content") != DCC_OK ||
        dcc_message_builder_set_components_v2(&message, &container, 1U) != DCC_OK ||
        dcc_message_builder_build_json(&message, &message_json) != DCC_ERR_INVALID_ARG) {
        dcc_message_builder_json_free(message_json);
        fprintf(stderr, "component v2 message allowed legacy content\n");
        return 1;
    }

    dcc_component_v2_builder_t duplicate_buttons[] = {
        close_button,
        close_button,
    };
    if (dcc_component_v2_builder_build_array_json(
            duplicate_buttons,
            sizeof(duplicate_buttons) / sizeof(duplicate_buttons[0]),
            &component_json
        ) != DCC_ERR_INVALID_ARG) {
        dcc_component_v2_builder_json_free(component_json);
        fprintf(stderr, "component v2 validator allowed duplicate custom_id\n");
        return 1;
    }

    dcc_component_v2_builder_t channel_select;
    const dcc_component_v2_select_default_value_t default_channels[] = {
        {
            .id = 222,
            .type = DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL,
        },
    };
    const uint32_t channel_types[] = {0U, 5U};
    dcc_component_v2_builder_init(&channel_select, DCC_COMPONENT_V2_CHANNEL_SELECT);
    if (dcc_component_v2_builder_set_id(&channel_select, 0U) != DCC_OK ||
        dcc_component_v2_builder_set_custom_id(&channel_select, "status.channel") != DCC_OK ||
        dcc_component_v2_builder_set_default_values(
            &channel_select,
            default_channels,
            sizeof(default_channels) / sizeof(default_channels[0])
        ) != DCC_OK ||
        dcc_component_v2_builder_set_channel_types(
            &channel_select,
            channel_types,
            sizeof(channel_types) / sizeof(channel_types[0])
        ) != DCC_OK ||
        dcc_component_v2_builder_build_json(&channel_select, &component_json) != DCC_OK) {
        fprintf(stderr, "failed to serialize component v2 channel select\n");
        return 1;
    }
    failed =
        require_contains(component_json, "\"type\":8") ||
        require_contains(component_json, "\"id\":0") ||
        require_contains(component_json, "\"default_values\":[{\"id\":\"222\",\"type\":\"channel\"}]") ||
        require_contains(component_json, "\"channel_types\":[0,5]");
    dcc_component_v2_builder_json_free(component_json);
    component_json = NULL;
    if (failed) {
        return 1;
    }

    dcc_component_v2_builder_t string_select;
    dcc_component_v2_builder_init(&string_select, DCC_COMPONENT_V2_STRING_SELECT);
    if (dcc_component_v2_builder_set_custom_id(&string_select, "status.string") != DCC_OK ||
        dcc_component_v2_builder_set_options(
            &string_select,
            (const dcc_select_option_t[]){{ .label = "One", .value = "one" }},
            1U
        ) != DCC_OK ||
        dcc_component_v2_builder_set_default_values(
            &string_select,
            default_channels,
            sizeof(default_channels) / sizeof(default_channels[0])
        ) != DCC_OK ||
        dcc_component_v2_builder_build_json(&string_select, &component_json) != DCC_ERR_INVALID_ARG) {
        dcc_component_v2_builder_json_free(component_json);
        fprintf(stderr, "component v2 validator allowed default_values on string select\n");
        return 1;
    }

    return 0;
}

static int run_component_v2_modal_smoke(void) {
    dcc_component_v2_builder_t file_upload;
    dcc_component_v2_builder_init(&file_upload, DCC_COMPONENT_V2_FILE_UPLOAD);
    if (dcc_component_v2_builder_set_custom_id(&file_upload, "upload.config") != DCC_OK ||
        dcc_component_v2_builder_set_min_values(&file_upload, 1U) != DCC_OK ||
        dcc_component_v2_builder_set_max_values(&file_upload, 3U) != DCC_OK ||
        dcc_component_v2_builder_set_required(&file_upload, 1U) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t label;
    dcc_component_v2_builder_init(&label, DCC_COMPONENT_V2_LABEL);
    if (dcc_component_v2_builder_set_label(&label, "Config files") != DCC_OK ||
        dcc_component_v2_builder_set_description(&label, "Upload one or more JSON files") != DCC_OK ||
        dcc_component_v2_builder_set_children(&label, &file_upload, 1U) != DCC_OK) {
        return 1;
    }

    char *json = NULL;
    if (dcc_component_v2_builder_build_json(&label, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize component v2 modal label\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"type\":18") ||
        require_contains(json, "\"component\":{\"type\":19") ||
        require_contains(json, "\"custom_id\":\"upload.config\"") ||
        require_contains(json, "\"max_values\":3");
    dcc_component_v2_builder_json_free(json);
    json = NULL;
    if (failed) {
        return 1;
    }

    dcc_modal_builder_t modal;
    dcc_modal_builder_init(&modal);
    if (dcc_modal_builder_set_custom_id(&modal, "upload-modal") != DCC_OK ||
        dcc_modal_builder_set_title(&modal, "Upload config") != DCC_OK ||
        dcc_modal_builder_set_components_v2(&modal, &label, 1U) != DCC_OK ||
        dcc_modal_builder_build_json(&modal, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize component v2 modal payload\n");
        return 1;
    }
    failed =
        require_contains(json, "\"type\":9") ||
        require_contains(json, "\"custom_id\":\"upload-modal\"") ||
        require_contains(json, "\"components\":[{\"type\":18");
    dcc_modal_builder_json_free(json);
    json = NULL;
    if (failed) {
        return 1;
    }

    dcc_modal_builder_init(&modal);
    if (dcc_modal_builder_set_custom_id(&modal, "mixed-modal") != DCC_OK ||
        dcc_modal_builder_set_title(&modal, "Mixed") != DCC_OK ||
        dcc_modal_builder_set_components_json(&modal, "[]") != DCC_OK ||
        dcc_modal_builder_set_components_v2(&modal, &label, 1U) != DCC_OK ||
        dcc_modal_builder_build_json(&modal, &json) != DCC_ERR_INVALID_ARG) {
        dcc_modal_builder_json_free(json);
        fprintf(stderr, "component v2 modal allowed mixed component sources\n");
        return 1;
    }

    dcc_component_v2_builder_t checkbox_group;
    dcc_component_v2_builder_init(&checkbox_group, DCC_COMPONENT_V2_CHECKBOX_GROUP);
    if (dcc_component_v2_builder_set_custom_id(&checkbox_group, "check.group") != DCC_OK ||
        dcc_component_v2_builder_set_options(
            &checkbox_group,
            (const dcc_select_option_t[]){{ .label = "Only", .value = "only" }},
            1U
        ) != DCC_OK ||
        dcc_component_v2_builder_build_json(&checkbox_group, &json) != DCC_OK ||
        require_contains(json, "\"type\":22") ||
        require_contains(json, "\"options\":[{\"label\":\"Only\"")) {
        dcc_component_v2_builder_json_free(json);
        fprintf(stderr, "component v2 validator rejected one checkbox option\n");
        return 1;
    }
    dcc_component_v2_builder_json_free(json);
    json = NULL;

    dcc_component_v2_builder_t checkbox;
    dcc_component_v2_builder_init(&checkbox, DCC_COMPONENT_V2_CHECKBOX);
    if (dcc_component_v2_builder_set_custom_id(&checkbox, "check.one") != DCC_OK ||
        dcc_component_v2_builder_set_label(&checkbox, "Accept") != DCC_OK ||
        dcc_component_v2_builder_set_default(&checkbox, 1U) != DCC_OK ||
        dcc_component_v2_builder_build_json(&checkbox, &json) != DCC_OK ||
        require_contains(json, "\"default\":true") ||
        strstr(json, "\"checked\"") != NULL) {
        dcc_component_v2_builder_json_free(json);
        fprintf(stderr, "component v2 checkbox default serialization failed\n");
        return 1;
    }
    dcc_component_v2_builder_json_free(json);

    return failed;
}

int main(void) {
    if (run_component_v2_message_smoke() != 0) {
        return 1;
    }
    if (run_component_v2_modal_smoke() != 0) {
        return 1;
    }
    return 0;
}
