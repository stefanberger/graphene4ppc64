/* Copyright (C) 2014 Stony Brook University
   This file is part of Graphene Library OS.

   Graphene Library OS is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Graphene Library OS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/*
 * shim_table-ppc64.c
 *
 * This file contains the system call table used by application libraries.
 */

#include <linux/sched.h>

#include "shim_internal.h"
#include "shim_table.h"
#include "shim_table-arch.h"

shim_fp shim_table[LIBOS_SYSCALL_BOUND] = {
    (shim_fp)0,//shim_do_restart_syscall,
    (shim_fp)shim_do_exit,
    (shim_fp)shim_do_fork,
    (shim_fp)shim_do_read,
    (shim_fp)shim_do_write,
    (shim_fp)shim_do_open,			/* 5 */
    (shim_fp)shim_do_close,
    (shim_fp)shim_do_waitpid,
    (shim_fp)shim_do_creat,
    (shim_fp)0, // shim_do_link,
    (shim_fp)shim_do_unlink,			/* 10 */
    (shim_fp)shim_do_execve,
    (shim_fp)shim_do_chdir,
    (shim_fp)shim_do_time,
    (shim_fp)shim_do_mknod,
    (shim_fp)shim_do_chmod,			/* 15 */
    (shim_fp)0, // shim_do_lchown,
    (shim_fp)0,
    (shim_fp)shim_do_stat,
    (shim_fp)shim_do_lseek,
    (shim_fp)shim_do_getpid,			/* 20 */
    (shim_fp)0, // shim_do_mount,
    (shim_fp)0,
    (shim_fp)shim_do_setuid,
    (shim_fp)shim_do_getuid,
    (shim_fp)0,					/* 25 */
    (shim_fp)0, // shim_do_ptrace,
    (shim_fp)shim_do_alarm,
    (shim_fp)shim_do_fstat,
    (shim_fp)shim_do_pause,
    (shim_fp)0, // shim_do_utime,		/* 30 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)shim_do_access,
    (shim_fp)0, // shim_do_nice
    (shim_fp)0,					/* 35 */
    (shim_fp)0, // shim_do_sync,
    (shim_fp)shim_do_kill,
    (shim_fp)shim_do_rename,
    (shim_fp)shim_do_mkdir,
    (shim_fp)shim_do_rmdir,			/* 40 */
    (shim_fp)shim_do_dup,
    (shim_fp)shim_do_pipe,
    (shim_fp)0, // shim_do_times,
    (shim_fp)0,
    (shim_fp)shim_do_brk,			/* 45 */
    (shim_fp)shim_do_setgid,
    (shim_fp)shim_do_getgid,
    (shim_fp)0,//shim_do_signal
    (shim_fp)shim_do_geteuid,
    (shim_fp)shim_do_getegid,			/* 50 */
    (shim_fp)0, // shim_do_acct,
    (shim_fp)0, // shim_do_umount2,
    (shim_fp)0,
    (shim_fp)shim_do_ioctl,
    (shim_fp)shim_do_fcntl,			/* 55 */
    (shim_fp)0,
    (shim_fp)shim_do_setpgid,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)shim_do_umask,			/* 60 */
    (shim_fp)shim_do_chroot,
    (shim_fp)0, // shim_do_ustat,
    (shim_fp)shim_do_dup2,
    (shim_fp)shim_do_getppid,
    (shim_fp)shim_do_getpgrp,			/* 65 */
    (shim_fp)shim_do_setsid,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0, // shim_do_setreuid,		/* 70 */
    (shim_fp)0, // shim_do_setregid,
    (shim_fp)0,
    (shim_fp)shim_do_rt_sigpending,
    (shim_fp)shim_do_sethostname,
    (shim_fp)shim_do_setrlimit,			/* 75 */
    (shim_fp)0, // shim_do_getrlimit
    (shim_fp)0, // shim_do_getrusage,
    (shim_fp)shim_do_gettimeofday,
    (shim_fp)0, // shim_do_settimeofday,
    (shim_fp)shim_do_getgroups,			/* 80 */
    (shim_fp)shim_do_setgroups,
    (shim_fp)shim_do_select,  // ppc_select??
    (shim_fp)0, // shim_do_symlink,
    (shim_fp)shim_do_lstat,
    (shim_fp)shim_do_readlink,			/* 85 */
    (shim_fp)0, // shim_do_uselib,
    (shim_fp)0, // shim_do_swapon,
    (shim_fp)0, // shim_do_reboot,
    (shim_fp)0,
    (shim_fp)shim_do_mmap,			/* 90 */
    (shim_fp)shim_do_munmap,
    (shim_fp)shim_do_truncate,
    (shim_fp)shim_do_ftruncate,
    (shim_fp)shim_do_fchmod,
    (shim_fp)shim_do_fchown,			/* 95 */
    (shim_fp)shim_do_getpriority,
    (shim_fp)shim_do_setpriority,
    (shim_fp)0,
    (shim_fp)shim_do_statfs,
    (shim_fp)shim_do_fstatfs,			/* 100 */
    (shim_fp)0,
    (shim_fp)shim_do_socketcall,
    (shim_fp)0, // shim_do_syslog,
    (shim_fp)shim_do_setitimer,
    (shim_fp)shim_do_getitimer,			/* 105 */
    (shim_fp)shim_do_stat,
    (shim_fp)shim_do_lstat,
    (shim_fp)shim_do_fstat,
    (shim_fp)shim_do_uname,
    (shim_fp)0,					/* 110 */
    (shim_fp)0, // shim_do_vhangup,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)shim_do_wait4,
    (shim_fp)0, // shim_do_swapoff,		/* 115 */
    (shim_fp)0, // shim_do_sysinfo,
    (shim_fp)0, // shim_do_ipc,
    (shim_fp)shim_do_fsync,
    (shim_fp)0,
    (shim_fp)shim_do_clone,			/* 120 */
    (shim_fp)shim_do_setdomainname,
    (shim_fp)shim_do_uname,  // newuname ?
    (shim_fp)0,
    (shim_fp)0, // shim_do_adjtimex,
    (shim_fp)shim_do_mprotect,			/* 125 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0, // shim_do_init_module,
    (shim_fp)0, // shim_do_delete_module,
    (shim_fp)0,					/* 130 */
    (shim_fp)0, // shim_do_quotactl,
    (shim_fp)shim_do_getpgid,
    (shim_fp)shim_do_fchdir,
    (shim_fp)0,
    (shim_fp)0, // shim_do_sysfs,		/* 135 */
    (shim_fp)0, // shim_do_personality,
    (shim_fp)0,
    (shim_fp)0, // shim_do_setfsuid,
    (shim_fp)0, // shim_do_setfsgid,
    (shim_fp)shim_do__llseek,			/* 140 */
    (shim_fp)shim_do_getdents,
    (shim_fp)shim_do_select,
    (shim_fp)0, // shim_do_flock,
    (shim_fp)shim_do_msync,
    (shim_fp)shim_do_readv,			/* 145 */
    (shim_fp)shim_do_writev,
    (shim_fp)shim_do_getsid,
    (shim_fp)shim_do_fdatasync,
    (shim_fp)0, // sysctl
    (shim_fp)0, // shim_do_mlock,		/* 150 */
    (shim_fp)0, // shim_do_munlock,
    (shim_fp)0, // shim_do_mlockall,
    (shim_fp)0, // shim_do_munlockall,
    (shim_fp)shim_do_sched_setparam,
    (shim_fp)shim_do_sched_getparam,		/* 155 */
    (shim_fp)shim_do_sched_setscheduler,
    (shim_fp)shim_do_sched_getscheduler,
    (shim_fp)shim_do_sched_yield,
    (shim_fp)shim_do_sched_get_priority_max,
    (shim_fp)shim_do_sched_get_priority_min,	/* 160 */
    (shim_fp)shim_do_sched_rr_get_interval,
    (shim_fp)shim_do_nanosleep,
    (shim_fp)0, //shim_do_mremap,
    (shim_fp)0, //shim_do_setresuid,
    (shim_fp)0, // shim_do_getresuid,			/* 165 */
    (shim_fp)0,
    (shim_fp)shim_do_poll,
    (shim_fp)0,
    (shim_fp)0, // shim_do_setresgid,
    (shim_fp)0, // shim_do_getresgid,			/* 170 */
    (shim_fp)0, // shim_do_prctl,
    (shim_fp)shim_do_rt_sigreturn,
    (shim_fp)shim_do_rt_sigaction,
    (shim_fp)shim_do_rt_sigprocmask,
    (shim_fp)shim_do_rt_sigpending,		/* 175 */
    (shim_fp)shim_do_rt_sigtimedwait,
    (shim_fp)0, // shim_do_rt_sigqueueinfo,
    (shim_fp)shim_do_rt_sigsuspend,
    (shim_fp)shim_do_pread64,
    (shim_fp)shim_do_pwrite64,			/* 180 */
    (shim_fp)shim_do_chown,
    (shim_fp)shim_do_getcwd,
    (shim_fp)0, // shim_do_capget,
    (shim_fp)0, // shim_do_capset,
    (shim_fp)shim_do_sigaltstack,		/* 185 */
    (shim_fp)shim_do_sendfile,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)shim_do_vfork,
    (shim_fp)shim_do_getrlimit,			/* 190 */
    (shim_fp)0, // shim_do_readahead,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 195 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0, // shim_do_pciconfig_read,
    (shim_fp)0, // shim_do_pciconfig_write,
    (shim_fp)0, // shim_do_pciconfig_iobase,	/* 200 */
    (shim_fp)0,
    (shim_fp)shim_do_getdents64,
    (shim_fp)0, // shim_do_pivot_root,
    (shim_fp)0,
    (shim_fp)shim_do_madvise,			/* 205 */
    (shim_fp)shim_do_mincore,
    (shim_fp)shim_do_gettid,
    (shim_fp)shim_do_tkill,
    (shim_fp)0, // shim_do_setxattr,
    (shim_fp)0, // shim_do_lsetxattr,			/* 210 */
    (shim_fp)0, // shim_do_fsetxattr,
    (shim_fp)0, // shim_do_getxattr,
    (shim_fp)0, // shim_do_lgetxattr,
    (shim_fp)0, // shim_do_fgetxattr,
    (shim_fp)0, // shim_do_listxattr,		/* 215 */
    (shim_fp)0, // shim_do_llistxattr,
    (shim_fp)0, // shim_do_flistxattr,
    (shim_fp)0, // shim_do_removexattr,
    (shim_fp)0, // shim_do_lremovexattr,
    (shim_fp)0, // shim_do_fremovexattr,		/* 220 */
    (shim_fp)shim_do_futex,
    (shim_fp)shim_do_sched_setaffinity,
    (shim_fp)shim_do_sched_getaffinity,
    (shim_fp)0,
    (shim_fp)0,					/* 225 */
    (shim_fp)0,
    (shim_fp)0, // shim_do_io_setup,
    (shim_fp)0, // shim_do_io_destroy,
    (shim_fp)0, // shim_do_io_getevents,
    (shim_fp)0, // shim_do_io_submit,			/* 230 */
    (shim_fp)0, // shim_do_io_cancel,
    (shim_fp)shim_do_set_tid_address,
    (shim_fp)0, //shim_do_fadvise64,
    (shim_fp)shim_do_exit_group,
    (shim_fp)0, //shim_do_lookup_dcookie,	/* 235 */
    (shim_fp)shim_do_epoll_create,
    (shim_fp)shim_do_epoll_ctl,
    (shim_fp)shim_do_epoll_wait,
    (shim_fp)0, //shim_do_remap_file_pages,
    (shim_fp)0, //shim_do_timer_create,		/* 240 */
    (shim_fp)0, //shim_do_timer_settime,
    (shim_fp)0, //shim_do_timer_gettime,
    (shim_fp)0, //shim_do_timer_getoverrun,
    (shim_fp)0, //shim_do_timer_delete,
    (shim_fp)0, // shim_do_clock_settime,	/* 245 */
    (shim_fp)shim_do_clock_gettime,
    (shim_fp)shim_do_clock_getres,
    (shim_fp)shim_do_clock_nanosleep,
    (shim_fp)0, // ppc64_swapcontext
    (shim_fp)shim_do_tgkill,			/* 250 */
    (shim_fp)0, //shim_do_utimes,
    (shim_fp)shim_do_statfs, // statfs64
    (shim_fp)shim_do_fstatfs,// fstatfs64
    (shim_fp)0,
    (shim_fp)0, // shim_do_rtas			/* 255 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0, //shim_do_migrate_pages,
    (shim_fp)shim_do_mbind,
    (shim_fp)0, //shim_do_get_mempolicy,		/* 260 */
    (shim_fp)0, //shim_do_set_mempolicy,
    (shim_fp)0, //shim_do_mq_open,
    (shim_fp)0, //shim_do_mq_unlink,
    (shim_fp)0, //shim_do_mq_timedsend,
    (shim_fp)0, //shim_do_mq_timedreceive,		/* 265 */
    (shim_fp)0, //shim_do_mq_notify,
    (shim_fp)0, //shim_do_mq_getsetattr,
    (shim_fp)0, //shim_do_kexec_load,
    (shim_fp)0, //shim_do_add_key,
    (shim_fp)0, //shim_do_request_key,		/* 270 */
    (shim_fp)0, //shim_do_keyctl,
    (shim_fp)shim_do_waitid,
    (shim_fp)0, //shim_do_ioprio_set,
    (shim_fp)0, //shim_do_ioprio_get,
    (shim_fp)0, //shim_do_inotify_init,		/* 275 */
    (shim_fp)0, //shim_do_inotify_add_watch,
    (shim_fp)0, //shim_do_inotify_rm_watch,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)shim_do_pselect6,			/* 280 */
    (shim_fp)shim_do_ppoll,
    (shim_fp)0, //shim_do_unshare,
    (shim_fp)0, //shim_do_splice,
    (shim_fp)0, //shim_do_tee,
    (shim_fp)0, //shim_do_vmsplice,			/* 285 */
    (shim_fp)shim_do_openat,
    (shim_fp)shim_do_mkdirat,
    (shim_fp)shim_do_mknodat,
    (shim_fp)shim_do_fchownat,
    (shim_fp)0, //shim_do_futimesat,			/* 290 */
    (shim_fp)shim_do_newfstatat,
    (shim_fp)shim_do_unlinkat,
    (shim_fp)shim_do_renameat,
    (shim_fp)0, //shim_do_linkat,
    (shim_fp)0, //shim_do_symlinkat,			/* 295 */
    (shim_fp)shim_do_readlinkat,
    (shim_fp)shim_do_fchmodat,
    (shim_fp)shim_do_faccessat,
    (shim_fp)shim_do_get_robust_list,
    (shim_fp)shim_do_set_robust_list,		/* 300 */
    (shim_fp)0, //shim_do_move_pages,
    (shim_fp)shim_do_getcpu,
    (shim_fp)shim_do_epoll_pwait,
    (shim_fp)0, //shim_do_utimensat,
    (shim_fp)0, //shim_do_signalfd,		/* 305 */
    (shim_fp)0, //shim_do_timerfd_create,
    (shim_fp)shim_do_eventfd,
    (shim_fp)0, //shim_do_sync_file_range,
    (shim_fp)0, //shim_do_fallocate,
    (shim_fp)0, //shim_do_subpage_prot,		/* 310 */
    (shim_fp)0, //shim_do_timerfd_settime,
    (shim_fp)0, //shim_do_timerfd_gettime,
    (shim_fp)0, //shim_do_signalfd4,
    (shim_fp)shim_do_eventfd2,
    (shim_fp)shim_do_epoll_create1,		/* 315 */
    (shim_fp)shim_do_dup3,
    (shim_fp)shim_do_pipe2,
    (shim_fp)0, //shim_do_inotify_init1,
    (shim_fp)0, //shim_do_perf_event_open,
    (shim_fp)0, //shim_do_preadv,		/* 320 */
    (shim_fp)0, //shim_do_pwritev,
    (shim_fp)0, //shim_do_rt_tgsigqueueinfo,
    (shim_fp)0, //shim_do_fanotify_init,
    (shim_fp)0, //shim_do_fanotify_mark,
    (shim_fp)shim_do_prlimit64,			/* 325 */
    (shim_fp)shim_do_socket,
    (shim_fp)shim_do_bind,
    (shim_fp)shim_do_connect,
    (shim_fp)shim_do_listen,
    (shim_fp)shim_do_accept,			/* 330 */
    (shim_fp)shim_do_getsockname,
    (shim_fp)shim_do_getpeername,
    (shim_fp)shim_do_socketpair,
    (shim_fp)shim_do_send,
    (shim_fp)shim_do_sendto,			/* 335 */
    (shim_fp)shim_do_recv,
    (shim_fp)shim_do_recvfrom,
    (shim_fp)shim_do_shutdown,
    (shim_fp)shim_do_setsockopt,
    (shim_fp)shim_do_getsockopt,		/* 340 */
    (shim_fp)shim_do_sendmsg,
    (shim_fp)shim_do_recvmsg,
    (shim_fp)shim_do_recvmmsg,
    (shim_fp)shim_do_accept4,
    (shim_fp)0, //shim_do_name_to_handle_at,	/* 345 */
    (shim_fp)0, //shim_do_open_by_handle_at,
    (shim_fp)0, //shim_do_clock_adjtime,
    (shim_fp)0, //shim_do_sysnfs,
    (shim_fp)shim_do_sendmmsg,
    (shim_fp)0, // shim_do_setns,		/* 350 */
    (shim_fp)0, //shim_do_process_vm_readv,
    (shim_fp)0, //shim_do_process_vm_writev,
    (shim_fp)0, //shim_do_finit_module,
    (shim_fp)0, //shim_do_kcmp,
    (shim_fp)0, //shim_do_sched_setattr		/* 355 */
    (shim_fp)0, //shim_do_sched_getattr
    (shim_fp)0, //shim_do_renameat2,
    (shim_fp)0, //shim_do_seccomp,
    (shim_fp)shim_do_getrandom,
    (shim_fp)0, //shim_do_memfd_create		/* 360 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 365 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 370 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 375 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 380 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 385 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 390*/
    (shim_fp)0,
    (shim_fp)0,//shim_do_semtimedop,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 395*/
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 400 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)shim_do_clock_gettime64,
    (shim_fp)0,
    (shim_fp)0,					/* 405 */
    (shim_fp)shim_do_clock_getres_time64,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 410 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,					/* 415 */
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,
    (shim_fp)0,//shim_do_semtimedop_time64,		/* 420 */
    (shim_fp)0,
    (shim_fp)shim_do_futex_time64,
    (shim_fp)0,
    (shim_fp)0,
};

