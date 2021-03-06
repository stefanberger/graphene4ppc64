/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 IBM Corporation */

/* This file is imported and modified from the GNU C Library */

#ifndef _LINUX_PPC64_SYSDEP_H
#define _LINUX_PPC64_SYSDEP_H

#include <syscall.h>
#include <sysdeps/generic/sysdep.h>

/* For Linux we can use the system call table in the header file
    /usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#define SYS_ify(syscall_name) __NR_##syscall_name

#ifdef __ASSEMBLER__

/* ELF uses byte-counts for .align, most others use log2 of count of bytes.  */
#define ALIGNARG(log2)       (1 << (log2))
#define ASM_GLOBAL_DIRECTIVE .global
/* For ELF we need the `.type' directive to make shared libs work right.  */
#define ASM_TYPE_DIRECTIVE(name, typearg) .type name,typearg;
#define ASM_SIZE_DIRECTIVE(name)          .size name,.-name;

#define C_LABEL(name) name

/* Define an entry point visible from C.  */
#define ENTRY(name)                      \
    ASM_GLOBAL_DIRECTIVE name;           \
    ASM_TYPE_DIRECTIVE(name, @function); \
    .align ALIGNARG(4);                  \
    name:                                \
    cfi_startproc;

#undef END
#define END(name) \
    cfi_endproc;  \
    ASM_SIZE_DIRECTIVE(name)

/* The Linux/ppc64 kernel expects the system call parameters in
   registers according to the following table:

    syscall number    r0
    arg 1             r3
    arg 2             r4
    arg 3             r5
    arg 4             r6
    arg 5             r7
    arg 6             r8

    The Linux kernel uses and destroys internally these registers:
    return address from
    syscall           r0
    additionally clobbered: r3-r8, cr0, cr2-c4, lr

    Normal function call, including calls to the system call stub
    functions in the libc, get the first six parameters passed in
    registers and the seventh parameter and later on the stack.  The
    register use is as follows:

     system call number    in the DO_CALL macro
     arg 1        r3
     arg 2        r4
     arg 3        r5
     arg 4        r5
     arg 5        r7
     arg 6        r8

    Syscalls of more than 6 arguments are not supported.  */

#ifndef DO_SYSCALL
#define DO_SYSCALL sc
#endif

#undef DO_CALL
#define DO_CALL(syscall_number)  \
    li %r0, syscall_number;      \
    DO_SYSCALL;

#else /* !__ASSEMBLER__ */
/* Define a macro which expands inline into the wrapper code for a system
   call.  */
#undef INLINE_SYSCALL

#define INLINE_SYSCALL(name, nr, args...) INTERNAL_SYSCALL(name, , nr, args)

#define INTERNAL_SYSCALL_NCS(name, err, nr, args...) \
  ({									\
    register long r0  __asm__ ("r0");				\
    register long r3  __asm__ ("r3");				\
    register long r4  __asm__ ("r4");				\
    register long r5  __asm__ ("r5");				\
    register long r6  __asm__ ("r6");				\
    register long r7  __asm__ ("r7");				\
    register long r8  __asm__ ("r8");				\
    LOADARGS_##nr (name, ##args);					\
    __asm__ __volatile__						\
      ("sc\n\t"								\
       "mfcr  %0\n\t"							\
       "andis. %%r9, %0, 4096\n\t"					\
       "beq 0f\n\t"							\
       "neg %%r3,%%r3\n\t"						\
       "0:"								\
       : "=&r" (r0),							\
         "=&r" (r3), "=&r" (r4), "=&r" (r5),				\
         "=&r" (r6), "=&r" (r7), "=&r" (r8)				\
       : ASM_INPUT_##nr							\
       : "r9", "r10", "r11", "r12",					\
         "cc", "ctr", "memory");					\
    r3;									\
  })

#define LOADARGS_0(name, dummy) \
	r0 = name
#define LOADARGS_1(name, __arg1) \
	long arg1 = (long) (__arg1); \
	LOADARGS_0(name, 0); \
	r3 = arg1
#define LOADARGS_2(name, __arg1, __arg2) \
	long arg2 = (long) (__arg2); \
	LOADARGS_1(name, __arg1); \
	r4 = arg2
#define LOADARGS_3(name, __arg1, __arg2, __arg3) \
	long arg3 = (long) (__arg3); \
	LOADARGS_2(name, __arg1, __arg2); \
	r5 = arg3
#define LOADARGS_4(name, __arg1, __arg2, __arg3, __arg4) \
	long arg4 = (long) (__arg4); \
	LOADARGS_3(name, __arg1, __arg2, __arg3); \
	r6 = arg4
#define LOADARGS_5(name, __arg1, __arg2, __arg3, __arg4, __arg5) \
	long arg5 = (long) (__arg5); \
	LOADARGS_4(name, __arg1, __arg2, __arg3, __arg4); \
	r7 = arg5
#define LOADARGS_6(name, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
	long arg6 = (long) (__arg6); \
	LOADARGS_5(name, __arg1, __arg2, __arg3, __arg4, __arg5); \
	r8 = arg6

#define ASM_INPUT_0 "0" (r0)
#define ASM_INPUT_1 ASM_INPUT_0, "1" (r3)
#define ASM_INPUT_2 ASM_INPUT_1, "2" (r4)
#define ASM_INPUT_3 ASM_INPUT_2, "3" (r5)
#define ASM_INPUT_4 ASM_INPUT_3, "4" (r6)
#define ASM_INPUT_5 ASM_INPUT_4, "5" (r7)
#define ASM_INPUT_6 ASM_INPUT_5, "6" (r8)

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...) INTERNAL_SYSCALL_NCS(__NR_##name, err, nr, ##args)

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val) ((unsigned long)(val) >= (unsigned long)-4095L)

#undef INTERNAL_SYSCALL_ERRNO_P
#define INTERNAL_SYSCALL_ERRNO_P(val) (-((long)val))

#endif /* __ASSEMBLER__ */

#endif /* linux/ppc64/sysdep.h */
