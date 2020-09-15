/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * shim_context-ppc64.c
 *
 * This file contains code for ppc64-specific CPU context manipulation.
 */

#include <asm/unistd.h>
#include <stdalign.h>
#include <stdnoreturn.h>

#include "asm-offsets.h"
#include "pal.h"
#include "shim_context.h"
#include "shim_internal.h"
#include "shim_thread.h"
#include "stack.h"
#include "ucontext.h"
#include "shim_table-arch.h"

#define CR0_SO (0x80000000 >> 3)  /* summary overflow; indicates syscall error */

/* Glib'c passes us the thread's function to call and the parameter to pass via the stack
 */
static void fixup_child_context_thread(PAL_CONTEXT* context, uint64_t r13) {
    void *stack = (void *)pal_context_get_sp(context);
    struct stackframe *sf = stack - offsetof(struct stackframe, backchain);
    // glibc's clone.S set parm_save and lr_save
    context->gpregs.gpr[3] = sf->parm_save[0];                // parameter to function
    context->gpregs.gpr[12] = sf->lr_save;                    // function to call
    context->gpregs.gpr[13] = r13;                            // TLS
    context->gpregs.nip = sf->lr_save;                        // function to call

    log_debug("child swapping stack to %p return 0x%lx", stack, context->gpregs.nip);
}

/* A child process needs to return 0 on the fork() or clone() call to indicate child.
 */
static void fixup_child_context_process(PAL_CONTEXT* context) {
    context->gpregs.gpr[3] = 0;
    context->gpregs.ccr &= ~CR0_SO;
}

noreturn void restore_child_context_after_clone(struct shim_context* context, bool is_process) {
    assert(context->regs);

    uint64_t tls = context->tls;

    if (is_process)
        fixup_child_context_process(context->regs);
    else
        fixup_child_context_thread(context->regs, tls);

    context->syscall_nr = -1;

    // the process we will resume points to an old PAL_TCB via r13; its
    // glibc_tcb.LibOS_TCB is inaccessible and so we need to update it
    // to the one we are using here
    shim_r13_set_tcb(context->regs->gpregs.gpr[13], shim_get_tcb());

    PAL_CONTEXT *regs = context->regs;
    context->regs = NULL;

#if 0
    log_debug("restore context: SP = 0x%08lx, IP = 0x%08lx, R2 = 0x%lx, R3 = 0x%lx, R4 = 0x%lx, R5 = 0x%lx, R12 = 0x%lx, R13 = 0x%lx",
          regs->gpregs.gpr[1], regs->gpregs.nip, regs->gpregs.gpr[2],
          regs->gpregs.gpr[3], regs->gpregs.gpr[4], regs->gpregs.gpr[5],
          regs->gpregs.gpr[12], regs->gpregs.gpr[13]);
    log_debug("restore context: vrsave: 0x%lx fpscr: 0x%lx vscr: 0x%08lx%08lx",
          regs->xtregs.vrsave, regs->xtregs.fpscr,
          regs->xtregs.vscr.a, regs->xtregs.vscr.b);
    struct stackframe *sf = (void *)regs->gpregs.gpr[1];
    log_debug("Resume: Stack of child: sp: %p lr_save: 0x%lx  toc_save: 0x%lx",
          sf, sf->lr_save, sf->toc_save);
#endif

    /* context is all set up for process and thread to directly return */
    return_with_context(regs);
}

