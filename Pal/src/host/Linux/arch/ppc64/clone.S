/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2014 Stony Brook University
 * Copyright (C) 2020 IBM Corporation
 */

/*
 * clone-ppc64.S
 *
 * This file contains architecture-specific implementation of clone
 * method.
 * The source code is imported and modified from the GNU C Library.
 */

/* clone() is even more special than fork() as it mucks with stacks
   and invokes a function in the right context after its all over.  */

#include <asm/unistd.h>
#include <asm/errno.h>

#include "sysdep-arch.h"
#include "stack.h"

#define CLONE_VM	0x00000100
#define CLONE_THREAD	0x00010000

#define PPC64_LOAD_FUNCPTR(ptr) \
	mr	%r12, ptr;	\
	mtctr 	%r12

	.text
ENTRY (clone)
	/* Check for child_stack == NULL || fn == NULL.  */
	cmpdi	cr0,%r4,0
	cmpdi	cr1,%r3,0
	cror	cr0*4+eq,cr1*4+eq,cr0*4+eq
	beq-	cr0,.Lbadargs

	/* Save some regs in the "red zone".  */
	std	%r29,-24(%r1)
	std	%r30,-16(%r1)
	std	%r31,-8(%r1)
	cfi_offset(%r29,-24)
	cfi_offset(%r30,-16)
	cfi_offset(%r31,-8)

	/* Set up stack frame for child.  */
	clrrdi	%r4,%r4,4
	li	%r0,0
	stdu	%r0,-FRAME_MIN_SIZE_PARM(%r4)

	/* Save fn, args, stack across syscall.  */
	mr	%r30,%r3			/* Function in r30.  */
	mr	%r29,%r5			/* Flags in r29.  */
	mr	%r31,%r6			/* Argument in r31.  */

	/* 'flags' argument is first parameter to clone syscall.
	   Second is the stack pointer, already in r4.  */
	mr	%r3,%r5
	/* Move the parent_tid, child_tid and tls arguments. */
	mr	%r5,%r7
	mr	%r6,%r8
	mr	%r7,%r9

	/* End FDE now, because in the child the unwind info will be
	   wrong.  */
	cfi_endproc

	/* Do the call.  */
	DO_CALL(SYS_ify(clone))

	/* Check for child process.  */
	cmpdi	cr1,%r3,0
	crandc	cr1*4+eq,cr1*4+eq,cr0*4+so
	bne-	cr1,.Lparent		/* The '-' is to minimise the race.  */

	std	%r2,FRAME_TOC_SAVE(%r1)
	/* Call procedure.  */
	PPC64_LOAD_FUNCPTR(%r30)
	mr	%r3,%r31
	bctrl
	ld	%r2,FRAME_TOC_SAVE(%r1)

	DO_CALL(SYS_ify(exit))
	/* We won't ever get here but provide a nop so that the linker
	   will insert a toc adjusting stub if necessary.  */
	nop

.Lbadargs:
	cfi_startproc
	li	%r3,-1	/* -1 is sufficient for Graphene */
	blr

.Lparent:
	/* Parent.  Restore registers & return.  */
	cfi_offset(%r29,-24)
	cfi_offset(%r30,-16)
	cfi_offset(%r31,-8)
	ld	%r29,-24(%r1)
	ld	%r30,-16(%r1)
	ld	%r31,-8(%r1)
	cfi_restore(%r29)
	cfi_restore(%r30)
	cfi_restore(%r31)

	bnslr+
	li	%r3, -1
	blr

END (clone)
