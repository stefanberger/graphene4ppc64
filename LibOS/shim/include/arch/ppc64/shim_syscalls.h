/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 IBM Corporation */

/*
 * shim_sysdep-ppc64.h
 */

#ifndef SHIM_SYSCALLS_H_
#define SHIM_SYSCALLS_H_

/* This is to help old header files where various syscall numbers are not
   available.  */
#if !defined __NR_rseq
#define __NR_rseq 387
#endif
#if !defined __NR_io_pgetevents
#define __NR_io_pgetevents 388
#endif
#if !defined __NR_semtimedop
#define __NR_semtimedop 392
#endif
#if !defined __NR_semget
#define __NR_semget 393
#endif
#if !defined __NR_semctl
#define __NR_semctl 394
#endif
#if !defined __NR_shmget
#define __NR_shmget 395
#endif
#if !defined __NR_shmctl
#define __NR_shmctl 396
#endif
#if !defined __NR_shmat
#define __NR_shmat 397
#endif
#if !defined __NR_shmdt
#define __NR_shmdt 398
#endif
#if !defined __NR_msgget
#define __NR_msgget 399
#endif
#if !defined __NR_msgsnd
#define __NR_msgsnd 400
#endif
#if !defined __NR_msgrcv
#define __NR_msgrcv 401
#endif
#if !defined __NR_msgctl
#define __NR_msgctl 402
#endif
#if !defined __NR_clock_gettime64
#define __NR_clock_gettime64 403
#endif
#if !defined __NR_clock_getres_time64
#define __NR_clock_getres_time64 406
#endif
#if !defined __NR_semtimedop_time64
#define __NR_semtimedop_time64 420
#endif
#if !defined __NR_futex_time64
#define __NR_futex_time64 422
#endif
#if !defined __NR_pidfd_send_signal
#define __NR_pidfd_send_signal 424
#endif
#if !defined __NR_io_uring_setup
#define __NR_io_uring_setup 425
#endif
#if !defined __NR_io_uring_enter
#define __NR_io_uring_enter 426
#endif
#if !defined __NR_io_uring_register
#define __NR_io_uring_register 427
#endif
#ifndef __NR_syscalls
#define __NR_syscalls 428
#endif

#endif /* SHIM_SYSCALLS_H_ */
