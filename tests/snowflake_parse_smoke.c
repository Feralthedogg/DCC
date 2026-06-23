#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int expect_parse(
    dcc_status_t (*parse_fn)(const char *, dcc_snowflake_t *),
    const char *text,
    dcc_snowflake_t expected
) {
    dcc_snowflake_t parsed = 0;
    dcc_status_t status = parse_fn(text, &parsed);
    if (status != DCC_OK || parsed != expected) {
        fprintf(
            stderr,
            "parse failed: text=%s status=%d parsed=%llu expected=%llu\n",
            text != NULL ? text : "(null)",
            (int)status,
            (unsigned long long)parsed,
            (unsigned long long)expected
        );
        return 1;
    }
    return 0;
}

static int expect_reject(
    dcc_status_t (*parse_fn)(const char *, dcc_snowflake_t *),
    const char *text
) {
    dcc_snowflake_t parsed = 999;
    dcc_status_t status = parse_fn(text, &parsed);
    if (status == DCC_OK) {
        fprintf(
            stderr,
            "parse should have failed: text=%s parsed=%llu\n",
            text != NULL ? text : "(null)",
            (unsigned long long)parsed
        );
        return 1;
    }
    return 0;
}

static int check_raw_ids(void) {
    dcc_snowflake_t parsed = 0;
    return
        DCC_PARSE_SNOWFLAKE("123456789012345678", &parsed) == DCC_OK &&
        parsed == 123456789012345678ULL &&
        dcc_snowflake_parse_len("9876xx", 4U, &parsed) == DCC_OK &&
        parsed == 9876ULL &&
        DCC_PARSE_ID("42", &parsed) == DCC_OK &&
        parsed == 42ULL
            ? 0
            : 1;
}

static int check_mentions(void) {
    return
        expect_parse(dcc_snowflake_parse_user_mention, "<@123>", 123ULL) ||
        expect_parse(dcc_snowflake_parse_user_mention, "<@!123>", 123ULL) ||
        expect_parse(dcc_snowflake_parse_member_mention, "<@!123>", 123ULL) ||
        expect_parse(dcc_snowflake_parse_role_mention, "<@&456>", 456ULL) ||
        expect_parse(dcc_snowflake_parse_channel_mention, "<#789>", 789ULL) ||
        expect_parse(dcc_snowflake_parse_any, "<#789>", 789ULL) ||
        expect_parse(dcc_snowflake_parse_any, "<@&456>", 456ULL) ||
        expect_parse(dcc_snowflake_parse_any, "<@123>", 123ULL);
}

static int check_rejections(void) {
    dcc_snowflake_t parsed = 0;
    return
        expect_reject(dcc_snowflake_parse, "0") ||
        expect_reject(dcc_snowflake_parse, "") ||
        expect_reject(dcc_snowflake_parse, "123x") ||
        expect_reject(dcc_snowflake_parse, "18446744073709551616") ||
        expect_reject(dcc_snowflake_parse_channel_mention, "<@123>") ||
        expect_reject(dcc_snowflake_parse_role_mention, "<#123>") ||
        expect_reject(dcc_snowflake_parse_user_mention, "<@&123>") ||
        expect_reject(dcc_snowflake_parse_any, "<t:1700000000:R>") ||
        DCC_PARSE_ID(NULL, &parsed) == DCC_OK ||
        DCC_PARSE_ID("123", NULL) == DCC_OK;
}

static int check_created_at_helpers(void) {
    const dcc_snowflake_t snowflake = 1174109840998400000ULL;
    uint64_t unix_ms = 0U;
    int64_t unix_seconds = 0;

    if (DCC_SNOWFLAKE_CREATED_AT_MS(snowflake, &unix_ms) != DCC_OK ||
        unix_ms != 1700000000000ULL ||
        DCC_SNOWFLAKE_CREATED_AT_UNIX_SECONDS(snowflake, &unix_seconds) != DCC_OK ||
        unix_seconds != 1700000000 ||
        strcmp(DCC_SNOWFLAKE_TIMESTAMP(snowflake, 'f'), "<t:1700000000:f>") != 0 ||
        strcmp(DCC_SNOWFLAKE_RELATIVE(snowflake), "<t:1700000000:R>") != 0 ||
        strcmp(DCC_ID_CREATED_RELATIVE(snowflake), "<t:1700000000:R>") != 0) {
        return 1;
    }

    return
        DCC_SNOWFLAKE_CREATED_AT_MS(0ULL, &unix_ms) == DCC_ERR_INVALID_ARG &&
        DCC_SNOWFLAKE_CREATED_AT_MS(snowflake, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_SNOWFLAKE_CREATED_AT_UNIX_SECONDS(0ULL, &unix_seconds) == DCC_ERR_INVALID_ARG &&
        strcmp(DCC_SNOWFLAKE_RELATIVE(0ULL), "") == 0
            ? 0
            : 1;
}

int main(void) {
    if (check_raw_ids() != 0) {
        return 1;
    }
    if (check_mentions() != 0) {
        return 1;
    }
    if (check_rejections() != 0) {
        return 1;
    }
    if (check_created_at_helpers() != 0) {
        return 1;
    }
    return 0;
}
