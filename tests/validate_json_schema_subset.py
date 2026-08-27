#!/usr/bin/env python3
"""Small fail-closed JSON Schema subset validator used only by repository tests.

Supported keywords: type, const, enum, required, properties, additionalProperties,
minimum, maximum, minLength, pattern. No external Python packages are required.
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path


def fail(path: str, message: str) -> None:
    raise ValueError(f"{path}: {message}")


def validate(schema: dict, value, path: str = "$") -> None:
    if "const" in schema and value != schema["const"]:
        fail(path, f"expected const {schema['const']!r}, got {value!r}")
    if "enum" in schema and value not in schema["enum"]:
        fail(path, f"value {value!r} not in enum")

    expected_type = schema.get("type")
    if expected_type == "object":
        if not isinstance(value, dict):
            fail(path, "expected object")
        required = schema.get("required", [])
        for key in required:
            if key not in value:
                fail(path, f"missing required key {key!r}")
        props = schema.get("properties", {})
        if schema.get("additionalProperties") is False:
            extra = sorted(set(value) - set(props))
            if extra:
                fail(path, f"unexpected keys: {extra}")
        for key, subschema in props.items():
            if key in value:
                validate(subschema, value[key], f"{path}.{key}")
        return

    if expected_type == "array":
        if not isinstance(value, list):
            fail(path, "expected array")
        return

    if expected_type == "integer":
        if isinstance(value, bool) or not isinstance(value, int):
            fail(path, "expected integer")
        if "minimum" in schema and value < schema["minimum"]:
            fail(path, f"value below minimum {schema['minimum']}")
        if "maximum" in schema and value > schema["maximum"]:
            fail(path, f"value above maximum {schema['maximum']}")
        return

    if expected_type == "boolean":
        if not isinstance(value, bool):
            fail(path, "expected boolean")
        return

    if expected_type == "string":
        if not isinstance(value, str):
            fail(path, "expected string")
        if "minLength" in schema and len(value) < schema["minLength"]:
            fail(path, f"string shorter than {schema['minLength']}")
        if "pattern" in schema and re.fullmatch(schema["pattern"], value) is None:
            fail(path, f"string does not match {schema['pattern']!r}")
        return

    if expected_type is not None:
        fail(path, f"unsupported schema type {expected_type!r}")


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: validate_json_schema_subset.py SCHEMA.json VALUE.json", file=sys.stderr)
        return 64
    schema_path = Path(sys.argv[1])
    value_path = Path(sys.argv[2])
    schema = json.loads(schema_path.read_text(encoding="utf-8"))
    value = json.loads(value_path.read_text(encoding="utf-8"))
    try:
        validate(schema, value)
    except ValueError as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        return 1
    print(f"PASS: {value_path} conforms to supported contract in {schema_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
