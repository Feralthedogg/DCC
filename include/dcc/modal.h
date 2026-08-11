#ifndef DCC_MODAL_H
#define DCC_MODAL_H

#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_MODAL_BUILDER_VERSION 1U

#define DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID UINT64_C(1)
#define DCC_MODAL_BUILDER_PRESENT_TITLE (UINT64_C(1) << 1U)
#define DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON (UINT64_C(1) << 2U)
#define DCC_MODAL_BUILDER_PRESENT_COMPONENTS (UINT64_C(1) << 3U)
#define DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2 (UINT64_C(1) << 4U)
#define DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON (UINT64_C(1) << 5U)

typedef struct dcc_modal_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *custom_id;
    const char *title;
    const char *components_json;
    const dcc_component_builder_t *components;
    size_t components_count;
    const dcc_component_v2_builder_t *components_v2;
    size_t components_v2_count;
    const char *components_v2_json;
} dcc_modal_builder_t;

#define DCC_MODAL_BUILDER_INIT \
    { \
        sizeof(dcc_modal_builder_t), DCC_MODAL_BUILDER_VERSION, UINT64_C(0), \
        NULL, NULL, NULL, NULL, 0U, NULL, 0U, NULL \
    }

DCC_API void dcc_modal_builder_init(dcc_modal_builder_t *builder);
DCC_API dcc_status_t dcc_modal_builder_set_custom_id(dcc_modal_builder_t *builder, const char *custom_id);
DCC_API dcc_status_t dcc_modal_builder_set_title(dcc_modal_builder_t *builder, const char *title);
DCC_API dcc_status_t dcc_modal_builder_set_components(
    dcc_modal_builder_t *builder,
    const dcc_component_builder_t *components,
    size_t components_count
);
DCC_API dcc_status_t dcc_modal_builder_set_components_v2(
    dcc_modal_builder_t *builder,
    const dcc_component_v2_builder_t *components,
    size_t components_count
);
DCC_API dcc_status_t dcc_modal_builder_set_components_json(
    dcc_modal_builder_t *builder,
    const char *components_json
);
DCC_API dcc_status_t dcc_modal_builder_set_components_v2_json(
    dcc_modal_builder_t *builder,
    const char *components_json
);
DCC_API dcc_status_t dcc_modal_builder_build_json(const dcc_modal_builder_t *builder, char **out_json);
DCC_API void dcc_modal_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
