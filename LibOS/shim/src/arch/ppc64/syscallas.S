/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 IBM Corporation */

/*
 * syscallas-ppc64.S
 *
 * This file contains the entry point of system call table in library OS.
 */

#include <shim_defs.h>
#include "asm-offsets.h"

#define SAVE_GPRS(OFFSET, FRAME, R1_FIXUP)		\
	std	%r0, OFFSET+SHIM_REGS_GPR0(FRAME);	\
	addi	%r0, %r1, R1_FIXUP;			\
	std	%r0, OFFSET+SHIM_REGS_GPR1(FRAME);	\
	std	%r2, OFFSET+SHIM_REGS_GPR2(FRAME);	\
	std	%r3, OFFSET+SHIM_REGS_GPR3(FRAME);	\
	std	%r3, OFFSET+SHIM_REGS_ORIG_GPR3(FRAME);	\
	std	%r4, OFFSET+SHIM_REGS_GPR4(FRAME);	\
	std	%r5, OFFSET+SHIM_REGS_GPR5(FRAME);	\
	std	%r6, OFFSET+SHIM_REGS_GPR6(FRAME);	\
	std	%r7, OFFSET+SHIM_REGS_GPR7(FRAME);	\
	std	%r8, OFFSET+SHIM_REGS_GPR8(FRAME);	\
	std	%r9, OFFSET+SHIM_REGS_GPR9(FRAME);	\
	std	%r10, OFFSET+SHIM_REGS_GPR10(FRAME);	\
	std	%r11, OFFSET+SHIM_REGS_GPR11(FRAME);	\
	std	%r12, OFFSET+SHIM_REGS_GPR12(FRAME)

#define SAVE_NV_GPRS(OFFSET, FRAME)			\
	std	%r13, OFFSET+SHIM_REGS_GPR13(FRAME);	\
	std	%r14, OFFSET+SHIM_REGS_GPR14(FRAME);	\
	std	%r15, OFFSET+SHIM_REGS_GPR15(FRAME);	\
	std	%r16, OFFSET+SHIM_REGS_GPR16(FRAME);	\
	std	%r17, OFFSET+SHIM_REGS_GPR17(FRAME);	\
	std	%r18, OFFSET+SHIM_REGS_GPR18(FRAME);	\
	std	%r19, OFFSET+SHIM_REGS_GPR19(FRAME);	\
	std	%r20, OFFSET+SHIM_REGS_GPR20(FRAME);	\
	std	%r21, OFFSET+SHIM_REGS_GPR21(FRAME);	\
	std	%r22, OFFSET+SHIM_REGS_GPR22(FRAME);	\
	std	%r23, OFFSET+SHIM_REGS_GPR23(FRAME);	\
	std	%r24, OFFSET+SHIM_REGS_GPR24(FRAME);	\
	std	%r25, OFFSET+SHIM_REGS_GPR25(FRAME);	\
	std	%r26, OFFSET+SHIM_REGS_GPR26(FRAME);	\
	std	%r27, OFFSET+SHIM_REGS_GPR27(FRAME);	\
	std	%r28, OFFSET+SHIM_REGS_GPR28(FRAME);	\
	std	%r29, OFFSET+SHIM_REGS_GPR29(FRAME);	\
	std	%r30, OFFSET+SHIM_REGS_GPR30(FRAME);	\
	std	%r31, OFFSET+SHIM_REGS_GPR31(FRAME);

#define SAVE_SPRS(OFFSET, FRAME)			\
	mflr	%r0;					\
	std	%r0, OFFSET+SHIM_REGS_LINK(FRAME);	\
	std	%r0, OFFSET+SHIM_REGS_NIP(FRAME);	\
	mfctr	%r0;					\
	std	%r0, OFFSET+SHIM_REGS_CTR(FRAME);	\
	mfxer	%r0;					\
	std	%r0, OFFSET+SHIM_REGS_XER(FRAME);	\
	mfcr	%r0;					\
	std	%r0, OFFSET+SHIM_REGS_CCR(FRAME);	\
	/* msr, dar, dsisr cannot be read */		\
	li	%r0, 0;					\
	std	%r0, OFFSET+SHIM_REGS_MSR(FRAME);	\
	std	%r0, OFFSET+SHIM_REGS_DAR(FRAME);	\
	std	%r0, OFFSET+SHIM_REGS_DSISR(FRAME);

#define SAVE_STACK_GUARD(OFFSET, FRAME)			\
	ld	%r0, -28688(%r13);			\
	std	%r0, OFFSET+SHIM_STACK_GUARD(FRAME);

#define SAVE_VSR(REG, FRAME, OFFSET)			\
	li	%r0, OFFSET;				\
	stxvd2x	%vs##REG, FRAME, %r0;

