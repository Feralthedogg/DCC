#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stdlib.h>
#include <string.h>

static char *dcc_command_registry_strdup_or_null(const char *value) {
    if (value == NULL) {
        return NULL;
    }
    size_t len = strlen(value);
    if (len == SIZE_MAX) {
        return NULL;
    }
    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1U);
    return copy;
}

static dcc_status_t dcc_command_registry_copy_string(const char **dst, const char *src) {
    *dst = NULL;
    if (src == NULL) {
        return DCC_OK;
    }
    char *copy = dcc_command_registry_strdup_or_null(src);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    *dst = copy;
    return DCC_OK;
}

static dcc_status_t dcc_command_registry_copy_choices(
    dcc_autocomplete_choice_t **dst,
    const dcc_autocomplete_choice_t *src,
    size_t count
) {
    *dst = NULL;
    if (count == 0U) {
        return DCC_OK;
    }
    if (src == NULL || count > SIZE_MAX / sizeof(**dst)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_autocomplete_choice_t *choices = (dcc_autocomplete_choice_t *)calloc(count, sizeof(*choices));
    if (choices == NULL) {
        return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < count; ++i) {
        choices[i] = src[i];
        choices[i].name = NULL;
        choices[i].name_localizations_json = NULL;
        choices[i].value_string = NULL;
        dcc_status_t status = dcc_command_registry_copy_string(&choices[i].name, src[i].name);
        if (status == DCC_OK) {
            status = dcc_command_registry_copy_string(
                &choices[i].name_localizations_json,
                src[i].name_localizations_json
            );
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_copy_string(&choices[i].value_string, src[i].value_string);
        }
        if (status != DCC_OK) {
            for (size_t j = 0U; j <= i; ++j) {
                free((char *)choices[j].name);
                free((char *)choices[j].name_localizations_json);
                free((char *)choices[j].value_string);
            }
            free(choices);
            return status;
        }
    }
    *dst = choices;
    return DCC_OK;
}

static void dcc_command_registry_option_deinit(dcc_application_command_option_builder_t *option);

static dcc_status_t dcc_command_registry_copy_options(
    dcc_application_command_option_builder_t **dst,
    const dcc_application_command_option_builder_t *src,
    size_t count
) {
    *dst = NULL;
    if (count == 0U) {
        return DCC_OK;
    }
    if (src == NULL || count > SIZE_MAX / sizeof(**dst)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_application_command_option_builder_t *options =
        (dcc_application_command_option_builder_t *)calloc(count, sizeof(*options));
    if (options == NULL) {
        return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < count; ++i) {
        dcc_status_t status = DCC_OK;
        options[i] = src[i];
        options[i].name = NULL;
        options[i].description = NULL;
        options[i].name_localizations_json = NULL;
        options[i].description_localizations_json = NULL;
        options[i].choices_json = NULL;
        options[i].choices = NULL;
        options[i].options_json = NULL;
        options[i].options = NULL;
        options[i].channel_types = NULL;

        status = dcc_command_registry_copy_string(&options[i].name, src[i].name);
        if (status == DCC_OK) {
            status = dcc_command_registry_copy_string(&options[i].description, src[i].description);
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_copy_string(
                &options[i].name_localizations_json,
                src[i].name_localizations_json
            );
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_copy_string(
                &options[i].description_localizations_json,
                src[i].description_localizations_json
            );
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_copy_string(&options[i].choices_json, src[i].choices_json);
        }
        if (status == DCC_OK) {
            dcc_autocomplete_choice_t *choices = NULL;
            status = dcc_command_registry_copy_choices(&choices, src[i].choices, src[i].choices_count);
            options[i].choices = choices;
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_copy_string(&options[i].options_json, src[i].options_json);
        }
        if (status == DCC_OK) {
            dcc_application_command_option_builder_t *children = NULL;
            status = dcc_command_registry_copy_options(&children, src[i].options, src[i].options_count);
            options[i].options = children;
        }
        if (status == DCC_OK && src[i].channel_types_count != 0U) {
            if (src[i].channel_types == NULL ||
                src[i].channel_types_count > SIZE_MAX / sizeof(*src[i].channel_types)) {
                status = DCC_ERR_INVALID_ARG;
            } else {
                uint32_t *channel_types =
                    (uint32_t *)malloc(src[i].channel_types_count * sizeof(*channel_types));
                if (channel_types == NULL) {
                    status = DCC_ERR_NOMEM;
                } else {
                    memcpy(
                        channel_types,
                        src[i].channel_types,
                        src[i].channel_types_count * sizeof(*channel_types)
                    );
                    options[i].channel_types = channel_types;
                }
            }
        }
        if (status != DCC_OK) {
            dcc_command_registry_option_deinit(&options[i]);
            for (size_t j = 0U; j < i; ++j) {
                dcc_command_registry_option_deinit(&options[j]);
            }
            free(options);
            return status;
        }
    }
    *dst = options;
    return DCC_OK;
}

static void dcc_command_registry_option_deinit(dcc_application_command_option_builder_t *option) {
    if (option == NULL) {
        return;
    }
    free((char *)option->name);
    free((char *)option->description);
    free((char *)option->name_localizations_json);
    free((char *)option->description_localizations_json);
    free((char *)option->choices_json);
    if (option->choices != NULL) {
        for (size_t i = 0U; i < option->choices_count; ++i) {
            free((char *)option->choices[i].name);
            free((char *)option->choices[i].name_localizations_json);
            free((char *)option->choices[i].value_string);
        }
        free((dcc_autocomplete_choice_t *)option->choices);
    }
    free((char *)option->options_json);
    if (option->options != NULL) {
        for (size_t i = 0U; i < option->options_count; ++i) {
            dcc_command_registry_option_deinit(
                (dcc_application_command_option_builder_t *)&option->options[i]
            );
        }
        free((dcc_application_command_option_builder_t *)option->options);
    }
    free((uint32_t *)option->channel_types);
    memset(option, 0, sizeof(*option));
}

dcc_status_t dcc_command_registry_builder_copy(
    dcc_application_command_builder_t *dst,
    const dcc_application_command_builder_t *src
) {
    if (dst == NULL || src == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(dst, 0, sizeof(*dst));
    *dst = *src;
    dst->name = NULL;
    dst->name_localizations_json = NULL;
    dst->description = NULL;
    dst->description_localizations_json = NULL;
    dst->options_json = NULL;
    dst->options = NULL;
    dst->integration_types_json = NULL;
    dst->contexts_json = NULL;

    dcc_status_t status = dcc_command_registry_copy_string(&dst->name, src->name);
    if (status == DCC_OK) {
        status = dcc_command_registry_copy_string(
            &dst->name_localizations_json,
            src->name_localizations_json
        );
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_copy_string(&dst->description, src->description);
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_copy_string(
            &dst->description_localizations_json,
            src->description_localizations_json
        );
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_copy_string(&dst->options_json, src->options_json);
    }
    if (status == DCC_OK) {
        dcc_application_command_option_builder_t *options = NULL;
        status = dcc_command_registry_copy_options(&options, src->options, src->options_count);
        dst->options = options;
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_copy_string(
            &dst->integration_types_json,
            src->integration_types_json
        );
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_copy_string(&dst->contexts_json, src->contexts_json);
    }
    if (status != DCC_OK) {
        dcc_command_registry_builder_deinit(dst);
    }
    return status;
}

void dcc_command_registry_builder_deinit(dcc_application_command_builder_t *builder) {
    if (builder == NULL) {
        return;
    }
    free((char *)builder->name);
    free((char *)builder->name_localizations_json);
    free((char *)builder->description);
    free((char *)builder->description_localizations_json);
    free((char *)builder->options_json);
    if (builder->options != NULL) {
        for (size_t i = 0U; i < builder->options_count; ++i) {
            dcc_command_registry_option_deinit(
                (dcc_application_command_option_builder_t *)&builder->options[i]
            );
        }
        free((dcc_application_command_option_builder_t *)builder->options);
    }
    free((char *)builder->integration_types_json);
    free((char *)builder->contexts_json);
    memset(builder, 0, sizeof(*builder));
}
