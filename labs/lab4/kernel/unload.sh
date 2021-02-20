#!/bin/sh
module="Hardware-Device"
device="hardware"

# invoke rmmod to remove the module from the kernel
/sbin/rmmod ./$module.ko

# remove stale node
rm -f /dev/${device}
