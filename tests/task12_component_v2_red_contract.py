#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import tempfile


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--binary", type=Path, required=True)
    args = parser.parse_args()
    source = r'''
#include <dcc/component_v2.h>
static dcc_component_v2_builder_t value =
    DCC_COMPONENT_V2_BUILDER_INIT(DCC_COMPONENT_V2_TEXT_DISPLAY);
_Static_assert(DCC_COMPONENT_V2_BUILDER_VERSION == 1, "version");
_Static_assert(DCC_COMPONENT_V2_FIELD_COUNT < 64, "presence width");
int contract(void) {
    value.size = sizeof(value);
    value.version = DCC_COMPONENT_V2_BUILDER_VERSION;
    value.present = dcc_component_v2_field_mask(
        DCC_COMPONENT_V2_FIELD_CONTENT);
    value.as.text_display.content = "tagged";
    dcc_component_v2_validation_error_t error =
        DCC_COMPONENT_V2_VALIDATION_ERROR_INIT;
    return dcc_component_v2_validate(
        &value, 1U, DCC_COMPONENT_V2_CONTEXT_MESSAGE_V2, &error);
}
'''
    with tempfile.TemporaryDirectory(prefix="dcc-task12-red-") as directory:
        unit = Path(directory) / "contract.c"
        unit.write_text(source, encoding="utf-8")
        result = subprocess.run(
            [str(args.compiler), "-std=c11", "-fsyntax-only",
             f"-I{args.source / 'include'}",
             f"-I{args.binary / 'generated/include'}", str(unit)],
            capture_output=True, text=True, check=False)
    if result.returncode == 0:
        print("Task 12 RED unexpectedly compiled: tagged ABI already present")
        return 1
    output = result.stdout + result.stderr
    required = ("DCC_COMPONENT_V2_BUILDER_INIT", "no member named 'size'")
    if not any(marker in output for marker in required):
        print("Task 12 RED failed for an unrelated reason")
        print(output[:12000])
        return 1
    print("Task 12 RED confirmed: flat builder lacks tagged size/version/presence ABI")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
