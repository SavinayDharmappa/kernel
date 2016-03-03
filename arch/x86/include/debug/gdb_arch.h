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
#include <misc/debug/gdb_server.h>

/* define */

#define GDB_ARCH_HAS_ALL_REGS
#ifndef CONFIG_GDB_SERVER_BOOTLOADER
#undef  GDB_ARCH_HAS_HW_BP
#define GDB_ARCH_HAS_WRCONS
#define GDB_ARCH_HAS_RUNCONTROL
#define GDB_ARCH_CAN_STEP       gdb_arch_can_step
#endif

#ifndef CONFIG_GDB_RAM_SIZE
#define CONFIG_GDB_RAM_SIZE	CONFIG_RAM_SIZE*1024
#endif
#ifndef CONFIG_GDB_RAM_ADDRESS
#define CONFIG_GDB_RAM_ADDRESS	0x100000
#endif

#ifdef CONFIG_GDB_SERVER_BOOTLOADER
#define GDB_BUF_SIZE    8192   /* Default GDB buffer size */
#else
#define GDB_BUF_SIZE    600    /* Default GDB buffer size */
#endif

#define GDB_TGT_ARCH    "i386"

#define GDBNUMREGS	(16)
#define GDBNUMREGBYTES  (GDBNUMREGS*4)

#define GDB_PC_REG      8

#define GDB_BREAK_INST	(0xcc)    /* int 3 */

#ifndef _ASMLANGUAGE

/* typedef */

typedef unsigned char GDB_INSTR;

typedef struct {
    NANO_ISF regs;
    unsigned int pad1;	/* padding for ss register */
    unsigned int pad2;	/* padding for ds register */
    unsigned int pad3;	/* padding for es register */
    unsigned int pad4;	/* padding for fs register */
    unsigned int pad5;	/* padding for gs register */
} GDB_REG_SET;

typedef struct {
    unsigned int    db0;    /* debug register 0 */
    unsigned int    db1;    /* debug register 1 */
    unsigned int    db2;    /* debug register 2 */
    unsigned int    db3;    /* debug register 3 */
    unsigned int    db6;    /* debug register 6 */
    unsigned int    db7;    /* debug register 7 */
} GDB_DBG_REGS;

#ifdef GDB_ARCH_HAS_RUNCONTROL
#ifdef GDB_ARCH_HAS_HW_BP
extern volatile int gdb_cpu_stop_bp_type;
extern long gdb_cpu_stop_hw_bp_addr;
#endif
#endif

/* function declaration */

extern void gdb_arch_init (void);
extern void gdb_arch_regs_from_esf (GDB_REG_SET * regs, NANO_ESF *esf);
extern void gdb_arch_regs_to_esf (GDB_REG_SET * regs, NANO_ESF *esf);
extern void gdb_arch_regs_from_isf (GDB_REG_SET * regs, NANO_ISF *esf);
extern void gdb_arch_regs_to_isf (GDB_REG_SET * regs, NANO_ISF *esf);
extern void gdb_arch_regs_get (GDB_REG_SET * regs, char * buffer) ;
extern void gdb_arch_regs_set (GDB_REG_SET * regs,  char * buffer);
extern void gdb_arch_reg_info_get (int reg_id, int * size, int * offset);
extern void gdb_trace_mode_clear (GDB_REG_SET * regs, int arg);
extern int gdb_trace_mode_set (GDB_REG_SET * regs);
extern int gdb_arch_can_step (GDB_REG_SET * regs);
#ifdef GDB_ARCH_HAS_HW_BP
extern int gdb_hw_bp_set (GDB_DBG_REGS * regs, long addr, GDB_BP_TYPE type, int length, GDB_ERROR_CODE * err);
extern int gdb_hw_bp_clear (GDB_DBG_REGS * regs, long addr, GDB_BP_TYPE type, int length, GDB_ERROR_CODE * err);
extern void gdb_dbg_regs_set (GDB_DBG_REGS * regs);
extern void gdb_dbg_regs_get (GDB_DBG_REGS * regs);
extern void gdb_dbg_regs_clear (void);
#endif

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __INCgdb_archh */
