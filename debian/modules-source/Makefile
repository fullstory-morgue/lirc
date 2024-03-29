include /etc/lirc/lirc-modules-source.conf

# An space should be left at the end of each CFLAGS variable for the
# sanity-check to work.
IT87_CFLAGS     := $(subst ",,$(LIRC_IT87_CFLAGS) )
PARALLEL_CFLAGS	:= $(subst ",,-DLIRC_PORT=$(LIRC_PARALLEL_PORT) \
				-DLIRC_TIMER=$(LIRC_PARALLEL_TIMER) \
				-DLIRC_IRQ=$(LIRC_PARALLEL_IRQ) )
SERIAL_CFLAGS   := $(subst ",,$(LIRC_SERIAL_CFLAGS) \
				-DLIRC_PORT=$(LIRC_SERIAL_PORT) \
				-DLIRC_IRQ=$(LIRC_SERIAL_IRQ) ) 
SIR_CFLAGS		:= $(subst ",,$(LIRC_SIR_CFLAGS) \
				-DLIRC_PORT=$(LIRC_SIR_PORT) \
				-DLIRC_IRQ=$(LIRC_SIR_IRQ) )

KSRC=/lib/modules/$(shell uname -r)/build
KERNEL_LOCATION = $(KSRC)
CC = gcc -D__KERNEL__ -I $(KSRC)/include
CFLAGS= -O2 -g -Wall
DEFS=-DHAVE_CONFIG_H -I. -I../..

KVERS=$(shell sed -n -e '/UTS_RELEASE/s/^[^"]*"\([^"]*\)".*$\/\1/p' $(KSRC)/include/linux/utsrelease.h)
KPATCHLEVEL=$(shell echo $(KVERS) | sed -n -e 's/^[0-9]*\.\([0-9]*\)\..*/\1/p')

KEXT=o
ifeq ($(KPATCHLEVEL),6)
KEXT=ko
endif


export KERNEL_LOCATION CC

all: atiusb, bt829, cmdir, gpio, i2c, igorplugusb, imon, it87, mceusb, mceusb2, parallel, sasem, serial, sir, streamzap

test:
	@echo '$(IT87_CFLAGS)'
	@echo '$(PARALLEL_CFLAGS)'
	@echo '$(LIRC_SERIAL_IRQ)'
	@echo '$(LIRC_SIR_IRQ)'
	@echo '$(LIRC_PARALLEL_IRQ)'
	@echo '$(SERIAL_CFLAGS)'
	@echo '$(SIR_CFLAGS)'

UNCONFIGURED: #Do nothing if the user didn't configure which drivers to build

debconf: $(subst ", ,$(LIRC_MODULES:,=))

modules:
	mkdir modules

sanity-check:
	@if \
		expr "$(DEFS)" : '.* -DLIRC_IRQ= ' > /dev/null\
		|| expr "$(DEFS)" : '.* -DLIRC_PORT= ' > /dev/null \
		|| expr "$(DEFS)" : '.* -DLIRC_TIMER= '> /dev/null \
		|| expr "$(DEFS)" : '.*UNCONFIGURED'> /dev/null;\
	then \
		echo ;\
		echo "##########################";\
		echo "## CONFIGURATION ERROR: ##";\
		echo "##########################";\
		echo ;\
		echo "You should reconfigure lirc-modules-source and make";\
		echo "sure you don't leave blank any one of IRQ, IO Port or ";\
		echo "Timer (parallel only)";\
		echo ;\
		echo "Hint1: use \"dpkg-reconfigure lirc-modules-source\"";\
		echo "Hint2: If you selected \"automagical\" configuration of";\
		echo "	kernel modules you should probably reconfigure lirc";\
		echo "	instead.";\
		echo "Hint3: you may instead edit /etc/lirc/lirc-modules-source.conf";\
		exit 1;\
	fi

dev: modules sanity-check
	$(MAKE) -C drivers SUBDIRS="lirc_dev"
	cp drivers/lirc_dev/lirc_dev.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_dev.$(KEXT).KVERS

atiusb: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_atiusb"
	cp drivers/lirc_atiusb/lirc_atiusb.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_atiusb.$(KEXT).KVERS

bt829: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_bt829"
	cp drivers/lirc_bt829/lirc_bt829.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_bt829.$(KEXT).KVERS

cmdir: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_cmdir"
	cp drivers/lirc_cmdir/lirc_cmdir.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_cmdir.$(KEXT).KVERS

gpio: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_gpio"
	cp drivers/lirc_gpio/lirc_gpio.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_gpio.$(KEXT).KVERS

i2c: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_i2c"
	cp drivers/lirc_i2c/lirc_i2c.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_i2c.$(KEXT).KVERS

igorplugusb: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_igorplugusb"
	cp drivers/lirc_igorplugusb/lirc_igorplugusb.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_igorplugusb.$(KEXT).KVERS

imon: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_imon"
	cp drivers/lirc_imon/lirc_imon.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_imon.$(KEXT).KVERS

it87: DEFS += $(IT87_CFLAGS)
it87: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_it87" DEFS="$(DEFS)"
	cp drivers/lirc_it87/lirc_it87.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_it87.$(KEXT).KVERS

mceusb: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_mceusb"
	cp drivers/lirc_mceusb/lirc_mceusb.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_mceusb.$(KEXT).KVERS

mceusb2: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_mceusb2"
	cp drivers/lirc_mceusb2/lirc_mceusb2.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_mceusb2.$(KEXT).KVERS

parallel: DEFS += $(PARALLEL_CFLAGS)
parallel: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_parallel" DEFS="$(DEFS)"
	cp drivers/lirc_parallel/lirc_parallel.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_parallel.$(KEXT).KVERS

sasem: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_sasem"
	cp drivers/lirc_sasem/lirc_sasem.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_sasem.$(KEXT).KVERS

serial: DEFS += $(SERIAL_CFLAGS)
serial: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_serial" DEFS="$(DEFS)"
	cp drivers/lirc_serial/lirc_serial.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_serial.$(KEXT).KVERS

sir: DEFS += $(SIR_CFLAGS)
sir: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_sir" DEFS="$(DEFS)"
	cp drivers/lirc_sir/lirc_sir.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_sir.$(KEXT).KVERS

streamzap: modules sanity-check dev
	$(MAKE) -C drivers SUBDIRS="lirc_streamzap"
	cp drivers/lirc_streamzap/lirc_streamzap.$(KEXT) modules
	@echo $(KVERS) $(KSRC) > modules/lirc_streamzap.$(KEXT).KVERS

install:
	@for file in modules/*.$(KEXT); \
	do \
		kvers=`cut -d" " -f1 $$file.KVERS` && \
		ksrc=`cut -d" " -f2 $$file.KVERS` && \
		dir="$(prefix)/lib/modules/$$kvers/misc/" && \
		( [ -d $$dir ] || mkdir -p $$dir ) && \
		cp -v $$file $$dir ; \
	done
depmod:
	@for file in modules/*.$(KEXT); \
	do \
		kvers=`cut -d" " -f1 $$file.KVERS` && \
		ksrc=`cut -d" " -f2 $$file.KVERS` && \
		dir="$(prefix)/lib/modules/$$kvers/misc/" && \
		if [ -f $$ksrc/System.map ];then \
			/sbin/depmod -r -b $(prefix)/ -a $$kvers \
					-F $$ksrc/System.map ; \
		elif [ -f /boot/System.map-$$kvers ];then \
			/sbin/depmod -r -b $(prefix)/ -a $$kvers \
					-F /boot/System.map-$$kvers; \
		else \
			/sbin/depmod -a ;\
		fi ;\
	done

clean:
	$(MAKE) clean -C drivers SUBDIRS="lirc_serial lirc_parallel lirc_i2c lirc_sir lirc_dev lirc_gpio lirc_it87 lirc_bt829 lirc_atiusb"
	rm -rf modules
