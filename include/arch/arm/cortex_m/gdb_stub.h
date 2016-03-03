/* gdb_stub.h - extra work performed upon exception entry/exit for GDB */

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

  Prep work done when entering exceptions consists of saving the callee-saved
  registers before they get used by exception handlers, and recording the fact
  that we are running in an exception.
 */

#ifndef _GDB_STUB__H_
#define _GDB_STUB__H_

#ifdef _ASMLANGUAGE

#if defined(CONFIG_GDB_INFO)
GTEXT(_GdbStubExcEntry)
_GDB_STUB_EXC_ENTRY:    .macro
	push {lr}
	bl irq_lock
	bl _GdbStubExcEntry
	bl irq_unlock
	pop {lr}
.endm

GTEXT(_GdbStubExcExit)
_GDB_STUB_EXC_EXIT:     .macro
	push {lr}
	bl irq_lock
	bl _GdbStubExcExit
	bl irq_unlock
	pop {lr}
.endm

_GDB_STUB_PENDSV_EXIT:     .macro
	push {lr}
	bl irq_lock
	bl _GdbStubExcExit
	bl irq_unlock
	pop {lr}
.endm
GTEXT(_irq_vector_table_entry_with_gdb_stub)

#elif defined(CONFIG_ARM_DEBUG_ESF)

#include <offsets.h>

.macro _GDB_STUB_EXC_ENTRY
	cpsid i

	_asm_get_esf ip r0

	mrs ip, MSP
	cmp ip, r0		/* is the ESF on MSP ? */

	/* should read bit 9 of xpsr to see if stack was aligned on 8 bytes */

	add.w r0, #32	/* point r0 to sp before ESF: do NOT update Z flag */

	sub.w ip, r0, #68 /* point ip to ESF: do NOT update Z flag */

	/* Z flags still contains result of cmp ip, r0 */
	ite eq
		moveq sp, ip
		msrne PSP, ip

	stmia ip, {r0, v1-v8}	/* store ptr-to-stack prior to exception and v1-v8
							 * in container for sw ESF right after CPU ESF */

	/* swap _nanokernel.isf with the new one; save the old one on the stack */
    ldr r1, =_nanokernel
    ldr r2, [r1, #__tNANO_isf_OFFSET]
	str ip, [r1, #__tNANO_isf_OFFSET]
	push {r2}

	cpsie i
.endm

.macro _GDB_STUB_EXC_EXIT
	cpsid i

	/* restore previous _nanokernel.isf, get current into r0 */
	pop {r2}
    ldr r1, =_nanokernel
    ldr r0, [r1, #__tNANO_isf_OFFSET]
    str r2, [r1, #__tNANO_isf_OFFSET]

	/* if sp is pointing to the ESF, sp is the MSP */
	cmp sp, r0

	add.w r0, #36 /* point r0 to pushed-by-CPU ESF */

	ite eq
		moveq sp, r0
		msrne PSP, r0

	/* and we're now ready to return from exception */

	cpsie i
.endm

.macro _GDB_STUB_PENDSV_EXIT

	/* ip contains psp entering here, r1 contains _nanokernel */

	cpsid i

	pop {r0}
    eors r0, r0
    str r0, [r1, #__tNANO_isf_OFFSET]

	add.w ip, #36 /* point ip to pushed-by-CPU ESF */

    msr PSP, ip

	/* and we're now ready to return from exception */

	cpsie i
.endm

#else
#define _GDB_STUB_EXC_ENTRY
#define _GDB_STUB_EXC_EXIT
#define _GDB_STUB_PENDSV_EXIT
#endif /* CONFIG_GDB_INFO */

#else
extern void _irq_vector_table_entry_with_gdb_stub(void);
#endif /* _ASMLANGUAGE */
#endif /* _GDB_STUB__H_ */
