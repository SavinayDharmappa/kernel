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

#define TRACE_FLAG          (0x0100)  /* TF in EFLAGS */
#define INT_FLAG            (0x0200)  /* IF in EFLAGS */

/* statics */

static NANO_CPU_EXC_STUB_DECL(nano_bp_exc_stub);
static NANO_CPU_EXC_STUB_DECL(nano_trace_exc_stub);
static NANO_CPU_EXC_STUB_DECL(nano_fpe_stub);
static NANO_CPU_EXC_STUB_DECL(nano_pfault_stub);

/* forward declarations */

static void gdb_bp_handler (NANO_ESF * pEsf);
static void gdb_trace_handler (NANO_ESF * esf);
static void gdb_fpe_handle (NANO_ESF * esf);
static void gdb_pfault_handle (NANO_ESF * esf);

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
    nanoCpuExcConnect (IV_DIVIDE_ERROR, gdb_fpe_handle, nano_fpe_stub);
    nanoCpuExcConnect (IV_DEBUG, gdb_trace_handler, nano_trace_exc_stub);
    nanoCpuExcConnect (IV_BREAKPOINT, gdb_bp_handler, nano_bp_exc_stub);
    nanoCpuExcConnect (IV_PAGE_FAULT, gdb_pfault_handle, nano_pfault_stub);
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
	    regs->regs.eax = esf->eax;
	    regs->regs.ecx = esf->ecx;
	    regs->regs.edx = esf->edx;
	    regs->regs.ebx = esf->ebx;
	    regs->regs.esp = esf->esp;
	    regs->regs.ebp = esf->ebp;
	    regs->regs.esi = esf->esi;
	    regs->regs.edi = esf->edi;
	    regs->regs.eip = esf->eip;
	    regs->regs.eflags = esf->eflags;
	    regs->regs.cs = esf->cs;
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
	    esf->eax = regs->regs.eax;
	    esf->ecx = regs->regs.ecx;
	    esf->edx = regs->regs.edx;
	    esf->ebx = regs->regs.ebx;
	    esf->esp = regs->regs.esp;
	    esf->ebp = regs->regs.ebp;
	    esf->esi = regs->regs.esi;
	    esf->edi = regs->regs.edi;
	    esf->eip = regs->regs.eip;
	    esf->eflags = regs->regs.eflags;
	    esf->cs = regs->regs.cs;
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
    *((uint32_t *)buffer) = regs->regs.eax;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.ecx;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.edx;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.ebx;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.esp;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.ebp;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.esi;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.edi;
    buffer += 4;
    *((uint32_t *)buffer) = (uint32_t) regs->regs.eip;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.eflags;
    buffer += 4;
    *((uint32_t *)buffer) = regs->regs.cs;
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
    regs->regs.eax = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.ecx = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.edx = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.ebx = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.esp = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.ebp = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.esi = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.edi = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.eip = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.eflags = *((uint32_t *)buffer);
    buffer += 4;
    regs->regs.cs = *((uint32_t *)buffer);
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
    if (reg_id >= 0 && reg_id < GDBNUMREGS)
        *size = 4;

    /* Determine register offset */
    if (reg_id >= 0 && reg_id < GDBNUMREGS)
        *offset = 4 * reg_id;
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
	regs->regs.eflags &= ~INT_FLAG;
	regs->regs.eflags |= (arg & INT_FLAG);
	regs->regs.eflags &= ~TRACE_FLAG;
}

/**
 *
 * @brief Test if single stepping is possible for current PC
 *
 * This routine indicates if step is possible for current PC
 *
 * @return 1 if it is possible to step the instruction; -1 otherwise
 *
 * \NOMANUAL
 */

int gdb_arch_can_step (GDB_REG_SET * regs) {
    unsigned char * pc = (unsigned char *) regs->regs.eip;

    if (*pc == 0xf4) return 0;         /* hlt instruction */
    return (1);
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
    int tmp;
    unsigned char * pc = (unsigned char *) regs->regs.eip;

    tmp = regs->regs.eflags;
    if (*pc == 0xfb) tmp |= INT_FLAG;   /* sti instruction */
    if (*pc == 0xfa) tmp &= ~INT_FLAG;  /* cli instruction */
    regs->regs.eflags &= ~INT_FLAG;
    regs->regs.eflags |= TRACE_FLAG;

    return (tmp);
}

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
