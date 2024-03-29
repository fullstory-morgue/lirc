/*      $Id: lirc_serial.c,v 5.80 2007/05/11 16:40:24 lirc Exp $      */

/****************************************************************************
 ** lirc_serial.c ***********************************************************
 ****************************************************************************
 *
 * lirc_serial - Device driver that records pulse- and pause-lengths
 *               (space-lengths) between DDCD event on a serial port.
 *
 * Copyright (C) 1996,97 Ralph Metzler <rjkm@thp.uni-koeln.de>
 * Copyright (C) 1998 Trent Piepho <xyzzy@u.washington.edu>
 * Copyright (C) 1998 Ben Pfaff <blp@gnu.org>
 * Copyright (C) 1999 Christoph Bartelmus <lirc@bartelmus.de>
 * Copyright (C) 2007 Andrei Tanas <andrei@tanas.ca> (suspend/resume support)
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* Steve's changes to improve transmission fidelity:
     - for systems with the rdtsc instruction and the clock counter, a 
       send_pule that times the pulses directly using the counter.
       This means that the LIRC_SERIAL_TRANSMITTER_LATENCY fudge is
       not needed. Measurement shows very stable waveform, even where
       PCI activity slows the access to the UART, which trips up other
       versions.
     - For other system, non-integer-microsecond pulse/space lengths,
       done using fixed point binary. So, much more accurate carrier
       frequency.
     - fine tuned transmitter latency, taking advantage of fractional
       microseconds in previous change
     - Fixed bug in the way transmitter latency was accounted for by
       tuning the pulse lengths down - the send_pulse routine ignored
       this overhead as it timed the overall pulse length - so the
       pulse frequency was right but overall pulse length was too
       long. Fixed by accounting for latency on each pulse/space
       iteration.

   Steve Davies <steve@daviesfam.org>  July 2001
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
 
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 2, 18)
#error "**********************************************************"
#error " Sorry, this driver needs kernel version 2.2.18 or higher "
#error "**********************************************************"
#endif

#include <linux/autoconf.h>

#if defined(CONFIG_SERIAL) || defined(CONFIG_SERIAL_8250)
#warning "******************************************"
#warning " Your serial port driver is compiled into "
#warning " the kernel. You will have to release the "
#warning " port you want to use for LIRC with:      "
#warning "    setserial /dev/ttySx uart none        "
#warning "******************************************"
#endif

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/serial_reg.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/poll.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
#include <linux/platform_device.h>
#endif

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/fcntl.h>

#if defined(LIRC_SERIAL_NSLU2)
#include <asm/hardware.h>
/* From Intel IXP42X Developer's Manual (#252480-005): */
/* ftp://download.intel.com/design/network/manuals/25248005.pdf */
#define UART_IE_IXP42X_UUE   0x40 /* IXP42X UART Unit enable */
#define UART_IE_IXP42X_RTOIE 0x10 /* IXP42X Receiver Data Timeout int.enable */
#endif

#include "drivers/lirc.h"
#include "drivers/kcompat.h"
#include "drivers/lirc_dev/lirc_dev.h"

#if defined(LIRC_SERIAL_SOFTCARRIER) && !defined(LIRC_SERIAL_TRANSMITTER)
#warning "Software carrier only affects transmitting"
#endif

#if defined(rdtsc)

#define USE_RDTSC
#warning "Note: using rdtsc instruction"
#endif

#ifdef LIRC_SERIAL_ANIMAX
#ifdef LIRC_SERIAL_TRANSMITTER
#warning "******************************************"
#warning " This receiver does not have a            "
#warning " transmitter diode                        "
#warning "******************************************"
#endif
#endif

#define LIRC_DRIVER_NAME "lirc_serial"

struct lirc_serial
{
	int signal_pin;
	int signal_pin_change;
	int on;
	int off;
	long (*send_pulse)(unsigned long length);
	void (*send_space)(long length);
	int features;
};

#define LIRC_HOMEBREW        0
#define LIRC_IRDEO           1
#define LIRC_IRDEO_REMOTE    2
#define LIRC_ANIMAX          3
#define LIRC_IGOR            4
#define LIRC_NSLU2           5

#ifdef LIRC_SERIAL_IRDEO
static int type=LIRC_IRDEO;
#elif defined(LIRC_SERIAL_IRDEO_REMOTE)
static int type=LIRC_IRDEO_REMOTE;
#elif defined(LIRC_SERIAL_ANIMAX)
static int type=LIRC_ANIMAX;
#elif defined(LIRC_SERIAL_IGOR)
static int type=LIRC_IGOR;
#elif defined(LIRC_SERIAL_NSLU2)
static int type=LIRC_NSLU2;
#else
static int type=LIRC_HOMEBREW;
#endif

/* Set defaults for NSLU2 */
#if defined(LIRC_SERIAL_NSLU2)
#ifndef LIRC_IRQ
#define LIRC_IRQ IRQ_IXP4XX_UART2
#endif
#ifndef LIRC_PORT
#define LIRC_PORT (IXP4XX_UART2_BASE_VIRT + REG_OFFSET)
#endif
#ifndef LIRC_IOMMAP
#define LIRC_IOMMAP IXP4XX_UART2_BASE_PHYS
#endif
#ifndef LIRC_IOSHIFT
#define LIRC_IOSHIFT 2
#endif
#ifndef LIRC_ALLOW_MMAPPED_IO
#define LIRC_ALLOW_MMAPPED_IO
#endif
#endif

