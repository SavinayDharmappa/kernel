/* gdb_dbg_regs.S - Utilities to read/write debug registers */

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

/*
DESCRIPTION
This module provides the implementation of the gdb_dbg_regs_set/get()
utilities.
 */

#define _ASMLANGUAGE

#include <arch/x86/asm.h>

	/* exports (internal APIs) */

	GTEXT(gdb_dbg_regs_set)
	GTEXT(gdb_dbg_regs_get)

/**
 *
 * @brief Write debug registers
 *
 * This function is used to write to debug registers
 *
 * C function prototype:
 *
 *   void gdb_dbg_regs_set (GDB_DBG_REGS * regs);
 *
 * @return N/A
 */

SECTION_FUNC(TEXT, gdb_dbg_regs_set)
    movl    SP_ARG1(%esp),%edx
    movl    0(%edx),%eax
    movl    %eax,%db0
    movl    4(%edx),%eax
    movl    %eax,%db1
    movl    8(%edx),%eax
    movl    %eax,%db2
    movl    12(%edx),%eax
    movl    %eax,%db3
    movl    16(%edx),%eax
    movl    %eax,%db6
    movl    20(%edx),%eax
    movl    %eax,%db7
    ret

/**
 *
 * @brief Read debug registers
 *
 * This function is used to read debug registers
 *
 * C function prototype:
 *
 *   void gdb_dbg_regs_get (GDB_DBG_REGS * regs);
 *
 * @return N/A
 */

SECTION_FUNC(TEXT, gdb_dbg_regs_get)
    movl    SP_ARG1(%esp),%edx
    movl    %db0,%eax
    movl    %eax,0(%edx)
    movl    %db1,%eax
    movl    %eax,4(%edx)
    movl    %db2,%eax
    movl    %eax,8(%edx)
    movl    %db3,%eax
    movl    %eax,12(%edx)
    movl    %db6,%eax
    movl    %eax,16(%edx)
    movl    %db7,%eax
    movl    %eax,20(%edx)
    ret
