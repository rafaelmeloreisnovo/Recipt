CC ?= cc

CPPFLAGS := -Isrc
CFLAGS := -std=c11 -Os -ffreestanding -fno-builtin -fno-stack-protector \
	-fno-asynchronous-unwind-tables -fno-unwind-tables -fno-pie -fno-ident \
	-ffunction-sections -fdata-sections -Wall -Wextra -Werror
LDFLAGS := -nostdlib -static -no-pie -Wl,-e,_start -Wl,--gc-sections \
	-Wl,--build-id=none -Wl,-z,noexecstack

BUILD_DIR := build
BIN := $(BUILD_DIR)/raf_stream_json_receipt
SRC := src/raf_stream_json_receipt.c

.PHONY: all clean test inspect

all: $(BIN)

$(BIN): $(SRC) src/raf_syscall_linux.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(BIN)

test: $(BIN)
	sh tests/test_stream_json_receipt.sh $(BIN)

inspect: $(BIN)
	file $(BIN)
	readelf -h $(BIN)
	readelf -d $(BIN) || true

clean:
	rm -f $(BIN)

