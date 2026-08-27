#!/bin/sh
set -eu

bin=${1:-build/raf_stream_json_receipt}

expect_contains() {
    text=$1
    needle=$2
    case "$text" in
        *"$needle"*) ;;
        *)
            echo "FAIL: missing $needle" >&2
            exit 1
            ;;
    esac
}

out=$(printf '%s' '[]' | "$bin")
expect_contains "$out" '"top_level_objects":0'
expect_contains "$out" '"structural_ok":true'

fixture='[{"id":"c1","mapping":{},"author":{"role":"user"}},{"id":"c2","message":{"author":{"role":"assistant"},"content":{"parts":[]}}}]'
out=$(printf '%s' "$fixture" | "$bin")
expect_contains "$out" '"top_level_objects":2'
expect_contains "$out" '"id":2'
expect_contains "$out" '"mapping":1'
expect_contains "$out" '"message":1'
expect_contains "$out" '"user":1'
expect_contains "$out" '"assistant":1'
expect_contains "$out" '"structural_ok":true'

out=$(printf '%s' '["{",{"title":"x\\\"y"}]' | "$bin")
expect_contains "$out" '"top_level_objects":1'
expect_contains "$out" '"title":1'
expect_contains "$out" '"structural_ok":true'

set +e
out=$(printf '%s' '[{"id":1}' | "$bin")
status=$?
set -e
test "$status" -eq 2
expect_contains "$out" '"structural_ok":false'

echo 'PASS: 4/4 freestanding stream receipt tests'

