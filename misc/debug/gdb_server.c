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

This module provides the embedded GDB Remote Serial Protocol for VxMicro

The following is a list of all currently defined GDB RSP commands.

`?'
    Indicate the reason the target halted.

`c [addr]'
    Continue. addr is address to resume. If addr is omitted, resume at
    current address.

`C sig[;addr]'
    Continue with signal sig (hex signal number). If `;addr' is omitted,
    resume at same address.

    _WRS_XXX - Current limitation: Even if this syntax is understood, the GDB
    server does not resume the context with the specified signal but resumes it
    with the exception vector that caused the context to stop.

`D'
    Detach GDB from the remote system.

`g'
    Read general registers.

`G XX...'
    Write general registers.

`k'
    Detach GDB from the remote system.

`m addr,length'
    Read length bytes of memory starting at address addr. Note that addr may
    not be aligned to any particular boundary.  The stub need not use any
    particular size or alignment when gathering data from memory for the
    response; even if addr is word-aligned and length is a multiple of the word
    size, the stub is free to use byte accesses, or not. For this reason, this
    packet may not be suitable for accessing memory-mapped I/O devices.

`M addr,length:XX...'
    Write length bytes of memory starting at address addr. XX... is the data.
    Each byte is transmitted as a two-digit hexadecimal number.

`p n'
    Read the value of register n; n is in hex.

`P n...=r...'
    Write register n... with value r.... The register number n is in
    hexadecimal, and r... contains two hex digits for each byte in the
    register (target byte order).

`q name params...'
    General query. See General Query Packets description

`s [addr]'
    Single step. addr is the address at which to resume. If addr is omitted,
    resume at same address.

`S sig[;addr]'
    Step with signal. This is analogous to the `C' packet, but requests a
    single-step, rather than a normal resumption of execution.

    _WRS_XXX - Current limitation: Even if this syntax is understood, the GDB server
    steps the context without the specified signal (i.e like the `s [addr]'
    command). 

`T thread-id'
    Find out if the thread thread-id is alive.

`vCont[;action[:thread-id]]...'
    Resume the inferior, specifying different actions for each thread. If an
    action is specified with no thread-id, then it is applied to any threads
    that don't have a specific action specified; if no default action is
    specified then other threads should remain stopped. Specifying multiple
    default actions is an error; specifying no actions is also an error.

    Currently supported actions are:

    `c'
        Continue.
    `C sig'
        Continue with signal sig. The signal sig should be two hex digits.
    `s'
        Step.
    `S sig'
        Step with signal sig. The signal sig should be two hex digits.

    The optional argument addr normally associated with the `c', `C', `s',
    and `S' packets is not supported in `vCont'.

`X addr,length:XX...'
    Write length bytes of memory starting at address addr, where the data is transmitted
    in binary. The binary data representation uses 7d (ascii ‘}’) as an escape character.
    Any escaped byte is transmitted as the escape character followed by the original
    character XORed with 0x20.

`z type,addr,length'
`Z type,addr,length'
    Insert (`Z') or remove (`z') a type breakpoint starting at addr address of
    length length.

