# Makefile - makefile for kernel level key generator
#
# 11-Feb-21  M. Watler          Created.
#
TARGET_MODULE:=Key-Generator

# If we running by kernel building system
ifneq ($(KERNELRELEASE),)
	$(TARGET_MODULE)-objs := keyGenerator.o
	obj-m := $(TARGET_MODULE).o

# If we are running without kernel build system
else
	BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)


all : 
# run kernel build system to make module
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

endif
