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
