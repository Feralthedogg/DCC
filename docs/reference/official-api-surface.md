# Official Discord API Surface

Normal application code includes `<dcc/bot.h>`. Explicit API consumers use
focused `<dcc/rest/*.h>`, `<dcc/oauth2.h>`, `<dcc/client.h>`, and
`<dcc/webhook_events.h>` headers. The [generated reference](api/index.md)
lists installed declarations; historical Sugar aliases are not the current API.

## Canonical REST requests and results

Named endpoint wrappers take typed input records and
`dcc_rest_call_options_t`, and can return an owned `dcc_rest_request_t *`.
Completion uses `dcc_rest_result_fn` and a borrowed `dcc_rest_result_t`.
Use `dcc_rest_submit` for a route that requires explicit low-level control.
Check admission separately from transport and HTTP completion status.

Use `dcc_rest_request_cancel`, `dcc_rest_request_wait`, and
`dcc_rest_request_destroy` for individual operation ownership. Builders borrow
input through the documented admission boundary. See
[REST request declarations](api/rest/request.md) and
[ownership and async](../concepts/ownership-and-async.md).

The installed surface includes message/channel/guild resources, interactions,
webhooks, application commands, soundboard, subscriptions, lobbies, and other
official-surface endpoint owners. Refer to the
[official-surface declarations](api/rest/official_surface.md) and focused owner
headers for exact parameter types and signatures.

## OAuth2 compatibility boundary

Standalone `dcc_oauth2_*` helpers separate authorization-code exchange,
refresh/revoke, and OAuth2 identity operations from bot REST clients. Their
current public callback remains
`dcc_oauth2_cb(const dcc_rest_response_t *, void *)`.
`dcc_rest_response_t` and `dcc_rest_cb` are retained public ABI names in
`<dcc/rest/base.h>`; they are not the canonical endpoint or Context callback
shape. See [OAuth2 declarations](api/oauth2.md).

## Gateway and webhook events

Gateway control operations use focused public Gateway headers.
Webhook event verification and routing live in
[webhook event declarations](api/webhook_events.md).
Read [Gateway and events](../guides/gateway-events.md) for listener registration
and borrowed-event lifetimes. Coverage audits track the checked-in Discord
surface manifest, not unknown future Discord API changes.
