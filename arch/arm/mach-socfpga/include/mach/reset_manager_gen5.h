/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  Copyright (C) 2012-2017 Altera Corporation <www.altera.com>
 */

#ifndef _RESET_MANAGER_GEN5_H_
#define _RESET_MANAGER_GEN5_H_

#include <dt-bindings/reset/altr,rst-mgr.h>

void socfpga_bridges_set_handoff_regs(bool h2f, bool lwh2f, bool f2h);
void socfpga_bridges_reset(int enable);

#define RSTMGR_GEN5_STATUS	0x00
#define RSTMGR_GEN5_CTRL	0x04
#define RSTMGR_GEN5_MPUMODRST	0x10
#define RSTMGR_GEN5_PERMODRST	0x14
#define RSTMGR_GEN5_PER2MODRST	0x18
#define RSTMGR_GEN5_BRGMODRST	0x1c
#define RSTMGR_GEN5_MISCMODRST	0x20

#define RSTMGR_CTRL		RSTMGR_GEN5_CTRL

/*
 * SocFPGA Cyclone V/Arria V reset IDs, bank mapping is as follows:
 * 0 ... mpumodrst
 * 1 ... permodrst
 * 2 ... per2modrst
 * 3 ... brgmodrst
 * 4 ... miscmodrst
 */
#define RSTMGR_EMAC0		RSTMGR_DEFINE(1, 0)
#define RSTMGR_EMAC1		RSTMGR_DEFINE(1, 1)
#define RSTMGR_USB0			RSTMGR_DEFINE(1, 2)
#define RSTMGR_USB1			RSTMGR_DEFINE(1, 3)
#define RSTMGR_NAND			RSTMGR_DEFINE(1, 4)
#define RSTMGR_QSPI			RSTMGR_DEFINE(1, 5)
#define RSTMGR_L4WD0		RSTMGR_DEFINE(1, 6)
#define RSTMGR_L4WD1		RSTMGR_DEFINE(1, 7)
#define RSTMGR_OSC1TIMER0	RSTMGR_DEFINE(1, 8)
#define RSTMGR_OSC1TIMER1	RSTMGR_DEFINE(1, 9)
#define RSTMGR_SPTIMER0		RSTMGR_DEFINE(1, 10)
#define RSTMGR_SPTIMER1		RSTMGR_DEFINE(1, 11)
#define RSTMGR_I2C0			RSTMGR_DEFINE(1, 12)
#define RSTMGR_I2C1			RSTMGR_DEFINE(1, 13)
#define RSTMGR_I2C2			RSTMGR_DEFINE(1, 14)
#define RSTMGR_I2C3			RSTMGR_DEFINE(1, 15)
#define RSTMGR_UART0		RSTMGR_DEFINE(1, 16)
#define RSTMGR_UART1		RSTMGR_DEFINE(1, 17)
#define RSTMGR_SPIM0		RSTMGR_DEFINE(1, 18)
#define RSTMGR_SPIM1		RSTMGR_DEFINE(1, 19)
#define RSTMGR_SPIS0		RSTMGR_DEFINE(1, 20)
#define RSTMGR_SPIS1		RSTMGR_DEFINE(1, 21)
#define RSTMGR_SDMMC		RSTMGR_DEFINE(1, 22)
#define RSTMGR_CAN0			RSTMGR_DEFINE(1, 23)
#define RSTMGR_CAN1			RSTMGR_DEFINE(1, 24)
#define RSTMGR_GPIO0		RSTMGR_DEFINE(1, 25)
#define RSTMGR_GPIO1		RSTMGR_DEFINE(1, 26)
#define RSTMGR_GPIO2		RSTMGR_DEFINE(1, 27)
#define RSTMGR_DMA			RSTMGR_DEFINE(1, 28)
#define RSTMGR_SDR			RSTMGR_DEFINE(1, 29)

#endif /* _RESET_MANAGER_GEN5_H_ */
