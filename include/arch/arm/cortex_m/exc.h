/* cortex_m/exc.h - Cortex-M public exception handling */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
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
ARM-specific nanokernel exception handling interface. Included by ARM/arch.h.
 */

#ifndef _ARCH_ARM_CORTEXM_EXC_H_
#define _ARCH_ARM_CORTEXM_EXC_H_

#ifdef _ASMLANGUAGE
GTEXT(_ExcExit);

/* Reimplements _ScbIsNestedExc() and fetches the ESF on the correct stack. */

#include <arch/arm/cortex_m/asm_inline_gcc.h>

.macro _asm_get_esf s e /* s: scratch, e: ESF returned */

    ldr \s, =_SCS_ICSR
    ldr \s, [\s]
    ands.w \s, #_SCS_ICSR_RETTOBASE

	ite eq			/* is the RETTOBASE bit zero ? */
	mrseq \e, MSP	/* if so, we're not returning to thread mode, thus this
					 * is a nested exception: the stack frame is on the MSP */
	mrsne \e, PSP	/* if not, we are returning to thread mode, thus this is
					 * not a nested exception: the stack frame is on the PSP */

.endm

#else
#include <stdint.h>

struct __esf {

	/* part of exception stack frame created in software */
#if CONFIG_ARM_DEBUG_ESF
	sys_define_gpr_with_alias(sp, r13);
	sys_define_gpr_with_alias(v1, r4);
	sys_define_gpr_with_alias(v2, r5);
	sys_define_gpr_with_alias(v3, r6);
	sys_define_gpr_with_alias(v4, r7);
	sys_define_gpr_with_alias(v5, r8);
	sys_define_gpr_with_alias(v6, r9);
	sys_define_gpr_with_alias(v7, r10);
	sys_define_gpr_with_alias(v8, r11);
#endif

	/* part of exception stack frame pushed by the CPU */
	sys_define_gpr_with_alias(a1, r0);
	sys_define_gpr_with_alias(a2, r1);
	sys_define_gpr_with_alias(a3, r2);
	sys_define_gpr_with_alias(a4, r3);
	sys_define_gpr_with_alias(ip, r12);
	sys_define_gpr_with_alias(lr, r14);
	sys_define_gpr_with_alias(pc, r15);
	uint32_t xpsr;

};

typedef struct __esf NANO_ESF;
typedef struct __esf NANO_ISF;

extern const NANO_ESF _default_esf;

extern void _ExcExit(void);

#if !defined(CONFIG_XIP)

/* currently, exception connecting is only available to non-XIP kernels */

/**
 * @brief signature for an exception handler
 */

#define sys_exc_handler_sig(x) void (x)(NANO_ESF *esf)

/**
 * @brief exception handler data type
 */

typedef sys_exc_handler_sig(sys_exc_handler_t);

/**
 * @brief connect a handler to an exception vector
 *
 * Connect the @a handler to the exception vector @a num.
 *
 * The @a unused parameter is only there to match the x86 signature.
 *
 * @param num Exception vector number
 * @param handler Exception handler to connect
 * @param unused Unused
 *
 * @return N/A
 */

extern void sys_exc_connect(unsigned int num, sys_exc_handler_t *handler,
							void *unused);

/**
 * @brief alias of sys_exc_connect
 *
 * See sys_exc_connect().
 */

extern void nanoCpuExcConnect(unsigned int, sys_exc_handler_t *, void *);

/**
 * @brief display the contents of a exception stack frame
 *
 * @return N/A
 */

extern void sys_exc_esf_dump(NANO_ESF *esf);

#endif

#endif /* _ASMLANGUAGE */
#endif /* _ARCH_ARM_CORTEXM_EXC_H_ */
