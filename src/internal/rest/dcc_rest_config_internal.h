#ifndef DCC_REST_CONFIG_INTERNAL_H
#define DCC_REST_CONFIG_INTERNAL_H

#include <stddef.h>

#define DCC_REST_MULTIPART_BOUNDARY "dcc-boundary-7MA4YWxkTrZu0gW"
#define DCC_REST_MULTIPART_CONTENT_TYPE "multipart/form-data; boundary=" DCC_REST_MULTIPART_BOUNDARY
#define DCC_REST_MAX_RATE_LIMIT_RETRIES 3U
#define DCC_REST_FIELD_END(type, field) (offsetof(type, field) + sizeof(((type *)0)->field))
#define DCC_REST_HAS_FIELD(params, type, field) ((params)->size >= DCC_REST_FIELD_END(type, field))

#endif