General Query Packets:
`qC'
    Return the current thread ID.

`qSupported'
    Query the GDB server for features it supports. This packet allows client to
    take advantage of GDB server's features.

    These are the currently defined GDB server features, in more detail:

    `PacketSize=bytes'
        The GDB server can accept packets up to at least bytes in length. client
        will send packets up to this size for bulk transfers, and will never
        send larger packets. This is a limit on the data characters in the
        packet, including the frame and checksum. There is no trailing NUL
        byte in a remote protocol packet;

    `qXfer:features:read'
        Access the target description. Target description can identify the
        architecture of the remote target and (for some architectures) provide
        information about custom register sets. They can also identify the OS
        ABI of the remote target. Client can use this information to
        autoconfigure for your target, or to warn you if you connect to an
        unsupported target.

        By default, the following simple target description is supported:

        <target version="1.0">
            <architecture>i386</architecture>
        </target>

        But architectures may also reports information on specific features
        such as extended registers definitions or hardware breakpoint
        definitions.

        Each `<feature>' describes some logical portion of the target system.
        A `<feature>' element has this form:
 
        <feature name="NAME">
            [TYPE...]
            REG...
        </feature>
 
        Each feature's name should be unique within the description.  The name
        of a feature does not matter unless GDB has some special knowledge of
        the contents of that feature; if it does, the feature should have its
        standard name.

        Extended registers definitions are reported following the standard
        register format defined by GDB Remote protocol:

        Each register is represented as an element with this form:
 
        <reg name="NAME"
           bitsize="SIZE"
           [regnum="NUM"]
           [save-restore="SAVE-RESTORE"]
           [type="TYPE"]
           [group="GROUP"]/>
 
        The components are as follows:
 
        NAME
            The register's name; it must be unique within the target
            description.
 
        BITSIZE
            The register's size, in bits.
 
        REGNUM
            The register's number.  If omitted, a register's number is one
            greater than that of the previous register (either in the current
            feature or in a preceding feature); the first register in the
            target description defaults to zero.  This register number is used
            to read or write the register; e.g. it is used in the remote `p'
            and `P' packets, and registers appear in the `g' and `G' packets
            in order of increasing register number.
 
        SAVE-RESTORE
            Whether the register should be preserved across inferior function
            calls; this must be either `yes' or `no'.  The default is `yes',
            which is appropriate for most registers except for some system
            control registers; this is not related to the target's ABI.
 
        TYPE
            The type of the register.  TYPE may be a predefined type, a type
            defined in the current feature, or one of the special types `int'
            and `float'.  `int' is an integer type of the correct size for
            BITSIZE, and `float' is a floating point type (in the
            architecture's normal floating point format) of the correct size
            for BITSIZE.  The default is `int'.
 
        GROUP
            The register group to which this register belongs.  GROUP must be
            either `general', `float', or `vector'.  If no GROUP is specified,
            GDB will not display the register in `info registers'.
 

        Hardware breakpoint definitions are reported using the following format:

        <feature name="HW_BP_FEATURE">
            <defaults
                    max_bp="MAX_BP"
                    max_inst_bp="MAX_INST_BP"
                max_watch_bp="MAX_WATCH_BP"
                length="LENGTH"
                >
            HW_BP_DESC...
        </feature>

        The defaults section allows to define some default values and avoid to
        list them in each HW_BP_DESC.

        Each HW_BP_DESC entry has the form:

        <hwbp type="ACCESS_TYPE"
              [length="LENGTH"]
              [max_bp="MAX_BP"]
              />

        If HW_BP_DESC defines an item which has a default value defined, then it
        overwrite the default value for HW_BP_DESC entry.

        Items in [brackets] are optional. The components are as follows:

        MAX_BP
            Maximum number of hardware breakpoints that can be set.

        MAX_INST_BP
            Maximum number of instruction hardware breakpoints that can be set.

        MAX_WATCH_BP
            Maximum number of data hardware breakpoints that can be set.

        LENGTH
            Supported access lengths (in hexadecimal without 0x prefix).
            Access lengths are encoded as powers of 2 which can be OR'ed.
            For example, if an hardware breakpoint type supports 1, 2, 4,
            8 bytes access, length will be f (0x1|0x2|0x4|0x8).

        ACCESS_TYPE
            Hardware breakpoint type:
                inst  : Instruction breakpoint
                watch : Write access breakpoint
                rwatch: Read access breakpoint
                awatch: Read|Write access breakpoint

        The GDB server can also reports additional information using the
        "WR_AGENT_FEATURE" feature. The purpose of this feature is to report
        information about the agent configuration.
        The GDB server feature is using the following format:

        <feature name="WR_AGENT_FEATURE">
            <config max_sw_bp="MAX_SW_BP"
                    step_only_on_bp="STEP_ONLY_ON_BP"
                    />
        </feature>

        The components are as follows:

        MAX_SW_BP
            Maximum number of software breakpoint that can be set.

        STEP_ONLY_ON_BP
            This parameter is set to 1 if the GDB server is only able to
            step the context which hit the breakpoint.
            This parameter is set to 0 if the GDB server is able to step
            any context.

    `QStartNoAckMode'
        By default, when either the client or the server sends a packet,
        the first response expected is an acknowledgment: either `+' (to
        indicate the package was received correctly) or `-' (to request
        retransmission). This mechanism allows the GDB remote protocol to
        operate over unreliable transport mechanisms, such as a serial line.

        In cases where the transport mechanism is itself reliable (such as a
        pipe or TCP connection), the `+'/`-' acknowledgments are redundant. It
        may be desirable to disable them in that case to reduce communication
        overhead, or for other reasons. This can be accomplished by means of
        the `QStartNoAckMode' packet.

    `WrCons`
        This parameter indicates that the GDB server supports transfer of
        VxMicro console I/O to the client using GDB notification packets.

        XXX - Current limitation: For now, the GDB server only supports the
        console output.

    `qwr.vmcs:thread-id,vmcs-addr,regnum'
            This is a WR extension of the GDB Remote Serial Protocol. This command
        reads the VMCS register <regnum> of the cpu <thread-id> with a VMCS
        area located at <vmcs-addr>.

    `Qwr.vmcs:thread-id,vmcs-addr,regnum=val'
            This is a WR extension of the GDB Remote Serial Protocol. This command
        writes <val> to the VMCS register <regnum> of the cpu <thread-id> with
        a VMCS area located at <vmcs-addr>.

    `qwr.eregs:regnum'
            This is a WR extension of the GDB Remote Serial Protocol. This command
        reads the extended register <regnum> of the selected cpu (i.e cpu
        selected by the `Hg thread-id' command).

    `qwr.cpuid:<arg1>,<arg2>"
        This is a WR extension of the GDB Remote Serial Protocol. This command
        execute the cpuid instruction if supported by current cpu. The command
        arguments are specified with <arg1> and <arg2>. The cpuid instruction
        results consist in four 32-bit values returned in hexadecimal format:

            "<res1><res2><res3><res4>"

Notification Packets:
    WR extension of the GDB Remote Serial Protocol uses notification packets
    (See `WrCons` support).
    Those packets are transfered using the following format:
        %<notificationName:<notificationData>#<checksum>

    For example:
        %WrCons:<notificationData>#<checksum>
*/

#if defined(CONFIG_ARM) && defined(CONFIG_GDB_SERVER_BOOTLOADER)
#define GDB_ARM_BOOTLOADER
#endif

#include <nanokernel.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <nano_private.h>
#include <board.h>
#include <device.h>
#include <uart.h>
#include <cache.h>
#include <init.h>
#include <debug/gdb_arch.h>
#include <misc/debug/mem_safe.h>
#include <gdb_server.h>
#include <debug_info.h>
#ifdef CONFIG_GDB_SERVER_INTERRUPT_DRIVEN
#include <drivers/console/uart_console.h>
#endif
#ifdef CONFIG_REBOOT
#include <misc/reboot.h>
#endif
#ifdef GDB_ARM_BOOTLOADER
#include <system_timer.h>
#endif

/* defines */

#define STUB_OK "OK"
#define STUB_ERROR "ENN"

#ifndef GDB_NOTIF_DATA_SIZE
#define GDB_NOTIF_DATA_SIZE     100     /* Size of notification data buffers */
#endif
#define NOTIF_PACKET_OVERHEAD   6       /* Overhead size for notification */
                                        /* packet encoding. */

#define MAX_SW_BP       CONFIG_GDB_SERVER_MAX_SW_BP     /* Maximum number of software breakpoints */

#define GDB_INVALID_REG_SET     ((void *)-1)

#define OUTBUF_FILL(x) strncpy ((char *)gdb_buffer, x, GDB_BUF_SIZE - 1)

#define STR_COMMA_SEPARATOR     ","
#ifdef GDB_ARCH_HAS_VMCS
#define STR_QWR_VMCS    ";qwr.vmcs+;Qwr.vmcs+"
#endif
#ifdef GDB_ARCH_HAS_EXTENDED_REGISTERS
#define STR_QWR_EREGS   ";qwr.eregs+"
#endif
#ifdef GDB_ARCH_HAS_CPUID
#define STR_QWR_CPUID   ";qwr.cpuid+"
#endif
#ifdef CONFIG_REBOOT
#define STR_REBOOT      ";reboot+"
#endif
#ifdef CONFIG_GDB_SERVER_BOOTLOADER
#define STR_TYPE      ";type=zephyr_boot"
#else
#define STR_TYPE      ";type=zephyr"
#endif

#ifdef GDB_ARCH_HAS_RUNCONTROL
#define	GDB_SYSTEM_RESUME	gdb_system_resume()
#define	GDB_BPS_REMOVE		gdb_bps_remove()
#define GDB_BPS_UNINSTALL	gdb_bps_uninstall()
#else
#define	GDB_SYSTEM_RESUME
#define	GDB_BPS_REMOVE
#define GDB_BPS_UNINSTALL
#endif

/* typedef */
 
#ifdef GDB_ARCH_HAS_RUNCONTROL
typedef struct
    {
    GDB_INSTR * addr;   /* breakpoint address */
    GDB_INSTR insn;     /* saved instruction */
    char valid;         /* breakpoint is valid? */
    char enabled;       /* breakpoint is enabled? */
    } BP_ARRAY;
#endif

/* local definitions */

static const unsigned char hexchars[] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static int gdb_is_connected = 0;        /* A client is connected to GDB Server */
static int gdb_no_ack = 0;
static int gdb_regs_not_valid = 1;
static volatile int gdb_event_is_pending = 0;
static volatile int gdb_cpu_stop_signal = GDB_SIG_NULL;
static volatile int gdb_cpu_pending_sig;
static GDB_REG_SET gdb_regs;

static const char * xml_target_header = "<?xml version=\"1.0\"?> " \
                            "<!DOCTYPE target SYSTEM " \
                            "\"gdb-target.dtd\"> <target version=\"1.0\">\n";
static const char * xml_target_footer = "</target>";
static unsigned char gdb_buffer [GDB_BUF_SIZE];
#if defined(GDB_ARCH_HAS_VMCS) || defined(GDB_ARCH_HAS_EXTENDED_REGISTERS)
static unsigned char tmp_reg_buffer [GDB_BUF_SIZE];
#else
static unsigned char tmp_reg_buffer [GDBNUMREGBYTES];
#endif

#ifdef GDB_ARM_BOOTLOADER
static int boot_regs_set = 0;
#endif

#ifdef GDB_ARCH_HAS_RUNCONTROL
#ifdef GDB_ARCH_HAS_HW_BP
static int gdb_hw_bp_cnt = 0;
static GDB_DBG_REGS dbg_regs;
#endif
static int gdb_stop_by_bp = 0;
static int gdb_trace_lock_key = 0;

/*
 * GDB breakpoint table. Note that all the valid entries in the
 * breakpoint table are kept contiguous. When parsing the table, the first
 * invalid entry in the table marks the end of the table.
 */
static BP_ARRAY bpArray[MAX_SW_BP];

#ifdef GDB_ARCH_NO_SINGLE_STEP
static GDB_INSTR * gdb_step_emu_next_pc;
static GDB_INSTR gdb_step_emu_insn;
#endif /* GDB_ARCH_NO_SINGLE_STEP */
#endif

#ifdef GDB_ARCH_HAS_WRCONS
static volatile int gdb_notif_pkt_pending = 0;
static volatile int gdb_notif_data_idx;
static unsigned char gdb_notif_data[GDB_NOTIF_DATA_SIZE];
#endif

#ifdef CONFIG_GDB_SERVER_INTERRUPT_DRIVEN
static struct nano_fifo avail_queue;
static struct nano_fifo cmds_queue;
#endif

static struct device *uart_console_dev;

/* global definitions */

volatile int gdb_debug_status = NOT_DEBUGGING;

#ifdef GDB_ARCH_HAS_RUNCONTROL
#ifdef GDB_ARCH_HAS_HW_BP
volatile int gdb_cpu_stop_bp_type = GDB_SOFT_BP;
long gdb_cpu_stop_hw_bp_addr = 0;
#endif
#endif

/* forward static declarations */
static int putPacket(unsigned char * buffer);
static void putDebugChar(unsigned char ch);
static int getDebugChar (void);

static void gdb_post_event (void);
static void gdb_ctrl_loop (void);
static void gdb_system_stop (NANO_ISF * reg, int sig);

#ifdef GDB_ARCH_HAS_RUNCONTROL
static void gdb_system_resume (void);
static int gdb_instruction_set(void *addr, GDB_INSTR *instruction, size_t size);
int gdb_bp_add (GDB_BP_TYPE type, long addr, int length, GDB_ERROR_CODE * pErrCode);
int gdb_bp_delete (GDB_BP_TYPE type, long addr, int length, GDB_ERROR_CODE * pErrCode);
void gdb_bps_remove (void);
#endif

#ifdef GDB_ARCH_HAS_WRCONS
static void gdb_notif_handle (void);
static void gdb_notif_flush_request (void);
static uint32_t gdb_console_write (char *buf, uint32_t len);
#endif

#ifdef CONFIG_GDB_SERVER_INTERRUPT_DRIVEN
static int gdb_irq_input_hook (struct device * dev, uint8_t ch);
#endif

static int hex(unsigned char ch)
{
        if ((ch >= 'a') && (ch <= 'f')) {
                return ch - 'a' + 10;
        }
        if ((ch >= '0') && (ch <= '9')) {
                return ch - '0';
        }
        if ((ch >= 'A') && (ch <= 'F')) {
                return ch - 'A' + 10;
        }
        return -1;
}

static int hex2llong (unsigned char ** ptr, long long * value)
{
        int numChars = 0;
        int hexValue;

        *value = 0;
        while (**ptr) {
                hexValue = hex(**ptr);
                if (hexValue < 0) {
                        break;
                }
                *value = (*value << 4) | hexValue;
                numChars++;
                (*ptr)++;
        }
        return numChars;
}

static int hex2int (unsigned char ** ptr, int * value)
    {
        int numChars = 0;
        int hexValue;
        int neg = 0;

        *value = 0;

        if (**ptr == '-') 
            {
            neg = 1;
            (*ptr)++;
            numChars ++;
            }

        while (**ptr) {
                hexValue = hex(**ptr);
                if (hexValue < 0) {
                        break;
                }
                *value = (*value << 4) | hexValue;
                numChars++;
                (*ptr)++;
        }

        if (neg)
            {
            if (numChars == 1)
                    {
                    (*ptr)--;
                numChars = 0;
                }
            else
                *value = -(*value);
            }
        return numChars;
    }

/*
 * Convert two ASCII hex characters to byte value
 */

static int get_hex_byte(unsigned char **ptrp)
{
        unsigned char *ptr = *ptrp;
        int rval;

        rval = hex(*ptr++);
        rval = rval*16 + hex(*ptr++);
        if(rval >= 0) {
                *ptrp = ptr;
        }
        return rval;
}

/*
 * Write two hex characters from int
 */

static unsigned char *put_hex_byte(unsigned char *ptr, int value)
{
    *ptr++ = hexchars[value >> 4];
    *ptr++ = hexchars[value & 0xf];
    return ptr;
}

/*
 * Convert the memory pointed to by mem into ascii (hex), placing
 * result in buf. Return a pointer to the last char put in buf (null)
 */

static unsigned char * compress(unsigned char * buf)
    {
    unsigned char * read_ptr = buf;
    unsigned char * write_ptr = buf;
    unsigned char        ch;
    size_t  count = strlen ((char *)buf);
    int max_repeat = 126 - 29;
    size_t ix;

    for (ix = 0; ix < count; ix++)
        {
        int        num = 0;
        int        jx;

        ch = *read_ptr++;
        *write_ptr++ = ch;
        for (jx = 1; ((jx + ix) < count) && (jx < max_repeat); jx++)
                {
            if (read_ptr[jx - 1] == ch)
                num ++;
            else
                break;
            }
        if (num >= 3)
            {
            /*
             * It is not possible to use the '$', '#' and '%' characters to 
             * encode the size per gdb remote protocol specification. 
             * skip them.
             */

            while (((num + 29) == '$') || ((num + 29) == '#') ||
                   ((num + 29) == '%'))
                num--;

            *write_ptr++ = '*';
            *write_ptr++ = (unsigned char)(num + 29);
            read_ptr += num;
            ix += num;
            }
        }
    *write_ptr = 0;
    return (write_ptr);
    }

/*******************************************************************************
*
* mem2hex - encode memory data using hexadecimal value of chars from '0' to 'f'
*
* For example, 0x3 (CTRL+C) will be encoded with hexadecimal values of
* '0' (0x30) and '3' (0x33): "3033".
* Using mem2hex() to encode a buffer avoid to send control chars that could
* perturbate communication protocol.
*
* RETURNS: Pointer to the last char put in buf (null).
*
* ERRNO: N/A
*
*/

static unsigned char * mem2hex
    (
    unsigned char *     mem,              /* data to encode */
    unsigned char *     buf,              /* output buffer */
    int                 count,            /* size of data to encode */
    int                 doCompress        /* Compress output data ? */
    )
    {
    int i;
    unsigned char ch;
    unsigned char * saved_buf = buf;

    for (i = 0; i < count; i++)
        {
        ch = *mem++;
        buf = put_hex_byte(buf, ch);
        }
    *buf = 0;

    if (doCompress)
        return compress (saved_buf);

    return (buf);
    }

/*******************************************************************************
*
* gdb_strncat - concatenate characters from one string to another
*
* This routine appends up to <n> characters from string <src> to the
* end of string <dst>.
*
* RETURNS: N/A
*/

static void gdb_strncat
    (
    char *       dst,   /* string to append to */
    const char * src,   /* string to append */
    size_t       n      /* max no. of characters to append */
    )
    {
    if (n != 0)
        {
        while (*dst++ != '\0')                  /* find end of string */
            ;
        dst--;                                  /* rewind back of \0 */
        while (((*dst++ = *src++) != '\0') && (--n > 0))
            ;
        }
    *dst = '\0';                        /* NULL terminate string */
    return;
    }

/*******************************************************************************
*
* gdb_mem_probe -
*
* This routine probe
*
* RETURNS: 0 if memory is accessible, -1 otherwise.
*
* ERRNO: N/A
*
*/

static int gdb_mem_probe
    (
    char *      addr,           /* address to test */
    int         mode,           /* VX_READ or VX_WRITE */
    int         size,           /* number of bytes to test */
    int         width,
    int         preserve        /* preserve memory on write test? */
    )
    {
    long        dummy;          /* dummy variable */

    if (size == 0)              /* memory length is null, test is done */
        return (0);

    /* Validate parameters */

    if (mode == SYS_MEM_SAFE_READ)
        preserve = 0;

    if (width == 0)
        width = 1;

    /* Check width parameter */

    if ((width != 1) && (width != 2) && (width != 4))
        return (-1);

    /* Check addr, size & width parameters coherency */

    if (((unsigned long) addr % width) || (size % width))
        return (-1);

    /* Check first address */

    if ((preserve && (_mem_probe (addr, SYS_MEM_SAFE_READ, width,
                          (char *)&dummy) != 0)) ||
        (_mem_probe (addr, mode, width, (char *)&dummy) != 0))
        return (-1);

    /* Check if we have tested the whole memory */

    if (width == size)
        return (0);

    /* Check last address */

    addr = addr + size - width;
    if ((preserve &&
        (_mem_probe (addr, SYS_MEM_SAFE_READ, width, (char *)&dummy) != 0)) ||
        (_mem_probe (addr, mode, width, (char *)&dummy) != 0)) {
        return (-1);
    }

    return (0);
}

static int putPacket(unsigned char * buffer)
    {
    unsigned char checksum = 0;
    int        count = 0;
    unsigned char ch;

    /*  $<packet info>#<checksum>. */
    do
        {
        putDebugChar ('$');
        checksum = 0;
        count = 0;

        /* Buffer terminated with null character */

        while ((ch = buffer[count]))
            {
            putDebugChar(ch);
            checksum = (unsigned char) (checksum + ch);
            count += 1;
            }

        putDebugChar ('#');
        putDebugChar (hexchars[(checksum >> 4)]); 
        putDebugChar (hexchars[(checksum & 0xf)]);

        if (gdb_no_ack == 0)
            {
            /* Wait for ack */

            ch = (unsigned char)getDebugChar ();
            if (ch == '+')
                return 0;
            if (ch == '$')
                {
                putDebugChar ('-');
                return 0;
                }
            if (ch == GDB_STOP_CHAR)
                {
                gdb_cpu_stop_signal = GDB_SIG_INT;
                gdb_debug_status = DEBUGGING;
                gdb_post_event ();
                return 0;
                }
            }
        else
            return 0;
        } while (1);
    }

#ifdef GDB_ARCH_HAS_WRCONS
/*******************************************************************************
*
* gdb_notif_flush_request - request a GDB Notification Packet flush
*
* This routine requests a flush of pending notification packets. This is done
* by setting gdb_notif_pkt_pending to 1 before stopping the CPU. Once stopped,
* the control loop will send pending packets before resuming the system.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

static void gdb_notif_flush_request (void) {
    /*
     * Before stopping CPU we must indicate that we're stopping the
     * system to handle a packet notification. During the packet notification,
     * we should prevent CPU from reading protocol...
     */

    
    if (gdb_debug_status != NOT_DEBUGGING) {
        return;
    }
    gdb_notif_pkt_pending = 1;
    gdb_debug_status = DEBUGGING;
    gdb_ctrl_loop ();
    gdb_debug_status = NOT_DEBUGGING;
    GDB_SYSTEM_RESUME;
}

