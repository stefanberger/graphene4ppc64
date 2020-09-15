/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 IBM Corporation */

#ifndef _SHIM_TCB_ARCH_H_
#define _SHIM_TCB_ARCH_H_

#include <stdint.h>

#include "api.h"
#include "pal.h"

#include "shim_entry_api.h"

static_assert(TCBHEAD_LIBOS_PTR_FROM_END_OFFSET + offsetof(tcbhead_t, LibOS_TCB) ==
              sizeof(tcbhead_t), "TCBHEAD_LIBOS_PTR_FROM_END_OFFSET is wrong");

struct shim_tcb;
typedef struct shim_tcb shim_tcb_t;

static inline uint64_t get_r13(void) {
    register uint64_t r13 __asm__("r13");

    return r13;
}

static inline PAL_TCB* shim_get_pal_tcb(void) {
    /* Do NOT use pal_get_tcb(). it will not work */
    uint64_t r13 = get_r13();

    if (r13)
        return r13_to_pal_tcb(r13);
    return NULL;
}

/* Set r13 given the PAL_TCB */
static inline void shim_set_r13(PAL_TCB* ptcb) {
    pal_set_r13(ptcb);
}

/* Given a register r13, update the LibOS_TCB the PAL_TCB points to */
static inline void shim_r13_set_tcb(uint64_t r13, shim_tcb_t* tcb) {
    assert(r13);
    assert(tcb);
    PAL_TCB* ptcb = r13_to_pal_tcb(r13);
    ptcb->glibc_tcb.LibOS_TCB = container_of(tcb, PAL_TCB, libos_tcb);
}

// r13 == 0: switch to the shim_tcb if there is a glibc TCB set
// r13 != 0: assume tls_base is from glibc, so we set it and
//           connect the shim_tcb to it via its LibOS_TCB pointer
static inline void* shim_tcb_set_r13(unsigned long r13, shim_tcb_t* tcb) {
    PAL_TCB* orig_ptcb = shim_get_pal_tcb();

    if (r13 == 0) {
        PAL_TCB *ptcb = NULL;

        if (orig_ptcb)
            ptcb = orig_ptcb->glibc_tcb.LibOS_TCB;

        /* If shim's and active TCB are different, switch to shim's TCB */
        if (ptcb != orig_ptcb)
            shim_set_r13(ptcb);
    } else {
        tcbhead_t *glibc_tcb = r13_to_tcbhead_t(r13);
        /* given a shim_tcb, get the PAL_TCB it is embedded in */
        PAL_TCB* ptcb = (void*)tcb - offsetof(PAL_TCB, libos_tcb);
        glibc_tcb->LibOS_TCB = ptcb;
        __asm__("addi %%r13, %0, 0\n\t"
                :
                : "r" (r13)
                : "r13"
        );
    }
    return orig_ptcb;
}

#define SHIM_TCB_GET(member)                                            \
    ({                                                                  \
        shim_tcb_t* tcb;                                                \
        PAL_TCB* ptcb;							\
        __typeof__(tcb->member) ret;                                    \
        static_assert(sizeof(ret) == 8 ||                               \
                      sizeof(ret) == 4 ||                               \
                      sizeof(ret) == 2 ||                               \
                      sizeof(ret) == 1,                                 \
                      "SHIM_TCB_GET can be used only for "              \
                      "8, 4, 2, or 1-byte(s) members");                 \
        ptcb = shim_get_pal_tcb();                                      \
        tcb = (shim_tcb_t*)&ptcb->glibc_tcb.LibOS_TCB->libos_tcb[0];    \
        ret = tcb->member;                                              \
        ret;                                                            \
    })

#define SHIM_TCB_SET(member, value)                                     \
    do {                                                                \
        shim_tcb_t* tcb;                                                \
        PAL_TCB* ptcb;							\
        static_assert(sizeof(tcb->member) == 8 ||                       \
                      sizeof(tcb->member) == 4 ||                       \
                      sizeof(tcb->member) == 2 ||                       \
                      sizeof(tcb->member) == 1,                         \
                      "SHIM_TCB_SET can be used only for "              \
                      "8, 4, 2, or 1-byte(s) members");                 \
        ptcb = shim_get_pal_tcb();                                      \
        tcb = (shim_tcb_t*)&ptcb->glibc_tcb.LibOS_TCB->libos_tcb[0];    \
        tcb->member = value;                                            \
    } while (0)

static inline unsigned long get_stack_guard(void) {
    tcbhead_t *tcbhead = r13_to_tcbhead_t(get_r13());

    return tcbhead->stack_guard;
}

static inline void set_stack_guard(uintptr_t stack_guard) {
    tcbhead_t *tcbhead = r13_to_tcbhead_t(get_r13());

    tcbhead->stack_guard = stack_guard;
}

#define DEBUG_STACK_GUARD \
  debug(">>> %s @ %u: stack_guard: 0x%lx [r13: %lx]\n", \
        __func__, __LINE__, get_stack_guard(), get_r13());

/* Set the TLS pointer; r13 contains the value to load into r13 */
/* Due due shim_get_tcb() we have to use a macro */
# define set_tls(r13)						\
    do {							\
        unsigned long stack_guard;				\
        if (r13)						\
            stack_guard = get_stack_guard();			\
        shim_tcb_set_r13(r13, r13 ? shim_get_tcb() : NULL);	\
        if (r13)						\
            set_stack_guard(stack_guard);			\
    } while (0);

#define set_default_tls() \
    set_tls(0)

/* Return the current plain TLS pointer = content of r13 */
static inline unsigned long get_tls(void) {
    return get_r13();
}

#endif /* _SHIM_TCB_ARCH_H_ */
