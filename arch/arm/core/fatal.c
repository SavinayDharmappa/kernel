/* fatal.c - nanokernel fatal error handler for ARM Cortex-M */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
DESCRIPTION
This module provides the _NanoFatalErrorHandler() routine for ARM Cortex-M.
 */

#include <toolchain.h>
#include <sections.h>

#include <nanokernel.h>
#include <nano_private.h>

#ifdef CONFIG_PRINTK
#include <misc/printk.h>
#define PR_EXC(...) printk(__VA_ARGS__)
#else
#define PR_EXC(...)
#endif /* CONFIG_PRINTK */

/*
 * Define a default ESF for use with _NanoFatalErrorHandler() in the event
 * the caller does not have a NANO_ESF to pass
 */
const NANO_ESF _default_esf = {
#if defined(CONFIG_ARM_DEBUG_ESF)
	{0xdeaddead}, /* r13/sp */
	{0xdeaddead}, /* r4/v1 */
	{0xdeaddead}, /* r5/v2 */
	{0xdeaddead}, /* r6/v3 */
	{0xdeaddead}, /* r7/v4 */
	{0xdeaddead}, /* r8/v5 */
	{0xdeaddead}, /* r9/v6 */
	{0xdeaddead}, /* r10/v7 */
	{0xdeaddead}, /* r11/v8 */
#endif
	{0xdeaddead}, /* r0/a1 */
	{0xdeaddead}, /* r1/a2 */
	{0xdeaddead}, /* r2/a3 */
	{0xdeaddead}, /* r3/a4 */
	{0xdeaddead}, /* r12/ip */
	{0xdeaddead}, /* r14/lr */
	{0xdeaddead}, /* r15/pc */
	 0xdeaddead,  /* xpsr */
};

/**
 *
 * @brief Nanokernel fatal error handler
 *
 * This routine is called when fatal error conditions are detected by software
 * and is responsible only for reporting the error. Once reported, it then
 * invokes the user provided routine _SysFatalErrorHandler() which is
 * responsible for implementing the error handling policy.
 *
 * The caller is expected to always provide a usable ESF. In the event that the
 * fatal error does not have a hardware generated ESF, the caller should either
 * create its own or use a pointer to the global default ESF <_default_esf>.
 *
 * @return This function does not return.
 *
 * \NOMANUAL
 */

FUNC_NORETURN void _NanoFatalErrorHandler(
	unsigned int reason, /* reason that handler was called */
	const NANO_ESF *pEsf /* pointer to exception stack frame */
	)
{
	switch (reason) {
	case _NANO_ERR_INVALID_TASK_EXIT:
		PR_EXC("***** Invalid Exit Software Error! *****\n");
		break;

#if defined(CONFIG_STACK_CANARIES)
	case _NANO_ERR_STACK_CHK_FAIL:
		PR_EXC("***** Stack Check Fail! *****\n");
		break;
#endif /* CONFIG_STACK_CANARIES */

	case _NANO_ERR_ALLOCATION_FAIL:
		PR_EXC("**** Kernel Allocation Failure! ****\n");
		break;

	default:
		PR_EXC("**** Unknown Fatal Error %d! ****\n", reason);
		break;
	}
	PR_EXC("Current thread ID = 0x%x\n"
	       "Faulting instruction address = 0x%x\n",
	       sys_thread_self_get(),
	       pEsf->pc);

	/*
	 * Now that the error has been reported, call the user implemented
	 * policy
	 * to respond to the error.  The decisions as to what responses are
	 * appropriate to the various errors are something the customer must
	 * decide.
	 */

	_SysFatalErrorHandler(reason, pEsf);

	for (;;)
		;
}
