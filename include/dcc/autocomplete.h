#ifndef DCC_AUTOCOMPLETE_H
#define DCC_AUTOCOMPLETE_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_autocomplete_choice_value_type {
    DCC_AUTOCOMPLETE_CHOICE_STRING = 1,
    DCC_AUTOCOMPLETE_CHOICE_INTEGER = 2,
    DCC_AUTOCOMPLETE_CHOICE_NUMBER = 3,
} dcc_autocomplete_choice_value_type_t;

typedef struct dcc_autocomplete_choice {
    const char *name;
    const char *name_localizations_json;
    const char *value_string;
    int64_t value_integer;
    double value_number;
    dcc_autocomplete_choice_value_type_t value_type;
    uint8_t has_name;
    uint8_t has_value;
} dcc_autocomplete_choice_t;

typedef struct dcc_autocomplete_builder {
    const dcc_autocomplete_choice_t *choices;
    size_t choices_count;
} dcc_autocomplete_builder_t;

DCC_API void dcc_autocomplete_choice_init(dcc_autocomplete_choice_t *choice, const char *name);
DCC_API dcc_status_t dcc_autocomplete_choice_set_name(dcc_autocomplete_choice_t *choice, const char *name);
DCC_API dcc_status_t dcc_autocomplete_choice_set_name_localizations_json(
    dcc_autocomplete_choice_t *choice,
    const char *name_localizations_json
);
DCC_API dcc_status_t dcc_autocomplete_choice_set_string_value(
    dcc_autocomplete_choice_t *choice,
    const char *value
);
DCC_API dcc_status_t dcc_autocomplete_choice_set_integer_value(
    dcc_autocomplete_choice_t *choice,
    int64_t value
);
DCC_API dcc_status_t dcc_autocomplete_choice_set_number_value(
    dcc_autocomplete_choice_t *choice,
    double value
);

DCC_API void dcc_autocomplete_builder_init(dcc_autocomplete_builder_t *builder);
DCC_API dcc_status_t dcc_autocomplete_builder_set_choices(
    dcc_autocomplete_builder_t *builder,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count
);
DCC_API dcc_status_t dcc_autocomplete_builder_build_json(
    const dcc_autocomplete_builder_t *builder,
    char **out_json
);
DCC_API void dcc_autocomplete_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