/*******************************************************************************
*
* gdb_console_write - write data to debug agent console
*
* This routine writes data to debug agent console. For performance reason, the
* data is bufferized until we receive a carriage return character or until
* the buffer gets full.
*
* The buffer is also automatically flushed when system is stopped.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

static uint32_t gdb_console_write (char *buf, uint32_t len) {
    uint32_t ix;
    unsigned char ch;
    int lock_key = irq_lock();

    /* Copy data to notification buffer for current CPU */
    for (ix = 0; ix < len; ix++) {
        ch = buf[ix];
        gdb_notif_data[gdb_notif_data_idx++] = ch;

        if ((gdb_notif_data_idx == GDB_NOTIF_DATA_SIZE) ||
            (ch == '\n') || (ch == '\r')) {
            /*
             * If the notification buffer for current CPU is full, or if we
             * found a new line or carriage return character, then we must
             * flush received data to remote client.
             */

            gdb_notif_data[gdb_notif_data_idx] = '\0';
            gdb_notif_flush_request ();
        }
    }
    irq_unlock (lock_key);
    return (len);
}

/*******************************************************************************
*
* gdb_notif_handle - handle notification packet
*
* This routine handles pending notification packets for the CPU. It is invoked
* while running in GDB CPU control loop (When system is stopped).
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

static void gdb_notif_handle (void) {
    const char * name = "WrCons";
    unsigned char checksum = 0;
    int ix = 0;
    unsigned char ch;
    int more_data = 0;
    uint32_t max_packet_size;
    uint32_t data_size;
    unsigned char * ptr = gdb_notif_data;

    /* First, check if there is pending data */

    if (gdb_notif_data[0] == '\0') return;
