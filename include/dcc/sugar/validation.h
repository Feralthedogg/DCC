#ifndef DCC_SUGAR_VALIDATION_H
#define DCC_SUGAR_VALIDATION_H

#include <dcc/app.h>
#include <stddef.h>

#define DCC_VALIDATE_STRING_REQUIRED(type_, field_, name_) \
    ((dcc_ctx_field_validator_t){ \
        .size = sizeof(dcc_ctx_field_validator_t), \
        .field = (name_), \
        .type = DCC_CTX_FIELD_VALIDATE_STRING_REQUIRED, \
        .field_offset = offsetof(type_, field_) \
    })

#define DCC_VALIDATE_STRING_LENGTH(type_, field_, name_, min_, max_) \
    ((dcc_ctx_field_validator_t){ \
        .size = sizeof(dcc_ctx_field_validator_t), \
        .field = (name_), \
        .type = DCC_CTX_FIELD_VALIDATE_STRING_LENGTH, \
        .field_offset = offsetof(type_, field_), \
        .min_length = (min_), \
        .max_length = (max_) \
    })

#define DCC_VALIDATE_INT_RANGE(type_, field_, name_, min_, max_) \
    ((dcc_ctx_field_validator_t){ \
        .size = sizeof(dcc_ctx_field_validator_t), \
        .field = (name_), \
        .type = DCC_CTX_FIELD_VALIDATE_INTEGER_RANGE, \
        .field_offset = offsetof(type_, field_), \
        .min_integer = (min_), \
        .max_integer = (max_) \
    })

#define DCC_VALIDATE_NUMBER_RANGE(type_, field_, name_, min_, max_) \
    ((dcc_ctx_field_validator_t){ \
        .size = sizeof(dcc_ctx_field_validator_t), \
        .field = (name_), \
        .type = DCC_CTX_FIELD_VALIDATE_NUMBER_RANGE, \
        .field_offset = offsetof(type_, field_), \
        .min_number = (min_), \
        .max_number = (max_) \
    })

#define DCC_VALIDATE_DATE_YYYYMMDD(type_, field_, name_) \
    ((dcc_ctx_field_validator_t){ \
        .size = sizeof(dcc_ctx_field_validator_t), \
        .field = (name_), \
        .type = DCC_CTX_FIELD_VALIDATE_DATE_YYYYMMDD, \
        .field_offset = offsetof(type_, field_) \
    })

#define DCC_VALIDATE_DATE_MMDD(type_, field_, name_) \
    ((dcc_ctx_field_validator_t){ \
        .size = sizeof(dcc_ctx_field_validator_t), \
        .field = (name_), \
        .type = DCC_CTX_FIELD_VALIDATE_DATE_MMDD, \
        .field_offset = offsetof(type_, field_) \
    })

#define DCC_VALIDATE_DATE_MMDD_OR_YYYYMMDD(type_, field_, name_) \
    ((dcc_ctx_field_validator_t){ \
        .size = sizeof(dcc_ctx_field_validator_t), \
        .field = (name_), \
        .type = DCC_CTX_FIELD_VALIDATE_DATE_MMDD_OR_YYYYMMDD, \
        .field_offset = offsetof(type_, field_) \
    })

#define DCC_VALIDATE_BIRTHDAY(type_, field_, name_) \
    DCC_VALIDATE_DATE_MMDD_OR_YYYYMMDD(type_, field_, name_)

#define DCC_PARSE_DATE(value_, out_) dcc_ctx_parse_date((value_), (out_))
#define DCC_PARSE_BIRTHDAY(value_, out_) DCC_PARSE_DATE((value_), (out_))
#define DCC_PARSE_DATE_MMDD(value_, out_) dcc_ctx_parse_date_mmdd((value_), (out_))
#define DCC_PARSE_DATE_YYYYMMDD(value_, out_) dcc_ctx_parse_date_yyyymmdd((value_), (out_))

#define DCC_CTX_VALIDATE_FIELDS(base_, out_error_, ...) \
    dcc_ctx_validate_fields( \
        (base_), \
        ((dcc_ctx_field_validator_t[]){ __VA_ARGS__ }), \
        sizeof((dcc_ctx_field_validator_t[]){ __VA_ARGS__ }) / sizeof(dcc_ctx_field_validator_t), \
        (out_error_) \
    )

#define DCC_CTX_VALIDATE_OR_REPLY(ctx_, base_, ...) \
    do { \
        dcc_ctx_validation_error_t dcc_validation_error_; \
        dcc_status_t dcc_validation_status_ = DCC_CTX_VALIDATE_FIELDS( \
            (base_), \
            &dcc_validation_error_, \
            __VA_ARGS__ \
        ); \
        if (dcc_validation_status_ != DCC_OK) { \
            (void)dcc_ctx_reply_validation_result( \
                (ctx_), \
                dcc_validation_status_, \
                &dcc_validation_error_, \
                NULL, \
                NULL \
            ); \
            return; \
        } \
    } while (0)

#define DCC_CTX_HANDLE_ERROR(ctx_, status_, message_) \
    dcc_ctx_handle_error((ctx_), (status_), (message_))

#define DCC_CTX_TRY(ctx_, expression_) \
    do { \
        dcc_status_t dcc_try_status_ = (expression_); \
        if (dcc_try_status_ != DCC_OK) { \
            (void)dcc_ctx_handle_error((ctx_), dcc_try_status_, dcc_status_string(dcc_try_status_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_TRY_MSG(ctx_, expression_, message_) \
    do { \
        dcc_status_t dcc_try_status_ = (expression_); \
        if (dcc_try_status_ != DCC_OK) { \
            (void)dcc_ctx_handle_error((ctx_), dcc_try_status_, (message_)); \
            return; \
        } \
    } while (0)

#define DCC_TRY(ctx_, expression_) DCC_CTX_TRY((ctx_), (expression_))
#define DCC_TRY_MSG(ctx_, expression_, message_) \
    DCC_CTX_TRY_MSG((ctx_), (expression_), (message_))

#endif
