#!/usr/bin/env python3
"""Convert a binary .suit manifest into STM32 C fixture source and header.

This replaces the Linux CLI input path based on open()/mmap(). It uses only the
Python standard library and does not parse or modify the SUIT bytes.
"""

from __future__ import annotations

import argparse
from pathlib import Path


def format_bytes(data: bytes) -> str:
    rows = []
    for offset in range(0, len(data), 12):
        chunk = data[offset : offset + 12]
        rows.append("    " + ", ".join(f"0x{byte:02x}" for byte in chunk) + ",")
    return "\n".join(rows)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path, help="Input signed or mutated .suit file")
    parser.add_argument(
        "--out-dir",
        type=Path,
        default=Path("."),
        help="Directory for manifest_fixture.c and manifest_fixture.h",
    )
    args = parser.parse_args()

    data = args.input.read_bytes()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    header = """#ifndef MANIFEST_FIXTURE_H
#define MANIFEST_FIXTURE_H

#include <stddef.h>
#include <stdint.h>

/* Raw .suit bytes embedded in read-only STM32 Flash. */
extern const uint8_t manifest_fixture[];
extern const size_t manifest_fixture_len;

#endif /* MANIFEST_FIXTURE_H */
"""

    source = f"""#include "manifest_fixture.h"

/*
 * Generated from: {args.input.name}
 * The bytes are intentionally preserved exactly for parser vulnerability tests.
 */
const uint8_t manifest_fixture[] = {{
{format_bytes(data)}
}};

const size_t manifest_fixture_len = sizeof(manifest_fixture);
"""

    (args.out_dir / "manifest_fixture.h").write_text(header, encoding="ascii")
    (args.out_dir / "manifest_fixture.c").write_text(source, encoding="ascii")
    print(f"Generated {len(data)} bytes in {args.out_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
