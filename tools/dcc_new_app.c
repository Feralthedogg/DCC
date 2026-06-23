#include "internal/dcc_new_app.h"

#include <stdio.h>

int main(int argc, char **argv) {
    dcc_new_app_options_t options;
    int parse_status = dcc_new_app_parse_options(argc, argv, &options);
    if (parse_status > 0) {
        dcc_new_app_usage(stdout, argv[0]);
        return 0;
    }
    if (parse_status < 0 || dcc_new_app_validate_options(&options) != 0) {
        dcc_new_app_usage(stderr, argv[0]);
        return 2;
    }
    return dcc_new_app_generate(&options) == 0 ? 0 : 1;
}