#if defined(LIRC_ALLOW_MMAPPED_IO)
#ifndef LIRC_IOMMAP
#define LIRC_IOMMAP 0
#endif
#ifndef LIRC_IOSHIFT
#define LIRC_IOSHIFT 0
#endif
static int iommap = LIRC_IOMMAP;
static int ioshift = LIRC_IOSHIFT;
#endif

#ifdef LIRC_SERIAL_SOFTCARRIER
static int softcarrier=1;
#else
static int softcarrier=0;
#endif

static int share_irq = 0;
static int debug = 0;

#define dprintk(fmt, args...)                                   \
	do{                                                     \
		if(debug)                                       \
	                printk(KERN_DEBUG LIRC_DRIVER_NAME ": " \
                               fmt, ## args);                   \
	}while(0)

/* forward declarations */
static long send_pulse_irdeo(unsigned long length);
static long send_pulse_homebrew(unsigned long length);
static void send_space_irdeo(long length);
static void send_space_homebrew(long length);

static struct lirc_serial hardware[]=
{
	/* home-brew receiver/transmitter */
	{
		UART_MSR_DCD,
		UART_MSR_DDCD,
		UART_MCR_RTS|UART_MCR_OUT2|UART_MCR_DTR,
		UART_MCR_RTS|UART_MCR_OUT2,
		send_pulse_homebrew,
		send_space_homebrew,
		(
#ifdef LIRC_SERIAL_TRANSMITTER
		 LIRC_CAN_SET_SEND_DUTY_CYCLE|
		 LIRC_CAN_SET_SEND_CARRIER|
		 LIRC_CAN_SEND_PULSE|
#endif
		 LIRC_CAN_REC_MODE2)
	},
	
	/* IRdeo classic */
	{
		UART_MSR_DSR,
		UART_MSR_DDSR,
		UART_MCR_OUT2,
		UART_MCR_RTS|UART_MCR_DTR|UART_MCR_OUT2,
		send_pulse_irdeo,
		send_space_irdeo,
		(LIRC_CAN_SET_SEND_DUTY_CYCLE|
		 LIRC_CAN_SEND_PULSE|
		 LIRC_CAN_REC_MODE2)
	},
	
	/* IRdeo remote */
	{
		UART_MSR_DSR,
		UART_MSR_DDSR,
		UART_MCR_RTS|UART_MCR_DTR|UART_MCR_OUT2,
		UART_MCR_RTS|UART_MCR_DTR|UART_MCR_OUT2,
		send_pulse_irdeo,
		send_space_irdeo,
		(LIRC_CAN_SET_SEND_DUTY_CYCLE|
		 LIRC_CAN_SEND_PULSE|
		 LIRC_CAN_REC_MODE2)
	},
	
	/* AnimaX */
	{
		UART_MSR_DCD,
		UART_MSR_DDCD,
		0,
		UART_MCR_RTS|UART_MCR_DTR|UART_MCR_OUT2,
		NULL,
		NULL,
		LIRC_CAN_REC_MODE2
	},
	
	/* home-brew receiver/transmitter (Igor Cesko's variation) */
	{
		UART_MSR_DSR,
		UART_MSR_DDSR,
		UART_MCR_RTS|UART_MCR_OUT2|UART_MCR_DTR,
		UART_MCR_RTS|UART_MCR_OUT2,
		send_pulse_homebrew,
		send_space_homebrew,
		(
#ifdef LIRC_SERIAL_TRANSMITTER
		 LIRC_CAN_SET_SEND_DUTY_CYCLE|
		 LIRC_CAN_SET_SEND_CARRIER|
		 LIRC_CAN_SEND_PULSE|
#endif
		 LIRC_CAN_REC_MODE2)
	},
	
#if defined(LIRC_SERIAL_NSLU2)
	/* Modified Linksys Network Storage Link USB 2.0 (NSLU2):
	   We receive on CTS of the 2nd serial port (R142,LHS), we
	   transmit with a IR diode between GPIO[1] (green status LED),
	   and ground (Matthias Goebl <matthias.goebl@goebl.net>).
	   See also http://www.nslu2-linux.org for this device */
	{
		UART_MSR_CTS,
		UART_MSR_DCTS,
		UART_MCR_RTS|UART_MCR_OUT2|UART_MCR_DTR,
		UART_MCR_RTS|UART_MCR_OUT2,
		send_pulse_homebrew,
		send_space_homebrew,
		(
#ifdef LIRC_SERIAL_TRANSMITTER
		 LIRC_CAN_SET_SEND_DUTY_CYCLE|
		 LIRC_CAN_SET_SEND_CARRIER|
		 LIRC_CAN_SEND_PULSE|
#endif
		 LIRC_CAN_REC_MODE2)
	},
#endif
	
};

#define RS_ISR_PASS_LIMIT 256

/* A long pulse code from a remote might take upto 300 bytes.  The
   daemon should read the bytes as soon as they are generated, so take
   the number of keys you think you can push before the daemon runs
   and multiply by 300.  The driver will warn you if you overrun this
   buffer.  If you have a slow computer or non-busmastering IDE disks,
   maybe you will need to increase this.  */

/* This MUST be a power of two!  It has to be larger than 1 as well. */

#define RBUF_LEN 256
#define WBUF_LEN 256

static int sense = -1;   /* -1 = auto, 0 = active high, 1 = active low */
static int txsense = 0;   /* 0 = active high, 1 = active low */

#ifndef LIRC_IRQ
#define LIRC_IRQ 4
#endif
#ifndef LIRC_PORT
#define LIRC_PORT 0x3f8
#endif

static int io = LIRC_PORT;
static int irq = LIRC_IRQ;

static struct timeval lasttv = {0, 0};

static struct lirc_buffer rbuf;

static lirc_t wbuf[WBUF_LEN];

static unsigned int freq = 38000;
static unsigned int duty_cycle = 50;

/* Initialized in init_timing_params() */
static unsigned long period = 0;
static unsigned long pulse_width = 0;
static unsigned long space_width = 0;

#if defined(__i386__)
/*
  From:
  Linux I/O port programming mini-HOWTO
  Author: Riku Saikkonen <Riku.Saikkonen@hut.fi>
  v, 28 December 1997
  
  [...]
  Actually, a port I/O instruction on most ports in the 0-0x3ff range
  takes almost exactly 1 microsecond, so if you're, for example, using
  the parallel port directly, just do additional inb()s from that port
  to delay.
  [...]
*/
/* transmitter latency 1.5625us 0x1.90 - this figure arrived at from
 * comment above plus trimming to match actual measured frequency.
 * This will be sensitive to cpu speed, though hopefully most of the 1.5us
 * is spent in the uart access.  Still - for reference test machine was a
 * 1.13GHz Athlon system - Steve
 */

/* changed from 400 to 450 as this works better on slower machines;
   faster machines will use the rdtsc code anyway */

#define LIRC_SERIAL_TRANSMITTER_LATENCY 450

#else

/* does anybody have information on other platforms ? */
/* 256 = 1<<8 */
#define LIRC_SERIAL_TRANSMITTER_LATENCY 256

#endif  /* __i386__ */

static inline unsigned int sinp(int offset)
{
#if defined(LIRC_ALLOW_MMAPPED_IO)
	if(iommap != 0) /* the register is memory-mapped */
	{
		offset <<= ioshift;
		return readb(io + offset);
	} 
#endif
	return inb(io + offset);
}

static inline void soutp(int offset, int value)
{
#if defined(LIRC_ALLOW_MMAPPED_IO)
	if(iommap != 0) /* the register is memory-mapped */
	{
		offset <<= ioshift;
		writeb(value, io + offset);
	}
#endif
	outb(value, io + offset);
}

static inline void on(void)
{
#if defined(LIRC_SERIAL_NSLU2)
	/* On NSLU2, we put the transmit diode between the output of the green
	   status LED and ground */
	if(type == LIRC_NSLU2)
	{
		gpio_line_set(NSLU2_LED_GRN, IXP4XX_GPIO_LOW);
		return;
	}
#endif
	if (txsense)
	{
		soutp(UART_MCR,hardware[type].off);
	}
	else
	{
		soutp(UART_MCR,hardware[type].on);
	}
}
  
static inline void off(void)
{
#if defined(LIRC_SERIAL_NSLU2)
	if(type == LIRC_NSLU2)
	{
		gpio_line_set(NSLU2_LED_GRN, IXP4XX_GPIO_HIGH);
		return;
	}
#endif
	if (txsense)
	{
		soutp(UART_MCR,hardware[type].on);
	}
	else
	{
		soutp(UART_MCR,hardware[type].off);
	}
}

#ifndef MAX_UDELAY_MS
#define MAX_UDELAY_US 5000
#else
#define MAX_UDELAY_US (MAX_UDELAY_MS*1000)
#endif

static inline void safe_udelay(unsigned long usecs)
{
	while(usecs>MAX_UDELAY_US)
	{
		udelay(MAX_UDELAY_US);
		usecs-=MAX_UDELAY_US;
	}
	udelay(usecs);
}

#ifdef USE_RDTSC
/* This is an overflow/precision juggle, complicated in that we can't
   do long long divide in the kernel */

/* When we use the rdtsc instruction to measure clocks, we keep the
 * pulse and space widths as clock cycles.  As this is CPU speed
 * dependent, the widths must be calculated in init_port and ioctl
 * time
 */

/* So send_pulse can quickly convert microseconds to clocks */
static unsigned long conv_us_to_clocks = 0;

static inline int init_timing_params(unsigned int new_duty_cycle,
		unsigned int new_freq)
{
	unsigned long long loops_per_sec,work;
	
	duty_cycle=new_duty_cycle;
	freq=new_freq;

	loops_per_sec=current_cpu_data.loops_per_jiffy;
	loops_per_sec*=HZ;
	
	/* How many clocks in a microsecond?, avoiding long long divide */
	work=loops_per_sec;
	work*=4295;  /* 4295 = 2^32 / 1e6 */
	conv_us_to_clocks=(work>>32);
	
	/* Carrier period in clocks, approach good up to 32GHz clock,
           gets carrier frequency within 8Hz */
	period=loops_per_sec>>3;
	period/=(freq>>3);

	/* Derive pulse and space from the period */

	pulse_width = period*duty_cycle/100;
	space_width = period - pulse_width;
	dprintk("in init_timing_params, freq=%d, duty_cycle=%d, "
		"clk/jiffy=%ld, pulse=%ld, space=%ld, "
		"conv_us_to_clocks=%ld\n",
		freq, duty_cycle, current_cpu_data.loops_per_jiffy,
		pulse_width, space_width, conv_us_to_clocks);
	return 0;
}
#else /* ! USE_RDTSC */
static inline int init_timing_params(unsigned int new_duty_cycle,
		unsigned int new_freq)
{
/* period, pulse/space width are kept with 8 binary places -
 * IE multiplied by 256. */
	if(256*1000000L/new_freq*new_duty_cycle/100<=
	   LIRC_SERIAL_TRANSMITTER_LATENCY) return(-EINVAL);
	if(256*1000000L/new_freq*(100-new_duty_cycle)/100<=
	   LIRC_SERIAL_TRANSMITTER_LATENCY) return(-EINVAL);
	duty_cycle=new_duty_cycle;
	freq=new_freq;
	period=256*1000000L/freq;
	pulse_width=period*duty_cycle/100;
	space_width=period-pulse_width;
	dprintk("in init_timing_params, freq=%d pulse=%ld, "
		"space=%ld\n", freq, pulse_width, space_width);
	return 0;
}
#endif /* USE_RDTSC */


/* return value: space length delta */

static long send_pulse_irdeo(unsigned long length)
{
	long rawbits;
	int i;
	unsigned char output;
	unsigned char chunk,shifted;
	
	/* how many bits have to be sent ? */
	rawbits=length*1152/10000;
	if(duty_cycle>50) chunk=3;
	else chunk=1;
	for(i=0,output=0x7f;rawbits>0;rawbits-=3)
	{
		shifted=chunk<<(i*3);
		shifted>>=1;
		output&=(~shifted);
		i++;
		if(i==3)
		{
			soutp(UART_TX,output);
			while(!(sinp(UART_LSR) & UART_LSR_THRE));
			output=0x7f;
			i=0;
		}
	}
	if(i!=0)
	{
		soutp(UART_TX,output);
		while(!(sinp(UART_LSR) & UART_LSR_TEMT));
	}

	if(i==0)
	{
		return((-rawbits)*10000/1152);
	}
	else
	{
		return((3-i)*3*10000/1152+(-rawbits)*10000/1152);
	}
}

#ifdef USE_RDTSC
/* Version that uses Pentium rdtsc instruction to measure clocks */

/* This version does sub-microsecond timing using rdtsc instruction,
 * and does away with the fudged LIRC_SERIAL_TRANSMITTER_LATENCY
 * Implicitly i586 architecture...  - Steve
 */

static inline long send_pulse_homebrew_softcarrier(unsigned long length)
{
	int flag;
	unsigned long target, start, now;

	/* Get going quick as we can */
	rdtscl(start);on();
	/* Convert length from microseconds to clocks */
	length*=conv_us_to_clocks;
	/* And loop till time is up - flipping at right intervals */
	now=start;
	target=pulse_width;
	flag=1;
	while((now-start)<length)
	{
		/* Delay till flip time */
		do
		{
			rdtscl(now);
		}
		while ((now-start)<target);
		/* flip */
		if(flag)
		{
			rdtscl(now);off();
			target+=space_width;
		}
		else
		{
			rdtscl(now);on();
			target+=pulse_width;
		}
		flag=!flag;
	}
	rdtscl(now);
	return(((now-start)-length)/conv_us_to_clocks);
}
#else /* ! USE_RDTSC */
/* Version using udelay() */

/* here we use fixed point arithmetic, with 8
   fractional bits.  that gets us within 0.1% or so of the right average
   frequency, albeit with some jitter in pulse length - Steve */

/* To match 8 fractional bits used for pulse/space length */

static inline long send_pulse_homebrew_softcarrier(unsigned long length)
{
	int flag;
	unsigned long actual, target, d;
	length<<=8;

	actual=target=0; flag=0;
	while(actual<length)
	{
		if(flag)
		{
			off();
			target+=space_width;
		}
		else
		{
			on();
			target+=pulse_width;
		}
		d=(target-actual-LIRC_SERIAL_TRANSMITTER_LATENCY+128)>>8;
		/* Note - we've checked in ioctl that the pulse/space
		   widths are big enough so that d is > 0 */
		udelay(d);
		actual+=(d<<8)+LIRC_SERIAL_TRANSMITTER_LATENCY;
		flag=!flag;
	}
	return((actual-length)>>8);
}
#endif /* USE_RDTSC */

static long send_pulse_homebrew(unsigned long length)
{
	if(length<=0) return 0;
	if(softcarrier)
	{
		return send_pulse_homebrew_softcarrier(length);
	}
	else
	{
		on();
		safe_udelay(length);
		return(0);
	}
}

static void send_space_irdeo(long length)
{
	if(length<=0) return;
	safe_udelay(length);
}

static void send_space_homebrew(long length)
{
        off();
	if(length<=0) return;
	safe_udelay(length);
}

static void inline rbwrite(lirc_t l)
{
	if(lirc_buffer_full(&rbuf))    /* no new signals will be accepted */
	{
		dprintk("Buffer overrun\n");
		return;
	}
	_lirc_buffer_write_1(&rbuf, (void *)&l);
}

static void inline frbwrite(lirc_t l)
{
	/* simple noise filter */
	static lirc_t pulse=0L,space=0L;
	static unsigned int ptr=0;
	
	if(ptr>0 && (l&PULSE_BIT))
	{
		pulse+=l&PULSE_MASK;
		if(pulse>250)
		{
			rbwrite(space);
			rbwrite(pulse|PULSE_BIT);
			ptr=0;
			pulse=0;
		}
		return;
	}
	if(!(l&PULSE_BIT))
	{
		if(ptr==0)
		{
			if(l>20000)
			{
				space=l;
				ptr++;
				return;
			}
		}
		else
		{
			if(l>20000)
			{
				space+=pulse;
				if(space>PULSE_MASK) space=PULSE_MASK;
				space+=l;
				if(space>PULSE_MASK) space=PULSE_MASK;
				pulse=0;
				return;
			}
			rbwrite(space);
			rbwrite(pulse|PULSE_BIT);
			ptr=0;
			pulse=0;
		}
	}
	rbwrite(l);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
static irqreturn_t irq_handler(int i, void *blah)
#else
static irqreturn_t irq_handler(int i, void *blah, struct pt_regs *regs)
#endif
{
	struct timeval tv;
	int status,counter,dcd;
	long deltv;
	lirc_t data;
	
	if((sinp(UART_IIR) & UART_IIR_NO_INT))
	{
		/* not our interrupt */
		return IRQ_RETVAL(IRQ_NONE);
	}
	
	counter=0;
	do{
		counter++;
		status=sinp(UART_MSR);
		if(counter>RS_ISR_PASS_LIMIT)
		{
			printk(KERN_WARNING LIRC_DRIVER_NAME ": AIEEEE: "
			       "We're caught!\n");
			break;
		}
		if((status&hardware[type].signal_pin_change) && sense!=-1)
		{
			/* get current time */
			do_gettimeofday(&tv);
			
			/* New mode, written by Trent Piepho 
			   <xyzzy@u.washington.edu>. */
			
			/* The old format was not very portable.
			   We now use the type lirc_t to pass pulses
			   and spaces to user space.
			   
			   If PULSE_BIT is set a pulse has been
			   received, otherwise a space has been
			   received.  The driver needs to know if your
			   receiver is active high or active low, or
			   the space/pulse sense could be
			   inverted. The bits denoted by PULSE_MASK are
			   the length in microseconds. Lengths greater
			   than or equal to 16 seconds are clamped to
			   PULSE_MASK.  All other bits are unused.
			   This is a much simpler interface for user
			   programs, as well as eliminating "out of
			   phase" errors with space/pulse
			   autodetection. */

			/* calculate time since last interrupt in
			   microseconds */
			dcd=(status & hardware[type].signal_pin) ? 1:0;
			
			deltv=tv.tv_sec-lasttv.tv_sec;
			if(tv.tv_sec<lasttv.tv_sec ||
			   (tv.tv_sec==lasttv.tv_sec &&
			    tv.tv_usec<lasttv.tv_usec))
			{
				printk(KERN_WARNING LIRC_DRIVER_NAME
				       ": AIEEEE: your clock just jumped "
				       "backwards\n");
				printk(KERN_WARNING LIRC_DRIVER_NAME
				       ": %d %d %lx %lx %lx %lx\n",
				       dcd,sense,
				       tv.tv_sec,lasttv.tv_sec,
				       tv.tv_usec,lasttv.tv_usec);
				data=PULSE_MASK;
			}
			else if(deltv>15) 
			{
				data=PULSE_MASK; /* really long time */
				if(!(dcd^sense)) /* sanity check */
				{
					printk(KERN_WARNING LIRC_DRIVER_NAME
					       "AIEEEE: %d %d %lx %lx %lx %lx\n",
					       dcd,sense,
					       tv.tv_sec,lasttv.tv_sec,
					       tv.tv_usec,lasttv.tv_usec);
				        /* detecting pulse while this
					   MUST be a space! */
				        sense=sense ? 0:1;
				}
			}
			else
			{
				data=(lirc_t) (deltv*1000000+
					       tv.tv_usec-
					       lasttv.tv_usec);
			}
			frbwrite(dcd^sense ? data : (data|PULSE_BIT));
			lasttv=tv;
			wake_up_interruptible(&rbuf.wait_poll);
		}
	} while(!(sinp(UART_IIR) & UART_IIR_NO_INT)); /* still pending ? */
	return IRQ_RETVAL(IRQ_HANDLED);
}

static void hardware_init_port(void)
{
	unsigned long flags;
	local_irq_save(flags);
	
	/* Set DLAB 0. */
	soutp(UART_LCR, sinp(UART_LCR) & (~UART_LCR_DLAB));
	
	/* First of all, disable all interrupts */
	soutp(UART_IER, sinp(UART_IER)&
	      (~(UART_IER_MSI|UART_IER_RLSI|UART_IER_THRI|UART_IER_RDI)));
	
	/* Clear registers. */
	sinp(UART_LSR);
	sinp(UART_RX);
	sinp(UART_IIR);
	sinp(UART_MSR);
	
#if defined(LIRC_SERIAL_NSLU2)
	if(type == LIRC_NSLU2) /* Setup NSLU2 UART */
	{
		/* Enable UART */
		soutp(UART_IER, sinp(UART_IER) | UART_IE_IXP42X_UUE);
		/* Disable Receiver data Time out interrupt */
		soutp(UART_IER, sinp(UART_IER) & ~UART_IE_IXP42X_RTOIE);
		/* set out2 = interupt unmask; off() doesn't set MCR
		   on NSLU2 */
		soutp(UART_MCR,UART_MCR_RTS|UART_MCR_OUT2);
	}
#endif

	/* Set line for power source */
	off();
	
	/* Clear registers again to be sure. */
	sinp(UART_LSR);
	sinp(UART_RX);
	sinp(UART_IIR);
	sinp(UART_MSR);

	switch(type)
	{
	case LIRC_IRDEO:
	case LIRC_IRDEO_REMOTE:
		/* setup port to 7N1 @ 115200 Baud */
		/* 7N1+start = 9 bits at 115200 ~ 3 bits at 38kHz */
		
		/* Set DLAB 1. */
		soutp(UART_LCR, sinp(UART_LCR) | UART_LCR_DLAB);
		/* Set divisor to 1 => 115200 Baud */
		soutp(UART_DLM,0);
		soutp(UART_DLL,1);
		/* Set DLAB 0 +  7N1 */
		soutp(UART_LCR,UART_LCR_WLEN7);
		/* THR interrupt already disabled at this point */
		break;
	default:
		break;
	}
	
	local_irq_restore(flags);
}
	
static int init_port(void)
{
	int i, nlow, nhigh;
	
	/* Reserve io region. */
#if defined(LIRC_ALLOW_MMAPPED_IO)
	/* Future MMAP-Developers: Attention!
	   For memory mapped I/O you *might* need to use ioremap() first,
	   for the NSLU2 it's done in boot code. */
	if(((iommap != 0)
	    && (request_mem_region(iommap, 8<<ioshift,
				   LIRC_DRIVER_NAME) == NULL))
	   || ((iommap == 0)
	       && (request_region(io, 8, LIRC_DRIVER_NAME) == NULL)))
#else
	if(request_region(io, 8, LIRC_DRIVER_NAME)==NULL)
#endif
	{
		printk(KERN_ERR  LIRC_DRIVER_NAME  
		       ": port %04x already in use\n", io);
		printk(KERN_WARNING LIRC_DRIVER_NAME  
		       ": use 'setserial /dev/ttySX uart none'\n");
		printk(KERN_WARNING LIRC_DRIVER_NAME  
		       ": or compile the serial port driver as module and\n");
		printk(KERN_WARNING LIRC_DRIVER_NAME  
		       ": make sure this module is loaded first\n");
		return(-EBUSY);
	}
	
	hardware_init_port();

	/* Initialize pulse/space widths */
	init_timing_params(duty_cycle, freq);

	/* If pin is high, then this must be an active low receiver. */
	if(sense==-1)
	{
		/* wait 1/2 sec for the power supply */
		
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ/2);
		
		/* probe 9 times every 0.04s, collect "votes" for
		   active high/low */
		nlow = 0;
		nhigh = 0;
		for(i = 0; i < 9; i ++)
		{
			if (sinp(UART_MSR) & hardware[type].signal_pin)
			{
				nlow++;
			}
			else
			{
				nhigh++;
			}
			schedule_timeout(HZ/25);
		}
		sense = (nlow >= nhigh ? 1 : 0);
		printk(KERN_INFO  LIRC_DRIVER_NAME  ": auto-detected active "
		       "%s receiver\n",sense ? "low":"high");
	}
	else
	{
		printk(KERN_INFO  LIRC_DRIVER_NAME  ": Manually using active "
		       "%s receiver\n",sense ? "low":"high");
	};
	
	return 0;
}

static int set_use_inc(void* data)
{
	int result;
	unsigned long flags;
	
	/* Init read buffer. */
	if (lirc_buffer_init(&rbuf, sizeof(lirc_t), RBUF_LEN) < 0)
		return -ENOMEM;
	
	/* initialize timestamp */
	do_gettimeofday(&lasttv);

	result=request_irq(irq,irq_handler,
			   SA_INTERRUPT | (share_irq ? SA_SHIRQ:0),
			   LIRC_DRIVER_NAME,(void *)&hardware);
	
	switch(result)
	{
	case -EBUSY:
		printk(KERN_ERR LIRC_DRIVER_NAME ": IRQ %d busy\n", irq);
                lirc_buffer_free(&rbuf);
		return -EBUSY;
	case -EINVAL:
		printk(KERN_ERR LIRC_DRIVER_NAME
		       ": Bad irq number or handler\n");
                lirc_buffer_free(&rbuf);
		return -EINVAL;
	default:
		dprintk("Interrupt %d, port %04x obtained\n", irq, io);
		break;
	};

	local_irq_save(flags);
	
	/* Set DLAB 0. */
	soutp(UART_LCR, sinp(UART_LCR) & (~UART_LCR_DLAB));
	
	soutp(UART_IER, sinp(UART_IER)|UART_IER_MSI);
	
	local_irq_restore(flags);
	
	MOD_INC_USE_COUNT;
	return 0;
}

static void set_use_dec(void* data)
{	unsigned long flags;
	
	local_irq_save(flags);
	
	/* Set DLAB 0. */
	soutp(UART_LCR, sinp(UART_LCR) & (~UART_LCR_DLAB));
	
	/* First of all, disable all interrupts */
	soutp(UART_IER, sinp(UART_IER)&
	      (~(UART_IER_MSI|UART_IER_RLSI|UART_IER_THRI|UART_IER_RDI)));
	local_irq_restore(flags);

	free_irq(irq, (void *)&hardware);
	
	dprintk("freed IRQ %d\n", irq);
	lirc_buffer_free(&rbuf);
	
	MOD_DEC_USE_COUNT;
}

static ssize_t lirc_write(struct file *file, const char *buf,
			 size_t n, loff_t * ppos)
{
	int i,count;
	unsigned long flags;
	long delta=0;
	
	if(!(hardware[type].features&LIRC_CAN_SEND_PULSE))
	{
		return(-EBADF);
	}
	
	if(n%sizeof(lirc_t)) return(-EINVAL);
	count=n/sizeof(lirc_t);
	if(count>WBUF_LEN || count%2==0) return(-EINVAL);
	if(copy_from_user(wbuf,buf,n)) return -EFAULT;
	local_irq_save(flags);
	if(type == LIRC_IRDEO)
	{
		/* DTR, RTS down */
		on();
	}
	for(i=0;i<count;i++)
	{
		if(i%2) hardware[type].send_space(wbuf[i]-delta);
		else delta=hardware[type].send_pulse(wbuf[i]);
	}
	off();
	local_irq_restore(flags);
	return(n);
}

static int lirc_ioctl(struct inode *node,struct file *filep,unsigned int cmd,
		      unsigned long arg)
{
        int result;
	unsigned long value;
	unsigned int ivalue;
	
	switch(cmd)
	{
	case LIRC_GET_SEND_MODE:
		if(!(hardware[type].features&LIRC_CAN_SEND_MASK))
		{
			return(-ENOIOCTLCMD);
		}
		
		result=put_user(LIRC_SEND2MODE
				(hardware[type].features&LIRC_CAN_SEND_MASK),
				(unsigned long *) arg);
		if(result) return(result); 
		break;
		
	case LIRC_SET_SEND_MODE:
		if(!(hardware[type].features&LIRC_CAN_SEND_MASK))
		{
			return(-ENOIOCTLCMD);
		}
		
		result=get_user(value,(unsigned long *) arg);
		if(result) return(result);
		/* only LIRC_MODE_PULSE supported */
		if(value!=LIRC_MODE_PULSE) return(-ENOSYS);
		break;
		
	case LIRC_GET_LENGTH:
		return(-ENOSYS);
		break;
		
	case LIRC_SET_SEND_DUTY_CYCLE:
		dprintk("SET_SEND_DUTY_CYCLE\n");
		if(!(hardware[type].features&LIRC_CAN_SET_SEND_DUTY_CYCLE))
		{
			return(-ENOIOCTLCMD);
		}
		
		result=get_user(ivalue,(unsigned int *) arg);
		if(result) return(result);
		if(ivalue<=0 || ivalue>100) return(-EINVAL);
		return init_timing_params(ivalue, freq);
		break;
		
	case LIRC_SET_SEND_CARRIER:
		dprintk("SET_SEND_CARRIER\n");
		if(!(hardware[type].features&LIRC_CAN_SET_SEND_CARRIER))
		{
			return(-ENOIOCTLCMD);
		}
		
		result=get_user(ivalue,(unsigned int *) arg);
		if(result) return(result);
		if(ivalue>500000 || ivalue<20000) return(-EINVAL);
		return init_timing_params(duty_cycle, ivalue);
		break;
		
	default:
		return(-ENOIOCTLCMD);
	}
	return(0);
}

static struct file_operations lirc_fops =
{
	write:   lirc_write,
};

static struct lirc_plugin plugin = {
	name:		LIRC_DRIVER_NAME,
	minor:		-1,
	code_length:	1,
	sample_rate:	0,
	data:		NULL,
	add_to_buf:	NULL,
	get_queue:	NULL,
	rbuf:		&rbuf,
	set_use_inc:	set_use_inc,
	set_use_dec:	set_use_dec,
	ioctl:		lirc_ioctl,
	fops:		&lirc_fops,
	dev:		NULL,
	owner:		THIS_MODULE,
};

#ifdef MODULE

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
static struct platform_device *lirc_serial_dev;

static int __devinit lirc_serial_probe(struct platform_device *dev) {
	return 0;
}

static int __devexit lirc_serial_remove(struct platform_device * dev) {
	return 0;
}

static int lirc_serial_suspend(struct platform_device *dev, pm_message_t state) {
	/* Set DLAB 0. */
	soutp(UART_LCR, sinp(UART_LCR) & (~UART_LCR_DLAB));
	
	/* Disable all interrupts */
	soutp(UART_IER, sinp(UART_IER)&
	      (~(UART_IER_MSI|UART_IER_RLSI|UART_IER_THRI|UART_IER_RDI)));

	/* Clear registers. */
	sinp(UART_LSR);
	sinp(UART_RX);
	sinp(UART_IIR);
	sinp(UART_MSR);

	return 0;
}

static int lirc_serial_resume(struct platform_device *dev) {
	unsigned long flags;

	hardware_init_port();

	local_irq_save(flags);
	/* Enable Interrupt */
	do_gettimeofday(&lasttv);
	soutp(UART_IER, sinp(UART_IER)|UART_IER_MSI);
	off();

	lirc_buffer_clear(&rbuf);

	local_irq_restore(flags);

	return 0;
}

static struct platform_driver lirc_serial_driver = {
	.probe		= lirc_serial_probe,
	.remove	= 	__devexit_p(lirc_serial_remove),
	.suspend	= lirc_serial_suspend,
	.resume		= lirc_serial_resume,
	.driver		= {
		.name	= "lirc_serial",
		.owner	= THIS_MODULE,
	},
};

static int __init lirc_serial_init(void)
{
	int result;

	lirc_serial_dev = platform_device_alloc("lirc_serial", 0);
	if (!lirc_serial_dev)
		return -ENOMEM;
	result = platform_device_add(lirc_serial_dev);
	if (result) {
		platform_device_put(lirc_serial_dev);
		return result;
	}
	result = platform_driver_register(&lirc_serial_driver);
	if (result) {
		printk("lirc register returned %d\n", result);
		platform_device_del(lirc_serial_dev);
		platform_device_put(lirc_serial_dev);
		return result;
	}
	return 0;
}
#endif

int __init init_module(void)
{
	int result;
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
	result = lirc_serial_init();
	if(result) return result;
#endif
	switch(type)
	{
	case LIRC_HOMEBREW:
	case LIRC_IRDEO:
	case LIRC_IRDEO_REMOTE:
	case LIRC_ANIMAX:
	case LIRC_IGOR:
#if defined(LIRC_SERIAL_NSLU2)
	case LIRC_NSLU2:
#endif
		break;
	default:
		return -EINVAL;
	}
	if(!softcarrier)
	{
		switch(type)
		{
		case LIRC_HOMEBREW:
		case LIRC_IGOR:
		case LIRC_NSLU2:
			hardware[type].features&=
				~(LIRC_CAN_SET_SEND_DUTY_CYCLE|
				  LIRC_CAN_SET_SEND_CARRIER);
			break;
		}
	}
	if((result = init_port()) < 0)
	{
		return result;
	}
	plugin.features = hardware[type].features;
	if ((plugin.minor = lirc_register_plugin(&plugin)) < 0) {
		printk(KERN_ERR  LIRC_DRIVER_NAME  
		       ": register_chrdev failed!\n");
		release_region(io, 8);
		return -EIO;
	}
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
static void __exit lirc_serial_exit(void)
{
	struct platform_device *pdev = lirc_serial_dev;
	lirc_serial_dev = NULL;
	platform_driver_unregister(&lirc_serial_driver);
	platform_device_unregister(pdev);
}
#endif

void __exit cleanup_module(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
	lirc_serial_exit();
#endif
#if defined(LIRC_ALLOW_MMAPPED_IO)
	if(iommap != 0)
	{
		release_mem_region(iommap, 8<<ioshift);
	}
	else
	{
		release_region(io, 8);
	}
#else
	release_region(io, 8);
#endif
	lirc_unregister_plugin(plugin.minor);
	dprintk("cleaned up module\n");
}

MODULE_DESCRIPTION("Infra-red receiver driver for serial ports.");
MODULE_AUTHOR("Ralph Metzler, Trent Piepho, Ben Pfaff, Christoph Bartelmus, Andrei Tanas");
MODULE_LICENSE("GPL");

module_param(type, int, 0444);
#if defined(LIRC_SERIAL_NSLU2)
MODULE_PARM_DESC(type, "Hardware type (0 = home-brew, 1 = IRdeo,"
		 " 2 = IRdeo Remote, 3 = AnimaX, 4 = IgorPlug,"
		 " 5 = NSLU2 RX:CTS2/TX:GreenLED"
		 );
#else
MODULE_PARM_DESC(type, "Hardware type (0 = home-brew, 1 = IRdeo,"
		 " 2 = IRdeo Remote, 3 = AnimaX, 4 = IgorPlug"
		 );
#endif

module_param(io, int, 0444);
MODULE_PARM_DESC(io, "I/O address base (0x3f8 or 0x2f8)");

#if defined(LIRC_ALLOW_MMAPPED_IO)
/* some architectures (e.g. intel xscale) have memory mapped registers */
module_param(iommap, bool, 0444);
MODULE_PARM_DESC(iommap, "physical base for memory mapped I/O"
		" (0 = no memory mapped io)");

/* some architectures (e.g. intel xscale) align the 8bit serial registers
   on 32bit word boundaries.
   See linux-kernel/drivers/serial/8250.c serial_in()/out() */
module_param(ioshift, int, 0444);
MODULE_PARM_DESC(ioshift, "shift I/O register offset (0 = no shift)");
#endif

module_param(irq, int, 0444);
MODULE_PARM_DESC(irq, "Interrupt (4 or 3)");

module_param(share_irq, bool, 0444);
MODULE_PARM_DESC(share_irq, "Share interrupts (0 = off, 1 = on)");

module_param(sense, bool, 0444);
MODULE_PARM_DESC(sense, "Override autodetection of IR receiver circuit"
		 " (0 = active high, 1 = active low )");

#ifdef LIRC_SERIAL_TRANSMITTER
module_param(txsense, bool, 0444);
MODULE_PARM_DESC(txsense, "Sense of transmitter circuit"
		 " (0 = active high, 1 = active low )");
#endif

module_param(softcarrier, bool, 0444);
MODULE_PARM_DESC(softcarrier, "Software carrier (0 = off, 1 = on)");

module_param(debug, bool, 0644);
MODULE_PARM_DESC(debug, "Enable debugging messages");

EXPORT_NO_SYMBOLS;

#endif /* MODULE */
