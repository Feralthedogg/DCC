#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

static atomic_size_t dcc_command_registry_copy_allocations_before_failure =
    ATOMIC_VAR_INIT(SIZE_MAX);

void dcc_command_registry_test_fail_copy_after(size_t successful_allocations) {
    atomic_store_explicit(
        &dcc_command_registry_copy_allocations_before_failure,
        successful_allocations,
        memory_order_release
    );
}

static int dcc_command_registry_copy_allocation_allowed(void) {
    size_t remaining = atomic_load_explicit(
        &dcc_command_registry_copy_allocations_before_failure,
        memory_order_acquire
    );
    while (remaining != SIZE_MAX) {
        if (remaining == 0U) {
            return 0;
        }
        if (atomic_compare_exchange_weak_explicit(
                &dcc_command_registry_copy_allocations_before_failure,
                &remaining,
                remaining - 1U,
                memory_order_acq_rel,
                memory_order_acquire
            )) {
            return 1;
        }
    }
    return 1;
}

static void *dcc_command_registry_copy_malloc(size_t size) {
    return dcc_command_registry_copy_allocation_allowed() ? malloc(size) : NULL;
}

static void *dcc_command_registry_copy_calloc(size_t count, size_t size) {
    return dcc_command_registry_copy_allocation_allowed() ? calloc(count, size) : NULL;
}

static dcc_status_t dcc_command_registry_copy_string(const char **dst, const char *src) {
    *dst = NULL;
    if (src == NULL) {
        return DCC_OK;
    }
    size_t len = strlen(src);
    if (len == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    char *copy = (char *)dcc_command_registry_copy_malloc(len + 1U);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(copy, src, len + 1U);
    *dst = copy;
    return DCC_OK;
}

static void dcc_command_registry_choice_deinit(dcc_autocomplete_choice_t *choice) {
    if (choice == NULL) {
        return;
    }
    free((char *)choice->name);
    free((char *)choice->name_localizations_json);
    free((char *)choice->value_string);
    memset(choice, 0, sizeof(*choice));
}

static void dcc_command_registry_option_deinit(dcc_application_command_option_builder_t *option);

static dcc_status_t dcc_command_registry_copy_choices(
    dcc_autocomplete_choice_t **dst,
    const dcc_autocomplete_choice_t *src,
    size_t count
) {
    *dst = NULL;
    if (count == 0U) {
        return DCC_OK;
    }
    size_t stride = 0U;
    if (dcc_autocomplete_choice_array_begin(src, count, &stride) != DCC_OK ||
        count > SIZE_MAX / sizeof(**dst)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_autocomplete_choice_t *choices =
        (dcc_autocomplete_choice_t *)dcc_command_registry_copy_calloc(count, sizeof(*choices));
    if (choices == NULL) {
        return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < count; ++i) {
        const dcc_autocomplete_choice_t *source = (const dcc_autocomplete_choice_t *)
            dcc_builder_abi_array_at(src, stride, i);
        dcc_builder_abi_view_t view;
        dcc_status_t status = dcc_autocomplete_choice_abi_validate(source, &view);
        if (status != DCC_OK || view.size != stride) {
            status = DCC_ERR_INVALID_ARG;
        }
        dcc_autocomplete_choice_init(&choices[i], NULL);
        if (status == DCC_OK) {
            choices[i].present = view.present;
        }
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME
            )) {
            status = dcc_command_registry_copy_string(&choices[i].name, source->name);
        }
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON
            )) {
            status = dcc_command_registry_copy_string(
                &choices[i].name_localizations_json, source->name_localizations_json
            );
        }
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE
            )) {
            choices[i].value_type = source->value_type;
            choices[i].value_integer = source->value_integer;
            choices[i].value_number = source->value_number;
            if (source->value_type == DCC_AUTOCOMPLETE_CHOICE_STRING) {
                status = dcc_command_registry_copy_string(
                    &choices[i].value_string, source->value_string
                );
            }
        }
        if (status != DCC_OK) {
            for (size_t j = 0U; j <= i; ++j) {
                dcc_command_registry_choice_deinit(&choices[j]);
            }
            free(choices);
            return status;
        }
    }
    *dst = choices;
    return DCC_OK;
}

