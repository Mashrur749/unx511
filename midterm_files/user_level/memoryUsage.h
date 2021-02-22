// memoryUsage.h - header file for memory monitoring
//
// 11-Feb-21  M. Watler         Created.
//
#ifndef MEMORY_USAGE_H
#define MEMORY_USAGE_H
#include "keyGenerator.h"

//Question 22: Is keyFile (/dev/keygen) a file that we can edit with vim?
//no, it's device node which we can communicate through ioctls

//Question 23: When we execute ls -l on /dev/keygen, we see the following:
//Question 23 (continued): crw-rw-rw- 1 root root 238, 0 Feb 11 11:30 /dev/keygen
//Question 23 (continued): What does the c mean in crw-rw-rw-?

//c means it's a character device

//Question 24: What does the number 238 represent?
//the major number of the device

const char keyFile[]="/dev/keygen";
const char logFile[]="midterm.log";

#endif// MEMORY_USAGE_H
