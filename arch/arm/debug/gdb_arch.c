/* gdb_arch.c - architecture dependent routine for the GDB server */

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

/* includes */

#include <nanokernel.h>
#include <nano_private.h>
#include <debug/gdb_arch.h>
#include <debug/gdb_server.h>

/* defines */

/* statics */

/* forward declarations */

/* Workaround until sys_debug_current_isf_get() is defined */
NANO_ISF *sys_debug_current_isf_get(void)
{
        return (NANO_ISF *) &_default_esf;
}

/* Workaround until sys_arch_reboot() is defined */
void sys_arch_reboot(int type)
{
}

/**
 *
 * @brief Initialize GDB server architecture part
 *
 * This routine initializes the architecture part of the GDB server. It mostly
 * register exception handler.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

void gdb_arch_init (void) {
}

/**
 *
 * @brief Fill a GDB register set from a given ESF register set
 *
 * This routine fills the provided GDB register set with values from given
 * NANO_ESF register set.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

void gdb_arch_regs_from_esf (GDB_REG_SET * regs, NANO_ESF *esf) {
	    regs->regs.a1 = esf->a1;
	    regs->regs.a2 = esf->a2;
	    regs->regs.a3 = esf->a3;
	    regs->regs.a4 = esf->a4;
	    regs->regs.ip = esf->ip;
	    regs->regs.lr = esf->lr;
	    regs->regs.pc = esf->pc;
	    regs->regs.xpsr = esf->xpsr;
}

/**
 *
 * @brief Fill an ESF register set from a given GDB register set
 *
 * This routine fills the provided NANO_ESF register set with values
 * from given GDB register set.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

void gdb_arch_regs_to_esf (GDB_REG_SET * regs, NANO_ESF *esf) {
	    esf->a1 = regs->regs.a1;
	    esf->a2 = regs->regs.a2;
	    esf->a3 = regs->regs.a3;
	    esf->a4 = regs->regs.a4;
	    esf->ip = regs->regs.ip;
	    esf->lr = regs->regs.lr;
	    esf->pc = regs->regs.pc;
	    esf->xpsr = regs->regs.xpsr;
}

/**
 *
 * @brief Fill given buffer from given register set
 *
 * This routine fills the provided buffer with values from given register set.
 * The provided buffer must be large enough to store all register values.
 * It is up to the caller to do this check.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

void gdb_arch_regs_get (GDB_REG_SET * regs, char * buffer) {
    *((uint32_t *)buffer) = regs->regs.a1;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.a2;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.a3;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.a4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.ip;
    buffer += 4;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.lr;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.pc;
    buffer += sizeof(GDB_INSTR **) + (8 * 12) + 4;
    *((uint32_t *)buffer) = regs->regs.xpsr;
}

/**
 *
 * @brief Write given registers buffer to GDB register set
 *
 * This routine fills given register set with value from provided buffer.
 * The provided buffer must be large enough to contain all register values.
 * It is up to the caller to do this check.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

void gdb_arch_regs_set (GDB_REG_SET * regs,  char * buffer) {
    regs->regs.a1 = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.a2 = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.a3 = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.a4 = *((uint32_t *)buffer);
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    buffer += 4;
    regs->regs.ip = *((uint32_t *)buffer);
    buffer += 4;
    buffer += 4;
    regs->regs.lr = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.pc = *((uint32_t *)buffer);
    buffer += sizeof(GDB_INSTR **) + (8 * 12) + 4;
    regs->regs.xpsr = *((uint32_t *)buffer);
}

/**
 *
 * @brief Get size and offset of given register
 *
 * This routine returns size and offset of given register.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

void gdb_arch_reg_info_get (int reg_id, int * size, int * offset) {
    /* Determine register size */
    if  ((reg_id >= 0 && reg_id < 16) || (reg_id >= 24 && reg_id <26))
        *size = 4;
    else
        *size = 12;

    /* determine register offset */

    if (reg_id >= 0 && reg_id < 16)
        *offset = 4 * reg_id;
    else if (reg_id < 24)
        *offset = 16 * 4 + (reg_id - 16)*12;
    else
        *offset = 16*4 + 8*12 + (reg_id - 24) * 4;
}

/**
 *
 * @brief Clear trace mode
 *
 * This routine makes CPU trace-disable.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

void gdb_trace_mode_clear (GDB_REG_SET * regs, int arg) {
#if 0
	regs->regs.eflags &= ~INT_FLAG;
	regs->regs.eflags |= (arg & INT_FLAG);
	regs->regs.eflags &= ~TRACE_FLAG;
#endif
}

/**
 *
 * @brief Set trace mode
 *
 * This routine makes CPU trace-enable.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

int gdb_trace_mode_set (GDB_REG_SET * regs) {
    int tmp = 0;

#if 0
    tmp = regs->regs.eflags;
    regs->regs.eflags &= ~INT_FLAG;
    regs->regs.eflags |= TRACE_FLAG;
#endif

    return (tmp);
}

#if 0
/**
 *
 * @brief GDB trace handler
 *
 * The GDB trace handler is used to catch and handle the trace mode exceptions
 * (single step).
 *
 * @return N/A
 *
 * \NOMANUAL
 */

static void gdb_trace_handler (NANO_ESF * esf) {
    (void)irq_lock();
    gdb_handler (GDB_EXC_TRACE, esf, GDB_SIG_TRAP);
    }

/**
 *
 * @brief GDB breakpoint handler
 *
 * The GDB breakpoint handler is used to catch and handle the breakpoint
 * exceptions.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

static void gdb_bp_handler (NANO_ESF * esf) {
    (void)irq_lock();
    gdb_debug_status = DEBUGGING;
    esf->eip -= sizeof(GDB_INSTR);
    gdb_handler (GDB_EXC_BP, esf, GDB_SIG_TRAP);
}

/**
 *
 * @brief GDB Floating point handler
 *
 * This GDB handler is used to catch and handle the floating point exceptions.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

static void gdb_fpe_handle (NANO_ESF * esf) {
    (void)irq_lock();
    gdb_debug_status = DEBUGGING;
    gdb_handler (GDB_EXC_OTHER, esf, GDB_SIG_FPE);
    }

/**
 *
 * @brief GDB page fault handler
 *
 * This GDB handler is used to catch and handle the page fault exceptions.
 *
 * @return N/A
 *
 * \NOMANUAL
 */

static void gdb_pfault_handle (NANO_ESF * esf) {
    (void)irq_lock();
    gdb_debug_status = DEBUGGING;
    gdb_handler (GDB_EXC_OTHER, esf, GDB_SIG_SIGSEGV);
    }
#endif

