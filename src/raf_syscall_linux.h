#ifndef RAF_SYSCALL_LINUX_H
#define RAF_SYSCALL_LINUX_H

/* SPDX-License-Identifier: LicenseRef-RAFCODE-PHI-PRE6-PENDING */

typedef unsigned long raf_usize;

#if defined(__x86_64__)

#define RAF_SYS_READ 0L
#define RAF_SYS_WRITE 1L
#define RAF_SYS_EXIT 60L
#define RAF_TARGET_ISA "x86_64-linux"

static inline long raf_syscall3(long n, long a0, long a1, long a2) {
    long out;
    __asm__ volatile(
        "syscall"
        : "=a"(out)
        : "a"(n), "D"(a0), "S"(a1), "d"(a2)
        : "rcx", "r11", "memory");
    return out;
}

#elif defined(__aarch64__)

#define RAF_SYS_READ 63L
#define RAF_SYS_WRITE 64L
#define RAF_SYS_EXIT 93L
#define RAF_TARGET_ISA "aarch64-linux"

static inline long raf_syscall3(long n, long a0, long a1, long a2) {
    register long x0 __asm__("x0") = a0;
    register long x1 __asm__("x1") = a1;
    register long x2 __asm__("x2") = a2;
    register long x8 __asm__("x8") = n;
    __asm__ volatile(
        "svc 0"
        : "+r"(x0)
        : "r"(x1), "r"(x2), "r"(x8)
        : "memory");
    return x0;
}

#elif defined(__arm__)

#define RAF_SYS_READ 3L
#define RAF_SYS_WRITE 4L
#define RAF_SYS_EXIT 1L
#define RAF_TARGET_ISA "armv7-eabi-linux"

static inline long raf_syscall3(long n, long a0, long a1, long a2) {
    register long r0 __asm__("r0") = a0;
    register long r1 __asm__("r1") = a1;
    register long r2 __asm__("r2") = a2;
    register long r7 __asm__("r7") = n;
    __asm__ volatile(
        "svc 0"
        : "+r"(r0)
        : "r"(r1), "r"(r2), "r"(r7)
        : "memory");
    return r0;
}

#else
#error "Unsupported ISA: implement and test an explicit Linux syscall contract"
#endif

static inline long raf_read(long fd, void *buf, raf_usize count) {
    return raf_syscall3(RAF_SYS_READ, fd, (long)buf, (long)count);
}

static inline long raf_write(long fd, const void *buf, raf_usize count) {
    return raf_syscall3(RAF_SYS_WRITE, fd, (long)buf, (long)count);
}

__attribute__((noreturn)) static inline void raf_exit(long code) {
    (void)raf_syscall3(RAF_SYS_EXIT, code, 0L, 0L);
    __builtin_unreachable();
}

#endif