again:

    /* * Build notification packet.
     * A notification packet has the form `%<data>#<checksum>', where data
     * is the content of the notification, and checksum is a checksum of
     * data, computed and formatted as for ordinary gdb packets. A
     * notification's data never contains `$', `%' or `#' characters. Upon
     * receiving a notification, the recipient sends no `+' or `-' to
     * acknowledge the notification's receipt or to report its corruption.
     * 
     * Every notification's data begins with a name, which contains no colon
     * characters, followed by a colon character.
     */
    putDebugChar ('%');
    checksum = 0;

    /* Add name to notification packet */ 
    ix = 0;
    while ((ch = name[ix++])) {
        putDebugChar (ch);
        checksum += ch;
    }

    /* Name must be followed by a colon character. */
    putDebugChar (':');
    checksum += ':';

    /*
     * Add data to notification packet.
     * Warning: The value to hex encoding double the size of the data,
     * so we must not encode more than the remaining GDB buffer size
     * divided by 2.
     */
    max_packet_size = GDB_BUF_SIZE - (strlen (name) + NOTIF_PACKET_OVERHEAD);

    data_size = strlen ((char *)ptr);
    if (data_size <= (max_packet_size / 2)) {
        more_data = 0;
    } else {
        data_size = max_packet_size / 2;
        more_data = 1;        /* Not enough room in notif packet */
    }

    /* Encode data using hex values */
    for (ix = 0; ix < data_size; ix++) {
        ch = hexchars[(*ptr >> 4)];
        putDebugChar (ch);
        checksum += ch;
        ch = hexchars[(*ptr & 0xf)];
        putDebugChar (ch);
        checksum += ch;
        ptr++;
    }

    /* Terminate packet with #<checksum> */
    putDebugChar ('#');
    putDebugChar (hexchars[(checksum >> 4)]); 
    putDebugChar (hexchars[(checksum & 0xf)]);

    if (more_data) goto again;

    /* Clear buffer & index */
    gdb_notif_data[0] = '\0';
    gdb_notif_data_idx = 0;
}
#endif

void gdb_post_event (void) {
    int async_stop = 0;

    gdb_event_is_pending = 0;
    if (gdb_cpu_stop_signal != GDB_SIG_NULL) {
        async_stop = 1;
    }
    if (async_stop) {
        unsigned char * ptr = gdb_buffer;
        size_t bufSize = GDB_BUF_SIZE;
        if (gdb_cpu_stop_signal != GDB_SIG_NULL) {
#ifndef GDB_ARCH_HAS_ALL_REGS
            int offset = 0;
            int size = 4;
#endif
            int count;
            unsigned char * savedPtr;
            if (ptr != gdb_buffer) {
                *ptr++ = '|';
                bufSize--;
            }
            count = snprintf ((char *)ptr, bufSize,
                        "T%02xthread:%02x", gdb_cpu_stop_signal, 1);
            ptr += count;
            bufSize -= count;
#ifdef GDB_ARCH_HAS_HW_BP
            /*
             * If it's an hardware breakpoint, report the address and access
             * type at the origin of the HW breakpoint. Supported syntaxes:
             *        watch:<dataAddr> : Write access
             *        rwatch:<dataAddr> : Read access
             *        awatch:<dataAddr> : Read/Write Access
             * Instruction hardware breakpoints are reported as software
             * breakpoints
             */

            if ((gdb_cpu_stop_signal == GDB_SIG_TRAP) &&
                (gdb_cpu_stop_bp_type != GDB_SOFT_BP) &&
                (gdb_cpu_stop_bp_type != GDB_HW_INST_BP)) {
                count = 0;
                switch (gdb_cpu_stop_bp_type) {
                    case GDB_HW_DATA_WRITE_BP:
                        count = snprintf ((char *)ptr, bufSize, ";watch");
                        break;

                    case GDB_HW_DATA_READ_BP:
                        count = snprintf ((char *)ptr, bufSize, ";rwatch");
                        break;

                    case GDB_HW_DATA_ACCESS_BP:
                        count = snprintf ((char *)ptr, bufSize, ";awatch");
                        break;
                }
                if (count != 0) {
                    ptr += count;
                    bufSize -=count;
                    count = snprintf ((char *)ptr, bufSize, ":%lx",
                                        gdb_cpu_stop_hw_bp_addr);
                    ptr += count;
                    bufSize -= count;
                }
            gdb_cpu_stop_hw_bp_addr = 0;
            gdb_cpu_stop_bp_type = GDB_SOFT_BP;
            gdb_cpu_stop_signal = GDB_SIG_NULL;
            }
#endif
            if (!gdb_regs_not_valid) {
                gdb_arch_regs_get (&gdb_regs, (char *)tmp_reg_buffer);
#ifdef GDB_ARCH_HAS_ALL_REGS
                count = snprintf ((char *)ptr, bufSize, ";regs:");
                ptr += count;
                bufSize -= count;
                savedPtr = ptr;
                ptr = mem2hex (tmp_reg_buffer, ptr, sizeof(gdb_regs), 1);
                bufSize -= (ptr - savedPtr);
#else
                count = snprintf ((char *)ptr, bufSize, ";%x:", GDB_PC_REG);
                ptr += count;
                bufSize -= count;
                gdb_arch_reg_info_get (GDB_PC_REG, &size, &offset);
                savedPtr = ptr;
                ptr = mem2hex (tmp_reg_buffer + offset, ptr, size, 1);
                bufSize -= (ptr - savedPtr);
#endif
            }

            /* clear stop reason */
            gdb_cpu_stop_signal = GDB_SIG_NULL;
        }
        *ptr = '\0';
    }
    else {
        (void)snprintf ((char *)gdb_buffer, GDB_BUF_SIZE, "S%02x", GDB_SIG_INT);
    }

    (void)putPacket (gdb_buffer);
}

/*******************************************************************************
*
* getDebugChar - get a character from serial line
*
* This routine gets a character from the serial line. It loops until it has
* received a character or until it has detected that a GDB event is pending
* and should be handled.
*
* Note that this routine should only be called from the gdb control loop
* when the system is stopped.
*
* RETURNS: -1 if no character has been received and there is a GDB event
* pending or debug operation pending, received character otherwise.
*/
static int getDebugChar (void)
    {
    char ch;

    while (uart_poll_in (uart_console_dev, &ch) != 0)
        {
        if (gdb_event_is_pending)
            return -1;
        }
    return (ch);
    }

/*******************************************************************************
*
* gdb_protocol_get_packet - gets a GDB serial packet
*
* This routine polls the serial line to get a full GDB serial packet. Once
* the packet is received, it computes its checksum and return acknowledgment.
* It then returns the packet to the caller.
*
* This routine must only be called when all CPUs are stopped (from the GDB
* CPU control loop).
*
* If a pending GDB event is detected or if a stop event is received from the
* client, the corresponding GDB stop event is sent to the client. This 
* loop does also handle the GDB cpu loop hooks by the intermediate of
* getDebugChar() API.
*
* If a debug operation is pending, this routine returns immediately.
*
* RETURNS: received packet or NULL on pending debug operation
*/

unsigned char * gdb_protocol_get_packet(unsigned char * buffer, size_t size)
    {
    unsigned char ch;
    unsigned char * pBuf;

    unsigned char checksum;

    while (1)
        {
        while ((ch = (unsigned char)getDebugChar()) != '$')
            {
            if (!gdb_event_is_pending)
                return NULL;

            /* ignore other chars than GDB break character */
            if ((ch == GDB_STOP_CHAR) || gdb_event_is_pending)
                gdb_post_event ();
            }

        checksum = 0;
        pBuf = buffer;

        /*****************************************************
        * Continue reading characters until a '#' is found
        * or until the end of the buffer is reached.
        *****************************************************/

        while (pBuf < &buffer[size])
            {
            ch = (unsigned char)getDebugChar();

            if (ch == '#')
                {
                break;
                }
            else if (ch == '$')
                {
                /* start over */
                checksum = 0;
                pBuf = buffer;
                continue;
                }
            else
                {
                checksum = (unsigned char) (checksum + ch);
                *pBuf++ = ch;
                }
            }
        *pBuf = 0;

        if (ch == '#')
            {
            if (gdb_no_ack)
                {
                (void) getDebugChar();
                (void) getDebugChar();
                return buffer;
                }
            else
                {
                if (checksum != (unsigned char)
                        ((hex((unsigned char)getDebugChar()) << 4) |
                         (hex((unsigned char)getDebugChar()))))
                    {
                    /* checksum failed */
                    putDebugChar ('-');
                    }
                else
                    {
                    /* checksum passed */
                    putDebugChar ('+');

                    if (buffer[2] == ':')
                        {
                        putDebugChar (buffer[0]);
                        putDebugChar (buffer[1]);
                        return &buffer[3];
                        }
                    return buffer;
                    }
                }
            }
        }

    return NULL;
    }


/*******************************************************************************
*
* write_xml_string - write a XML string into output buffer
*
* This routine is used to write a XML string to output buffer. It takes care
* of offset, length and also deal with overflow (if the XML string is bigger
* than the output buffer).
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

static void write_xml_string (char * buffer, const char * xml_string,
                                int offset, int length) 
    {
    size_t max_length = strlen (xml_string);
    if (offset == max_length) {
        gdb_strncat ((char *)buffer, "l", length - 1);
    }
    else if (offset > max_length)
        OUTBUF_FILL ("E00");
    else
        {
        if ((offset + max_length) <= length)
            {
            /* we can read the full data */
            buffer[0] = 'l';
            strncpy (&buffer[1], xml_string + offset,
                     (length <= GDB_BUF_SIZE-2) ? length : GDB_BUF_SIZE-2);
            }
        else
            {
            buffer[0] = 'm';
            strncpy (&buffer[1], xml_string + offset, GDB_BUF_SIZE-2);
            buffer[length+1] = '\0';
            }
        }
    }

/*******************************************************************************
*
* get_xml_target_description - get XML target description
*
* This routine is used to build the string that will hold the XML target
* description provided to the GDB client.
*
* RETURNS: a pointer on XML target description
*
* ERRNO: N/A
*/

