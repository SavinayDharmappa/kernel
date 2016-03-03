Title: Galileo2 ADC demo

Description:

Sample application demonstrates reading from ADC channels 0 - 3.
It prints results to serial console and Groove kit LCD display.

Connect LCD to I2C Groove board output
Connect proper sensors to A0-A3 connectors on Groove board.

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

Channel 0 - 00 00
Channel 1 - 0b 3c
Channel 2 - 0a 54
Channel 3 - 07 c4

<repeats endlessly>
