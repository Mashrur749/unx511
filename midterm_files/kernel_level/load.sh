#!/bin/sh
module="Key-Generator"
device="keygen"

#Question 32: What is the following line doing?
#inserting module to kernel

/sbin/insmod ./$module.ko

rm -f /dev/${device}

#Question 33: From what file is the major number being extracted from in the following line?
#/proc/devices

major=`awk "\\$2==\"$module\" {print \\$1}" /proc/devices`

#Question 34: What is the following line doing?
#make device node for the device $device with the major number $major

mknod /dev/${device} c $major 0

# permit the file to be accessible from user space
chmod 666 /dev/${device}
