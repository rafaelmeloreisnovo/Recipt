#ifndef RAF_JSON_SYNTAX_H
#define RAF_JSON_SYNTAX_H

/* SPDX-License-Identifier: LicenseRef-RAFCODE-PHI-PRE6-PENDING */
/* Streaming JSON grammar validator: syntax only, no schema or UTF-8 semantic validation. */

typedef unsigned char raf_json_u8;
typedef unsigned int raf_json_u32;
typedef unsigned long long raf_json_u64;

#define RAF_JSON_MAX_DEPTH 512U

#define RAF_JSON_FRAME_ARRAY  1U
#define RAF_JSON_FRAME_OBJECT 2U

#define RAF_JSON_ARR_VALUE_OR_END 0U
#define RAF_JSON_ARR_VALUE         1U
#define RAF_JSON_ARR_COMMA_OR_END  2U

#define RAF_JSON_OBJ_KEY_OR_END    0U
#define RAF_JSON_OBJ_KEY           1U
#define RAF_JSON_OBJ_COLON         2U
#define RAF_JSON_OBJ_VALUE         3U
#define RAF_JSON_OBJ_COMMA_OR_END  4U

#define RAF_JSON_NUM_NONE       0U
#define RAF_JSON_NUM_SIGN       1U
#define RAF_JSON_NUM_ZERO       2U
#define RAF_JSON_NUM_INT        3U
#define RAF_JSON_NUM_DOT        4U
#define RAF_JSON_NUM_FRAC       5U
#define RAF_JSON_NUM_EXP        6U
#define RAF_JSON_NUM_EXP_SIGN   7U
#define RAF_JSON_NUM_EXP_DIGITS 8U

#define RAF_JSON_LITERAL_NONE  0U
#define RAF_JSON_LITERAL_TRUE  1U
#define RAF_JSON_LITERAL_FALSE 2U
#define RAF_JSON_LITERAL_NULL  3U

struct raf_json_syntax {
    raf_json_u8 kind[RAF_JSON_MAX_DEPTH];
    raf_json_u8 state[RAF_JSON_MAX_DEPTH];
    raf_json_u32 depth;
    raf_json_u64 errors;
    raf_json_u32 root_started;
    raf_json_u32 root_done;
    raf_json_u32 in_string;
    raf_json_u32 string_is_key;
    raf_json_u32 escaped;
    raf_json_u32 unicode_remaining;
    raf_json_u32 in_number;
    raf_json_u32 number_state;
    raf_json_u32 literal_kind;
    raf_json_u32 literal_pos;
};

static raf_json_u32 raf_json_is_space(raf_json_u8 c) {
    return (c == (raf_json_u8)' ') || (c == (raf_json_u8)'\n') ||
           (c == (raf_json_u8)'\r') || (c == (raf_json_u8)'\t');
}

static raf_json_u32 raf_json_is_digit(raf_json_u8 c) {
    return (c >= (raf_json_u8)'0') && (c <= (raf_json_u8)'9');
}

static raf_json_u32 raf_json_is_nonzero_digit(raf_json_u8 c) {
    return (c >= (raf_json_u8)'1') && (c <= (raf_json_u8)'9');
}

static raf_json_u32 raf_json_is_hex(raf_json_u8 c) {
    return raf_json_is_digit(c) ||
           ((c >= (raf_json_u8)'a') && (c <= (raf_json_u8)'f')) ||
           ((c >= (raf_json_u8)'A') && (c <= (raf_json_u8)'F'));
}

static raf_json_u32 raf_json_number_accepting(raf_json_u32 state) {
    return (state == RAF_JSON_NUM_ZERO) ||
           (state == RAF_JSON_NUM_INT) ||
           (state == RAF_JSON_NUM_FRAC) ||
           (state == RAF_JSON_NUM_EXP_DIGITS);
}

static const char *raf_json_literal_text(raf_json_u32 kind) {
    if (kind == RAF_JSON_LITERAL_TRUE) return "true";
    if (kind == RAF_JSON_LITERAL_FALSE) return "false";
    if (kind == RAF_JSON_LITERAL_NULL) return "null";
    return "";
}