/* SAVE_VSRS clobbers %f0 and %v0 */
#define SAVE_VSRS(OFFSET, FRAME)			\
	mfvrsave %r0;					\
	std	%r0, OFFSET+SHIM_REGS_VRSAVE(FRAME);	\
	SAVE_VSR(0 , FRAME, OFFSET + SHIM_REGS_VSR0)	\
	SAVE_VSR(1 , FRAME, OFFSET + SHIM_REGS_VSR1)	\
	SAVE_VSR(2 , FRAME, OFFSET + SHIM_REGS_VSR2)	\
	SAVE_VSR(3 , FRAME, OFFSET + SHIM_REGS_VSR3)	\
	SAVE_VSR(4 , FRAME, OFFSET + SHIM_REGS_VSR4)	\
	SAVE_VSR(5 , FRAME, OFFSET + SHIM_REGS_VSR5)	\
	SAVE_VSR(6 , FRAME, OFFSET + SHIM_REGS_VSR6)	\
	SAVE_VSR(7 , FRAME, OFFSET + SHIM_REGS_VSR7)	\
	SAVE_VSR(8 , FRAME, OFFSET + SHIM_REGS_VSR8)	\
	SAVE_VSR(9 , FRAME, OFFSET + SHIM_REGS_VSR9)	\
	SAVE_VSR(10, FRAME, OFFSET + SHIM_REGS_VSR10)	\
	SAVE_VSR(11, FRAME, OFFSET + SHIM_REGS_VSR11)	\
	SAVE_VSR(12, FRAME, OFFSET + SHIM_REGS_VSR12)	\
	SAVE_VSR(13, FRAME, OFFSET + SHIM_REGS_VSR13)	\
	SAVE_VSR(14, FRAME, OFFSET + SHIM_REGS_VSR14)	\
	SAVE_VSR(15, FRAME, OFFSET + SHIM_REGS_VSR15)	\
	SAVE_VSR(16, FRAME, OFFSET + SHIM_REGS_VSR16)	\
	SAVE_VSR(17, FRAME, OFFSET + SHIM_REGS_VSR17)	\
	SAVE_VSR(18, FRAME, OFFSET + SHIM_REGS_VSR18)	\
	SAVE_VSR(19, FRAME, OFFSET + SHIM_REGS_VSR19)	\
	SAVE_VSR(20, FRAME, OFFSET + SHIM_REGS_VSR20)	\
	SAVE_VSR(21, FRAME, OFFSET + SHIM_REGS_VSR21)	\
	SAVE_VSR(22, FRAME, OFFSET + SHIM_REGS_VSR22)	\
	SAVE_VSR(23, FRAME, OFFSET + SHIM_REGS_VSR23)	\
	SAVE_VSR(24, FRAME, OFFSET + SHIM_REGS_VSR24)	\
	SAVE_VSR(25, FRAME, OFFSET + SHIM_REGS_VSR25)	\
	SAVE_VSR(26, FRAME, OFFSET + SHIM_REGS_VSR26)	\
	SAVE_VSR(27, FRAME, OFFSET + SHIM_REGS_VSR27)	\
	SAVE_VSR(28, FRAME, OFFSET + SHIM_REGS_VSR28)	\
	SAVE_VSR(29, FRAME, OFFSET + SHIM_REGS_VSR29)	\
	SAVE_VSR(30, FRAME, OFFSET + SHIM_REGS_VSR30)	\
	SAVE_VSR(31, FRAME, OFFSET + SHIM_REGS_VSR31)	\
	SAVE_VSR(32, FRAME, OFFSET + SHIM_REGS_VSR32)	\
	SAVE_VSR(33, FRAME, OFFSET + SHIM_REGS_VSR33)	\
	SAVE_VSR(34, FRAME, OFFSET + SHIM_REGS_VSR34)	\
	SAVE_VSR(35, FRAME, OFFSET + SHIM_REGS_VSR35)	\
	SAVE_VSR(36, FRAME, OFFSET + SHIM_REGS_VSR36)	\
	SAVE_VSR(37, FRAME, OFFSET + SHIM_REGS_VSR37)	\
	SAVE_VSR(38, FRAME, OFFSET + SHIM_REGS_VSR38)	\
	SAVE_VSR(39, FRAME, OFFSET + SHIM_REGS_VSR39)	\
	SAVE_VSR(40, FRAME, OFFSET + SHIM_REGS_VSR40)	\
	SAVE_VSR(41, FRAME, OFFSET + SHIM_REGS_VSR41)	\
	SAVE_VSR(42, FRAME, OFFSET + SHIM_REGS_VSR42)	\
	SAVE_VSR(43, FRAME, OFFSET + SHIM_REGS_VSR43)	\
	SAVE_VSR(44, FRAME, OFFSET + SHIM_REGS_VSR44)	\
	SAVE_VSR(45, FRAME, OFFSET + SHIM_REGS_VSR45)	\
	SAVE_VSR(46, FRAME, OFFSET + SHIM_REGS_VSR46)	\
	SAVE_VSR(47, FRAME, OFFSET + SHIM_REGS_VSR47)	\
	SAVE_VSR(48, FRAME, OFFSET + SHIM_REGS_VSR48)	\
	SAVE_VSR(49, FRAME, OFFSET + SHIM_REGS_VSR49)	\
	SAVE_VSR(50, FRAME, OFFSET + SHIM_REGS_VSR50)	\
	SAVE_VSR(51, FRAME, OFFSET + SHIM_REGS_VSR51)	\
	SAVE_VSR(52, FRAME, OFFSET + SHIM_REGS_VSR52)	\
	SAVE_VSR(53, FRAME, OFFSET + SHIM_REGS_VSR53)	\
	SAVE_VSR(54, FRAME, OFFSET + SHIM_REGS_VSR54)	\
	SAVE_VSR(55, FRAME, OFFSET + SHIM_REGS_VSR55)	\
	SAVE_VSR(56, FRAME, OFFSET + SHIM_REGS_VSR56)	\
	SAVE_VSR(57, FRAME, OFFSET + SHIM_REGS_VSR57)	\
	SAVE_VSR(58, FRAME, OFFSET + SHIM_REGS_VSR58)	\
	SAVE_VSR(59, FRAME, OFFSET + SHIM_REGS_VSR59)	\
	SAVE_VSR(60, FRAME, OFFSET + SHIM_REGS_VSR60)	\
	SAVE_VSR(61, FRAME, OFFSET + SHIM_REGS_VSR61)	\
	SAVE_VSR(62, FRAME, OFFSET + SHIM_REGS_VSR62)	\
	SAVE_VSR(63, FRAME, OFFSET + SHIM_REGS_VSR63)	\
	mfvrsave %r0;					\
	std	%r0, OFFSET + SHIM_REGS_VRSAVE(FRAME);	\
	mffs	%f0;					\
	stfd	%f0, OFFSET + SHIM_REGS_FPSCR(FRAME);	\
	mfvscr	%v0;					\
	li	%r0, OFFSET + SHIM_REGS_VSCR;		\
	stvx	%v0, FRAME, %r0;