noreturn void return_with_context(PAL_CONTEXT* context) {
    __asm__ __volatile__(
        "mr 31, %0\n\t"
        "ld  0," XSTRINGIFY(SHIM_REGS_VRSAVE) "(31)\n\t; mtvrsave 0\n\t"
        "lfd 0," XSTRINGIFY(SHIM_REGS_FPSCR)  "(31)\n\t; mtfsf 255,0,1,0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSCR) "\n\t; lvx 0,31,0\n\t; mtvscr 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR0)";lxvd2x  0, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR1)";lxvd2x  1, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR2)";lxvd2x  2, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR3)";lxvd2x  3, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR4)";lxvd2x  4, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR5)";lxvd2x  5, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR6)";lxvd2x  6, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR8)";lxvd2x  7, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR9)";lxvd2x  8, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR9)";lxvd2x  9, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR10)";lxvd2x 10, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR11)";lxvd2x 11, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR12)";lxvd2x 12, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR13)";lxvd2x 13, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR14)";lxvd2x 14, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR15)";lxvd2x 15, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR16)";lxvd2x 16, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR17)";lxvd2x 17, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR18)";lxvd2x 18, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR19)";lxvd2x 19, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR20)";lxvd2x 20, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR21)";lxvd2x 21, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR22)";lxvd2x 22, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR23)";lxvd2x 23, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR24)";lxvd2x 24, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR25)";lxvd2x 25, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR26)";lxvd2x 26, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR27)";lxvd2x 27, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR28)";lxvd2x 28, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR29)";lxvd2x 29, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR30)";lxvd2x 30, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR31)";lxvd2x 31, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR32)";lxvd2x 32, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR33)";lxvd2x 33, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR34)";lxvd2x 34, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR35)";lxvd2x 35, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR36)";lxvd2x 36, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR37)";lxvd2x 37, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR38)";lxvd2x 38, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR39)";lxvd2x 39, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR40)";lxvd2x 40, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR41)";lxvd2x 41, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR42)";lxvd2x 42, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR43)";lxvd2x 43, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR44)";lxvd2x 44, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR45)";lxvd2x 45, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR46)";lxvd2x 46, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR47)";lxvd2x 47, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR48)";lxvd2x 48, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR49)";lxvd2x 49, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR50)";lxvd2x 50, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR51)";lxvd2x 51, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR52)";lxvd2x 52, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR53)";lxvd2x 53, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR54)";lxvd2x 54, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR55)";lxvd2x 55, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR56)";lxvd2x 56, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR57)";lxvd2x 57, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR58)";lxvd2x 58, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR59)";lxvd2x 59, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR60)";lxvd2x 60, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR61)";lxvd2x 61, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR62)";lxvd2x 62, 31, 0\n\t"
        "li  0," XSTRINGIFY(SHIM_REGS_VSR63)";lxvd2x 63, 31, 0\n\t"
        "ld  1," XSTRINGIFY(SHIM_REGS_GPR1)  "(31)\n\t"
        "ld  2," XSTRINGIFY(SHIM_REGS_GPR2)  "(31)\n\t"
        "ld  3," XSTRINGIFY(SHIM_REGS_GPR3)  "(31)\n\t"
        "ld  4," XSTRINGIFY(SHIM_REGS_GPR4)  "(31)\n\t"
        "ld  5," XSTRINGIFY(SHIM_REGS_GPR5)  "(31)\n\t"
        "ld  6," XSTRINGIFY(SHIM_REGS_GPR6)  "(31)\n\t"
        "ld  7," XSTRINGIFY(SHIM_REGS_GPR7)  "(31)\n\t"
        "ld  8," XSTRINGIFY(SHIM_REGS_GPR8)  "(31)\n\t"
        "ld  9," XSTRINGIFY(SHIM_REGS_GPR9)  "(31)\n\t"
        "ld 10," XSTRINGIFY(SHIM_REGS_GPR10) "(31)\n\t"
        "ld 11," XSTRINGIFY(SHIM_REGS_GPR11) "(31)\n\t"
        "ld 12," XSTRINGIFY(SHIM_REGS_GPR12) "(31)\n\t"
        "ld 13," XSTRINGIFY(SHIM_REGS_GPR13) "(31)\n\t"
        "ld 14," XSTRINGIFY(SHIM_REGS_GPR14) "(31)\n\t"
        "ld 15," XSTRINGIFY(SHIM_REGS_GPR15) "(31)\n\t"
        "ld 16," XSTRINGIFY(SHIM_REGS_GPR16) "(31)\n\t"
        "ld 17," XSTRINGIFY(SHIM_REGS_GPR17) "(31)\n\t"
        "ld 18," XSTRINGIFY(SHIM_REGS_GPR18) "(31)\n\t"
        "ld 19," XSTRINGIFY(SHIM_REGS_GPR19) "(31)\n\t"
        "ld 20," XSTRINGIFY(SHIM_REGS_GPR20) "(31)\n\t"
        "ld 21," XSTRINGIFY(SHIM_REGS_GPR21) "(31)\n\t"
        "ld 22," XSTRINGIFY(SHIM_REGS_GPR22) "(31)\n\t"
        "ld 23," XSTRINGIFY(SHIM_REGS_GPR23) "(31)\n\t"
        "ld 24," XSTRINGIFY(SHIM_REGS_GPR24) "(31)\n\t"
        "ld 25," XSTRINGIFY(SHIM_REGS_GPR25) "(31)\n\t"
        "ld 26," XSTRINGIFY(SHIM_REGS_GPR26) "(31)\n\t"
        "ld 27," XSTRINGIFY(SHIM_REGS_GPR27) "(31)\n\t"
        "ld 28," XSTRINGIFY(SHIM_REGS_GPR28) "(31)\n\t"
        "ld 29," XSTRINGIFY(SHIM_REGS_GPR29) "(31)\n\t"
        "ld 30," XSTRINGIFY(SHIM_REGS_GPR30) "(31)\n\t"
        "ld  0," XSTRINGIFY(SHIM_REGS_LINK)  "(31)\n\t"
        "mtlr 0\n\t"
        "ld  0," XSTRINGIFY(SHIM_REGS_XER)   "(31)\n\t"
        "mtxer 0\n\t"
        "ld  0," XSTRINGIFY(SHIM_REGS_CCR)   "(31)\n\t"
        "mtcr 0\n\t"
        "ld  0," XSTRINGIFY(SHIM_REGS_NIP)   "(31)\n\t"
        "mtctr 0\n\t"
        "ld  0," XSTRINGIFY(SHIM_STACK_GUARD)"(31)\n\t"
        "std 0,-28688(13)\n\t"
        "ld  0," XSTRINGIFY(SHIM_REGS_GPR0)  "(31)\n\t"
        "ld 31," XSTRINGIFY(SHIM_REGS_GPR31) "(31)\n\t"
        "bctr\n\t"
        :
        : "r"(context)
        :
    );
    __builtin_unreachable();
}

