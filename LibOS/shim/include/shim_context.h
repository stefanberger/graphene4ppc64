/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * This file contains definitions for CPU context.
 */

#ifndef _SHIM_CONTEXT_H_
#define _SHIM_CONTEXT_H_

void shim_xstate_init(void);
uint64_t shim_xstate_size(void);
void shim_xstate_restore(const void* xstate_extended);

noreturn void return_with_context(PAL_CONTEXT* context);

#endif /* _SHIM_CONTEXT_H_ */
