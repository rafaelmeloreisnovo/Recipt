CC ?= cc
ARMV7_CC ?= arm-linux-gnueabihf-gcc
AARCH64_CC ?= aarch64-linux-gnu-gcc

CPPFLAGS := -Isrc
CFLAGS := -std=c11 -Os -ffreestanding -fno-builtin -fno-stack-protector \
	-fno-asynchronous-unwind-tables -fno-unwind-tables -fno-pie -fno-ident \
	-fno-common -ffunction-sections -fdata-sections -Wall -Wextra -Werror
LDFLAGS := -nostdlib -static -no-pie -Wl,-e,_start -Wl,--gc-sections \
	-Wl,--build-id=none -Wl,-z,noexecstack -Wl,--fatal-warnings

BUILD_DIR := build
BIN ?= $(BUILD_DIR)/raf_stream_json_receipt
SRC := src/raf_stream_json_receipt.c
EXTRA_SRC ?=
DEPS := src/raf_syscall_linux.h src/raf_json_syntax.h
V2_SCHEMA := schemas/stream-json-receipt.schema.v2.json
V1_ENVELOPE_SCHEMA := schemas/stream-export-receipt-envelope.schema.v1.json
V1_RECEIPT := receipts/2026-08-27/stream-export-receipt.v1.json

.PHONY: all clean test inspect verify-static armv7 aarch64 cross

all: $(BIN)

$(BIN): $(SRC) $(EXTRA_SRC) $(DEPS)
	mkdir -p $(dir $(BIN))
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC) $(EXTRA_SRC) $(LDFLAGS) -o $(BIN)

verify-static: $(BIN)
	file $(BIN)
	readelf -h $(BIN)
	@if readelf -d $(BIN) 2>/dev/null | grep -q '(NEEDED)'; then \
		echo 'FAIL: dynamic dependency detected' >&2; exit 1; \
	fi
	@if nm -u $(BIN) | grep -q .; then \
		echo 'FAIL: unresolved symbol detected' >&2; nm -u $(BIN) >&2; exit 1; \
	fi
	@test "$$(stat -c '%s' $(BIN))" -le 65536

test: $(BIN)
	sh tests/test_stream_json_receipt.sh $(BIN)
	python3 tests/test_json_differential.py $(BIN)
	printf '%s' '[]' | $(BIN) > $(BUILD_DIR)/contract-v2.json
	python3 tests/validate_json_schema_subset.py $(V2_SCHEMA) $(BUILD_DIR)/contract-v2.json
	python3 tests/validate_json_schema_subset.py $(V1_ENVELOPE_SCHEMA) $(V1_RECEIPT)

inspect: verify-static
	@true

armv7:
	$(MAKE) CC=$(ARMV7_CC) EXTRA_SRC=src/raf_aeabi_uldivmod.S \
		BIN=$(BUILD_DIR)/armv7/raf_stream_json_receipt all verify-static

aarch64:
	$(MAKE) CC=$(AARCH64_CC) BIN=$(BUILD_DIR)/aarch64/raf_stream_json_receipt all verify-static

cross: armv7 aarch64

clean:
	rm -rf $(BUILD_DIR)