static char * get_xml_target_description (void)
    {
    static char target_description[GDB_BUF_SIZE] = {0};
    char * ptr = target_description;
    size_t bufSize = sizeof (target_description);
    size_t size;
    if (target_description[0] != 0) return target_description;
    strncpy (ptr, xml_target_header, GDB_BUF_SIZE-1);
    size = strlen (ptr);
    ptr += size;
    bufSize -= size;

    /* Add architecture definition */

    (void)snprintf (ptr, bufSize, "  <architecture>%s</architecture>\n",
                    GDB_TGT_ARCH);
    size = strlen (ptr);
    ptr += size;
    bufSize -= size;

    strncpy (ptr, xml_target_footer,
             GDB_BUF_SIZE - (ptr - target_description) - 1);
    return target_description;
    }

/*******************************************************************************
*
* gdb_protocol_parse - parse given GDB command string
*
* This routine parses and executes the given GDB command string, and send
* acknowledgment if acknowledgment is enabled.
*
* RETURNS: 0, or -1 if failed to send acknowledgment.
*
* ERRNO: N/A
*
*/

int gdb_protocol_parse (unsigned char * ptr) {
    unsigned char ch;
#ifdef RESUME_AT_SUPPORT
    long long save_pc;
#endif
    int no_ack = 0;
   
    ch = *ptr++;

    switch (ch)
        {
            case '?':
                {
                /*
                 * This is a new connection. Clear gdb_no_ack field if it was
                 * set and send acknowledgment for this command that has
                 * not been sent as it should have.
                 */
                if (gdb_no_ack)
                    {
                    putDebugChar ('+');
                    gdb_no_ack = 0;
                    }

                (void)snprintf ((char *)gdb_buffer, GDB_BUF_SIZE,
                                "T02thread:%02x;", 1);

                /*
                 * This is an initial connection, should remove all 
                 * the breakpoints and cleanup.
                 */
                GDB_BPS_REMOVE;
                gdb_is_connected = 1;
                break;
                }
            case 'k':   /* kill but do not kill here */
#ifdef  CONFIG_REBOOT
                sys_reboot(SYS_REBOOT_COLD);
                OUTBUF_FILL (STUB_OK);
                break;
#endif
            case 'D':   /* detach  */
                {
                OUTBUF_FILL (STUB_OK);
                GDB_BPS_REMOVE;
                gdb_is_connected = 0;
                gdb_debug_status = NOT_DEBUGGING;
                GDB_SYSTEM_RESUME;
                no_ack = 1;
                gdb_no_ack = 0;
                break;
                }
            case 'T':
                {
                int        thread;

                if (!hex2int (&ptr, &thread))
                    {
                    gdb_buffer[0] = '\0';
                    break;
                    }
                if (thread != 1)
                    OUTBUF_FILL (STUB_ERROR);
                else
                    OUTBUF_FILL (STUB_OK);
                break;
                }
            case 'Q':                                /* Set Packet */
                {
                if (strcmp((const char *)ptr, "StartNoAckMode") == 0)
                    {
                    gdb_no_ack = 1;
                    OUTBUF_FILL (STUB_OK);
                    break;
                    }
#ifdef GDB_ARCH_HAS_VMCS
                else if (strncmp((const char *)ptr, "wr.vmcs:", 8) == 0)
                    {
                    long long addr;
                    int regNum = 0;
                    int thread_id;
                    ptr += 8;
                    if (hex2int((unsigned char ** )&ptr, &thread_id) &&
                        *ptr++ == ',' &&
                        hex2llong ((unsigned char ** )&ptr, &addr) &&
                        *ptr++ == ',' &&
                        hex2int ((unsigned char ** )&ptr, &regNum) &&
                        *ptr++ == '=')
                        {
                        long long value;
                        if (hex2llong ((unsigned char ** )&ptr, &value))
                            {
                            if (gdb_arch_write_vmcs (thread_id - 1,
                                                        (void *) ((long)addr),
                                                        regNum, value) > 0)
                                {
                                OUTBUF_FILL (STUB_OK);
                                }
                            }
                        else
                            OUTBUF_FILL (STUB_ERROR);
                        }
                    else 
                        OUTBUF_FILL (STUB_ERROR);
                    break;
                    }
#endif
                gdb_buffer[0] = '\0';
                break;
                }
            case 'q':                                /* Query Packet */
                {
                if (ptr[0] == 'C')
                    {
                    (void)snprintf ((char *)gdb_buffer, GDB_BUF_SIZE, "QC%x", 1);
                    }
                else if (strncmp((const char *)ptr, "wr.", 3) == 0) 
                    {
                    ptr += 3;
#ifdef GDB_ARCH_HAS_VMCS
                    if (strncmp ((const char *)ptr, "vmcs:", 5) == 0) 
                        {
                        long long addr;
                        int regNum = 0;
                        int thread_id;
                        ptr += 5;
                        if (hex2int((unsigned char ** )&ptr, &thread_id) &&
                            *ptr++ == ',' &&
                            hex2llong ((unsigned char ** )&ptr, &addr) &&
                            *ptr++ == ',' &&
                            hex2int ((unsigned char ** )&ptr, &regNum) &&
                            *ptr == '\0') 
                            {
                            int size;
                            size = gdb_arch_read_vmcs (thread_id - 1,
                                                        (void *) ((long)addr),
                                                        regNum, tmp_reg_buffer);
                            if (size > 0)
                                mem2hex (tmp_reg_buffer, gdb_buffer, size, 1);
                            else
                                OUTBUF_FILL (STUB_ERROR);
                            }
                        else
                            OUTBUF_FILL (STUB_ERROR);
                        break;
                        }
#endif
#ifdef GDB_ARCH_HAS_EXTENDED_REGISTERS
                    if (strncmp ((const char *)ptr, "eregs:", 6) == 0) 
                        {
                        int size = -1;
                        int regNum = 0;

                        ptr += 6;
                        if (hex2int(&ptr, &regNum)) 
                            {
                            size = gdb_arch_read_extended_registers
                                                    (global_cpu, regNum,
                                                    (char *)tmp_reg_buffer);
                            }
                        if (size > 0)
                            mem2hex (tmp_reg_buffer, gdb_buffer, size, 1);
                        else
                            OUTBUF_FILL (STUB_ERROR);
                        break;
                        }
#endif
#ifdef GDB_ARCH_HAS_CPUID
                    if (strncmp ((const char *)ptr, "cpuid:", 6) == 0) 
                        {
                        uint32_t arg1 = 0;
                        uint32_t arg2 = 0;
                        uint32_t res1 = 0;
                        uint32_t res2 = 0;
                        uint32_t res3 = 0;
                        uint32_t res4 = 0;

                        ptr += 6;
                        if (hex2int((unsigned char ** )&ptr, (int *)&arg1) &&
                            *ptr++ == ',' &&
                            hex2int ((unsigned char ** )&ptr, (int *)&arg2) &&
                            *ptr == '\0') 
                            {
                            if (gdb_arch_cpuid_get (arg1, arg2,
                                        &res1, &res2, &res3, &res4) == 0)
                                (void)snprintf ((char *)gdb_buffer, sizeof(gdb_buffer),
                                                "%08x%08x%08x%08x",
                                                res1, res2, res3, res4);
                            else
                                OUTBUF_FILL (STUB_ERROR);
                            }
                        else
                            OUTBUF_FILL (STUB_ERROR);
                        break;
                        }
#endif /* GDB_ARCH_HAS_CPUID */
                    gdb_buffer[0] = '\0';
                    }
                else if (strcmp ((const char *)ptr, "Supported") == 0)
                    {
                    size_t size = GDB_BUF_SIZE;

#ifdef GDB_ARCH_HAS_WRCONS
                    (void)snprintf ((char *)gdb_buffer, size,
                                  "PacketSize=%x;qXfer:features:read+;QStartNoAckMode+;WrCons+", GDB_BUF_SIZE);
#else
                    (void)snprintf ((char *)gdb_buffer, size,
                                  "PacketSize=%x;qXfer:features:read+;QStartNoAckMode+", GDB_BUF_SIZE);
#endif
                    size -= (strlen ((char *)gdb_buffer) + 1);
#ifdef GDB_ARCH_HAS_VMCS
                    gdb_strncat ((char *)gdb_buffer, STR_QWR_VMCS, size);
                    size -= sizeof (STR_QWR_VMCS);
#endif
#ifdef GDB_ARCH_HAS_EXTENDED_REGISTERS
                    gdb_strncat ((char *)gdb_buffer, STR_QWR_EREGS, size);
                    size -= sizeof (STR_QWR_EREGS);
#endif
#ifdef GDB_ARCH_HAS_CPUID
                    gdb_strncat ((char *)gdb_buffer, STR_QWR_CPUID, size);
                    size -= sizeof (STR_QWR_CPUID);
#endif
#ifdef STR_REBOOT
                    gdb_strncat ((char *)gdb_buffer, STR_REBOOT, size);
                    size -= sizeof (STR_REBOOT);
#endif
                    gdb_strncat ((char *)gdb_buffer, STR_TYPE, size);
                    size -= sizeof (STR_TYPE);
                    }
                else if (strncmp ((const char *)ptr, "Xfer:features:read:", 19) == 0)
                    {
                    ptr += 19;
                    if (strncmp ((const char *)ptr, "target.xml:", 11) == 0)
                        {
                        int offset;
                        int length;

                        ptr += 11;
                        if (hex2int ((unsigned char ** )&ptr, &offset) &&
                            *ptr++ == ',' &&
                            hex2int (&ptr, &length) &&
                            *ptr == '\0') 
                            {
                            write_xml_string ((char *) gdb_buffer,
                                                get_xml_target_description(),
                                                offset, length); 
                            }
                        else
                            OUTBUF_FILL (STUB_ERROR);
                        }
                    else
                        gdb_buffer[0] = '\0';
                    }
                else 
                    gdb_buffer[0] = '\0';
                break;
                }


            case 'g':                /* g -- Get Registers */
                {
                if (gdb_regs_not_valid)
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }
                (void)gdb_arch_regs_get (&gdb_regs,(char *)tmp_reg_buffer);
                mem2hex (tmp_reg_buffer, gdb_buffer, GDBNUMREGBYTES, 1);
                break;
                }
            case 'G':                /* GXX... -- Write Registers */
                {
                int i;
                int value;

                if (gdb_regs_not_valid)
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }
                (void)gdb_arch_regs_get (&gdb_regs,
                                      (char *)tmp_reg_buffer);
                for(i = 0; i < GDBNUMREGBYTES; i++) {
                    if((value = get_hex_byte(&ptr)) < 0)
                        break;
                    tmp_reg_buffer[i] = (unsigned char) value;
                }
                gdb_arch_regs_set (&gdb_regs, (char *)tmp_reg_buffer);
#ifdef GDB_ARM_BOOTLOADER
                boot_regs_set = 1;
#endif
                OUTBUF_FILL (STUB_OK);
                break;
                }

