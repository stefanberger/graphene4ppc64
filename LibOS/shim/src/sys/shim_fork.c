/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 Intel Corporation
 *                    Borys Popławski <borysp@invisiblethingslab.com>
 */

#include <stddef.h> // without this header we are missing `size_t` definition in "signal.h" ...
#include <linux/sched.h>
#include <linux/signal.h>

#include "shim_internal.h"
#include "shim_table.h"

long shim_do_fork(void) {
#if defined(__i386__) || defined(__x86_64__)
    return shim_do_clone(SIGCHLD, 0, NULL, NULL, 0);
#elif defined(__powerpc64__)
    return shim_do_clone(SIGCHLD, 0, NULL, 0, NULL);
#endif
}

long shim_do_vfork(void) {
#if defined(__i386__) || defined(__x86_64__)
    return shim_do_clone(CLONE_VFORK | CLONE_VM | SIGCHLD, 0, NULL, NULL, 0);
#elif defined(__powerpc64__)
    return shim_do_clone(CLONE_VFORK | CLONE_VM | SIGCHLD, 0, NULL, 0, NULL);
#endif
}
