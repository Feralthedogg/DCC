#!/usr/bin/env python3
"""Audit deploy template contracts that should not silently drift."""

from __future__ import annotations

import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(relative: str, errors: list[str]) -> str:
    path = ROOT / relative
    if not path.is_file():
        errors.append(f"missing deployment template: {relative}")
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def require_contains(relative: str, text: str, needle: str, errors: list[str]) -> None:
    if needle not in text:
        errors.append(f"{relative} is missing required text: {needle}")


def require_line(relative: str, text: str, line: str, errors: list[str]) -> None:
    lines = {entry.strip() for entry in text.splitlines()}
    if line not in lines:
        errors.append(f"{relative} is missing required line: {line}")


def audit_interaction_webhook(errors: list[str]) -> None:
    base = "deploy/interaction-webhook"
    service_rel = f"{base}/dcc-interaction-webhook.service"
    env_rel = f"{base}/dcc-interaction-webhook.env"
    nginx_rel = f"{base}/nginx.conf"
    caddy_rel = f"{base}/Caddyfile"
    compose_rel = f"{base}/docker-compose.yaml"
    k8s_rel = f"{base}/kubernetes.yaml"
    readme_rel = f"{base}/README.md"

    service = read(service_rel, errors)
    env = read(env_rel, errors)
    nginx = read(nginx_rel, errors)
    caddy = read(caddy_rel, errors)
    compose = read(compose_rel, errors)
    k8s = read(k8s_rel, errors)
    read(f"{base}/Containerfile", errors)
    readme = read(readme_rel, errors)

    require_line(service_rel, service, "EnvironmentFile=/etc/dcc/dcc-interaction-webhook.env", errors)
    require_line(service_rel, service, "ExecStartPre=/usr/local/bin/dcc_interaction_webhook --check", errors)
    require_line(service_rel, service, "ExecStart=/usr/local/bin/dcc_interaction_webhook", errors)
    require_line(service_rel, service, "KillSignal=SIGTERM", errors)
    require_line(service_rel, service, "NoNewPrivileges=true", errors)
    require_line(service_rel, service, "ProtectSystem=strict", errors)

    require_line(env_rel, env, "DCC_INTERACTION_ADDRESS=127.0.0.1", errors)
    require_line(env_rel, env, "DCC_INTERACTION_PATH=/interactions", errors)
    require_contains(env_rel, env, "DISCORD_PUBLIC_KEY=", errors)
    require_contains(env_rel, env, "DCC_INTERACTION_DRAIN_TIMEOUT_MS=", errors)

    require_contains(nginx_rel, nginx, "proxy_set_header X-Signature-Ed25519", errors)
    require_contains(nginx_rel, nginx, "proxy_set_header X-Signature-Timestamp", errors)
    require_contains(nginx_rel, nginx, "/healthz", errors)
    require_contains(caddy_rel, caddy, "X-Signature-Ed25519", errors)
    require_contains(caddy_rel, caddy, "X-Signature-Timestamp", errors)
    require_contains(caddy_rel, caddy, "/healthz", errors)
    require_contains(compose_rel, compose, "interaction-webhook", errors)
    require_contains(k8s_rel, k8s, "kind: Deployment", errors)
    require_contains(k8s_rel, k8s, "dcc-interaction-webhook", errors)
    require_contains(k8s_rel, k8s, "DISCORD_PUBLIC_KEY", errors)
    require_contains(k8s_rel, k8s, "DCC_INTERACTION_ADDRESS: \"0.0.0.0\"", errors)
    require_contains(k8s_rel, k8s, "DCC_INTERACTION_PATH: \"/interactions\"", errors)
    require_contains(k8s_rel, k8s, "args: [\"--check\"]", errors)
    require_contains(k8s_rel, k8s, "readOnlyRootFilesystem: true", errors)
    require_contains(k8s_rel, k8s, "terminationGracePeriodSeconds: 25", errors)
    require_contains(k8s_rel, k8s, "path: /healthz", errors)
    require_contains(k8s_rel, k8s, "path: /interactions", errors)
    require_contains(k8s_rel, k8s, "nginx.ingress.kubernetes.io/proxy-body-size: \"32k\"", errors)
    require_contains(k8s_rel, k8s, "X-Signature-Ed25519", errors)
    require_contains(readme_rel, readme, "preserve", errors)
    require_contains(readme_rel, readme, "X-Signature-Ed25519", errors)
    require_contains(readme_rel, readme, "dcc_interaction_webhook --check", errors)
    require_contains(readme_rel, readme, "kubectl apply -f kubernetes.yaml", errors)