noreturn void return_from_syscall(PAL_CONTEXT* context) {
    /* a special case is the syscall restart; we must not clobber gpr3 ! */
    if (context->gpregs.nip == (unsigned long)&syscalldb)
        return_with_context(context);

    int64_t *ret = (int64_t *)&context->gpregs.gpr[3];
    uint64_t *ccr = &context->gpregs.ccr;

    if (*ret < 0) {
        *ret = -(*ret);
        *ccr |= CR0_SO;
    } else {
        *ccr &= ~CR0_SO;
    }

    return_with_context(context);
}

noreturn void rt_sigreturn_caller(PAL_CONTEXT* context);
noreturn void rt_sigreturn_caller(PAL_CONTEXT* context) {
    /* synthesise a call to rt_sigreturn; do NOT call function directly! */
    pal_context_set_syscall(context, __NR_rt_sigreturn);
    shim_table_dispatch(context);
}

noreturn void rt_sigreturn_wrapper(void);
__asm__(
    "\t.section\t\".text\"\n\t"
    ".align 2\n\t"
    ".globl\trt_sigreturn_wrapper\n\t"
    ".type\trt_sigreturn_wrapper, @function\n\t"
    "rt_sigreturn_wrapper:\n\t"
    ".cfi_startproc\n\t"
    "ld 2,24(1)\n\t"   /* restore r2 */
    "ld 3,32(1)\n\t"   /* restore context */
    "b rt_sigreturn_caller\n\t"
    ".cfi_endproc\n\t"
    ".size\trt_sigreturn_wrapper,.-rt_sigreturn_wrapper\n\t"
);

