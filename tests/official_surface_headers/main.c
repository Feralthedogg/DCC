int dcc_official_surface_applications_header_smoke(void);
int dcc_official_surface_lobbies_header_smoke(void);
int dcc_official_surface_oauth2_header_smoke(void);
int dcc_official_surface_rest_header_smoke(void);
int dcc_official_surface_sugar_header_smoke(void);
int dcc_official_surface_webhook_events_header_smoke(void);

int main(void) {
    return dcc_official_surface_applications_header_smoke() +
        dcc_official_surface_lobbies_header_smoke() +
        dcc_official_surface_oauth2_header_smoke() +
        dcc_official_surface_rest_header_smoke() +
        dcc_official_surface_sugar_header_smoke() +
        dcc_official_surface_webhook_events_header_smoke();
}
