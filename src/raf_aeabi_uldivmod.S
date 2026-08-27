/* SPDX-License-Identifier: LicenseRef-RAFCODE-PHI-PRE6-PENDING */
/*
 * Freestanding ARM EABI unsigned 64-bit divmod helper.
 *
 * ABI:
 *   in:  r1:r0 = numerator, r3:r2 = denominator
 *   out: r1:r0 = quotient,  r3:r2 = remainder
 *
 * This removes the implicit libgcc dependency emitted for uint64_t / and %
 * on 32-bit ARM while keeping the kernel -nostdlib and syscall-only.
 */

.syntax unified
.text
.arm
.align 2
.global __aeabi_uldivmod
.type __aeabi_uldivmod, %function

__aeabi_uldivmod:
    push    {r4-r10, lr}

    mov     r9, r2          /* denominator low */
    mov     r10, r3         /* denominator high */
    mov     r4, #0          /* quotient low */
    mov     r5, #0          /* quotient high */
    mov     r6, #0          /* remainder low */
    mov     r7, #0          /* remainder high */
    mov     r8, #64

1:
    /* Shift numerator left; carry becomes the next MSB input bit. */
    movs    r0, r0, lsl #1
    adcs    r1, r1, r1

    /* remainder = (remainder << 1) | next_input_bit */
    adcs    r6, r6, r6
    adc     r7, r7, r7

    /* quotient <<= 1 */
    movs    r4, r4, lsl #1
    adc     r5, r5, r5

    /* if remainder >= denominator: remainder -= denominator; quotient |= 1 */
    cmp     r7, r10
    cmpeq   r6, r9
    blo     2f
    subs    r6, r6, r9
    sbc     r7, r7, r10
    orr     r4, r4, #1

2:
    subs    r8, r8, #1
    bne     1b

    mov     r0, r4
    mov     r1, r5
    mov     r2, r6
    mov     r3, r7
    pop     {r4-r10, pc}

.size __aeabi_uldivmod, .-__aeabi_uldivmod
.section .note.GNU-stack,"",%progbits