#ifdef GDB_HAS_SINGLE_REG_ACCESS
            case 'P':                /* Pn..=r.. -- Write register */
                {
                int regNum = 0;
                int offset = 0;
                int size = 4;
                int i, value;

                if (gdb_regs_not_valid)
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }
                if (!hex2int (&ptr, &regNum) || *(ptr++) != '=')
                    { 
                    OUTBUF_FILL ("E02");
                    break;
                    }

                gdb_arch_regs_get (&gdb_regs,(char *)tmp_reg_buffer);
                gdb_arch_reg_info_get (regNum, &size, &offset);

                for(i = 0; i < size; i++)
                    {
                    if((value = get_hex_byte(&ptr)) < 0)
                        break;
                    tmp_reg_buffer[offset+i] = (unsigned char)value;
                    }
                if (i != size) 
                    {
                    OUTBUF_FILL (STUB_ERROR);
                    break;
                    }
                gdb_arch_regs_set (&gdb_regs,(char *)tmp_reg_buffer);
                OUTBUF_FILL (STUB_OK);
                break;
                }
            case 'p':
                {
                int regNum = 0;
                int offset = 0;
                int size = 4;

                if (gdb_regs_not_valid)
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }
                /* p<regno> */
                
                if (!hex2int(&ptr, &regNum)) 
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }

                gdb_arch_regs_get (&gdb_regs, (char *)tmp_reg_buffer);
                gdb_arch_reg_info_get (regNum, &size, &offset);
                mem2hex (tmp_reg_buffer + offset, gdb_buffer, size, 1);
                break;
                }
#endif /* GDB_HAS_SINGLE_REG_ACCESS */

            case 'm':                                /* Read Memory */
                {                               /* m<addr>,<length> */
                long long addr;
                int length;

                if (hex2llong ((unsigned char ** )&ptr, &addr) == 0)
                    {
                    OUTBUF_FILL ("E01");
                    break;                
                    }

                if (!(*ptr++ == ',' && hex2int (&ptr, &length)))
                    {
                    OUTBUF_FILL ("E01");
                    break;
                    }

                if (gdb_mem_probe ((void *)((long)addr), SYS_MEM_SAFE_READ, length, 0, 1) == -1)
                    {
                    OUTBUF_FILL ("E01");
                    break;  /* No read access */
                    }

                /* Now read memory */
                mem2hex((unsigned char *)((long)addr), gdb_buffer, length, 1);
                break;
                }
            case 'M':                                /* Write Memory */
                {
                long long addr;
                int length;
                int value, i;

                /* M<addr>,<length>:<val><val>...<val> */

                if (hex2llong ((unsigned char ** )&ptr, &addr) == 0)
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }

                if (! (*ptr++ == ',' &&
                       hex2int (&ptr, &length) &&
                       *ptr++ == ':'))
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }

                if (gdb_mem_probe ((void *)((long)addr), SYS_MEM_SAFE_WRITE, length, 0, 1) == -1)
                    {
                    OUTBUF_FILL ("E02");
                    break;  /* No write access */
                    }
                
                /* Now write memory */
                for(i = 0; i < length; i++)
                    {
                    if((value = get_hex_byte(&ptr)) < 0)
                        break;
                    ((unsigned char *)((long)addr))[i] = (unsigned char) value;
                    }

                OUTBUF_FILL (STUB_OK);
                break;
                }
            case 'X':                                /* Write Memory in binary format  */
                {
                long long addr;
                int length;
                int i;
                unsigned char value;
                unsigned char * buffer;

                /* M<addr>,<length>:<val><val>...<val> */

                if (hex2llong ((unsigned char ** )&ptr, &addr) == 0)
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }

                if (! (*ptr++ == ',' &&
                       hex2int (&ptr, &length) &&
                       *ptr++ == ':'))
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }

                if (gdb_mem_probe ((void *)((long)addr), SYS_MEM_SAFE_WRITE, length, 0, 1) == -1)
                    {
                    OUTBUF_FILL ("E02");
                    break;  /* No write access */
                    }

                /* Now write memory */
                buffer = ptr;
                for(i = 0; i < length; i++)
                    {
                    value = buffer[0];
                    buffer++;
                    if (value == '}')
                        {
                        value = buffer[0] ^ 0x20;
                        buffer++;
                        }
                    ((unsigned char *)((long)addr))[i] = (unsigned char) value;
                    }

                OUTBUF_FILL (STUB_OK);
                break;
                }
            case 'C':                                /* Pass the signal to context */
                                                    /* and continue execution */
                {
                int signal;

                /* read signal number */
                if (!hex2int (&ptr, &signal))
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }

                gdb_cpu_pending_sig = signal;

                if (*ptr == ';')
                    ptr++;

                /* fall through */
                }

            case 'c':                                /* Continue Execution */
                {
                long long addr;
                /*
                 * Try to read optional parameter, pc unchanged if
                 * no param
                 */

                if (hex2llong ((unsigned char **)&ptr, &addr))
                    {
#ifdef RESUME_AT_SUPPORT
                    save_pc = addr;

                    /*Need to flush cache ?*/
#endif
                    }

                gdb_debug_status = NOT_DEBUGGING;
                no_ack = 1;
                break;
                }
            case 'S':                                /* Pass the signal to context */
                                                    /* and step execution */
                {
                int signal;

                /* read signal number */
                if (!hex2int (&ptr, &signal))
                    {
                    OUTBUF_FILL ("E02");
                    break;
                    }

                gdb_cpu_pending_sig = signal;

                if (*ptr == ';')
                    ptr++;

                /* fall through */
                }
            case 's':                                /* step Execution */
                {
                long long addr;
                /*
                 * Try to read optional parameter, pc unchanged if
                 * no param
                 */

                if (hex2llong ((unsigned char **)&ptr, &addr))
                    {
#ifdef RESUME_AT_SUPPORT
                    save_pc = addr;

                    /*Need to flush cache ?*/
#endif
                    }

                gdb_debug_status = SINGLE_STEP;
                no_ack = 1;
                break;
                }
            case 'v':
                {
                char action;
                int signal = 0;

                if (strcmp((const char *)ptr, "Cont?") == 0)
                    {
                    OUTBUF_FILL ("vCont;c;s;C;S");
                    break;
                    }
                else  if (strncmp((const char *)ptr, "Cont;", 5) != 0)
                    {
                    gdb_buffer[0] = '\0';
                    break;
                    }
                
                ptr += 5;
                action = *ptr++;
                if ((action != 'c') && (action != 'C') &&
                    (action != 's') && (action != 'S'))
                    {
                    gdb_buffer[0] = '\0';
                    break;
                    }

                if ((action == 'C') || (action == 'S'))
                    {
                    /* read signal number */
                    if (!hex2int (&ptr, &signal))
                        {
                        OUTBUF_FILL ("E02");
                        break;
                        }
                    }

                if (*ptr == ':')
                    {
                    int thread;
                    ptr++;
                    hex2int (&ptr, &thread);
                    }

                if (signal != 0)
                    gdb_cpu_pending_sig = signal;

                if ((action == 'c') || (action == 'C'))
                    gdb_debug_status = NOT_DEBUGGING;
                else
                    gdb_debug_status = SINGLE_STEP;
                no_ack = 1;
                break;
                }

#ifdef GDB_ARCH_HAS_RUNCONTROL
            case 'z':   /* ztype,addr,length */
                {
                int     type, length;
                long long addr;
                GDB_ERROR_CODE gdbErrCode;

                /* get address which is required parameter */

                if (hex2int (&ptr, &type) &&
                    *ptr++ == ',' &&
                    hex2llong ((unsigned char **)&ptr, &addr) &&
                    *ptr++ == ',' &&
                    hex2int (&ptr, &length))
                    {
                    if (gdb_bp_delete (type, (long) addr, length,
                                     &gdbErrCode) == 0)
                        OUTBUF_FILL ("OK");
                    else
                        {
                        (void) snprintf ((char *)gdb_buffer,
                                        GDB_BUF_SIZE, "E%02d", gdbErrCode);
                        }
                    }
                else
                    OUTBUF_FILL ("E07");
                break;
                }
            case 'Z':   /* Ztype,addr[:<vaddr1>[:...:<vaddrN>]],length */
                {
                int type, length;
                long long addr;
                GDB_ERROR_CODE gdbErrCode;


                /* read <type> & <addr> */
                if (! (hex2int (&ptr, &type) && *ptr++ == ',' &&
                       hex2llong ((unsigned char **)&ptr, &addr)))
                    {
                    OUTBUF_FILL ("E07");
                    break;
                    }

                if (! (*ptr++ == ',' && hex2int (&ptr, &length)))
                    {
                    OUTBUF_FILL ("E07");
                    break;
                    }

                if (gdb_bp_add (type, (long) addr, length, &gdbErrCode) == 0)
                    {
                    OUTBUF_FILL (STUB_OK);
                    }
                else
                    {
                    (void) snprintf ((char *)gdb_buffer, GDB_BUF_SIZE, "E%02d",
                                  gdbErrCode);
                    }
                break;
                }
