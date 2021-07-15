/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2021 IBM Corporation
 *                    Stefan Berger <stefanb@linux.ibm.com>
 */

#ifndef SHIM_ENTRY_API_H_
#define SHIM_ENTRY_API_H_

/* Offsets of shim_tcb entry vectors relative to pal_tcb */
#define SHIM_SYSCALLDB_OFFSET         24
#define SHIM_REGISTER_LIBRARY_OFFSET  32

/* offset of LIBOS_PTR from tcbhead_t's end */
#define TCBHEAD_LIBOS_PTR_FROM_END_OFFSET (14 * 8)

#ifdef __ASSEMBLER__

/*
 * SYSCALLDB_R9:
 * This macro is a replacement for the 'sc' instruction.
 * As such its register clobbering behavior is like that of a Linux syscall
 * where the following registers are volatile: r0, r3-r8, cr0
 * Since we are doing an API call here we may be clobbering the following
 * registers: r0, r3-r12, lr, ctr, xer, cr0-cr1, cr5-cr7 (+some fp registers)
 * So we have to store: r9 - r12, lr, ctr, xer, cr1, cr5-cr7
 *
 * We do an ABI-compliant call to Graphene's syscalldb with r9 holding the
 * syscall number (rather than r0) and registers r3-r8 holding parameters.
 */
/* FIXME: we may need to store more registers at a SAFE PLACE! */
# define SYSCALLDB_R9(FRAMESIZE)                 \
    std 0,-FRAMESIZE(1); /* save syscall nr */   \
    mflr 0;                                      \
    std 0,16(1);         /* save lr */           \
    ld 9,-FRAMESIZE(1);  /* restore syscall nr */\
    stdu 1,-FRAMESIZE(1);                        \
    /* call syscalld using tcbhead_t->LibOS */   \
    std 2,24(1);                                 \
    subi 12,13,0x7000+TCBHEAD_LIBOS_PTR_FROM_END_OFFSET; \
    /* pointer to PAL_TCB in tcbhead_t */        \
    ld 12,0(12);                                 \
    /* our special offset in PAL_TCB for syscalldb */ \
    ld 12,SHIM_SYSCALLDB_OFFSET(12);             \
    /* address of function must be in r12 */     \
    mtctr 12;                                    \
    bctrl;                                       \
    ld 2,24(1);                                  \
    addi 1,1,FRAMESIZE;                          \
    ld 0,16(1);         /* restore lr */         \
    mtlr 0;

#else /* !__ASSEMBLER__ */

static inline int shim_register_library(const char* name, unsigned long load_address) {
    typedef int(*shim_register_library_func)(const char *, unsigned long);
    shim_register_library_func shim_register_library_fn;

    __asm__ __volatile__("subi %0,13,%1\n\t"
                         "ld %0,0(%0)\n\t"
                         "ld %0,%2(%0)\n\t"
                         : "=&r" (shim_register_library_fn)
                         : "i" (0x7000 + TCBHEAD_LIBOS_PTR_FROM_END_OFFSET),
                           "i" (SHIM_REGISTER_LIBRARY_OFFSET)
                         :);
    return shim_register_library_fn(name, load_address);
}

#endif /* __ASSEMBLER__ */

#endif /* SHIM_ENTRY_API_H_ */
