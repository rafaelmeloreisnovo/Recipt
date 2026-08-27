/* SPDX-License-Identifier: LicenseRef-RAFCODE-PHI-PRE6-PENDING */
/* Author: ∆RafaelVerboΩ — RAFCODE-Φ */

#include "raf_syscall_linux.h"
#include "raf_json_syntax.h"

typedef unsigned char raf_u8;
typedef unsigned int raf_u32;
typedef unsigned long long raf_u64;

#define RAF_BUFFER_SIZE 65536U
#define RAF_STACK_SIZE 512U
#define RAF_TOKEN_SIZE 32U

enum raf_key {
    RAF_KEY_ID = 0,
    RAF_KEY_TITLE,
    RAF_KEY_CREATE_TIME,
    RAF_KEY_UPDATE_TIME,
    RAF_KEY_MAPPING,
    RAF_KEY_CURRENT_NODE,
    RAF_KEY_CONVERSATION_ID,
    RAF_KEY_MESSAGE,
    RAF_KEY_AUTHOR,
    RAF_KEY_ROLE,
    RAF_KEY_CONTENT,
    RAF_KEY_PARTS,
    RAF_KEY_COUNT,
    RAF_KEY_NONE = 255
};

enum raf_role {
    RAF_ROLE_USER = 0,
    RAF_ROLE_ASSISTANT,
    RAF_ROLE_SYSTEM,
    RAF_ROLE_TOOL,
    RAF_ROLE_DEVELOPER,
    RAF_ROLE_COUNT
};

struct raf_stats {
    raf_u64 bytes;
    raf_u64 fnv1a64;
    raf_u64 top_level_objects;
    raf_u64 object_open;
    raf_u64 object_close;
    raf_u64 array_open;
    raf_u64 array_close;
    raf_u64 strings;
    raf_u64 escapes;
    raf_u64 structural_errors;
    raf_u64 fields[RAF_KEY_COUNT];
    raf_u64 roles[RAF_ROLE_COUNT];
    raf_u32 depth;
    raf_u32 max_depth;
    raf_u32 root_started;
    raf_u32 root_array;
    raf_u32 root_done;
    raf_u32 in_string;
    raf_u32 escaped;
    raf_u32 read_error;
};

struct raf_parser {
    struct raf_stats s;
    raf_u8 stack[RAF_STACK_SIZE];
    raf_u8 token[RAF_TOKEN_SIZE];
    raf_u32 token_len;
    raf_u32 token_valid;
    raf_u32 closed_string_pending;
    raf_u8 closed_string_key;
    raf_u8 expected_value_key;
    raf_u8 string_value_key;
};

static raf_u8 g_buffer[RAF_BUFFER_SIZE];
static struct raf_parser g_parser;
static struct raf_json_syntax g_syntax;

