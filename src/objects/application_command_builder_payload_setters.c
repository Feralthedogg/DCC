#include "internal/objects/dcc_builder_abi_internal.h"

static int dcc_command_payload_target(
    dcc_application_command_builder_t *builder,
    size_t first_offset,
    size_t first_width,
    size_t second_offset,
    size_t second_width,
    size_t third_offset,
    size_t third_width
) {
    dcc_builder_abi_view_t view;
    return dcc_application_command_builder_abi_validate(builder, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, first_offset, first_width) &&
        (second_width == 0U || dcc_builder_abi_field_covered(view.size, second_offset, second_width)) &&
        (third_width == 0U || dcc_builder_abi_field_covered(view.size, third_offset, third_width));
}

static void dcc_command_payload_presence(
    dcc_application_command_builder_t *builder,
    uint64_t bit,
    int present
) {
    if (present) {
        builder->present |= bit;
    } else {
        builder->present &= ~bit;
    }
}

#define DCC_COMMAND_PAYLOAD_ONE(builder_, field_) \
    dcc_command_payload_target( \
        (builder_), offsetof(dcc_application_command_builder_t, field_), \
        sizeof(((dcc_application_command_builder_t *)0)->field_), 0U, 0U, 0U, 0U \
    )

dcc_status_t dcc_application_command_builder_set_name_localizations_json(
    dcc_application_command_builder_t *builder,
    const char *name_localizations_json
) {
    if (!DCC_COMMAND_PAYLOAD_ONE(builder, name_localizations_json)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->name_localizations_json = name_localizations_json;
    dcc_command_payload_presence(
        builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON,
        name_localizations_json != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_description_localizations_json(
    dcc_application_command_builder_t *builder,
    const char *description_localizations_json
) {
    if (!DCC_COMMAND_PAYLOAD_ONE(builder, description_localizations_json)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->description_localizations_json = description_localizations_json;
    dcc_command_payload_presence(
        builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON,
        description_localizations_json != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_options_json(
    dcc_application_command_builder_t *builder,
    const char *options_json
) {
    if (!dcc_command_payload_target(
            builder,
            offsetof(dcc_application_command_builder_t, options_json), sizeof(builder->options_json),
            offsetof(dcc_application_command_builder_t, options), sizeof(builder->options),
            offsetof(dcc_application_command_builder_t, options_count), sizeof(builder->options_count)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->options_json = options_json;
    if (options_json != NULL) {
        builder->options = NULL;
        builder->options_count = 0U;
        builder->present &= ~DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS;
    }
    dcc_command_payload_presence(
        builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON, options_json != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_options(
    dcc_application_command_builder_t *builder,
    const dcc_application_command_option_builder_t *options,
    size_t options_count
) {
    if (!dcc_command_payload_target(
            builder,
            offsetof(dcc_application_command_builder_t, options), sizeof(builder->options),
            offsetof(dcc_application_command_builder_t, options_count), sizeof(builder->options_count),
            offsetof(dcc_application_command_builder_t, options_json), sizeof(builder->options_json)
        ) || (options_count != 0U && options == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->options = options;
    builder->options_count = options_count;
    if (options_count != 0U) {
        builder->options_json = NULL;
        builder->present &= ~DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON;
    }
    dcc_command_payload_presence(
        builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS, options_count != 0U
    );
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_integration_types_json(
    dcc_application_command_builder_t *builder,
    const char *integration_types_json
) {
    if (!DCC_COMMAND_PAYLOAD_ONE(builder, integration_types_json)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->integration_types_json = integration_types_json;
    dcc_command_payload_presence(
        builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON,
        integration_types_json != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_contexts_json(
    dcc_application_command_builder_t *builder,
    const char *contexts_json
) {
    if (!DCC_COMMAND_PAYLOAD_ONE(builder, contexts_json)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->contexts_json = contexts_json;
    dcc_command_payload_presence(
        builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON,
        contexts_json != NULL
    );
    return DCC_OK;
}
