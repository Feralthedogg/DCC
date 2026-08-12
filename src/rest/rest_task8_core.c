#include "internal/rest/dcc_rest_sensitive_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"

#include <stdlib.h>
#include <string.h>

static void dcc_task8_json_free(char *json,
                                const dcc_rest_call_options_t *resolved) {
  if (json == NULL)
    return;
  size_t byte_count = strlen(json) + 1U;
  if (resolved != NULL &&
      (resolved->flags & DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY) != 0U) {
    dcc_rest_sensitive_free(json, byte_count);
  } else {
    free(json);
  }
}

dcc_status_t dcc_task8_prepare(dcc_client_t *client,
                               const dcc_rest_call_options_t *options,
                               dcc_endpoint_auth_policy_t auth,
                               dcc_endpoint_audit_policy_t audit,
                               dcc_rest_request_t **out_request,
                               dcc_rest_call_options_t *resolved) {
  dcc_status_t status =
      dcc_endpoint_prepare_policy(options, out_request, resolved, auth, audit);
  if (status != DCC_OK || client == NULL) {
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  }
  return DCC_OK;
}

dcc_status_t dcc_task8_submit(dcc_client_t *client, const char *operation,
                              dcc_rest_method_t method, const char *path,
                              char *json,
                              const dcc_rest_call_options_t *resolved,
                              dcc_rest_request_t **out_request) {
  dcc_endpoint_body_t body = {json, json != NULL ? strlen(json) : 0U,
                              "application/json"};
  dcc_status_t status = dcc_endpoint_submit_named(
      client, operation, method, path, json != NULL ? &body : NULL, resolved,
      DCC_ENDPOINT_PATH_PUBLIC, out_request);
  dcc_task8_json_free(json, resolved);
  return status;
}

dcc_status_t dcc_task8_submit_built(dcc_status_t build_status,
                                    dcc_client_t *client, const char *operation,
                                    dcc_rest_method_t method, const char *path,
                                    char *json,
                                    const dcc_rest_call_options_t *resolved,
                                    dcc_rest_request_t **out_request) {
  if (build_status != DCC_OK) {
    dcc_task8_json_free(json, resolved);
    return build_status;
  }
  return dcc_task8_submit(client, operation, method, path, json, resolved,
                          out_request);
}

dcc_status_t dcc_task8_empty_object(char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_rest_buffer_t body = {0};
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{}");
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  *out = body.data;
  return DCC_OK;
}

int dcc_task8_iso8601_valid(const char *value) {
  if (value == NULL)
    return 0;
  size_t length = strlen(value);
  if (length < 20U || value[4] != '-' || value[7] != '-' ||
      (value[10] != 'T' && value[10] != 't') || value[13] != ':' ||
      value[16] != ':')
    return 0;
  static const unsigned digit_positions[] = {0U, 1U,  2U,  3U,  5U,  6U,  8U,
                                             9U, 11U, 12U, 14U, 15U, 17U, 18U};
  for (size_t index = 0U;
       index < sizeof(digit_positions) / sizeof(digit_positions[0]); ++index) {
    char digit = value[digit_positions[index]];
    if (digit < '0' || digit > '9')
      return 0;
  }
  unsigned year = (unsigned)(value[0] - '0') * 1000U +
                  (unsigned)(value[1] - '0') * 100U +
                  (unsigned)(value[2] - '0') * 10U + (unsigned)(value[3] - '0');
  unsigned month =
      (unsigned)(value[5] - '0') * 10U + (unsigned)(value[6] - '0');
  unsigned day = (unsigned)(value[8] - '0') * 10U + (unsigned)(value[9] - '0');
  unsigned hour =
      (unsigned)(value[11] - '0') * 10U + (unsigned)(value[12] - '0');
  unsigned minute =
      (unsigned)(value[14] - '0') * 10U + (unsigned)(value[15] - '0');
  unsigned second =
      (unsigned)(value[17] - '0') * 10U + (unsigned)(value[18] - '0');
  static const unsigned month_days[] = {0U,  31U, 28U, 31U, 30U, 31U, 30U,
                                        31U, 31U, 30U, 31U, 30U, 31U};
  if (month < 1U || month > 12U || hour > 23U || minute > 59U || second > 60U)
    return 0;
  unsigned days = month_days[month];
  if (month == 2U && (year % 4U == 0U) &&
      (year % 100U != 0U || year % 400U == 0U))
    ++days;
  if (day < 1U || day > days)
    return 0;

  size_t index = 19U;
  if (index < length && value[index] == '.') {
    ++index;
    size_t fraction_start = index;
    while (index < length && value[index] >= '0' && value[index] <= '9')
      ++index;
    if (index == fraction_start)
      return 0;
  }
  if (index + 1U == length && (value[index] == 'Z' || value[index] == 'z'))
    return 1;
  if (index + 6U != length || (value[index] != '+' && value[index] != '-') ||
      value[index + 3U] != ':')
    return 0;
  for (size_t offset = 1U; offset <= 5U; ++offset) {
    if (offset == 3U)
      continue;
    if (value[index + offset] < '0' || value[index + offset] > '9')
      return 0;
  }
  unsigned offset_hour = (unsigned)(value[index + 1U] - '0') * 10U +
                         (unsigned)(value[index + 2U] - '0');
  unsigned offset_minute = (unsigned)(value[index + 4U] - '0') * 10U +
                           (unsigned)(value[index + 5U] - '0');
  return offset_hour <= 23U && offset_minute <= 59U;
}

static uint32_t dcc_task8_decode_scalar(const unsigned char *text) {
  if (text[0] < 0x80U)
    return text[0];
  if (text[0] < 0xE0U)
    return ((uint32_t)(text[0] & 0x1FU) << 6U) | (uint32_t)(text[1] & 0x3FU);
  if (text[0] < 0xF0U)
    return ((uint32_t)(text[0] & 0x0FU) << 12U) |
           ((uint32_t)(text[1] & 0x3FU) << 6U) | (uint32_t)(text[2] & 0x3FU);
  return ((uint32_t)(text[0] & 0x07U) << 18U) |
         ((uint32_t)(text[1] & 0x3FU) << 12U) |
         ((uint32_t)(text[2] & 0x3FU) << 6U) | (uint32_t)(text[3] & 0x3FU);
}

static int dcc_task8_scalar_is_whitespace(uint32_t scalar) {
  return (scalar >= UINT32_C(0x0009) && scalar <= UINT32_C(0x000D)) ||
         scalar == UINT32_C(0x0020) || scalar == UINT32_C(0x0085) ||
         scalar == UINT32_C(0x00A0) || scalar == UINT32_C(0x1680) ||
         (scalar >= UINT32_C(0x2000) && scalar <= UINT32_C(0x200A)) ||
         scalar == UINT32_C(0x2028) || scalar == UINT32_C(0x2029) ||
         scalar == UINT32_C(0x202F) || scalar == UINT32_C(0x205F) ||
         scalar == UINT32_C(0x3000);
}

int dcc_task8_text_has_edge_whitespace(const char *value) {
  if (value == NULL || value[0] == '\0')
    return 0;
  const unsigned char *start = (const unsigned char *)value;
  const unsigned char *end = start + strlen(value);
  const unsigned char *last = end - 1U;
  while (last > start && (*last & 0xC0U) == 0x80U)
    --last;
  return dcc_task8_scalar_is_whitespace(dcc_task8_decode_scalar(start)) ||
         dcc_task8_scalar_is_whitespace(dcc_task8_decode_scalar(last));
}
