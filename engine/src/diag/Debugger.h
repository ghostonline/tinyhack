#pragma once

#if BREAKPOINTS_ENABLED

#if WIN32
#include <intrin.h>

#define BREAK_POINT() __debugbreak()
#elif defined(__APPLE__)
#include <unistd.h>

#if defined(__aarch64__)
#define BREAK_POINT() __asm__ __volatile__(            \
"   mov    x0, %x0;    \n" /* pid                */ \
"   mov    x1, #0x11;  \n" /* SIGSTOP            */ \
"   mov    x16, #0x25; \n" /* syscall 37 = kill  */ \
"   svc    #0x80       \n" /* software interrupt */ \
"   mov    x0, x0      \n" /* nop                */ \
::  "r"(getpid())                                   \
:   "x0", "x1", "x16", "memory")
#elif defined(__arm__)
#define BREAK_POINT() __asm__ __volatile__(            \
"   mov    r0, %0;     \n" /* pid                */ \
"   mov    r1, #0x11;  \n" /* SIGSTOP            */ \
"   mov    r12, #0x25; \n" /* syscall 37 = kill  */ \
"   svc    #0x80       \n" /* software interrupt */ \
"   mov    r0, r0      \n" /* nop                */ \
::  "r"(getpid())                                   \
:   "r0", "r1", "r12", "memory")
#elif defined(__i386__) || defined(__x86_64__)
#define BREAK_POINT() __asm__ __volatile__("int $3; mov %eax, %eax")
#else
#error Unknown breakpoint implementation for apply systems
#endif

#else
#error Unknown breakpoint implementation for this platform
#endif

#else

#define BREAK_POINT() do {} while(false)

#endif
