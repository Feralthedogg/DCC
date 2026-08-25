# Components v2

DCC uses one tagged `dcc_component_v2_builder_t` representation for normal
message action rows, Components-v2 layouts, and modals. `DCC_UI_*` constructors
allocate nothing and borrow strings and child arrays until the enclosing
message, modal, or context action serializes them.

```c
dcc_component_v2_builder_t row_children[] = {
    DCC_UI_PRIMARY("Refresh", "status.refresh"),
    DCC_UI_DANGER("Close", "status.close"),
};
dcc_component_v2_builder_t row = DCC_UI_ROW_ARRAY(row_children, 2U);
dcc_component_v2_builder_t card_children[] = {
    DCC_UI_TEXT("## Runtime status"),
    row,
};
dcc_component_v2_builder_t card =
    DCC_UI_CARD_ACCENT_ARRAY(card_children, 2U, 0x5865F2U);
return DCC_CTX_REPLY_UI(ctx, card);
```

Explicit `_ARRAY` functions are the safe C and C++ interface. The nine
variadic array helpers exist only in C11 and create block-lifetime compound
literals. Invalid tags, placement, count, pointer pairs, and Discord limits
remain visible through `dcc_component_v2_validate`; convenience functions do
not clamp or silently rewrite input.

Modal children use `DCC_UI_LABEL`, `DCC_UI_INPUT`, file upload, radio group,
checkbox group, and checkbox values, then `DCC_UI_MODAL_ARRAY` and
`DCC_CTX_SHOW_MODAL`.
