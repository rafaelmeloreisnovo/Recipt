#!/usr/bin/env python3
"""Fail-closed assertions for the public repository provenance receipt."""

import json
from pathlib import Path


RECEIPT = Path("receipts/2026-08-28/public-repo-provenance-envelope.recipt.v1.json")


def main() -> int:
    data = json.loads(RECEIPT.read_text(encoding="utf-8"))

    assert data["claim_allowed"] is False, "claim_allowed must remain false"
    assert any(gap["state"] == "TOKEN_VAZIO" for gap in data["gaps"]), (
        "at least one unresolved gap must remain explicit while evidence is incomplete"
    )
    assert data["license"]["state"] == "TOKEN_VAZIO_OWNER_LICENSE_SELECTION", (
        "owner license selection must not be inferred or auto-promoted"
    )

    print("public provenance fail-closed gate: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