static raf_u32 raf_strlen(const char *s) {
    raf_u32 n = 0U;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

static void raf_write_all(const char *s, raf_u32 n) {
    raf_u32 off = 0U;
    while (off < n) {
        long wrote = raf_write(1L, s + off, (raf_usize)(n - off));
        if (wrote <= 0L) {
            raf_exit(3L);
        }
        off += (raf_u32)wrote;
    }
}

static void raf_out(const char *s) {
    raf_write_all(s, raf_strlen(s));
}

static void raf_out_u64(raf_u64 value) {
    char digits[32];
    raf_u32 n = 0U;
    if (value == 0ULL) {
        raf_write_all("0", 1U);
        return;
    }
    while (value != 0ULL) {
        digits[n++] = (char)('0' + (char)(value % 10ULL));
        value /= 10ULL;
    }
    while (n != 0U) {
        n--;
        raf_write_all(&digits[n], 1U);
    }
}

static void raf_out_u32(raf_u32 value) {
    raf_out_u64((raf_u64)value);
}

static void raf_out_hex64(raf_u64 value) {
    static const char hex[] = "0123456789abcdef";
    char out[16];
    raf_u32 i;
    for (i = 0U; i < 16U; i++) {
        raf_u32 shift = (15U - i) * 4U;
        out[i] = hex[(raf_u32)((value >> shift) & 15ULL)];
    }
    raf_write_all(out, 16U);
}

static raf_u32 raf_is_space(raf_u8 c) {
    return (c == (raf_u8)' ') || (c == (raf_u8)'\n') ||
           (c == (raf_u8)'\r') || (c == (raf_u8)'\t');
}

static raf_u32 raf_token_eq(const struct raf_parser *p, const char *s) {
    raf_u32 i;
    raf_u32 n = raf_strlen(s);
    if ((p->token_valid == 0U) || (p->token_len != n)) {
        return 0U;
    }
    for (i = 0U; i < n; i++) {
        if (p->token[i] != (raf_u8)s[i]) {
            return 0U;
        }
    }
    return 1U;
}

static raf_u8 raf_match_key(const struct raf_parser *p) {
    if (raf_token_eq(p, "id") != 0U) return RAF_KEY_ID;
    if (raf_token_eq(p, "title") != 0U) return RAF_KEY_TITLE;
    if (raf_token_eq(p, "create_time") != 0U) return RAF_KEY_CREATE_TIME;
    if (raf_token_eq(p, "update_time") != 0U) return RAF_KEY_UPDATE_TIME;
    if (raf_token_eq(p, "mapping") != 0U) return RAF_KEY_MAPPING;
    if (raf_token_eq(p, "current_node") != 0U) return RAF_KEY_CURRENT_NODE;
    if (raf_token_eq(p, "conversation_id") != 0U) return RAF_KEY_CONVERSATION_ID;
    if (raf_token_eq(p, "message") != 0U) return RAF_KEY_MESSAGE;
    if (raf_token_eq(p, "author") != 0U) return RAF_KEY_AUTHOR;
    if (raf_token_eq(p, "role") != 0U) return RAF_KEY_ROLE;
    if (raf_token_eq(p, "content") != 0U) return RAF_KEY_CONTENT;
    if (raf_token_eq(p, "parts") != 0U) return RAF_KEY_PARTS;
    return RAF_KEY_NONE;
}

static void raf_count_role(struct raf_parser *p) {
    if (raf_token_eq(p, "user") != 0U) p->s.roles[RAF_ROLE_USER]++;
    else if (raf_token_eq(p, "assistant") != 0U) p->s.roles[RAF_ROLE_ASSISTANT]++;
    else if (raf_token_eq(p, "system") != 0U) p->s.roles[RAF_ROLE_SYSTEM]++;
    else if (raf_token_eq(p, "tool") != 0U) p->s.roles[RAF_ROLE_TOOL]++;
    else if (raf_token_eq(p, "developer") != 0U) p->s.roles[RAF_ROLE_DEVELOPER]++;
}

static void raf_start_string(struct raf_parser *p, raf_u8 value_key) {
    p->s.in_string = 1U;
    p->s.escaped = 0U;
    p->token_len = 0U;
    p->token_valid = 1U;
    p->string_value_key = value_key;
}

static void raf_push(struct raf_parser *p, raf_u8 kind) {
    if (p->s.depth >= RAF_STACK_SIZE) {
        p->s.structural_errors++;
        return;
    }
    p->stack[p->s.depth++] = kind;
    if (p->s.depth > p->s.max_depth) {
        p->s.max_depth = p->s.depth;
    }
}

static void raf_pop(struct raf_parser *p, raf_u8 expected) {
    if ((p->s.depth == 0U) || (p->stack[p->s.depth - 1U] != expected)) {
        p->s.structural_errors++;
        return;
    }
    p->s.depth--;
    if (p->s.depth == 0U) {
        p->s.root_done = 1U;
    }
}

static void raf_feed_byte(struct raf_parser *p, raf_u8 c) {
    p->s.bytes++;
    p->s.fnv1a64 ^= (raf_u64)c;
    p->s.fnv1a64 *= 1099511628211ULL;

    if (p->s.in_string != 0U) {
        if (p->s.escaped != 0U) {
            p->s.escaped = 0U;
            p->token_valid = 0U;
            return;
        }
        if (c == (raf_u8)'\\') {
            p->s.escaped = 1U;
            p->s.escapes++;
            return;
        }
        if (c == (raf_u8)'"') {
            p->s.in_string = 0U;
            p->s.strings++;
            if (p->string_value_key == RAF_KEY_ROLE) {
                raf_count_role(p);
            }
            p->closed_string_key = raf_match_key(p);
            p->closed_string_pending = 1U;
            p->string_value_key = RAF_KEY_NONE;
            return;
        }
        if (c < 0x20U) {
            p->s.structural_errors++;
            p->token_valid = 0U;
            return;
        }
        if (p->token_len < RAF_TOKEN_SIZE) {
            p->token[p->token_len++] = c;
        } else {
            p->token_valid = 0U;
        }
        return;
    }

    if (p->closed_string_pending != 0U) {
        if (raf_is_space(c) != 0U) {
            return;
        }
        if (c == (raf_u8)':') {
            if (p->closed_string_key != RAF_KEY_NONE) {
                p->s.fields[p->closed_string_key]++;
                p->expected_value_key = p->closed_string_key;
            }
            p->closed_string_pending = 0U;
            return;
        }
        p->closed_string_pending = 0U;
    }

    if (p->expected_value_key != RAF_KEY_NONE) {
        if (raf_is_space(c) != 0U) {
            return;
        }
        if (c == (raf_u8)'"') {
            raf_u8 value_key = p->expected_value_key;
            p->expected_value_key = RAF_KEY_NONE;
            raf_start_string(p, value_key);
            return;
        }
        p->expected_value_key = RAF_KEY_NONE;
    }

    if (p->s.root_done != 0U) {
        if (raf_is_space(c) == 0U) {
            p->s.structural_errors++;
        }
        return;
    }

    if (c == (raf_u8)'"') {
        raf_start_string(p, RAF_KEY_NONE);
    } else if (c == (raf_u8)'{') {
        if ((p->s.root_array != 0U) && (p->s.depth == 1U)) {
            p->s.top_level_objects++;
        }
        if (p->s.root_started == 0U) {
            p->s.root_started = 1U;
        }
        p->s.object_open++;
        raf_push(p, (raf_u8)'{');
    } else if (c == (raf_u8)'[') {
        if (p->s.root_started == 0U) {
            p->s.root_started = 1U;
            p->s.root_array = 1U;
        }
        p->s.array_open++;
        raf_push(p, (raf_u8)'[');
    } else if (c == (raf_u8)'}') {
        p->s.object_close++;
        raf_pop(p, (raf_u8)'{');
    } else if (c == (raf_u8)']') {
        p->s.array_close++;
        raf_pop(p, (raf_u8)'[');
    } else if ((p->s.root_started == 0U) && (raf_is_space(c) == 0U)) {
        p->s.structural_errors++;
    }
}

static raf_u32 raf_structural_ok(const struct raf_parser *p) {
    return (p->s.root_started != 0U) &&
           (p->s.root_done != 0U) &&
           (p->s.root_array != 0U) &&
           (p->s.depth == 0U) &&
           (p->s.in_string == 0U) &&
           (p->s.escaped == 0U) &&
           (p->s.structural_errors == 0ULL) &&
           (p->s.read_error == 0U);
}

static void raf_out_bool(raf_u32 value) {
    if (value != 0U) raf_out("true");
    else raf_out("false");
}

static void raf_emit_receipt(const struct raf_parser *p, const struct raf_json_syntax *syntax) {
    raf_out("{\n");
    raf_out("  \"schema\":\"rafcodephi.freestanding.stream-json-receipt.v2\",\n");
    raf_out("  \"evidence_class\":\"MEASURED_LOCAL\",\n");
    raf_out("  \"parser_scope\":\"structural_scan_plus_json_grammar_no_utf8_semantic_schema\",\n");
    raf_out("  \"target_isa\":\""); raf_out(RAF_TARGET_ISA); raf_out("\",\n");
    raf_out("  \"buffer_bytes\":"); raf_out_u32(RAF_BUFFER_SIZE); raf_out(",\n");
    raf_out("  \"bytes\":"); raf_out_u64(p->s.bytes); raf_out(",\n");
    raf_out("  \"fnv1a64_raw\":\""); raf_out_hex64(p->s.fnv1a64); raf_out("\",\n");
    raf_out("  \"root_array\":"); raf_out_bool(p->s.root_array); raf_out(",\n");
    raf_out("  \"top_level_objects\":"); raf_out_u64(p->s.top_level_objects); raf_out(",\n");
    raf_out("  \"object_open\":"); raf_out_u64(p->s.object_open); raf_out(",\n");
    raf_out("  \"object_close\":"); raf_out_u64(p->s.object_close); raf_out(",\n");
    raf_out("  \"array_open\":"); raf_out_u64(p->s.array_open); raf_out(",\n");
    raf_out("  \"array_close\":"); raf_out_u64(p->s.array_close); raf_out(",\n");
    raf_out("  \"strings\":"); raf_out_u64(p->s.strings); raf_out(",\n");
    raf_out("  \"escapes\":"); raf_out_u64(p->s.escapes); raf_out(",\n");
    raf_out("  \"max_depth\":"); raf_out_u32(p->s.max_depth); raf_out(",\n");
    raf_out("  \"field_counts\":{\n");
    raf_out("    \"id\":"); raf_out_u64(p->s.fields[RAF_KEY_ID]); raf_out(",\n");
    raf_out("    \"title\":"); raf_out_u64(p->s.fields[RAF_KEY_TITLE]); raf_out(",\n");
    raf_out("    \"create_time\":"); raf_out_u64(p->s.fields[RAF_KEY_CREATE_TIME]); raf_out(",\n");
    raf_out("    \"update_time\":"); raf_out_u64(p->s.fields[RAF_KEY_UPDATE_TIME]); raf_out(",\n");
    raf_out("    \"mapping\":"); raf_out_u64(p->s.fields[RAF_KEY_MAPPING]); raf_out(",\n");
    raf_out("    \"current_node\":"); raf_out_u64(p->s.fields[RAF_KEY_CURRENT_NODE]); raf_out(",\n");
    raf_out("    \"conversation_id\":"); raf_out_u64(p->s.fields[RAF_KEY_CONVERSATION_ID]); raf_out(",\n");
    raf_out("    \"message\":"); raf_out_u64(p->s.fields[RAF_KEY_MESSAGE]); raf_out(",\n");
    raf_out("    \"author\":"); raf_out_u64(p->s.fields[RAF_KEY_AUTHOR]); raf_out(",\n");
    raf_out("    \"role\":"); raf_out_u64(p->s.fields[RAF_KEY_ROLE]); raf_out(",\n");
    raf_out("    \"content\":"); raf_out_u64(p->s.fields[RAF_KEY_CONTENT]); raf_out(",\n");
    raf_out("    \"parts\":"); raf_out_u64(p->s.fields[RAF_KEY_PARTS]); raf_out("\n  },\n");
    raf_out("  \"role_value_counts\":{\n");
    raf_out("    \"user\":"); raf_out_u64(p->s.roles[RAF_ROLE_USER]); raf_out(",\n");
    raf_out("    \"assistant\":"); raf_out_u64(p->s.roles[RAF_ROLE_ASSISTANT]); raf_out(",\n");
    raf_out("    \"system\":"); raf_out_u64(p->s.roles[RAF_ROLE_SYSTEM]); raf_out(",\n");
    raf_out("    \"tool\":"); raf_out_u64(p->s.roles[RAF_ROLE_TOOL]); raf_out(",\n");
    raf_out("    \"developer\":"); raf_out_u64(p->s.roles[RAF_ROLE_DEVELOPER]); raf_out("\n  },\n");
    raf_out("  \"structural_errors\":"); raf_out_u64(p->s.structural_errors); raf_out(",\n");
    raf_out("  \"read_error\":"); raf_out_bool(p->s.read_error); raf_out(",\n");
    raf_out("  \"structural_ok\":"); raf_out_bool(raf_structural_ok(p)); raf_out(",\n");
    raf_out("  \"json_syntax_errors\":"); raf_out_u64((raf_u64)syntax->errors); raf_out(",\n");
    raf_out("  \"json_syntax_ok\":"); raf_out_bool((raf_u32)raf_json_ok(syntax)); raf_out(",\n");
    raf_out("  \"claim_allowed\":false\n}\n");
}

__attribute__((noreturn)) void _start(void) {
    struct raf_parser *p = &g_parser;
    struct raf_json_syntax *syntax = &g_syntax;
    p->s.fnv1a64 = 14695981039346656037ULL;
    p->expected_value_key = RAF_KEY_NONE;
    p->string_value_key = RAF_KEY_NONE;
    p->closed_string_key = RAF_KEY_NONE;

    for (;;) {
        long got = raf_read(0L, g_buffer, (raf_usize)RAF_BUFFER_SIZE);
        raf_u32 i;
        if (got == 0L) {
            break;
        }
        if (got < 0L) {
            p->s.read_error = 1U;
            break;
        }
        for (i = 0U; i < (raf_u32)got; i++) {
            raf_json_feed(syntax, g_buffer[i]);
            raf_feed_byte(p, g_buffer[i]);
        }
    }

    raf_json_finish(syntax);
    raf_emit_receipt(p, syntax);
    if ((raf_structural_ok(p) != 0U) && (raf_json_ok(syntax) != 0U)) {
        raf_exit(0L);
    }
    raf_exit(2L);
}
