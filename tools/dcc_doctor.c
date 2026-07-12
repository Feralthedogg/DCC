#include <dcc/dcc.h>

#include <llam/runtime.h>
#include <openssl/crypto.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <io.h>
#define dcc_doctor_access _access
#define DCC_DOCTOR_READ 4
#else
#include <unistd.h>
#define dcc_doctor_access access
#define DCC_DOCTOR_READ R_OK
#endif

typedef struct dcc_doctor_options {
    int json;
    int require_token;
    int require_dave;
} dcc_doctor_options_t;

typedef struct dcc_doctor_result {
    const char *dcc_version;
    const char *llam_version;
    const char *openssl_version;
    uint32_t llam_abi;
    int dcc_ok;
    int llam_ok;
    int token_present;
    int dave_available;
    int cert_file_ok;
    int cert_dir_ok;
    int required_ok;
} dcc_doctor_result_t;

static void dcc_doctor_usage(FILE *stream, const char *argv0) {
    fprintf(
        stream,
        "usage: %s [--json] [--require-token] [--require-dave]\n"
        "\n"
        "Runs local, no-network checks for DCC, LLAM 2.2.0+, OpenSSL, token\n"
        "discovery, certificate environment paths, and the official libdave ABI.\n"
        "Optional token/DAVE checks become fatal only with --require-* flags.\n",
        argv0
    );
}

static int dcc_doctor_version_at_least(const char *text, unsigned major, unsigned minor, unsigned patch) {
    unsigned got_major = 0U;
    unsigned got_minor = 0U;
    unsigned got_patch = 0U;
    char tail = '\0';
    if (text == NULL || sscanf(text, "%u.%u.%u%c", &got_major, &got_minor, &got_patch, &tail) != 3) {
        return 0;
    }
    if (got_major != major) return got_major > major;
    if (got_minor != minor) return got_minor > minor;
    return got_patch >= patch;
}

static int dcc_doctor_path_ok(const char *name) {
    const char *path = getenv(name);
    return path == NULL || path[0] == '\0' || dcc_doctor_access(path, DCC_DOCTOR_READ) == 0;
}

static int dcc_doctor_check_dave(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "dcc-doctor-local-check",
        .intents = 0U,
    };
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice = NULL;
    int available = 0;
    if (dcc_client_create(&options, &client) == DCC_OK &&
        dcc_voice_client_create(client, &voice) == DCC_OK) {
        available = dcc_voice_client_set_dave_enabled(voice, 1U) == DCC_OK &&
            dcc_voice_client_dave_backend_available(voice) != 0U;
    }
    dcc_voice_client_destroy(voice);
    dcc_client_destroy(client);
    return available;
}

static void dcc_doctor_json_string(const char *text) {
    fputc('"', stdout);
    if (text != NULL) {
        for (const unsigned char *p = (const unsigned char *)text; *p != '\0'; ++p) {
            if (*p == '"' || *p == '\\') fputc('\\', stdout);
            if (*p >= 0x20U) fputc(*p, stdout);
        }
    }
    fputc('"', stdout);
}

static void dcc_doctor_print_json(const dcc_doctor_result_t *result) {
    fputs("{\"ok\":", stdout);
    fputs(result->required_ok ? "true" : "false", stdout);
    fputs(",\"dcc_version\":", stdout);
    dcc_doctor_json_string(result->dcc_version);
    fputs(",\"dcc_version_ok\":", stdout);
    fputs(result->dcc_ok ? "true" : "false", stdout);
    fputs(",\"llam_version\":", stdout);
    dcc_doctor_json_string(result->llam_version);
    fprintf(stdout, ",\"llam_abi\":%u,\"llam_version_ok\":%s", result->llam_abi,
        result->llam_ok ? "true" : "false");
    fputs(",\"openssl_version\":", stdout);
    dcc_doctor_json_string(result->openssl_version);
    fprintf(
        stdout,
        ",\"token_present\":%s,\"dave_available\":%s,"
        "\"ssl_cert_file_ok\":%s,\"ssl_cert_dir_ok\":%s}\n",
        result->token_present ? "true" : "false",
        result->dave_available ? "true" : "false",
        result->cert_file_ok ? "true" : "false",
        result->cert_dir_ok ? "true" : "false"
    );
}

static void dcc_doctor_print_text(const dcc_doctor_result_t *result, const dcc_doctor_options_t *options) {
    printf("[%s] DCC headers/runtime %s\n", result->dcc_ok ? "ok" : "fail", result->dcc_version);
    printf("[%s] LLAM runtime %s (ABI %u; required >= 2.2.0)\n",
        result->llam_ok ? "ok" : "fail", result->llam_version, result->llam_abi);
    printf("[ok] OpenSSL %s\n", result->openssl_version);
    printf("[%s] Discord token %s\n",
        result->token_present ? "ok" : (options->require_token ? "fail" : "warn"),
        result->token_present ? "found (value hidden)" : "not found");
    printf("[%s] official libdave backend %s\n",
        result->dave_available ? "ok" : (options->require_dave ? "fail" : "warn"),
        result->dave_available ? "available" : "unavailable");
    printf("[%s] SSL_CERT_FILE\n", result->cert_file_ok ? "ok" : "fail");
    printf("[%s] SSL_CERT_DIR\n", result->cert_dir_ok ? "ok" : "fail");
}

int main(int argc, char **argv) {
    dcc_doctor_options_t options = {0};
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--json") == 0) options.json = 1;
        else if (strcmp(argv[i], "--require-token") == 0) options.require_token = 1;
        else if (strcmp(argv[i], "--require-dave") == 0) options.require_dave = 1;
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            dcc_doctor_usage(stdout, argv[0]);
            return 0;
        } else {
            fprintf(stderr, "unknown option: %s\n", argv[i]);
            dcc_doctor_usage(stderr, argv[0]);
            return 2;
        }
    }

    const char *token = NULL;
    dcc_doctor_result_t result = {
        .dcc_version = dcc_version_string(),
        .llam_version = llam_version_string(),
        .openssl_version = OpenSSL_version(OPENSSL_VERSION),
        .llam_abi = llam_abi_version(),
        .cert_file_ok = dcc_doctor_path_ok("SSL_CERT_FILE"),
        .cert_dir_ok = dcc_doctor_path_ok("SSL_CERT_DIR"),
    };
    result.dcc_ok = strcmp(result.dcc_version, DCC_VERSION_STRING) == 0 &&
        DCC_VERSION_MAJOR == 1 && DCC_VERSION_MINOR == 5 && DCC_VERSION_PATCH == 0;
    result.llam_ok = dcc_doctor_version_at_least(result.llam_version, 2U, 2U, 0U);
    result.token_present = dcc_app_env_get_token(NULL, &token) == DCC_OK;
    result.dave_available = dcc_doctor_check_dave();
    result.required_ok = result.dcc_ok && result.llam_ok && result.cert_file_ok && result.cert_dir_ok &&
        (!options.require_token || result.token_present) &&
        (!options.require_dave || result.dave_available);

    if (options.json) dcc_doctor_print_json(&result);
    else dcc_doctor_print_text(&result, &options);
    return result.required_ok ? 0 : 1;
}
