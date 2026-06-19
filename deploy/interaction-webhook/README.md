# DCC Interaction Webhook Deployment

These templates run `dcc_interaction_webhook` behind nginx, systemd, or
Compose/Caddy. `kubernetes.yaml` provides the same deployment shape for clusters.
DCC verifies Discord Ed25519 signatures and exposes an unsigned local `/healthz`
route for the reverse proxy, kubelet probes, or host supervisor.

## Files

- `dcc-interaction-webhook.env`: environment file consumed by systemd.
- `dcc-interaction-webhook.service`: hardened systemd unit.
- `nginx.conf`: TLS reverse-proxy server block.
- `docker-compose.yaml`: container deployment with Caddy TLS termination.
- `kubernetes.yaml`: Secret, ConfigMap, Deployment, Service, and Ingress.
- `Caddyfile`: Caddy reverse-proxy route template.
- `Containerfile`: multi-stage build for the webhook example image.

## Install

```sh
sudo install -d -m 0750 -o dcc -g dcc /etc/dcc
sudo install -m 0640 -o root -g dcc dcc-interaction-webhook.env /etc/dcc/
sudo install -m 0644 dcc-interaction-webhook.service /etc/systemd/system/
sudo install -m 0644 nginx.conf /etc/nginx/conf.d/dcc-interaction-webhook.conf
sudo systemctl daemon-reload
sudo systemctl enable --now dcc-interaction-webhook
sudo nginx -t
sudo systemctl reload nginx
```

Set `DISCORD_PUBLIC_KEY` to the application public key from the Discord
Developer Portal, then set the interaction endpoint URL to:

```text
https://interactions.example.com/interactions
```

Keep `DCC_INTERACTION_ADDRESS=127.0.0.1` unless the process is isolated behind a
private network interface. Let the reverse proxy own TLS and preserve
`X-Signature-Ed25519`, `X-Signature-Timestamp`, and the exact request body.
`DCC_INTERACTION_DRAIN_TIMEOUT_MS` controls how long the process waits for
active callbacks to finish after SIGTERM before forcing runtime stop.
The systemd unit runs `dcc_interaction_webhook --check` before start so invalid
public-key or route configuration fails before the service binds its port.

## Checks

```sh
/usr/local/bin/dcc_interaction_webhook --check
curl -fsS http://127.0.0.1:8080/healthz
journalctl -u dcc-interaction-webhook -n 100 --no-pager
```

Run the local interaction smoke before deploying changes to route logic:

```sh
ctest --test-dir build -R '^dcc_interaction_server_smoke$' --output-on-failure
```

## Compose

The default Compose build expects `DCC/` and `LLAM/` to be sibling directories,
matching the local development layout. From `DCC/deploy/interaction-webhook`,
update `DCC_INTERACTION_HOST`, then run:

```sh
docker compose -f docker-compose.yaml build interaction-webhook
docker compose -f docker-compose.yaml up -d
docker compose -f docker-compose.yaml logs -f interaction-webhook
```

If your source tree uses different directory names, adjust `build.context`,
`DCC_DIR`, and `LLAM_DIR`, or replace `image` with a prebuilt image reference.

## Kubernetes

Build and push an image from `Containerfile`, then update
`ghcr.io/your-org/dcc-interaction-webhook:latest`,
`DISCORD_PUBLIC_KEY`, `interactions.example.com`, and the TLS secret name in
`kubernetes.yaml`.

```sh
kubectl apply -f kubernetes.yaml
kubectl -n dcc rollout status deploy/dcc-interaction-webhook
kubectl -n dcc get pods -l app.kubernetes.io/name=dcc-interaction-webhook
```

The Deployment runs `dcc_interaction_webhook --check` as an initContainer before
serving traffic. Readiness, liveness, and startup probes use `/healthz`.
`terminationGracePeriodSeconds` is intentionally longer than
`DCC_INTERACTION_DRAIN_TIMEOUT_MS` so SIGTERM can drain active callbacks before
the kubelet forces termination. Keep Ingress middleware from rewriting,
decompressing, or mutating the request body, and preserve
`X-Signature-Ed25519` plus `X-Signature-Timestamp`.