def audit_hot_reload(errors: list[str]) -> None:
    base = "deploy/hot-reload"
    service_rel = f"{base}/dcc-hot-reload.service"
    env_rel = f"{base}/dcc-hot-reload.env"
    tmpfiles_rel = f"{base}/dcc-hot-reload.tmpfiles.conf"
    caddy_rel = f"{base}/Caddyfile"
    compose_rel = f"{base}/docker-compose.yaml"
    container_rel = f"{base}/Containerfile"
    readme_rel = f"{base}/README.md"

    service = read(service_rel, errors)
    env = read(env_rel, errors)
    tmpfiles = read(tmpfiles_rel, errors)
    caddy = read(caddy_rel, errors)
    compose = read(compose_rel, errors)
    container = read(container_rel, errors)
    readme = read(readme_rel, errors)

    require_line(service_rel, service, "WorkingDirectory=/var/lib/dcc/hot_reload", errors)
    require_line(service_rel, service, "EnvironmentFile=/etc/dcc/dcc-hot-reload.env", errors)
    require_line(service_rel, service, "ExecStartPre=/usr/local/bin/dcc_hot_reload_host --check", errors)
    require_line(service_rel, service, "ExecStart=/usr/local/bin/dcc_hot_reload_host", errors)
    require_line(service_rel, service, "KillSignal=SIGTERM", errors)
    require_line(service_rel, service, "NoNewPrivileges=true", errors)
    require_line(service_rel, service, "ProtectSystem=strict", errors)
    require_line(service_rel, service, "ReadWritePaths=/var/lib/dcc /var/log/dcc", errors)

    require_line(env_rel, env, "DCC_BOT_MODULE=/var/lib/dcc/hot_reload/bot.so", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_WORKER=/usr/local/bin/dcc_hot_reload_worker", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_INTENTS=guilds", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_GUILD_ID=", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_HEALTH_ADDRESS=127.0.0.1", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_HEALTH_PORT=18080", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_POLL_MS=250", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_SETTLE_MS=100", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_WORKER_HEALTH_MS=2000", errors)
    require_line(env_rel, env, "DCC_HOT_RELOAD_WORKER_DRAIN_MS=500", errors)
    require_contains(env_rel, env, "DCC_TOKEN=", errors)

    require_line(tmpfiles_rel, tmpfiles, "d /var/lib/dcc 0750 dcc dcc -", errors)
    require_line(tmpfiles_rel, tmpfiles, "d /var/lib/dcc/hot_reload 0750 dcc dcc -", errors)
    require_line(tmpfiles_rel, tmpfiles, "d /var/log/dcc 0750 dcc dcc -", errors)

    require_contains(readme_rel, readme, "atomically", errors)
    require_contains(readme_rel, readme, "dcc_hot_reload_host --check", errors)
    require_contains(readme_rel, readme, "last-good", errors)
    require_contains(readme_rel, readme, "/hot-reloadz", errors)
    require_contains(readme_rel, readme, "worker_events", errors)
    require_contains(readme_rel, readme, "DCC_HOT_RELOAD_POLL_MS", errors)
    require_contains(readme_rel, readme, "global slash-command registration", errors)
    require_contains(readme_rel, readme, "docker compose up --build", errors)

    require_contains(container_rel, container, "dcc_hot_reload_host", errors)
    require_contains(container_rel, container, "dcc_hot_reload_worker", errors)
    require_contains(container_rel, container, "DCC_LLAM_USE_SUBDIRECTORY=ON", errors)
    require_contains(compose_rel, compose, "hot-reload", errors)
    require_contains(compose_rel, compose, "DCC_BOT_MODULE: /var/lib/dcc/hot_reload/bot.so", errors)
    require_contains(compose_rel, compose, "DCC_HOT_RELOAD_HEALTH_ADDRESS: 0.0.0.0", errors)
    require_contains(compose_rel, compose, "127.0.0.1:18080:8080", errors)
    require_contains(compose_rel, compose, "./modules:/var/lib/dcc/hot_reload", errors)
    require_contains(caddy_rel, caddy, "/hot-reloadz", errors)
    require_contains(caddy_rel, caddy, "reverse_proxy hot-reload:18080", errors)


def main() -> int:
    errors: list[str] = []
    audit_interaction_webhook(errors)
    audit_hot_reload(errors)

    if errors:
        print("DCC deployment template audit failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    print("DCC deployment template audit passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
