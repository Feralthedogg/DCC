# Ownership And Async

C does not encode ownership in the type system. DCC therefore uses naming and
callback rules consistently. Treat an undocumented pointer as borrowed until
the declaring header says otherwise.

## Ownership At A Glance

| Value source | Ownership | Valid until | Retain it by |
| --- | --- | --- | --- |
| Event accessor such as `dcc_event_message()` | Borrowed | Event callback returns | `dcc_message_clone()` |
| `dcc_event_data()` | Borrowed parser view | Callback returns or another frame is parsed | `dcc_gateway_event_data_clone()` |
| Cache getter | Borrowed client view | Object replacement, removal, eviction, cache clear, or client destruction | Matching `dcc_*_clone()` |
| REST callback `response->body` | Borrowed | REST callback returns | Copy the bytes or use a future |
| REST future response body | Future-owned | `dcc_rest_future_destroy()` | Copy before destroying the future |
| `dcc_*_clone()` result | Caller-owned | Matching `dcc_*_free()` | Already owned |
| `dcc_*_create()` result | Caller-owned runtime object | Matching `dcc_*_destroy()` | Already owned |
| Sugar variadic builder literals | Block-scoped | End of the current C block | Use explicit arrays or `*_ARRAY` forms |

## Retaining An Event Object

```c
static dcc_message_t *latest_message;

static void on_event(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;

    const dcc_message_t *borrowed = dcc_event_message(event);
    if (borrowed == NULL) {
        return;
    }

    dcc_message_t *copy = dcc_message_clone(borrowed);
    if (copy == NULL) {
        return;
    }

    dcc_message_free(latest_message);
    latest_message = copy;
}
```

Never store `borrowed` itself. It points into event/parser-owned memory.

## REST Callback Rules

The callback receives a response view. Check transport status and HTTP status
separately:

```c
static void on_response(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    (void)user_data;

    if (response == NULL || response->error != DCC_OK) {
        return;
    }
    if (response->status < 200U || response->status >= 300U) {
        return;
    }

    /* response->body is valid only during this callback. */
}
```

`DCC_OK` means the DCC operation completed. It does not by itself mean Discord
returned a 2xx response.

## REST Futures

Use a future when sequential code is clearer than a callback. Initialize the
output `size`, wait, consume the response, then destroy the completed future:

```c
dcc_rest_future_t *future = NULL;
dcc_status_t status = dcc_rest_request_future(
    client,
    "GET",
    "/users/@me",
    NULL,
    &future
);

dcc_rest_response_t response = { .size = sizeof(response) };
if (status == DCC_OK) {
    status = dcc_rest_future_wait(future, 5000U, &response);
}
if (status == DCC_OK && response.status >= 200U && response.status < 300U) {
    /* response.body is owned by future here. */
}
if (future != NULL && dcc_rest_future_completed(future)) {
    (void)dcc_rest_future_destroy(future);
}
```

A future cannot be destroyed while incomplete; that returns `DCC_ERR_STATE`.
If a wait times out, retain the future and wait again later. It remains your
runtime object until the request completes and `dcc_rest_future_destroy()`
succeeds.

## Callback Discipline

- Keep Gateway and supervisor callbacks short.
- Do not block a callback on long CPU work, a subprocess, or unbounded I/O.
- Copy or clone inputs before handing work to another task or thread.
- Ensure callback `user_data` outlives every registered callback.
- Stop the app/client and drain outstanding work before freeing shared state.

## Builder Lifetimes

Sugar macros often use compound literals. Values passed immediately to a DCC
function are convenient and safe. Values stored beyond the current block need
explicit backing arrays or the corresponding `*_ARRAY` macro. Dynamic or
cross-module builders should use the explicit builder API.