#endif
            default:
                /*
                 * In case of an unsupported command, send an empty 
                 * response.
                 */

                gdb_buffer[0] = '\0';
                break;
        }

        /* Send the acknowledgment command when necessary */

        if (!no_ack)
            {
            if (putPacket (gdb_buffer) < 0)
                return -1;
            }

    return 0;
}

/*
 * function: putDebugChar
 * description:
 *      - "What you must do for the stub"
 *      - Write a single character from a port.
 */
static void putDebugChar(unsigned char ch) {
    (void) uart_poll_out (uart_console_dev, ch);
    return;
}
  
#ifdef GDB_ARCH_HAS_RUNCONTROL
#ifdef GDB_ARCH_HAS_HW_BP
/*******************************************************************************
*
* gdb_hw_bp_add - add an hardware breakpoint to debug registers set
*
* This routine adds an hardware breakpoint to debug registers structure.
* It does not update the debug registers, this is only done using
* wdbDbgRegsSet().
*
* RETURNS: 0, or -1 if failed (Error code returned via pErrCode).
*
* ERRNO: N/A
*
*/

int gdb_hw_bp_add (long addr, GDB_BP_TYPE type, int length, GDB_ERROR_CODE * err) {
    if (gdb_hw_bp_set (&dbg_regs, addr, type, length, err) == -1) {
        return -1;
    }

    gdb_hw_bp_cnt++;
    return 0;
}

/*******************************************************************************
*
* gdb_hw_bp_remove - remove an hardware breakpoint from debug registers set
*
* This routine removes an hardware breakpoint from debug registers structure.
* It does not update the debug registers, this is only done using
* wdbDbgRegsSet().
*
* RETURNS: OK or ERROR (Error code returned via pErrCode).
*
* ERRNO: N/A
*
*/

int gdb_hw_bp_remove (long addr, GDB_BP_TYPE type, int length, GDB_ERROR_CODE * err) {
    if (gdb_hw_bp_clear (&dbg_regs, addr, type, length, err) == -1) {
        return -1;
    }
    gdb_hw_bp_cnt--;
    return 0;
}
#endif

/*******************************************************************************
*
* gdb_bp_add - add a new breakpoint or watchpoint to breakpoint list
*
* This routine adds a new breakpoint or watchpoint to breakpoint list.
* For watchpoints, this routine checks that the given type/length combination
* is supported on current architecture, and that debug registers are not full.
*
* <type> : GDB breakpoint type:
*        0 : software breakpoint        (GDB_SOFT_BP)
*        1 : hardware breakpoint        (GDB_HW_INST_BP)
*        2 : write watchpoint        (GDB_HW_DATA_WRITE_BP)
*        3 : read watchpoint        (GDB_HW_DATA_READ_BP)
*        4 : access watchpoint        (GDB_HW_DATA_ACCESS_BP)
*
* <addr>     : breakpoint address.
* <length>   : length is in bytes. For a software breakpoint, length specifies
*              the size of the instruction to be patched. For hardware
*               breakpoints and watchpoints length specifies the memory
*               region to be monitored.
* <pErrCode> : ptr to error code if failed to add breakpoint.
*
* RETURNS: 0, or -1 if failed to add breakpoint.
*
* ERRNO: N/A
*
*/

int gdb_bp_add (GDB_BP_TYPE type, long addr, int length, GDB_ERROR_CODE * pErrCode) {
    int ix;

    if (type != GDB_SOFT_BP) {
#ifdef GDB_ARCH_HAS_HW_BP
        return (gdb_hw_bp_add (addr, type, length, pErrCode));
#else
        *pErrCode = GDB_ERROR_HW_BP_NOT_SUP;
        return -1;
#endif
    }

    if (gdb_mem_probe ((void *)addr, SYS_MEM_SAFE_READ, length, 0, 1) == -1)
            return (-1);

    /* Add software breakpoint to BP list */

    for (ix = 0; ix < MAX_SW_BP; ix++)
        {
        if (bpArray[ix].valid == 0)
            {
            bpArray[ix].valid = 1;
            bpArray[ix].enabled = 0;
            bpArray[ix].addr = (GDB_INSTR *) addr;
            return (0);
            }
        }

    *pErrCode = GDB_ERROR_BP_LIST_FULL;
    return -1;
    }

/*******************************************************************************
*
* gdb_bp_delete - delete a breakpoint or watchpoint from breakpoint list
*
* This routine removes a breakpoint or watchpoint from breakpoint list.
*
* RETURNS: 0, or -1 if failed to remove breakpoint.
*/

int gdb_bp_delete (GDB_BP_TYPE type, long addr, int length, GDB_ERROR_CODE * pErrCode) {
    int ix, jx;

    if (type != GDB_SOFT_BP)
        {
#ifdef GDB_ARCH_HAS_HW_BP
        return (gdb_hw_bp_remove (addr, type, length, pErrCode));
#else
        *pErrCode = GDB_ERROR_HW_BP_NOT_SUP;
        return -1;
#endif
        }

    for (ix = 0; ix < MAX_SW_BP; ix++) {
        if (bpArray[ix].valid == 1 &&
            bpArray[ix].addr == (GDB_INSTR *) addr) {
            bpArray[ix].valid = 0;      /* invalidate entry */

            /*
             * Make sure all valid entries are contiguous to speed up
             * breakpoint table parsing.
             */

            for (jx = ix + 1; jx < MAX_SW_BP; jx++) {
                if (bpArray[jx].valid == 1) {
                    bpArray[jx - 1] = bpArray[jx];
                    bpArray[jx].valid = 0;
                } else
                    break;
            }
            return (0);
        } else if (bpArray[ix].valid == 0) {
            /* The first non valid entry in the table is the last entry */

            break;
        }
    }

    *pErrCode = GDB_ERROR_INVALID_BP;
    return (-1);
    }

/*******************************************************************************
*
* gdb_bps_remove - remove all installed breakpoints
*
* This routine removes all installed breakpoints.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

void gdb_bps_remove (void) {
    int ix;

    for (ix = 0; ix < MAX_SW_BP; ix++)
        {
        /* First non valid entry in the table is the last entry */

        if (bpArray[ix].valid == 0)
            break;
        bpArray[ix].valid = 0;
        }
}

/*******************************************************************************
*
* gdb_bps_install - install breakpoints
*
* This routine physically install breakpoints, and make sure that modified
* memory is flushed on all CPUs.
* This routine must only be called on CPU0, and when all CPUs are ready to be
* resumed (ready to exit the CPU control loop).
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

static void gdb_bps_install (void) {
    uint32_t ix;
    GDB_INSTR bpInst = GDB_BREAK_INST;

    /* Software breakpoints installation */

    for (ix = 0; ix < MAX_SW_BP; ix++) {
        if (bpArray[ix].valid == 1 && !bpArray[ix].enabled) {
            GDB_INSTR * addr = bpArray[ix].addr;
            bpArray[ix].insn = *addr;
            (void)gdb_instruction_set (addr, &bpInst, sizeof(GDB_INSTR));
            bpArray[ix].enabled = 1;
        } else if (bpArray[ix].valid == 0) {
            /* First non valid entry in the table is the last entry */
            break;
        }
    }

#ifdef GDB_ARCH_HAS_HW_BP
    if (gdb_hw_bp_cnt > 0) {
        gdb_dbg_regs_set (&dbg_regs);
    }
#endif
}

/*******************************************************************************
*
* gdb_bps_uninstall - uninstall breakpoints
*
* This routine physically uninstall breakpoints, and make sure that modified
* memory is flushed on all CPUs.
* This routine must only be called on CPU0, and when all CPUs have been stopped
* (entered the CPU control loop).
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

static void gdb_bps_uninstall (void) {
    uint32_t ix;

    for (ix = 0; ix < MAX_SW_BP; ix++) {
    	if (bpArray[ix].valid == 1 && bpArray[ix].enabled) {
    		GDB_INSTR * addr = bpArray[ix].addr;
            (void)gdb_instruction_set(addr, &bpArray[ix].insn, sizeof(GDB_INSTR));
            bpArray[ix].enabled = 0;
        } else if (bpArray[ix].valid == 0) {
            /* First non valid entry in the table is the last entry */
            break;
        }
    }
    
#ifdef GDB_ARCH_HAS_HW_BP
    if (gdb_hw_bp_cnt > 0) {
    	gdb_dbg_regs_clear ();
    }
#endif
}

/*******************************************************************************
*
* gdb_system_resume - resume the system
*
* This routine re-installs breakpoint and resumes the system.
*
* RETURNS: always 1.
*
* ERRNO: N/A
*
*/

static void gdb_system_resume (void)
    {
    /* system must not be resumed if we're going to execute a single step */

    if (gdb_debug_status == SINGLE_STEP)
        return;

    /* Re-install breakpoints */
    gdb_bps_install ();
    }

