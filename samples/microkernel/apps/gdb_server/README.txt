Title: GDB Server demo

Description:

A simple application that demonstates the GDB Server.
A task is polling the serial line to wait for the GDB RSP stop character (i.e
CTRL+C). Once the GDB RSP stop character is received, the VxMicro system is
stopped and the GDB Server is waiting for more GDB RSP packets to read/write
memory and/or control the VxMicro OS.
This microkernel project is using the first serial line to communicate with the
GDB client.

How to connect a GDB client?
Start the vx_stopmode_server in interactive mode with the following command:

vx_stopmode_server -L <GDB log file> -l gdb -s TCP::<port number> -i

From the vx_stopmode_server command line execute the following commands to
connect to the GDB Server and resume the execution of VxMicro.

> connect TCP::<port number>
> tcf GdbRemote addConfig "gdb0" {"Device":"tcp:localhost:<serial port tcp redirection>"}
> tcf GdbRemote connect "gdb0"
> tcf RunControl resume "gdb0" 0 1
> tcf GdbRemote disconnect "gdb0"

--------------------------------------------------------------------------------

Building and Running Project:

The microkernel project can be built and executed on x86 platforms as follows:

    make

--------------------------------------------------------------------------------

Troubleshooting:

Problems caused by out-dated project information can be addressed by
issuing one of the following commands then rebuilding the project:

    make clean          # discard results of previous builds
                        # but keep existing configuration info
or
    make pristine       # discard results of previous builds
                        # and restore pre-defined configuration info

--------------------------------------------------------------------------------

The GDB log file should contain GDB RSP packets that look like:

TCF 15:50:30.664: w: +
TCF 15:50:30.664: w: <break> (03)
TCF 15:50:30.764: r: T02thread:01;8:00000000
TCF 15:50:30.764: w: +
TCF 15:50:30.764: gdb_client_parse_event: set running flag to 0
TCF 15:50:30.764: w: $?#3f
TCF 15:50:30.764: r: +
TCF 15:50:30.765: r: T02thread:01;
TCF 15:50:30.765: w: +
TCF 15:50:30.765: gdb_client_parse_event: set running flag to 0
TCF 15:50:30.765: w: $vCont?#49
TCF 15:50:30.765: r: +
TCF 15:50:30.765: r: vCont;c;s;C;S
TCF 15:50:30.765: w: +
TCF 15:50:30.765: w: $qfThreadInfo#bb
TCF 15:50:30.765: r: +
TCF 15:50:30.765: r: 
TCF 15:50:30.765: w: +
TCF 15:50:30.765: w: $qSupported#37
TCF 15:50:30.765: r: +
TCF 15:50:30.766: r: PacketSize=258;qXfer:features:read+;QStartNoAckMode+
TCF 15:50:30.766: w: +
TCF 15:50:30.766: w: $qXfer:features:read:target.xml:0,244#e5
TCF 15:50:30.766: r: +
TCF 15:50:30.767: r: l<?xml version="1.0"?> <!DOCTYPE target SYSTEM "gdb-target.dtd"> <target version="1.0">
  <architecture>i386</architecture>
</target>
TCF 15:50:30.767: w: +
TCF 15:50:30.767: w: $p8#a8
TCF 15:50:30.768: r: +
TCF 15:50:30.768: r: 00000000
TCF 15:50:30.768: w: +
TCF 16:11:49.813: w: $c#63
TCF 16:11:49.813: r: +
TCF 16:11:49.813: gdb_client_cont: set running flag to 1
TCF 16:11:52.712: w: <break> (03)
TCF 16:11:52.717: r: T02thread:01;8:00000000
TCF 16:11:52.717: w: +
TCF 16:11:52.717: gdb_client_parse_event: set running flag to 0
TCF 16:11:52.732: w: $D#44
TCF 16:11:52.732: r: +