static void raf_json_value_complete(struct raf_json_syntax *s) {
    if (s->depth == 0U) {
        if ((s->root_started == 0U) || (s->root_done != 0U)) {
            s->errors++;
            return;
        }
        s->root_done = 1U;
        return;
    }

    if (s->kind[s->depth - 1U] == RAF_JSON_FRAME_ARRAY) {
        raf_json_u8 st = s->state[s->depth - 1U];
        if ((st != RAF_JSON_ARR_VALUE_OR_END) && (st != RAF_JSON_ARR_VALUE)) {
            s->errors++;
            return;
        }
        s->state[s->depth - 1U] = RAF_JSON_ARR_COMMA_OR_END;
        return;
    }

    if (s->kind[s->depth - 1U] == RAF_JSON_FRAME_OBJECT) {
        if (s->state[s->depth - 1U] != RAF_JSON_OBJ_VALUE) {
            s->errors++;
            return;
        }
        s->state[s->depth - 1U] = RAF_JSON_OBJ_COMMA_OR_END;
        return;
    }

    s->errors++;
}

static raf_json_u32 raf_json_context_expects_value(const struct raf_json_syntax *s) {
    if (s->depth == 0U) {
        return (s->root_started == 0U);
    }
    if (s->kind[s->depth - 1U] == RAF_JSON_FRAME_ARRAY) {
        raf_json_u8 st = s->state[s->depth - 1U];
        return (st == RAF_JSON_ARR_VALUE_OR_END) || (st == RAF_JSON_ARR_VALUE);
    }
    if (s->kind[s->depth - 1U] == RAF_JSON_FRAME_OBJECT) {
        return s->state[s->depth - 1U] == RAF_JSON_OBJ_VALUE;
    }
    return 0U;
}

static void raf_json_push(struct raf_json_syntax *s, raf_json_u8 kind, raf_json_u8 state) {
    if (s->depth >= RAF_JSON_MAX_DEPTH) {
        s->errors++;
        return;
    }
    s->kind[s->depth] = kind;
    s->state[s->depth] = state;
    s->depth++;
}

static void raf_json_begin_value(struct raf_json_syntax *s, raf_json_u8 c) {
    if (raf_json_context_expects_value(s) == 0U) {
        s->errors++;
        return;
    }

    if (s->depth == 0U) {
        s->root_started = 1U;
    }

    if (c == (raf_json_u8)'{') {
        raf_json_push(s, RAF_JSON_FRAME_OBJECT, RAF_JSON_OBJ_KEY_OR_END);
        return;
    }
    if (c == (raf_json_u8)'[') {
        raf_json_push(s, RAF_JSON_FRAME_ARRAY, RAF_JSON_ARR_VALUE_OR_END);
        return;
    }
    if (c == (raf_json_u8)'"') {
        s->in_string = 1U;
        s->string_is_key = 0U;
        s->escaped = 0U;
        s->unicode_remaining = 0U;
        return;
    }
    if (c == (raf_json_u8)'-') {
        s->in_number = 1U;
        s->number_state = RAF_JSON_NUM_SIGN;
        return;
    }
    if (c == (raf_json_u8)'0') {
        s->in_number = 1U;
        s->number_state = RAF_JSON_NUM_ZERO;
        return;
    }
    if (raf_json_is_nonzero_digit(c) != 0U) {
        s->in_number = 1U;
        s->number_state = RAF_JSON_NUM_INT;
        return;
    }
    if (c == (raf_json_u8)'t') {
        s->literal_kind = RAF_JSON_LITERAL_TRUE;
        s->literal_pos = 1U;
        return;
    }
    if (c == (raf_json_u8)'f') {
        s->literal_kind = RAF_JSON_LITERAL_FALSE;
        s->literal_pos = 1U;
        return;
    }
    if (c == (raf_json_u8)'n') {
        s->literal_kind = RAF_JSON_LITERAL_NULL;
        s->literal_pos = 1U;
        return;
    }

    s->errors++;
}

static void raf_json_close_container(struct raf_json_syntax *s, raf_json_u8 expected_kind) {
    if ((s->depth == 0U) || (s->kind[s->depth - 1U] != expected_kind)) {
        s->errors++;
        return;
    }
    s->depth--;
    raf_json_value_complete(s);
}