/*******************************************************************************
*
* gdb_handler - stop mode agent BP/trace handler
* 
* This handler is a common handler of breakpoint and trace mode exceptions.
* This handler is invoked with interrupts locked.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

void gdb_handler (int mode, void * exc_regs, int signal) {
    /* Save BP/Trace handler registers */
    gdb_arch_regs_from_esf(&gdb_regs, (NANO_ESF *) exc_regs);
    gdb_regs_not_valid = 0;

    if (mode == GDB_EXC_TRACE)
        {
        /* Check if GDB did request a step */
        if (gdb_debug_status != SINGLE_STEP)
            return;

        /* No longer pending trace mode exception */
        gdb_debug_status = DEBUGGING;

#ifdef GDB_ARCH_NO_SINGLE_STEP
        /* remove temporary breakpoint */
        (void)gdb_instruction_set (gdb_step_emu_next_pc, &gdb_step_emu_insn, sizeof(GDB_INSTR));
        /* Disable trace mode */
        gdb_int_regs_unlock (&gdb_regs, gdb_trace_lock_key);
#else
        /* Disable trace mode */
        gdb_trace_mode_clear (&gdb_regs, gdb_trace_lock_key);
#endif /* GDB_ARCH_NO_SINGLE_STEP */
        }

    if ((mode == GDB_EXC_TRACE) || (mode == GDB_EXC_BP))
        gdb_stop_by_bp = 1;

    gdb_event_is_pending = 1;
    gdb_cpu_stop_signal = signal;

    /* Enter stop mode agent control loop */
    gdb_ctrl_loop ();

    /* Restore BP handler registers */
    gdb_arch_regs_to_esf(&gdb_regs, (NANO_ESF *) exc_regs);

    if ((mode == GDB_EXC_TRACE) || (mode == GDB_EXC_BP))
        gdb_stop_by_bp = 0;

    /* Resume system if not handing a single step */
    GDB_SYSTEM_RESUME;
    }

static int gdb_instruction_set(void *addr, GDB_INSTR *instruction, size_t size) {
    if (_mem_safe_write_to_text_section(addr,
		(char *) instruction, size) < 0) return -EFAULT;
    sys_cache_flush((vaddr_t)addr, size);
    return 0;
}

#endif

/*******************************************************************************
*
* gdb_ctrl_loop - GDB control loop
*
* The CPU control loop is an active wait loop used to stop CPU activity on
* all CPUs. This routine must be called with interrupts locked.
* This routine loops while waiting for debug events which can be:
*
* - System resumed: gdb_debug_status != NOT_DEBUGGING
*       The control loop must be exited.
*
* - Single step request for current CPU: gdb_debug_status == SINGLE_STEP
*       Notify client that CPU is already stopped.
*       This is done by setting gdb_event_is_pending = 1.
*       gdb_event_is_pending will be handled by next gdb_protocol_get_packet() on CPU 0.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

static void gdb_ctrl_loop (void) {
    char ch;
    /* uninstall breakpoints. */
    GDB_BPS_UNINSTALL;

    /* Flush input buffer */
    while (uart_poll_in (uart_console_dev, &ch) == 0) {
        if (ch == GDB_STOP_CHAR) {
            gdb_debug_status = DEBUGGING;
            gdb_cpu_stop_signal = GDB_SIG_INT;
            gdb_event_is_pending = 1;
            break;
        }
    }

    while (gdb_debug_status != NOT_DEBUGGING) {
        unsigned char * ptr;

#ifdef GDB_ARCH_HAS_WRCONS
        /*  Check if system has been stopped to handle a notification packet:
         * If a notification is pending (gdb_notif_pkt_pending), but no stop
         * signal has been set.
         */
        if ((gdb_cpu_stop_signal == GDB_SIG_NULL) && gdb_notif_pkt_pending) {
            gdb_notif_handle ();
            /* Mark packet notification as done */
            gdb_notif_pkt_pending = 0;
            break;
        }
#endif

        ptr = gdb_protocol_get_packet (gdb_buffer, GDB_BUF_SIZE);
        if (ptr != NULL)
            gdb_protocol_parse (ptr);

#ifdef GDB_ARCH_HAS_RUNCONTROL
#ifdef GDB_ARCH_CAN_STEP
        if (gdb_debug_status == SINGLE_STEP) {
           if (!GDB_ARCH_CAN_STEP(&gdb_regs)) {
                gdb_debug_status = DEBUGGING;
                gdb_event_is_pending = 1;
                gdb_cpu_stop_signal = GDB_SIG_TRAP;
            }
        }
#endif /* GDB_ARCH_CAN_STEP */

        if (gdb_debug_status == SINGLE_STEP) {
#ifdef GDB_ARCH_NO_SINGLE_STEP
            GDB_INSTR bp_instr = GDB_BREAK_INST;

            gdb_step_emu_next_pc = gdb_get_next_pc(&gdb_regs);
            gdb_step_emu_insn = *gdb_step_emu_next_pc;
            (void)gdb_instruction_set (gdb_step_emu_next_pc, &bp_instr, sizeof(GDB_INSTR));
            gdb_trace_lock_key = gdb_int_regs_lock (&gdb_regs);
#else 
            /* Handle single step request for for runcontrol CPU */

            gdb_trace_lock_key = gdb_trace_mode_set (&gdb_regs);
#endif /* GDB_ARCH_NO_SINGLE_STEP */
            return;
        }
#endif
    }
}

/*******************************************************************************
*
* gdb_system_stop - handle a system stop request
*
* The purpose of this routine is to handle a stop request issued by remote
* debug client. It is called when receiving a break char.
*
* This routine stops all enabled CPUs. It indicates that a GDB event is pending
* (the answer to stop request) and transfer control from the runtime system to
* the stop mode agent. The event will be posted by this control loop.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

static void gdb_system_stop (NANO_ISF * regs, int sig)
    {
    int oldlevel = irq_lock();

    gdb_debug_status = DEBUGGING;
    if (sig != 0) gdb_cpu_stop_signal = sig;
    else gdb_cpu_stop_signal = GDB_SIG_INT; /* Stopped by a command */

    /* Save registers */
    if (regs == GDB_INVALID_REG_SET) gdb_regs_not_valid = 1;
    else {
        if (regs == NULL) regs = sys_debug_current_isf_get();
        gdb_arch_regs_from_isf(&gdb_regs, regs);
        gdb_regs_not_valid = 0;
    }

    /* A GDB event is pending */
    gdb_event_is_pending = 1;

    /* Transfer control to the control loop */
    gdb_ctrl_loop ();

    /* Load registers */
    if (!gdb_regs_not_valid) {
        gdb_arch_regs_to_isf(&gdb_regs,regs);
    }

    /* Resume system if not a single step request */
    GDB_SYSTEM_RESUME;

    irq_unlock(oldlevel);
    }


/*******************************************************************************
*
* gdb_console_out - wrapper to send a character to console
*
* This routine is a specific wrapper to send a character to console.
* If the GDB Server is started, this routine intercepts the data and transfer
* it to the connected debug clients using a GDB notification packet.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

int gdb_console_out (char val) {
#ifdef GDB_ARCH_HAS_WRCONS
	/*
	 * If remote debug client is connected, then transfer data to remote
	 * client. Otherwise, discard this character.
	 */
	if (gdb_is_connected) {
		gdb_console_write (&val, 1);
		return 1;
	}
#endif
	return 0;
}

#ifdef CONFIG_GDB_SERVER_INTERRUPT_DRIVEN
static int gdb_irq_input_hook (struct device * dev, uint8_t ch) {
    if (ch == GDB_STOP_CHAR) {
        (void)irq_lock();

        gdb_system_stop (NULL, 0);
#ifdef GDB_ARM_BOOTLOADER
        if (boot_regs_set) {
            sys_clock_disable();
            irq_disable(uart_irq_get(uart_console_dev));
            boot_regs_set = 0;
            return 1;
        }
#endif
        return 1;
    }
    return 0;
}
#endif

void gdb_system_stop_here (void * regs) {
    int oldlevel = irq_lock();
    gdb_system_stop ((NANO_ISF *) regs, GDB_SIG_STOP);
    irq_unlock(oldlevel);
}

static int init_gdb_server(struct device *unused) {
    static int gdb_is_initialized = 0;

    if (!gdb_is_initialized) {
        gdb_arch_init();

		uart_console_dev = device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);

#ifdef CONFIG_GDB_SERVER_INTERRUPT_DRIVEN
        nano_fifo_init(&cmds_queue);
        nano_fifo_init(&avail_queue);
        uart_irq_input_hook_set(uart_console_dev, gdb_irq_input_hook);
        uart_register_input(&avail_queue, &cmds_queue);
#endif
#ifdef CONFIG_MEM_SAFE_NUM_EXTRA_REGIONS
        (void)_mem_safe_region_add((void *) CONFIG_GDB_RAM_ADDRESS, CONFIG_GDB_RAM_SIZE, SYS_MEM_SAFE_READ);
        (void)_mem_safe_region_add((void *) CONFIG_GDB_RAM_ADDRESS, CONFIG_GDB_RAM_SIZE, SYS_MEM_SAFE_WRITE);
#endif
        gdb_is_initialized = 1;
        gdb_system_stop_here(GDB_INVALID_REG_SET);
    }
    return 0;
}

SYS_INIT(init_gdb_server, NANOKERNEL, 1);
