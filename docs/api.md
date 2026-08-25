# Public API Guide

Application code normally starts at `<dcc/bot.h>`. It exposes exactly five
leaves:

- handlers: nine status-returning declaration macros;
- listeners: 19 short/advanced constructor pairs and one config value;
- replies: state-aware reply, update, followup, send, modal, and autocomplete;
- UI: 44 tagged fixed/array constructors plus nine C-only variadic helpers;
- main: versioned config, one runner, and four entrypoint macros.

Advanced code includes focused owner headers. `<dcc/app.h>` has exactly eight
focused leaves, and `<dcc/rest.h>` exposes the 224 endpoint request surface plus
35 generic request/result/firewall operations. Arbitrary routes use
`dcc_rest_submit()`.

The complete generated declaration catalog is in the
[DCC 2 API Reference](reference/api/index.md). Ownership rules are summarized
in [Ownership and Async](concepts/ownership-and-async.md); removed DCC 1 names
appear only in the [migration guide](migration-1-to-2.md).