static void raf_json_feed(struct raf_json_syntax *s, raf_json_u8 c) {
    raf_json_u32 again = 1U;

    while (again != 0U) {
        again = 0U;

        if (s->in_string != 0U) {
            if (s->unicode_remaining != 0U) {
                if (raf_json_is_hex(c) == 0U) {
                    s->errors++;
                    s->unicode_remaining = 0U;
                    return;
                }
                s->unicode_remaining--;
                return;
            }
            if (s->escaped != 0U) {
                s->escaped = 0U;
                if (c == (raf_json_u8)'u') {
                    s->unicode_remaining = 4U;
                    return;
                }
                if ((c == (raf_json_u8)'"') || (c == (raf_json_u8)'\\') ||
                    (c == (raf_json_u8)'/') || (c == (raf_json_u8)'b') ||
                    (c == (raf_json_u8)'f') || (c == (raf_json_u8)'n') ||
                    (c == (raf_json_u8)'r') || (c == (raf_json_u8)'t')) {
                    return;
                }
                s->errors++;
                return;
            }
            if (c == (raf_json_u8)'\\') {
                s->escaped = 1U;
                return;
            }
            if (c == (raf_json_u8)'"') {
                s->in_string = 0U;
                if (s->string_is_key != 0U) {
                    if ((s->depth == 0U) ||
                        (s->kind[s->depth - 1U] != RAF_JSON_FRAME_OBJECT) ||
                        ((s->state[s->depth - 1U] != RAF_JSON_OBJ_KEY_OR_END) &&
                         (s->state[s->depth - 1U] != RAF_JSON_OBJ_KEY))) {
                        s->errors++;
                        return;
                    }
                    s->state[s->depth - 1U] = RAF_JSON_OBJ_COLON;
                } else {
                    raf_json_value_complete(s);
                }
                return;
            }
            if (c < 0x20U) {
                s->errors++;
            }
            return;
        }

        if (s->literal_kind != RAF_JSON_LITERAL_NONE) {
            const char *text = raf_json_literal_text(s->literal_kind);
            if (c != (raf_json_u8)text[s->literal_pos]) {
                s->errors++;
                s->literal_kind = RAF_JSON_LITERAL_NONE;
                return;
            }
            s->literal_pos++;
            if (text[s->literal_pos] == '\0') {
                s->literal_kind = RAF_JSON_LITERAL_NONE;
                s->literal_pos = 0U;
                raf_json_value_complete(s);
            }
            return;
        }

        if (s->in_number != 0U) {
            if (s->number_state == RAF_JSON_NUM_SIGN) {
                if (c == (raf_json_u8)'0') s->number_state = RAF_JSON_NUM_ZERO;
                else if (raf_json_is_nonzero_digit(c) != 0U) s->number_state = RAF_JSON_NUM_INT;
                else { s->errors++; s->in_number = 0U; }
                return;
            }
            if (s->number_state == RAF_JSON_NUM_ZERO) {
                if (c == (raf_json_u8)'.') { s->number_state = RAF_JSON_NUM_DOT; return; }
                if ((c == (raf_json_u8)'e') || (c == (raf_json_u8)'E')) { s->number_state = RAF_JSON_NUM_EXP; return; }
                if (raf_json_is_digit(c) != 0U) { s->errors++; s->in_number = 0U; return; }
            } else if (s->number_state == RAF_JSON_NUM_INT) {
                if (raf_json_is_digit(c) != 0U) return;
                if (c == (raf_json_u8)'.') { s->number_state = RAF_JSON_NUM_DOT; return; }
                if ((c == (raf_json_u8)'e') || (c == (raf_json_u8)'E')) { s->number_state = RAF_JSON_NUM_EXP; return; }
            } else if (s->number_state == RAF_JSON_NUM_DOT) {
                if (raf_json_is_digit(c) != 0U) { s->number_state = RAF_JSON_NUM_FRAC; return; }
                s->errors++; s->in_number = 0U; return;
            } else if (s->number_state == RAF_JSON_NUM_FRAC) {
                if (raf_json_is_digit(c) != 0U) return;
                if ((c == (raf_json_u8)'e') || (c == (raf_json_u8)'E')) { s->number_state = RAF_JSON_NUM_EXP; return; }
            } else if (s->number_state == RAF_JSON_NUM_EXP) {
                if ((c == (raf_json_u8)'+') || (c == (raf_json_u8)'-')) { s->number_state = RAF_JSON_NUM_EXP_SIGN; return; }
                if (raf_json_is_digit(c) != 0U) { s->number_state = RAF_JSON_NUM_EXP_DIGITS; return; }
                s->errors++; s->in_number = 0U; return;
            } else if (s->number_state == RAF_JSON_NUM_EXP_SIGN) {
                if (raf_json_is_digit(c) != 0U) { s->number_state = RAF_JSON_NUM_EXP_DIGITS; return; }
                s->errors++; s->in_number = 0U; return;
            } else if (s->number_state == RAF_JSON_NUM_EXP_DIGITS) {
                if (raf_json_is_digit(c) != 0U) return;
            }

            if (raf_json_number_accepting(s->number_state) == 0U) {
                s->errors++;
                s->in_number = 0U;
                return;
            }
            s->in_number = 0U;
            s->number_state = RAF_JSON_NUM_NONE;
            raf_json_value_complete(s);
            again = 1U;
            continue;
        }

        if (raf_json_is_space(c) != 0U) {
            return;
        }

        if (s->root_done != 0U) {
            s->errors++;
            return;
        }

        if (s->depth == 0U) {
            raf_json_begin_value(s, c);
            return;
        }

        if (s->kind[s->depth - 1U] == RAF_JSON_FRAME_ARRAY) {
            raf_json_u8 st = s->state[s->depth - 1U];
            if (st == RAF_JSON_ARR_VALUE_OR_END) {
                if (c == (raf_json_u8)']') {
                    raf_json_close_container(s, RAF_JSON_FRAME_ARRAY);
                    return;
                }
                raf_json_begin_value(s, c);
                return;
            }
            if (st == RAF_JSON_ARR_VALUE) {
                raf_json_begin_value(s, c);
                return;
            }
            if (st == RAF_JSON_ARR_COMMA_OR_END) {
                if (c == (raf_json_u8)',') {
                    s->state[s->depth - 1U] = RAF_JSON_ARR_VALUE;
                    return;
                }
                if (c == (raf_json_u8)']') {
                    raf_json_close_container(s, RAF_JSON_FRAME_ARRAY);
                    return;
                }
                s->errors++;
                return;
            }
            s->errors++;
            return;
        }

        if (s->kind[s->depth - 1U] == RAF_JSON_FRAME_OBJECT) {
            raf_json_u8 st = s->state[s->depth - 1U];
            if (st == RAF_JSON_OBJ_KEY_OR_END) {
                if (c == (raf_json_u8)'}') {
                    raf_json_close_container(s, RAF_JSON_FRAME_OBJECT);
                    return;
                }
                if (c == (raf_json_u8)'"') {
                    s->in_string = 1U;
                    s->string_is_key = 1U;
                    s->escaped = 0U;
                    s->unicode_remaining = 0U;
                    return;
                }
                s->errors++;
                return;
            }
            if (st == RAF_JSON_OBJ_KEY) {
                if (c == (raf_json_u8)'"') {
                    s->in_string = 1U;
                    s->string_is_key = 1U;
                    s->escaped = 0U;
                    s->unicode_remaining = 0U;
                    return;
                }
                s->errors++;
                return;
            }
            if (st == RAF_JSON_OBJ_COLON) {
                if (c == (raf_json_u8)':') {
                    s->state[s->depth - 1U] = RAF_JSON_OBJ_VALUE;
                } else {
                    s->errors++;
                }
                return;
            }
            if (st == RAF_JSON_OBJ_VALUE) {
                raf_json_begin_value(s, c);
                return;
            }
            if (st == RAF_JSON_OBJ_COMMA_OR_END) {
                if (c == (raf_json_u8)',') {
                    s->state[s->depth - 1U] = RAF_JSON_OBJ_KEY;
                    return;
                }
                if (c == (raf_json_u8)'}') {
                    raf_json_close_container(s, RAF_JSON_FRAME_OBJECT);
                    return;
                }
                s->errors++;
                return;
            }
            s->errors++;
            return;
        }

        s->errors++;
    }
}

static void raf_json_finish(struct raf_json_syntax *s) {
    if (s->in_number != 0U) {
        if (raf_json_number_accepting(s->number_state) != 0U) {
            s->in_number = 0U;
            s->number_state = RAF_JSON_NUM_NONE;
            raf_json_value_complete(s);
        } else {
            s->errors++;
            s->in_number = 0U;
        }
    }
    if ((s->in_string != 0U) || (s->escaped != 0U) ||
        (s->unicode_remaining != 0U) ||
        (s->literal_kind != RAF_JSON_LITERAL_NONE) ||
        (s->depth != 0U) ||
        (s->root_started == 0U) || (s->root_done == 0U)) {
        s->errors++;
    }
}

static raf_json_u32 raf_json_ok(const struct raf_json_syntax *s) {
    return (s->errors == 0ULL) &&
           (s->root_started != 0U) && (s->root_done != 0U) &&
           (s->depth == 0U) && (s->in_string == 0U) &&
           (s->in_number == 0U) &&
           (s->literal_kind == RAF_JSON_LITERAL_NONE);
}

#endif
