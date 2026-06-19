#ifndef DCC_MODAL_H
#define DCC_MODAL_H

#include <dcc/component.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_modal_builder {
    const char *custom_id;
    const char *title;
    const char *components_json;
    const dcc_component_builder_t *components;
    size_t components_count;
    uint8_t has_custom_id;
    uint8_t has_title;
} dcc_modal_builder_t;

DCC_API void dcc_modal_builder_init(dcc_modal_builder_t *builder);
DCC_API dcc_status_t dcc_modal_builder_set_custom_id(dcc_modal_builder_t *builder, const char *custom_id);
DCC_API dcc_status_t dcc_modal_builder_set_title(dcc_modal_builder_t *builder, const char *title);
DCC_API dcc_status_t dcc_modal_builder_set_components(
    dcc_modal_builder_t *builder,
    const dcc_component_builder_t *components,
    size_t components_count
);
DCC_API dcc_status_t dcc_modal_builder_set_components_json(
    dcc_modal_builder_t *builder,
    const char *components_json
);
DCC_API dcc_status_t dcc_modal_builder_build_json(const dcc_modal_builder_t *builder, char **out_json);
DCC_API void dcc_modal_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
