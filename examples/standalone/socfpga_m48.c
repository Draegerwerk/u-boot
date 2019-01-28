/*
 * (C) Copyright 2016
 * Drägerwerk AG & Co. KGaA
 */


/* This file is intended to be loaded as u-boot standalone program
 * into the OCRAM of the SOC
 * When run, it brings the uP2 into a low power state from where it never
 * is able to return.
 */
#include <common.h>
#include <exports.h>
#include <asm/arch/sdram.h>
#include <asm/hardware.h>

int socfpga_m48 (int argc, char * const argv[])
{
	unsigned long old,temp;

	/*deep-sleep SDRAM*/
	*((volatile ulong  *)(SOCFPGA_SDR_ADDRESS + SDR_CTRLGRP_LOWPWREQ_ADDRESS)) = (ulong ) 0x00000007;

	/*disable clocks SDRAMClock Group*/
	*((volatile ulong  *)(SOCFPGA_CLKMGR_ADDRESS + 0xD8)) = (ulong ) 0;

	/*disable clocks Peripheral PLL*/
	*((volatile ulong  *)(SOCFPGA_CLKMGR_ADDRESS + 0xA0)) = (ulong ) 0;

	/*disable clocks Main PLL*/
	*((volatile ulong  *)(SOCFPGA_CLKMGR_ADDRESS + 0x60)) = (ulong ) 0;

	/*shut down VCO*/
	*((volatile ulong  *)(SOCFPGA_CLKMGR_ADDRESS + 0x40)) = (ulong ) 0x80003F05;

	/*disable irq's*/
	__asm__ __volatile__("mrs %0, cpsr\n"
			"orr %1, %0, #0xc0\n"
			"msr cpsr_c, %1"
			: "=r" (old), "=r" (temp)
			  :
			  : "memory");

	while (1) {
		asm("wfi");
	}
	return (0);
}
