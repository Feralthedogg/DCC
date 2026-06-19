# REST Firewall

DCC's REST transport already serializes major-route buckets and handles 429
retry timing. The firewall layer adds policy and observability for production
bots.

## Attach

```c
dcc_rest_firewall_options_t options;
dcc_rest_firewall_options_init(&options);
options.invalid_request_soft_limit = 8000;
options.invalid_request_hard_limit = 9500;

dcc_rest_firewall_attach(client, &options);
```

## Snapshot

```c
dcc_rest_firewall_snapshot_t snapshot;
dcc_rest_firewall_snapshot(client, &snapshot);

char json[2048];
dcc_rest_firewall_snapshot_json(&snapshot, json, sizeof(json));
```

Expose the JSON through your existing health endpoint or sidecar.

## Policy

Track:

- route bucket pressure
- global rate limit state
- 401, 403, and 429 counts
- invalid request budget over the rolling window

For large bots, rejecting noncritical traffic before Discord invalid-request
limits are reached is safer than continuing to send doomed requests.
