#include <dcc/app.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define DCC_CTX_FIELD_VALIDATOR_MIN_SIZE \
    (offsetof(dcc_ctx_field_validator_t, max_number) + \
     sizeof(((dcc_ctx_field_validator_t *)0)->max_number))

static void dcc_ctx_validation_error_clear(dcc_ctx_validation_error_t *out_error) {
    if (out_error == NULL) {
        return;
    }
    out_error->size = sizeof(*out_error);
    out_error->field = NULL;
    out_error->message = NULL;
    out_error->status = DCC_OK;
}

static dcc_status_t dcc_ctx_validation_fail(
    const dcc_ctx_field_validator_t *validator,
    const char *message,
    dcc_ctx_validation_error_t *out_error
) {
    if (out_error != NULL) {
        out_error->size = sizeof(*out_error);
        out_error->field = validator != NULL ? validator->field : NULL;
        out_error->message = message;
        out_error->status = DCC_ERR_INVALID_ARG;
    }
    return DCC_ERR_INVALID_ARG;
}

static uint8_t dcc_ctx_is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

static int dcc_ctx_two_digits(const char *value) {
    return ((int)(value[0] - '0') * 10) + (int)(value[1] - '0');
}

static int dcc_ctx_four_digits(const char *value) {
    return ((int)(value[0] - '0') * 1000) +
           ((int)(value[1] - '0') * 100) +
           ((int)(value[2] - '0') * 10) +
           (int)(value[3] - '0');
}

