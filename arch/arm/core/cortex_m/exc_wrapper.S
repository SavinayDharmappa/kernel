/*
 * Copyright (c) 2015 Wind River Systems, Inc.
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

/**
 * @file dynamic exception handler wrapper
 */

#define _ASMLANGUAGE

#include <toolchain.h>
#include <arch/cpu.h>

_ASM_FILE_PROLOGUE

GDATA(_sw_exc_table)
GTEXT(_exc_wrapper)

SECTION_FUNC(TEXT, _exc_wrapper)

	_GDB_STUB_EXC_ENTRY

	_asm_get_esf ip r0

	/* r0 now contains the pointer to the full ESF */

	push {lr}	/* lr is now the first item on the stack */

	ldr r1, =_sw_exc_table

	mrs r2, IPSR	/* get exception number */

	/* find and call handler: table is 4-bytes wide, shift index by 2 */
	ldr r1, [r1, r2, LSL #2]
	blx r1

	pop {lr}

	/* exception return is done in _ExcExit(), including _GDB_STUB_EXC_EXIT */
	b _ExcExit

	.end