noreturn void shim_table_dispatch(PAL_CONTEXT *context) {
    /* after a restart_syscall we need to adjust nip to be able to resume properly */
    context->gpregs.nip = context->gpregs.link;

    unsigned long syscallnr = pal_context_get_syscall(context);
#if 0
    log_debug("Calling Syscall %lu.  r3=0x%lx r4=0x%lx r5=0x%lx r6=0x%lx r7=0x%lx r8=0x%lx",
          syscallnr, ALL_SYSCALL_ARGS(context));
    log_debug("%s: SYSCALLNR: %ld, FUNCTION to return to: 0x%lx", __func__, syscallnr, context->gpregs.link);
#endif
    if (syscallnr == 2 || syscallnr == 189 || syscallnr == 120) {
        log_debug("SYSCALLNR: %ld  SP = 0x%lx, IP = 0x%lx, R2 = 0x%lx, R12 = 0x%lx, R13 = 0x%lx",
                  syscallnr, context->gpregs.gpr[1], context->gpregs.nip,
                  context->gpregs.gpr[2], context->gpregs.gpr[12], context->gpregs.gpr[13]);

        log_debug("vrsave: 0x%lx fpscr: 0x%lx vscr: 0x%08lx%08lx",
                  context->xtregs.vrsave, context->xtregs.fpscr,
                  context->xtregs.vscr.a, context->xtregs.vscr.b);
    }

    switch (syscallnr) {
    case 2:
        log_debug("fork syscall (%ld) not supported. HALTING", syscallnr);
        while(1);
        break;
    }

    shim_emulate_syscall(context);
}

