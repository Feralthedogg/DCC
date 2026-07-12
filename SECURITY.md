# Security Policy

## Supported Versions

| Version | Security fixes |
| --- | --- |
| 1.5.x | Supported |
| 1.4.x and older | Upgrade required |

## Reporting a Vulnerability

Do not open a public issue for suspected vulnerabilities. Use GitHub's private
security advisory reporting for this repository. Include the affected version,
platform, attack preconditions, impact, reproduction steps, and any proposed
mitigation. Remove real Discord tokens, signatures, replay payloads, and user
data from reports.

The project aims to acknowledge a report within 7 days, provide an initial
assessment within 14 days, and coordinate disclosure after a fix is available.
Timelines can change for complex cross-platform or upstream protocol issues.

Security-sensitive areas include interaction signature verification, token and
replay-file handling, REST URL construction, Gateway/voice frame parsing,
libdave loading, hot-reload worker IPC, installers, and release workflows.
