#include <dcc/app/context.h>

#include <string.h>

#ifdef DCC_APP_LEGACY_H
#error "<dcc/app/context.h> must not include the transition-only legacy App surface"
#endif

int main(void) {
    dcc_ctx_parsed_date_t parsed;
    dcc_ctx_validation_error_t error;
    memset(&parsed, 0, sizeof(parsed));
    memset(&error, 0, sizeof(error));
    return parsed.size == 0U && error.size == 0U ? 0 : 1;
}
