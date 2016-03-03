/* gdb_server.h - GDB Server header file */

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

#ifndef __INCgdb_serverh
#define __INCgdb_serverh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* define */

#define GDB_STOP_CHAR   0x3             /* GDB RSP default value */

#define NOT_DEBUGGING   0
#define DEBUGGING       1
#define STOP_RUNNING    2
#define SINGLE_STEP     3

#ifndef _ASMLANGUAGE

/* typedef */

typedef enum gdb_exception_mode
     {
     GDB_EXC_TRACE,              /* trace exception */
     GDB_EXC_BP,                 /* breakpoint exception */
     GDB_EXC_OTHER,              /* other exceptions */
     } EXC_MODE;

enum gdb_signal
    {
    GDB_SIG_NULL = -1,
    GDB_SIG_INT = 2,
    GDB_SIG_TRAP = 5,
    GDB_SIG_FPE = 8,
    GDB_SIG_SIGSEGV = 11,
    GDB_SIG_STOP = 17
    };

/* externs */

extern volatile int gdb_debug_status;

/* function declaration */

extern void gdb_system_stop_here (void * regs);
extern void gdb_handler (int mode, void * pEsf, int signal);
extern int gdb_console_out (char val);

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __INCgdb_serverh */