static dcc_status_t dcc_command_registry_copy_options(
    dcc_application_command_option_builder_t **dst,
    const dcc_application_command_option_builder_t *src,
    size_t count
) {
    *dst = NULL;
    if (count == 0U) {
        return DCC_OK;
    }
    size_t stride = 0U;
    if (dcc_application_command_option_builder_array_begin(src, count, &stride) != DCC_OK ||
        count > SIZE_MAX / sizeof(**dst)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_application_command_option_builder_t *options =
        (dcc_application_command_option_builder_t *)dcc_command_registry_copy_calloc(
            count, sizeof(*options)
        );
    if (options == NULL) {
        return DCC_ERR_NOMEM;
    }

    for (size_t i = 0U; i < count; ++i) {
        const dcc_application_command_option_builder_t *source =
            (const dcc_application_command_option_builder_t *)
                dcc_builder_abi_array_at(src, stride, i);
        dcc_builder_abi_view_t view;
        dcc_status_t status = dcc_application_command_option_builder_abi_validate(
            source, &view
        );
        if (status != DCC_OK || view.size != stride) {
            status = DCC_ERR_INVALID_ARG;
        }
        dcc_application_command_option_builder_init(&options[i]);
        if (status == DCC_OK) {
            options[i].present = view.present;
        }
#define DCC_COPY_OPTION_STRING(bit_, field_) \
        if (status == DCC_OK && dcc_builder_abi_view_has(&view, (bit_))) { \
            status = dcc_command_registry_copy_string(&options[i].field_, source->field_); \
        }
        DCC_COPY_OPTION_STRING(DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME, name)
        DCC_COPY_OPTION_STRING(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION, description
        )
        DCC_COPY_OPTION_STRING(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON,
            name_localizations_json
        )
        DCC_COPY_OPTION_STRING(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON,
            description_localizations_json
        )
        DCC_COPY_OPTION_STRING(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES_JSON, choices_json
        )
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES
            )) {
            dcc_autocomplete_choice_t *choices = NULL;
            status = dcc_command_registry_copy_choices(
                &choices, source->choices, source->choices_count
            );
            options[i].choices = choices;
            options[i].choices_count = source->choices_count;
        }
        DCC_COPY_OPTION_STRING(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS_JSON, options_json
        )
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS
            )) {
            dcc_application_command_option_builder_t *children = NULL;
            status = dcc_command_registry_copy_options(
                &children, source->options, source->options_count
            );
            options[i].options = children;
            options[i].options_count = source->options_count;
        }
        if (status == DCC_OK && dcc_builder_abi_view_has(
                &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHANNEL_TYPES
            )) {
            options[i].channel_types_count = source->channel_types_count;
            if (source->channel_types_count != 0U) {
                if (source->channel_types == NULL ||
                    source->channel_types_count > SIZE_MAX / sizeof(*source->channel_types)) {
                    status = DCC_ERR_INVALID_ARG;
                } else {
                    uint32_t *types = (uint32_t *)dcc_command_registry_copy_malloc(
                        source->channel_types_count * sizeof(*types)
                    );
                    if (types == NULL) {
                        status = DCC_ERR_NOMEM;
                    } else {
                        memcpy(
                            types,
                            source->channel_types,
                            source->channel_types_count * sizeof(*types)
                        );
                        options[i].channel_types = types;
                    }
                }
            }
        }
#define DCC_COPY_OPTION_SCALAR(bit_, field_) \
        if (status == DCC_OK && dcc_builder_abi_view_has(&view, (bit_))) { \
            options[i].field_ = source->field_; \
        }
        DCC_COPY_OPTION_SCALAR(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_INTEGER_VALUE,
            min_integer_value
        )
        DCC_COPY_OPTION_SCALAR(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_INTEGER_VALUE,
            max_integer_value
        )
        DCC_COPY_OPTION_SCALAR(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_NUMBER_VALUE,
            min_number_value
        )
        DCC_COPY_OPTION_SCALAR(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_NUMBER_VALUE,
            max_number_value
        )
        DCC_COPY_OPTION_SCALAR(DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE, type)
        DCC_COPY_OPTION_SCALAR(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED, required
        )
        DCC_COPY_OPTION_SCALAR(
            DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_AUTOCOMPLETE, autocomplete
        )
#undef DCC_COPY_OPTION_SCALAR
#undef DCC_COPY_OPTION_STRING
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
            dcc_command_registry_choice_deinit(
                &((dcc_autocomplete_choice_t *)option->choices)[i]
            );
        }
        free((dcc_autocomplete_choice_t *)option->choices);
    }
    free((char *)option->options_json);
    if (option->options != NULL) {
        for (size_t i = 0U; i < option->options_count; ++i) {
            dcc_command_registry_option_deinit(
                &((dcc_application_command_option_builder_t *)option->options)[i]
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
    dcc_application_command_builder_init(dst);
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_application_command_builder_abi_validate(src, &view);
    if (status != DCC_OK) {
        return status;
    }
    char *validation_json = NULL;
    status = dcc_application_command_builder_build_json(src, &validation_json);
    dcc_application_command_builder_json_free(validation_json);
    if (status != DCC_OK) {
        return status;
    }
    dst->present = view.present;
#define DCC_COPY_COMMAND_STRING(bit_, field_) \
    if (status == DCC_OK && dcc_builder_abi_view_has(&view, (bit_))) { \
        status = dcc_command_registry_copy_string(&dst->field_, src->field_); \
    }
    DCC_COPY_COMMAND_STRING(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME, name)
    DCC_COPY_COMMAND_STRING(
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON,
        name_localizations_json
    )
    DCC_COPY_COMMAND_STRING(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION, description)
    DCC_COPY_COMMAND_STRING(
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON,
        description_localizations_json
    )
    DCC_COPY_COMMAND_STRING(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON, options_json)
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS
        )) {
        dcc_application_command_option_builder_t *options = NULL;
        status = dcc_command_registry_copy_options(&options, src->options, src->options_count);
        dst->options = options;
        dst->options_count = src->options_count;
    }
    DCC_COPY_COMMAND_STRING(
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON,
        integration_types_json
    )
    DCC_COPY_COMMAND_STRING(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON, contexts_json)
#undef DCC_COPY_COMMAND_STRING
#define DCC_COPY_COMMAND_SCALAR(bit_, field_) \
    if (status == DCC_OK && dcc_builder_abi_view_has(&view, (bit_))) { \
        dst->field_ = src->field_; \
    }
    DCC_COPY_COMMAND_SCALAR(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE, type)
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS
        )) {
        dst->default_member_permissions = src->default_member_permissions;
        dst->default_member_permissions_null = src->default_member_permissions_null;
    }
    DCC_COPY_COMMAND_SCALAR(
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DM_PERMISSION, dm_permission
    )
    DCC_COPY_COMMAND_SCALAR(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NSFW, nsfw)
#undef DCC_COPY_COMMAND_SCALAR
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
                &((dcc_application_command_option_builder_t *)builder->options)[i]
            );
        }
        free((dcc_application_command_option_builder_t *)builder->options);
    }
    free((char *)builder->integration_types_json);
    free((char *)builder->contexts_json);
    memset(builder, 0, sizeof(*builder));
}