struct sigframe {
    ucontext_t uc;
    siginfo_t siginfo;
};

void prepare_sigframe(PAL_CONTEXT* context, siginfo_t* siginfo, void* handler, void* restorer,
                      bool should_use_altstack, __sigset_t* old_mask) {
    struct shim_thread* current = get_cur_thread();

    uint64_t stack = get_stack_for_sighandler(context->gpregs.gpr[1], should_use_altstack);

    struct sigframe* sigframe = NULL;
    stack = ALIGN_DOWN(stack - sizeof(*sigframe), alignof(*sigframe));
    stack = ALIGN_DOWN(stack, 0x10);
    /* Make sure `stack` is now aligned to both `alignof(*sigframe)` and 0x10. */
    static_assert(alignof(*sigframe) % 0x10 == 0 || 0x10 % alignof(*sigframe) == 0,
                  "Incorrect sigframe alignment");
    sigframe = (struct sigframe*)stack;
    /* This could probably be omited as we set all fields explicitly below. */
    memset(sigframe, 0, sizeof(*sigframe));

    sigframe->siginfo = *siginfo;

    sigframe->uc.uc_flags = 0;
    sigframe->uc.uc_link = NULL;
    sigframe->uc.uc_stack = current->signal_altstack;

    pal_context_to_ucontext(&sigframe->uc, context);

    sigframe->uc.uc_sigmask = *old_mask;

    /* Below the sigframe is out function stack frame;
     * We need a 'struct stackframe' with 1 parameter
     */
    stack = ALIGN_DOWN(stack - sizeof(struct stackframe) - 8, alignof(struct stackframe));
    stack = ALIGN_DOWN(stack, 0x10);

    context->gpregs.gpr[1] = stack;
    /* parameters to signal handler 'sa_sigaction' */
    context->gpregs.gpr[3] = (uint64_t)siginfo->si_signo;
    context->gpregs.gpr[4] = (uint64_t)&sigframe->siginfo;
    context->gpregs.gpr[5] = (uint64_t)&sigframe->uc;
    /* the signal handler to call */
    context->gpregs.nip = (uint64_t)handler;
    context->gpregs.gpr[12] = (uint64_t)handler;

    context->gpregs.link = (uint64_t)rt_sigreturn_wrapper;

    /* build a stackframe for rt_sigreturn_wrapper to get r2 and parameter from */
    struct stackframe *stackframe = (struct stackframe *)stack;
    register uint64_t r2 __asm__("r2");
    stackframe->toc_save = r2;
    stackframe->parm_save[0] = (uint64_t)context;

    log_debug("Created sigframe for sig: %d at %p (handler: %p, restorer: %p)",
              siginfo->si_signo, sigframe, handler, restorer);
}

void restore_sigreturn_context(PAL_CONTEXT* context, __sigset_t* new_mask) {
    uint64_t stack = context->gpregs.gpr[1];
    stack = ALIGN_UP(stack + sizeof(struct stackframe) + 8, alignof(struct stackframe));
    stack = ALIGN_UP(stack, 0x10);
    struct sigframe *sigframe = (struct sigframe *)stack;
    log_debug("Restoring sigframe at %p", sigframe);

    *new_mask = sigframe->uc.uc_sigmask;

    ucontext_to_pal_context(context, &sigframe->uc);
}

void restart_syscall(PAL_CONTEXT* context, uint64_t sysnr) {
    context->gpregs.gpr[3] = context->gpregs.orig_gpr3;
    pal_context_set_syscall(context, sysnr);
    context->gpregs.nip = (uint64_t)&syscalldb;
}

bool maybe_emulate_syscall(PAL_CONTEXT* context) {
    __UNUSED(context);
    log_debug("%s: not implemented!", __func__);
    die_or_inf_loop();
}

void shim_xstate_init(void) {
}