/* restore v0 and f0 */
#define LOAD_F0_V0(OFFSET, FRAME)			\
	li	%r0, OFFSET + SHIM_REGS_VSR32;		\
	lvx	%v0, FRAME, %r0;			\
	lfd	%f0, OFFSET + SHIM_REGS_VSR0(FRAME);

	.global syscalldb
	.type syscalldb, @function

	.global shim_table_dispatch
	.type shim_table_dispatch, @function

# We are called with syscall number in r9, syscall parameters in r3-r8
# We pass them straight through to shim_table_dispatch & pointer to cr in r10
syscalldb:
	.cfi_startproc
0:
	# we want to store our registers in the blue zone, so extend the
	# stack to make room on stack for the context we store below
	stdu	%r1,-SHIM_REGS_SIZE-32(%r1)	# @1: make room on stack
						# SHIM_REGS_SIZE: sizeof(PAL_CONTEXT)
						# 32: for normal frame

	# before we clobber any more registers, we store them on the stack's
	# blue zone now; this will clobber r0 due to r1 fixup and the SPRS,
	# but it's saved on the stack before
	SAVE_GPRS(32, %r1, 32+SHIM_REGS_SIZE)
	SAVE_SPRS(32, %r1)

	# We need to always save all registers since we may have to build
	# up a signal stack
	SAVE_NV_GPRS(32, %r1)
	# We need to save the stack guard, especially to support fork()
	SAVE_STACK_GUARD(32, %r1)

	# storing VSRS clobbers f0 and v0 ...
	SAVE_VSRS(32, %r1)
	# ... we immediately restore them
	LOAD_F0_V0(32, %r1)

	addis	%r2, %r12, (.TOC. - 0b)@ha
	addi	%r2, %r2, (.TOC. - 0b)@l

	addi	%r3, %r1, 32				# r3 points to context now
	rldicr	%r1, %r1, 0, 59				# ensure 16-byte aligned stack

	# call shim_table_display with context ptr in r3; this will NOT return
	b	shim_table_dispatch

	.cfi_endproc
	.size syscalldb, .-syscalldb
