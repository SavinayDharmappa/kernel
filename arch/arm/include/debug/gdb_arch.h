/* gdb_arch.h - architecture dependent GDB Server header file */

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

#ifndef __INCgdb_archh
#define __INCgdb_archh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <nano_private.h>

/* define */

#define GDB_ARCH_HAS_WRCONS
#define GDB_ARCH_HAS_ALL_REGS
#define GDB_ARCH_NO_CACHE

#ifndef CONFIG_GDB_RAM_SIZE
#define CONFIG_GDB_RAM_SIZE	CONFIG_SRAM_SIZE*1024
#endif
#ifndef CONFIG_GDB_RAM_ADDRESS
#define CONFIG_GDB_RAM_ADDRESS	CONFIG_SRAM_BASE_ADDRESS
#endif

#define GDB_BUF_SIZE    600    /* Default GDB buffer size */

#define GDB_TGT_ARCH    "arm"

#define GDBNUMREGBYTES  (16*4 + 8*12 + 2*4)

#define GDB_PC_REG      15

#define GDB_BREAK_INST	0xE7FDDEFE

#ifndef _ASMLANGUAGE

/* typedef */

/* Workaround until NANO_ISF exists for ARM */
typedef struct __esf NANO_ISF;

typedef unsigned int GDB_INSTR;

typedef struct
    {
    NANO_ISF regs;
    } GDB_REG_SET;

/* function declaration */

extern void gdb_arch_init (void);
extern void gdb_arch_regs_from_esf (GDB_REG_SET * regs, NANO_ESF *esf);
extern void gdb_arch_regs_to_esf (GDB_REG_SET * regs, NANO_ESF *esf);
extern void gdb_arch_regs_get (GDB_REG_SET * regs, char * buffer) ;
extern void gdb_arch_regs_set (GDB_REG_SET * regs,  char * buffer);
extern void gdb_arch_reg_info_get (int reg_id, int * size, int * offset);
extern void gdb_trace_mode_clear (GDB_REG_SET * regs, int arg);
extern int gdb_trace_mode_set (GDB_REG_SET * regs);

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __INCgdb_archh */
