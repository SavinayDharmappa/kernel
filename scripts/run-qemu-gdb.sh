#!/usr/bin/env sh

qemu=$1
kernel_type=$2
gdb=$3
shift 3

qemu_bin_name=$(basename ${qemu})
pidfile=/tmp/zephyr-${qemu_bin_name}-$(id -u)-$$.pid
kernel=${kernel_type}.elf

setsid xterm -e ${qemu} $@ -pidfile ${pidfile} -S -s -kernel ${kernel} &

while [ ! -f ${pidfile} ]; do sleep 1; done

gdb_pid=$(cat ${pidfile})
xterm_pid=$(ps -ef | grep ${gdb_pid} | grep -v grep | awk '{print $3}')

#echo ${gdb_pid} >> ${ZEPHYR_BASE}/blah
#echo ${xterm_pid} >> ${ZEPHYR_BASE}/blah

${gdb} -d ${ZEPHYR_BASE} -nx -ex "target remote localhost:1234" -tui ${kernel}

kill ${gdb_pid}
kill ${xterm_pid}

exit 0
