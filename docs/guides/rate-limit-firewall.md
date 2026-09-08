# REST Firewall

DCC's REST transport already serializes major-route buckets and handles 429
retry timing. The firewall layer adds policy and observability for production
bots.

## Attach And Snapshot

These functions are compiled from `examples/docs/guide_configuration.c` against
the installed headers. The caller supplies its client and snapshot output
buffer; check status before using the JSON bytes or returned length.

<!-- DCC_DOC_SNIPPET_BEGIN(rest-firewall) -->
```c
#include <dcc/rest/firewall.h>

dcc_status_t dcc_example_firewall_attach(dcc_client_t *client) {
    dcc_rest_firewall_options_t options;
    dcc_rest_firewall_options_init(&options);
    options.invalid_request_soft_limit = 8000U;
    options.invalid_request_hard_limit = 9500U;
    return dcc_rest_firewall_attach(client, &options);
}

dcc_status_t dcc_example_firewall_json(dcc_client_t *client,
                                      char *json, size_t capacity, size_t *length) {
    dcc_rest_firewall_snapshot_t snapshot = { .size = sizeof(snapshot) };
    dcc_status_t status = dcc_rest_firewall_snapshot(client, &snapshot);
    if (status == DCC_OK)
        status = dcc_rest_firewall_snapshot_json(&snapshot, json, capacity, length);
    return status;
}
```
<!-- DCC_DOC_SNIPPET_END(rest-firewall) -->

Expose the JSON through your existing health endpoint or sidecar. Detach with
`dcc_rest_firewall_detach(client)` when removing the policy from a live client.

## Policy

Track:

- route bucket pressure
- global rate limit state
- 401, 403, and 429 counts
- invalid request budget over the rolling window

For large bots, rejecting noncritical traffic before Discord invalid-request
limits are reached is safer than continuing to send doomed requests.
