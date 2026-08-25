#include "support/component_v2_expected_layout.h"

#include <stdio.h>
#include <string.h>

_Static_assert(sizeof(dcc_component_v2_builder_t) ==
                   sizeof(dcc_test_component_v2_builder_t), "builder size");
_Static_assert(_Alignof(dcc_component_v2_builder_t) ==
                   _Alignof(dcc_test_component_v2_builder_t), "builder align");
_Static_assert(offsetof(dcc_component_v2_builder_t, size) ==
                   offsetof(dcc_test_component_v2_builder_t, size), "size offset");
_Static_assert(offsetof(dcc_component_v2_builder_t, version) ==
                   offsetof(dcc_test_component_v2_builder_t, version), "version offset");
_Static_assert(offsetof(dcc_component_v2_builder_t, present) ==
                   offsetof(dcc_test_component_v2_builder_t, present), "present offset");
_Static_assert(offsetof(dcc_component_v2_builder_t, as) ==
                   offsetof(dcc_test_component_v2_builder_t, as), "union offset");

typedef struct historical_text_display {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_component_v2_type_t type;
  uint32_t id;
  struct { const char *content; } as;
} historical_text_display_t;

static int require_error(dcc_component_v2_builder_t *value,
                         dcc_component_v2_validation_reason_t reason,
                         const char *path) {
  dcc_component_v2_validation_error_t error =
      DCC_COMPONENT_V2_VALIDATION_ERROR_INIT;
  dcc_status_t status = dcc_component_v2_validate(
      value, 1U, DCC_COMPONENT_V2_CONTEXT_ANY, &error);
  if (status == DCC_ERR_INVALID_ARG && error.reason == reason &&
      strcmp(error.field_path, path) == 0)
    return 0;
  fprintf(stderr, "error mismatch: status=%d reason=%d path=%s expected=%d %s\n",
          status, error.reason, error.field_path, reason, path);
  return 1;
}

int main(void) {
  dcc_component_v2_builder_t text = dcc_component_v2_text_display("hello");
  if (text.size != sizeof(text) ||
      text.version != DCC_COMPONENT_V2_BUILDER_VERSION ||
      text.type != DCC_COMPONENT_V2_TEXT_DISPLAY ||
      text.present != dcc_component_v2_field_mask(
                          DCC_COMPONENT_V2_FIELD_CONTENT))
    return 1;

  dcc_component_v2_builder_t invalid = text;
  invalid.size = 0U;
  if (require_error(&invalid, DCC_COMPONENT_V2_VALIDATION_ABI_SIZE,
                    "components[0].size"))
    return 1;
  invalid = text;
  invalid.present |= UINT64_C(1) << 63U;
  if (require_error(&invalid, DCC_COMPONENT_V2_VALIDATION_PRESENCE,
                    "components[0].present"))
    return 1;
  invalid = text;
  invalid.present |= dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_URL);
  if (require_error(&invalid,
                    DCC_COMPONENT_V2_VALIDATION_FIELD_NOT_ALLOWED,
                    "components[0].url"))
    return 1;

  dcc_component_v2_builder_t checkbox =
      dcc_component_v2_checkbox("accept", 0U);
  dcc_component_v2_builder_t before = checkbox;
  if (dcc_component_v2_builder_set_label(&checkbox, "invalid") !=
          DCC_ERR_INVALID_ARG ||
      memcmp(&checkbox, &before, sizeof(checkbox)) != 0)
    return 1;

  before = checkbox;
  if (dcc_component_v2_builder_set_default(&checkbox, 2U) !=
          DCC_ERR_INVALID_ARG ||
      memcmp(&checkbox, &before, sizeof(checkbox)) != 0) {
    fprintf(stderr, "noncanonical boolean setter mutated builder\n");
    return 1;
  }

  historical_text_display_t historical[2] = {
      {sizeof(historical_text_display_t), DCC_COMPONENT_V2_BUILDER_VERSION,
       UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CONTENT,
       DCC_COMPONENT_V2_TEXT_DISPLAY, 0U, {"first"}},
      {sizeof(historical_text_display_t), DCC_COMPONENT_V2_BUILDER_VERSION,
       UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CONTENT,
       DCC_COMPONENT_V2_TEXT_DISPLAY, 0U, {"second"}},
  };
  _Alignas(dcc_component_v2_builder_t)
      unsigned char historical_bytes[2U * sizeof(dcc_component_v2_builder_t)];
  memset(historical_bytes, 0xA5, sizeof(historical_bytes));
  memcpy(historical_bytes, &historical[0], sizeof(historical[0]));
  memcpy(historical_bytes + sizeof(historical[0]), &historical[1],
         sizeof(historical[1]));
  dcc_component_v2_validation_error_t error =
      DCC_COMPONENT_V2_VALIDATION_ERROR_INIT;
  dcc_status_t status = dcc_component_v2_validate(
      (const dcc_component_v2_builder_t *)(const void *)historical_bytes, 2U,
      DCC_COMPONENT_V2_CONTEXT_ANY, &error);
  if (status != DCC_OK) {
    fprintf(stderr, "historical stride failed: %d %d %s\n", status,
            error.reason, error.field_path);
    return 1;
  }
  char *json = NULL;
  status = dcc_component_v2_builder_build_array_json(
      (const dcc_component_v2_builder_t *)(const void *)historical_bytes, 2U,
      &json);
  if (status != DCC_OK || json == NULL ||
      strcmp(json,
             "[{\"type\":10,\"content\":\"first\"},"
             "{\"type\":10,\"content\":\"second\"}]") != 0) {
    fprintf(stderr, "historical build failed: %d %s\n", status,
            json != NULL ? json : "(null)");
    dcc_component_v2_builder_json_free(json);
    return 1;
  }
  dcc_component_v2_builder_json_free(json);

  struct {
    uint64_t before;
    historical_text_display_t value;
    uint64_t after;
  } historical_setter = {
      UINT64_C(0x1122334455667788),
      {sizeof(historical_text_display_t), DCC_COMPONENT_V2_BUILDER_VERSION,
       UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CONTENT,
       DCC_COMPONENT_V2_TEXT_DISPLAY, 0U, {"old"}},
      UINT64_C(0x8877665544332211),
  };
  if (dcc_component_v2_builder_set_content(
          (dcc_component_v2_builder_t *)(void *)&historical_setter.value,
          "updated") != DCC_OK ||
      strcmp(historical_setter.value.as.content, "updated") != 0 ||
      historical_setter.before != UINT64_C(0x1122334455667788) ||
      historical_setter.after != UINT64_C(0x8877665544332211)) {
    fprintf(stderr, "historical covered setter failed\n");
    return 1;
  }
  return 0;
}
