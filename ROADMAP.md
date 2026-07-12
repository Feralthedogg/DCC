# DCC Roadmap

DCC's direction after 1.5.0 is stability-first. New surface area should make
existing bots easier to operate, test, or upgrade rather than duplicate Discord
objects without a demonstrated use case.

## 1. Stabilize the 1.x Contract

- Keep ABI-safe, size-versioned public structs and automated symbol/header gates.
- Publish migration notes and deprecation windows before removal.
- Expand installed-package consumer tests across CMake and pkg-config.

## 2. Track Discord Deliberately

- Keep Components V2 and interaction behavior aligned with official contracts.
- Turn protocol/documentation drift into auditable tests.
- Prefer focused typed additions over one unbounded catch-all API.

## 3. Make Operations Boring

- Improve cluster admission, health, metrics, drain, and rollout primitives.
- Maintain production templates and preflight diagnostics.
- Treat replay fixtures and chaos tests as release gates.

## 4. Prove Voice and DAVE Interoperability

- Test official libdave ABI updates and MLS transition failures fail-closed.
- Expand multi-participant, packet-loss, reconnect, and long-running voice soak
  coverage without inventing a competing cryptographic implementation.

## 5. Grow a Maintainable Ecosystem

- Keep the sugar API learnable while preserving lower-level escape hatches.
- Improve examples, issue triage, contribution guidance, and release provenance.
- Consider integrations only when they can be supported across the 1.x line.

Milestones are accepted when implementation, tests, operational documentation,
and compatibility impact land together. This file describes priorities, not a
promise of dates.