static uint8_t dcc_ctx_is_leap_year(int year) {
    return (uint8_t)((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

static uint8_t dcc_ctx_valid_month_day(int year, int month, int day, uint8_t has_year) {
    static const int month_days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };
    if (month < 1 || month > 12 || day < 1) {
        return 0U;
    }
    int max_day = month_days[month - 1];
    if (month == 2 && (!has_year || dcc_ctx_is_leap_year(year))) {
        max_day = 29;
    }
    return (uint8_t)(day <= max_day);
}

static void dcc_ctx_parsed_date_clear(dcc_ctx_parsed_date_t *out) {
    if (out == NULL) {
        return;
    }
    out->size = sizeof(*out);
    out->year = 0;
    out->month = 0U;
    out->day = 0U;
    out->has_year = 0U;
}

dcc_status_t dcc_ctx_parse_date_yyyymmdd(
    const char *value,
    dcc_ctx_parsed_date_t *out
) {
    dcc_ctx_parsed_date_clear(out);
    if (value == NULL || out == NULL || strlen(value) != 8U) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < 8U; ++i) {
        if (!dcc_ctx_is_digit(value[i])) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    int year = dcc_ctx_four_digits(value);
    int month = dcc_ctx_two_digits(value + 4);
    int day = dcc_ctx_two_digits(value + 6);
    if (year <= 0 || !dcc_ctx_valid_month_day(year, month, day, 1U)) {
        return DCC_ERR_INVALID_ARG;
    }
    out->year = year;
    out->month = (uint8_t)month;
    out->day = (uint8_t)day;
    out->has_year = 1U;
    return DCC_OK;
}

dcc_status_t dcc_ctx_parse_date_mmdd(
    const char *value,
    dcc_ctx_parsed_date_t *out
) {
    dcc_ctx_parsed_date_clear(out);
    if (value == NULL || out == NULL || strlen(value) != 4U) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < 4U; ++i) {
        if (!dcc_ctx_is_digit(value[i])) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    int month = dcc_ctx_two_digits(value);
    int day = dcc_ctx_two_digits(value + 2);
    if (!dcc_ctx_valid_month_day(0, month, day, 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    out->month = (uint8_t)month;
    out->day = (uint8_t)day;
    return DCC_OK;
}

dcc_status_t dcc_ctx_parse_date(
    const char *value,
    dcc_ctx_parsed_date_t *out
) {
    if (value == NULL || out == NULL) {
        dcc_ctx_parsed_date_clear(out);
        return DCC_ERR_INVALID_ARG;
    }
    size_t len = strlen(value);
    if (len == 4U) {
        return dcc_ctx_parse_date_mmdd(value, out);
    }
    if (len == 8U) {
        return dcc_ctx_parse_date_yyyymmdd(value, out);
    }
    dcc_ctx_parsed_date_clear(out);
    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_ctx_validate_string_required(
    const char *value,
    const dcc_ctx_field_validator_t *validator,
    dcc_ctx_validation_error_t *out_error
) {
    if (value == NULL || value[0] == '\0') {
        return dcc_ctx_validation_fail(validator, "This field is required.", out_error);
    }
    return DCC_OK;
}

static dcc_status_t dcc_ctx_validate_string_length(
    const char *value,
    const dcc_ctx_field_validator_t *validator,
    dcc_ctx_validation_error_t *out_error
) {
    if (value == NULL) {
        return dcc_ctx_validation_fail(validator, "This field is required.", out_error);
    }
    size_t len = strlen(value);
    if (len < validator->min_length) {
        return dcc_ctx_validation_fail(validator, "This field is too short.", out_error);
    }
    if (validator->max_length != 0U && len > validator->max_length) {
        return dcc_ctx_validation_fail(validator, "This field is too long.", out_error);
    }
    return DCC_OK;
}

static dcc_status_t dcc_ctx_validate_one_field(
    const void *base,
    const dcc_ctx_field_validator_t *validator,
    dcc_ctx_validation_error_t *out_error
) {
    if (validator == NULL || validator->size < DCC_CTX_FIELD_VALIDATOR_MIN_SIZE) {
        return dcc_ctx_validation_fail(NULL, "Validator configuration is invalid.", out_error);
    }

    const unsigned char *field =
        (const unsigned char *)base + validator->field_offset;
    switch (validator->type) {
        case DCC_CTX_FIELD_VALIDATE_STRING_REQUIRED:
            return dcc_ctx_validate_string_required(
                *(const char *const *)field,
                validator,
                out_error
            );
        case DCC_CTX_FIELD_VALIDATE_STRING_LENGTH:
            return dcc_ctx_validate_string_length(
                *(const char *const *)field,
                validator,
                out_error
            );
        case DCC_CTX_FIELD_VALIDATE_INTEGER_RANGE: {
            int64_t value = *(const int64_t *)field;
            if (value < validator->min_integer || value > validator->max_integer) {
                return dcc_ctx_validation_fail(
                    validator,
                    "This number is outside the allowed range.",
                    out_error
                );
            }
            return DCC_OK;
        }
        case DCC_CTX_FIELD_VALIDATE_NUMBER_RANGE: {
            double value = *(const double *)field;
            if (value != value ||
                value < validator->min_number ||
                value > validator->max_number) {
                return dcc_ctx_validation_fail(
                    validator,
                    "This number is outside the allowed range.",
                    out_error
                );
            }
            return DCC_OK;
        }
        case DCC_CTX_FIELD_VALIDATE_DATE_YYYYMMDD:
            if (dcc_ctx_parse_date_yyyymmdd(*(const char *const *)field, &(dcc_ctx_parsed_date_t){0}) != DCC_OK) {
                return dcc_ctx_validation_fail(
                    validator,
                    "Use a real date in YYYYMMDD format.",
                    out_error
                );
            }
            return DCC_OK;
        case DCC_CTX_FIELD_VALIDATE_DATE_MMDD:
            if (dcc_ctx_parse_date_mmdd(*(const char *const *)field, &(dcc_ctx_parsed_date_t){0}) != DCC_OK) {
                return dcc_ctx_validation_fail(
                    validator,
                    "Use a real date in MMDD format.",
                    out_error
                );
            }
            return DCC_OK;
        case DCC_CTX_FIELD_VALIDATE_DATE_MMDD_OR_YYYYMMDD: {
            const char *value = *(const char *const *)field;
            if (dcc_ctx_parse_date(value, &(dcc_ctx_parsed_date_t){0}) != DCC_OK) {
                return dcc_ctx_validation_fail(
                    validator,
                    "Use a real date in MMDD or YYYYMMDD format.",
                    out_error
                );
            }
            return DCC_OK;
        }
    }

    return dcc_ctx_validation_fail(validator, "Validator type is invalid.", out_error);
}

dcc_status_t dcc_ctx_validate_fields(
    const void *base,
    const dcc_ctx_field_validator_t *validators,
    size_t validator_count,
    dcc_ctx_validation_error_t *out_error
) {
    dcc_ctx_validation_error_clear(out_error);
    if (base == NULL || (validators == NULL && validator_count != 0U)) {
        return dcc_ctx_validation_fail(NULL, "Validator input is invalid.", out_error);
    }

    for (size_t i = 0U; i < validator_count; ++i) {
        dcc_status_t status = dcc_ctx_validate_one_field(base, &validators[i], out_error);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
