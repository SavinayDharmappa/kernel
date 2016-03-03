Title: Hello World

Description:

A simple application that demonstates basic sanity of the microkernel.
Two tasks (A and B) take turns printing a greeting message to the console,
and use sleep requests and semaphores to control the rate at which messages
are generated. This demonstrates that microkernel scheduling, communication,
and timing are operating correctly.
In addition it PWM controls an LED on Galileo Gen2 header pin IO11, and use
GPIO to turn on/off header pin IO7, as well as use GPIO to mux PWM8
to enable output via IO11 when PWM9 is modulated.

Connect an LED to pins IO7 and pwm controlled IO11 to observe output.
Connect a voltage to IO8 to read input as 0 or 1

--------------------------------------------------------------------------------

Building and Running Project:

This microkernel project outputs to the console.  QEMU is not supported since
it does not have the devices excercised in this example. This demo is intended
for the Galileo Gen2 platform. Galileo Gen1 does not have the same devices
populated.

make pristine && make PLATFORM_CONFIG=galileo

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

Sample Output:

******** START PWM TASKA ***********
Pin IO11: 100 percent on
Pin IO11: 99 percent on
Pin IO11: 0 percent on
Pin IO11: Cycle of 0 -> 100 percent
******** END OF PWM TASKA **********
Pin IO7 on
taskA: Hello World!
******** START PWM TASKB ***********
Pin IO11 on/of inverse cycle
******** END OF PWM TASKB **********
Pin IO8 on
taskB: Hello World!


<repeats endlessly>
