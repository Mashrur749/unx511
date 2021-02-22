#!/bin/sh
module="Key-Generator"
device="keygen"

#Question 35: What is the following line doing?
#removing module from kernel

/sbin/rmmod ./$module.ko

# remove stale node
rm -f /dev/${device}
