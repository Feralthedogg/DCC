#ifndef DCC_AUTOCOMPLETE_H
#define DCC_AUTOCOMPLETE_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_AUTOCOMPLETE_MAX_CHOICES 25U
#define DCC_AUTOCOMPLETE_CHOICE_VERSION 1U
#define DCC_AUTOCOMPLETE_BUILDER_VERSION 1U

#define DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME UINT64_C(1)
#define DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON (UINT64_C(1) << 1U)
#define DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE (UINT64_C(1) << 2U)
#define DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES UINT64_C(1)

typedef enum dcc_autocomplete_choice_value_type {
    DCC_AUTOCOMPLETE_CHOICE_STRING = 1,
    DCC_AUTOCOMPLETE_CHOICE_INTEGER = 2,
    DCC_AUTOCOMPLETE_CHOICE_NUMBER = 3,
} dcc_autocomplete_choice_value_type_t;

typedef struct dcc_autocomplete_choice {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *name;
    const char *value_string;
    int64_t value_integer;
    double value_number;
    dcc_autocomplete_choice_value_type_t value_type;
    /* Reserved ABI padding: initialize to zero; version 1 ignores this field. */
    uint32_t abi_padding;
    const char *name_localizations_json;
} dcc_autocomplete_choice_t;

typedef struct dcc_autocomplete_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    const dcc_autocomplete_choice_t *choices;
    size_t choices_count;
} dcc_autocomplete_builder_t;

#define DCC_AUTOCOMPLETE_CHOICE_INIT \
    { \
        sizeof(dcc_autocomplete_choice_t), DCC_AUTOCOMPLETE_CHOICE_VERSION, \
        UINT64_C(0), NULL, NULL, INT64_C(0), 0.0, \
        (dcc_autocomplete_choice_value_type_t)0, 0U, NULL \
    }
#define DCC_AUTOCOMPLETE_CHOICE_NAMED_INIT(name_) \
    { \
        sizeof(dcc_autocomplete_choice_t), DCC_AUTOCOMPLETE_CHOICE_VERSION, \
        DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME, (name_), NULL, INT64_C(0), 0.0, \
        (dcc_autocomplete_choice_value_type_t)0, 0U, NULL \
    }
#define DCC_AUTOCOMPLETE_BUILDER_INIT \
    { \
        sizeof(dcc_autocomplete_builder_t), DCC_AUTOCOMPLETE_BUILDER_VERSION, \
        UINT64_C(0), NULL, 0U \
    }

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
