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

expect_pass() {
    fixture=$1
    shift
    out=$(printf '%s' "$fixture" | "$bin")
    expect_contains "$out" '"structural_ok":true'
    expect_contains "$out" '"json_syntax_ok":true'
    while [ "$#" -gt 0 ]; do
        expect_contains "$out" "$1"
        shift
    done
}

expect_fail() {
    fixture=$1
    set +e
    out=$(printf '%s' "$fixture" | "$bin")
    status=$?
    set -e
    test "$status" -eq 2
    expect_contains "$out" '"json_syntax_ok":false'
}

expect_pass '[]' '"top_level_objects":0'

fixture='[{"id":"c1","mapping":{},"author":{"role":"user"}},{"id":"c2","message":{"author":{"role":"assistant"},"content":{"parts":[]}}}]'
expect_pass "$fixture" '"top_level_objects":2' '"id":2' '"mapping":1' '"message":1' '"user":1' '"assistant":1'

expect_pass '["{",{"title":"x\\\"y"}]' '"top_level_objects":1' '"title":1'
expect_pass '[0,-1,1.25,1e3,-2.0E-2,true,false,null]' '"json_syntax_errors":0'
expect_pass '["\u03a9","\/","\\","\b","\f","\n","\r","\t"]' '"json_syntax_errors":0'
expect_pass '[{},[1,2,3],{"nested":{"ok":true}}]' '"json_syntax_errors":0'

expect_fail '[{"id":1}'
expect_fail '[1 2]'
expect_fail '[1,]'
expect_fail '{"a" 1}'
expect_fail '{"a":1 "b":2}'
expect_fail '[01]'
expect_fail '[tru]'
expect_fail '["\q"]'
expect_fail '["\u0x00"]'
expect_fail '[]x'
expect_fail '{"a":}'
expect_fail '{,"a":1}'

echo 'PASS: 18/18 freestanding stream receipt grammar tests'
