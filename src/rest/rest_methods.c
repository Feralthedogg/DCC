#include <dcc/rest.h>

const char *dcc_rest_method_name(dcc_rest_method_t method) {
    switch (method) {
        case DCC_REST_GET:
            return "GET";
        case DCC_REST_POST:
            return "POST";
        case DCC_REST_PUT:
            return "PUT";
        case DCC_REST_PATCH:
            return "PATCH";
        case DCC_REST_DELETE:
            return "DELETE";
        default:
            return NULL;
    }
}
