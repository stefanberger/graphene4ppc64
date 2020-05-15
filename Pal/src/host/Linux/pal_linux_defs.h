#ifndef PAL_LINUX_DEFS_H
#define PAL_LINUX_DEFS_H

#define USER_ADDRESS_LOWEST 0x10000

#if defined(i386) || defined(__x86_64__)
#define THREAD_STACK_SIZE (PRESET_PAGESIZE * 2)   /* 8KB initial stack (grows automatically) */
#define ALT_STACK_SIZE    (PRESET_PAGESIZE * 16)  /* 64KB signal stack */
#else
#define THREAD_STACK_SIZE (PRESET_PAGESIZE)
#define ALT_STACK_SIZE    (PRESET_PAGESIZE)
#endif

#ifdef __powerpc64__
#define USE_VSYSCALL_GETTIME 1
#define USE_VDSO_GETTIME     0
#else
#define USE_VSYSCALL_GETTIME 0
#define USE_VDSO_GETTIME     1
#endif
#define USE_CLOCK_GETTIME    1

#define USE_ARCH_RDRAND 0

#define BLOCK_SIGFAULT 0

#if defined(__i386__) || defined(__x86_64__)
# ifndef FIONREAD
#  define FIONREAD 0x541B
# endif
#elif defined(__powerpc64__)
# include <asm/ioctl.h>
# define FIONREAD _IOR('f', 127, int)
#endif

#endif /* PAL_LINUX_DEFS_H */
