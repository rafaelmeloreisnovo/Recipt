#!/usr/bin/env python3
"""Deterministic differential probes for the freestanding JSON grammar gate.

Python's stdlib parser is used only as an independent test oracle. The kernel
itself remains freestanding and has no Python/runtime dependency.
"""

from __future__ import annotations

import json
import math
import random
import subprocess
import sys


SEED = 0x52414632
VALID_RANDOM_CASES = 256


def random_scalar(rng: random.Random):
    pick = rng.randrange(6)
    if pick == 0:
        return None
    if pick == 1:
        return bool(rng.randrange(2))
    if pick == 2:
        return rng.randint(-(1 << 31), (1 << 31) - 1)
    if pick == 3:
        value = rng.uniform(-1e6, 1e6)
        assert math.isfinite(value)
        return round(value, rng.randrange(0, 7))
    if pick == 4:
        alphabet = "abcXYZ0123 \\ \" \n \t Ω Δ Φ"
        return "".join(rng.choice(alphabet) for _ in range(rng.randrange(0, 40)))
    return rng.choice([0, -0.0, 1e-9, 1e20])


def random_value(rng: random.Random, depth: int = 0):
    if depth >= 4 or rng.random() < 0.55:
        return random_scalar(rng)
    if rng.random() < 0.5:
        return [random_value(rng, depth + 1) for _ in range(rng.randrange(0, 5))]
    out = {}
    for i in range(rng.randrange(0, 5)):
        out[f"k{depth}_{i}_{rng.randrange(100)}"] = random_value(rng, depth + 1)
    return out


def run_kernel(binary: str, payload: bytes):
    proc = subprocess.run(
        [binary], input=payload, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False
    )
    try:
        receipt = json.loads(proc.stdout.decode("utf-8"))
    except Exception as exc:  # pragma: no cover - fail path diagnostic
        raise AssertionError(
            f"kernel emitted non-JSON receipt rc={proc.returncode}: {proc.stdout!r} {proc.stderr!r}"
        ) from exc
    return proc.returncode, receipt


def expect_valid(binary: str, text: str) -> None:
    parsed = json.loads(text)
    if not isinstance(parsed, list):
        raise AssertionError("test oracle payload must use a root array")
    rc, receipt = run_kernel(binary, text.encode("utf-8"))
    assert rc == 0, (text[:160], rc, receipt)
    assert receipt["structural_ok"] is True, (text[:160], receipt)
    assert receipt["json_syntax_ok"] is True, (text[:160], receipt)
    assert receipt["json_syntax_errors"] == 0, (text[:160], receipt)
    assert receipt["claim_allowed"] is False, receipt


def expect_invalid(binary: str, text: str) -> None:
    try:
        json.loads(text)
    except (json.JSONDecodeError, UnicodeDecodeError):
        pass
    else:
        raise AssertionError(f"invalid oracle case unexpectedly accepted by Python: {text!r}")
    rc, receipt = run_kernel(binary, text.encode("utf-8"))
    assert rc == 2, (text[:160], rc, receipt)
    assert receipt["json_syntax_ok"] is False or receipt["structural_ok"] is False, (
        text[:160],
        receipt,
    )
    assert receipt["claim_allowed"] is False, receipt


def main() -> int:
    binary = sys.argv[1] if len(sys.argv) > 1 else "build/raf_stream_json_receipt"
    rng = random.Random(SEED)

    for _ in range(VALID_RANDOM_CASES):
        root = [random_value(rng) for _ in range(rng.randrange(0, 7))]
        text = json.dumps(root, ensure_ascii=rng.choice([True, False]), separators=(",", ":"))
        expect_valid(binary, text)

    # Buffer-boundary probes: force token/string/escape state across 64 KiB reads.
    expect_valid(binary, json.dumps(["a" * 65535 + "Ω"], ensure_ascii=False))
    expect_valid(binary, '["' + ("a" * 65533) + '\\n"]')
    expect_valid(binary, '[{"k":"' + ("z" * 131071) + '"}]')

    invalid = [
        "[1 2]",
        "[1,]",
        "[,1]",
        "[01]",
        "[-]",
        "[1.]",
        "[.1]",
        "[1e]",
        "[1e+]",
        "[true false]",
        "[tru]",
        "[nul]",
        "[False]",
        "[NaN]",
        "[Infinity]",
        '["\\q"]',
        '["\\u12G4"]',
        '["unterminated]',
        '{"a":1}',  # valid JSON, but intentionally invalid for this kernel's root-array contract
        "[]x",
        "[][]",
        '[{"a" 1}]',
        '[{"a":}]',
        '[{"a":1 "b":2}]',
        '[{,"a":1}]',
        '[{"a":1,}]',
        '["' + ("a" * 65533) + '\\q"]',
    ]

    # Root-object is valid generic JSON but invalid by the kernel contract; test separately.
    root_object = invalid.pop(18)
    assert isinstance(json.loads(root_object), dict)
    rc, receipt = run_kernel(binary, root_object.encode("utf-8"))
    assert rc == 2 and receipt["structural_ok"] is False and receipt["claim_allowed"] is False

    for text in invalid:
        expect_invalid(binary, text)

    print(
        f"PASS: differential probes seed={SEED} valid={VALID_RANDOM_CASES + 3} invalid={len(invalid) + 1}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
