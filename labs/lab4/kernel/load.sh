#!/bin/sh
module="Hardware-Device"
device="hardware"

# invoke insmod to insert the module into the kernel
/sbin/insmod ./$module.ko

# remove stale node
rm -f /dev/${device}

major=`awk "\\$2==\"$module\" {print \\$1}" /proc/devices`

# create the file for the hardware device based on its major number
mknod /dev/${device} c $major 0

# permit the hardware to be accessible from user space
chmod 666 /dev/${device}
