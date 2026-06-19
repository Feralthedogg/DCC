# DCC Coding Style

## Symbols

Use the `dcc_` prefix for public functions, types, macros, and exported constants. File-local helper
functions must be `static`.

## Headers

Public headers belong in `include/dcc/`. Internal headers belong in `src/internal/`.

Public headers must:

- Include every standard or DCC public header needed by their declarations.
- Avoid private implementation fields unless the type is intentionally a public value object.
- Avoid `src/internal` includes.

Internal headers may include public headers and other internal headers through the `"internal/..."`
include path.

## Modules

Keep implementation files grouped by feature under `src/`: `gateway`, `json`, `rest`, `voice`,
`hot_reload`, and similar directories. Avoid broad buckets such as `utils` or `common`.

## Memory

Gateway parse paths should avoid allocation and use bounded scratch storage. Public clone APIs own
their result and must deep-copy any nested pointer fields.
