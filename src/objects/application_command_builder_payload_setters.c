#include "internal/objects/dcc_builder_abi_internal.h"

#include <string.h>

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

static int dcc_localizations_valid(
    const dcc_localization_t *localizations,
    size_t count
) {
    if (count != 0U && localizations == NULL) {
        return 0;
    }
    for (size_t i = 0U; i < count; ++i) {
        if (localizations[i].locale == NULL || localizations[i].locale[0] == '\0' ||
            localizations[i].value == NULL) {
            return 0;
        }
        for (size_t j = 0U; j < i; ++j) {
            if (strcmp(localizations[i].locale, localizations[j].locale) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

static int dcc_option_payload_target(
    dcc_application_command_option_builder_t *builder,
    size_t last_offset,
    size_t last_width
) {
    dcc_builder_abi_view_t view;
    return dcc_application_command_option_builder_abi_validate(builder, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, last_offset, last_width);
}

static dcc_status_t dcc_option_set_localizations(
    dcc_application_command_option_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t count,
    int description
) {
    if (!dcc_option_payload_target(
            builder,
            description
                ? offsetof(dcc_application_command_option_builder_t, description_localization_count)
                : offsetof(dcc_application_command_option_builder_t, name_localization_count),
            sizeof(size_t)) ||
        !dcc_localizations_valid(localizations, count)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (description) {
        builder->description_localizations = localizations;
        builder->description_localization_count = count;
        if (count != 0U) {
            builder->description_localizations_json = NULL;
            builder->present &=
                ~DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON;
        }
        if (count != 0U) builder->present |=
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS;
        else builder->present &=
            ~DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS;
    } else {
        builder->name_localizations = localizations;
        builder->name_localization_count = count;
        if (count != 0U) {
            builder->name_localizations_json = NULL;
            builder->present &=
                ~DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON;
        }
        if (count != 0U) builder->present |=
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS;
        else builder->present &=
            ~DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS;
    }
    return DCC_OK;
}

dcc_status_t dcc_application_command_option_builder_set_name_localizations(
    dcc_application_command_option_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
) {
    return dcc_option_set_localizations(builder, localizations, localization_count, 0);
}

dcc_status_t dcc_application_command_option_builder_set_description_localizations(
    dcc_application_command_option_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
) {
    return dcc_option_set_localizations(builder, localizations, localization_count, 1);
}

static dcc_status_t dcc_command_set_localizations(
    dcc_application_command_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t count,
    int description
) {
    const size_t last = description
        ? offsetof(dcc_application_command_builder_t, description_localization_count)
        : offsetof(dcc_application_command_builder_t, name_localization_count);
    if (!dcc_command_payload_target(builder, last, sizeof(size_t), 0U, 0U, 0U, 0U) ||
        !dcc_localizations_valid(localizations, count)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (description) {
        builder->description_localizations = localizations;
        builder->description_localization_count = count;
        if (count != 0U) {
            builder->description_localizations_json = NULL;
            builder->present &= ~DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON;
        }
        dcc_command_payload_presence(builder,
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS, count != 0U);
    } else {
        builder->name_localizations = localizations;
        builder->name_localization_count = count;
        if (count != 0U) {
            builder->name_localizations_json = NULL;
            builder->present &= ~DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON;
        }
        dcc_command_payload_presence(builder,
            DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS, count != 0U);
    }
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_name_localizations(
    dcc_application_command_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
) {
    return dcc_command_set_localizations(builder, localizations, localization_count, 0);
}

dcc_status_t dcc_application_command_builder_set_description_localizations(
    dcc_application_command_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count
) {
    return dcc_command_set_localizations(builder, localizations, localization_count, 1);
}

dcc_status_t dcc_application_command_builder_set_integration_types(
    dcc_application_command_builder_t *builder,
    const dcc_application_integration_type_t *values,
    size_t count
) {
    if (!dcc_command_payload_target(builder,
            offsetof(dcc_application_command_builder_t, integration_type_count), sizeof(size_t),
            0U, 0U, 0U, 0U) || (count != 0U && values == NULL)) return DCC_ERR_INVALID_ARG;
    for (size_t i = 0U; i < count; ++i) {
        if ((values[i] != DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL &&
             values[i] != DCC_APPLICATION_INTEGRATION_TYPE_USER_INSTALL)) return DCC_ERR_INVALID_ARG;
        for (size_t j = 0U; j < i; ++j) if (values[i] == values[j]) return DCC_ERR_INVALID_ARG;
    }
    builder->integration_types = values;
    builder->integration_type_count = count;
    if (count != 0U) {
        builder->integration_types_json = NULL;
        builder->present &= ~DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON;
    }
    dcc_command_payload_presence(builder,
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES, count != 0U);
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_contexts(
    dcc_application_command_builder_t *builder,
    const dcc_interaction_context_type_t *values,
    size_t count
) {
    if (!dcc_command_payload_target(builder,
            offsetof(dcc_application_command_builder_t, context_count), sizeof(size_t),
            0U, 0U, 0U, 0U) || (count != 0U && values == NULL)) return DCC_ERR_INVALID_ARG;
    for (size_t i = 0U; i < count; ++i) {
        if (values[i] < DCC_INTERACTION_CONTEXT_GUILD ||
            values[i] > DCC_INTERACTION_CONTEXT_PRIVATE_CHANNEL) return DCC_ERR_INVALID_ARG;
        for (size_t j = 0U; j < i; ++j) if (values[i] == values[j]) return DCC_ERR_INVALID_ARG;
    }
    builder->contexts = values;
    builder->context_count = count;
    if (count != 0U) {
        builder->contexts_json = NULL;
        builder->present &= ~DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON;
    }
    dcc_command_payload_presence(builder,
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS, count != 0U);
    return DCC_OK;
}
