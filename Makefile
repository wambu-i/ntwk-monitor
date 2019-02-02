# Makefile for the jasusi device driver.

# Enable debugging
DEBUG:=y

INCLUDE:=/home/wambui/openWRT/ntwk-jasusi/

ifeq ($(DEBUG),y)
	DEBFLAGS = -O -g -DDEBUGPRINT
else
	DEBFLAGS = -O2
endif

ifneq ($(KERNELRELEASE),)
	# call from kernel build system

	obj-$(CONFIG_JASUSI) += jasusi.o
	ccflags-y	:= -I$(INCLUDE) 
	ccflags-y	+=	$(DEBFLAGS)

	CONFIG_JASUSI_DEBUG	:= -DDEBUG
	jasusi-objs := device.o
	CFLAGS_device.o	:= -DDEBUG

else
	# external module build

	KDIR	:= /lib/modules/$(shell uname -r)/build
	MDIR	:= /lib/modules/$(shell uname -r)
	PWD	:= $(shell pwd)
	KREL	:= $(shell cd ${KDIR} && make -s kernelrelease)

	export CONFIG_JASUSI := m

	all:
		$(MAKE) -C $(KDIR)  M=$(PWD) modules

	clean:
		$(MAKE) -C $(KDIR) M=$(PWD) clean

	help:
		$(MAKE) -C $(KDIR) M=$(PWD) help

	install: jasusi.ko
		rm -f ${MDIR}/kernel/jasusi/jasusi.ko
		install -m644 -b -D jasusi.ko ${MDIR}/kernel/jasusi/jasusi.ko
		depmod -aq

	uninstall:
		rm -rf ${MDIR}/kernel/jasusi
		depmod -aq

endif

.PHONY : all clean install uninstall
