#ifndef DCC_JSON_TYPES_H
#define DCC_JSON_TYPES_H

typedef enum dcc_json_type {
    DCC_JSON_NULL = 0,
    DCC_JSON_BOOL,
    DCC_JSON_NUMBER,
    DCC_JSON_STRING,
    DCC_JSON_ARRAY,
    DCC_JSON_OBJECT
} dcc_json_type_t;

typedef struct dcc_json dcc_json_t;

#endif
