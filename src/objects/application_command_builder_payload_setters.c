#include <dcc/application_command.h>

dcc_status_t dcc_application_command_builder_set_name_localizations_json(
    dcc_application_command_builder_t *builder,
    const char *name_localizations_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->name_localizations_json = name_localizations_json;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_description_localizations_json(
    dcc_application_command_builder_t *builder,
    const char *description_localizations_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->description_localizations_json = description_localizations_json;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_options_json(
    dcc_application_command_builder_t *builder,
    const char *options_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->options_json = options_json;
    if (options_json != NULL) {
        builder->options = NULL;
        builder->options_count = 0U;
    }
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_options(
    dcc_application_command_builder_t *builder,
    const dcc_application_command_option_builder_t *options,
    size_t options_count
) {
    if (builder == NULL || (options_count != 0U && options == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->options = options;
    builder->options_count = options_count;
    if (options_count != 0U) {
        builder->options_json = NULL;
    }
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_integration_types_json(
    dcc_application_command_builder_t *builder,
    const char *integration_types_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->integration_types_json = integration_types_json;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_contexts_json(
    dcc_application_command_builder_t *builder,
    const char *contexts_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->contexts_json = contexts_json;
    return DCC_OK;
}
