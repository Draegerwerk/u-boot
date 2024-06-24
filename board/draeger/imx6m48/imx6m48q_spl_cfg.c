/*
 * mx6m48q_spl_cfg.c
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

/* Additional comments refer to several data sheets. They are part of the M48 documentation and stored in the configuration
 * managament system. The following references are used in the project documentation and in this document:
 * [M48_DS_DDR3_MICRON_R] Data Sheet from Micron: Micron.MT41K256M16.4Gb_1_35V_DDR3L.Rev.R.pdf
 * [M48_DS_DDR3_ISSI_I1] Data Sheet from ISSI: ISSI.43-46TR16256A-85120AL.RevI1.pdf
 * [M48_DS_DDR3_ALLIANCE_1.1] Data Sheet from Alliance Memory: Alliance Memory.4Gb_256M16_AS4C256M16D3LB-12BCN_B.Rev1.1.pdf
 * [M48_DS_IMX_RMDQ_5] Reference Manual of i.MX6 Quad: Freescale.IMX6.IMX6DQRM.Reference Manual.Rev5.pdf
 * [M48_DS_IMX_RMDL_4] Reference Manual of i.MX6 DualLite: Freescale.IMX6.IMX6SDLRM.Reference Manual.Rev4.pdf
 * [M48_DS_IMX_DSDQ_5] Data Sheet of i.MX6 Quad: Freescale.IMX6.IMX6DQIEC.Datasheet.Rev5.pdf
 * [M48_DS_IMX_DSDL_8] Data Sheet of i.MX6 DualLite: Freescale.IMX6.IMX6SDLIEC.Data Sheet.Rev8.pdf
 * [M48_DS_DDR3ZQCAL] Technical Note from Micron: Micron.DDR3.ZQ Calibration.TN4102.pdf
 * [M48_DS_IMX_EIB817] Engineering Bulletin from NXP: Freescale.IMX6.EB817.SDCLK Duty Cycle Optimization.pdf
 * [M48_DS_IMX_CONF] Internet discussion: Freescale.IMX6.IOMUX_DCD DRAM configuration confusion_NXP Community.pdf
 * [M48_DS_IMX_SCR_Q] Excel Tool: Freescale.I.MX6DQSDL DDR3 Script Aid V0.11.Q.xlsx
 *
 * Three different DDR3 memories are covered in the comments. The Draeger part number is 1888056 and the manufacturer
 * part numbers are:
 * [M48_DS_DDR3_MICRON_R]: MT41K256M16TW-107:P
 * [M48_DS_DDR3_ISSI_I1]: IS43TR16256A-125KBL
 * [M48_DS_DDR3_ALLIANCE_1.1]: AS4C256M16S3LB-12BCN
 * The manufacturer part numbers are important (especially the speed grade) is important to determine the correct
 * controller settings.
 */

#define CONFIG_MX6Q
#include <common.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6q-ddr.h>
#include "imx6m48_spl_cfg.h"

m48_configuration ddr_setup_mx6q [] =
{
 /***********************************/
 /* IOMUX                           */
 /***********************************/
 {MX6_IOM_DRAM_SDQS0,      0x00000030},
 {MX6_IOM_DRAM_SDQS1,      0x00000030},
 {MX6_IOM_DRAM_SDQS2,      0x00000030},
 {MX6_IOM_DRAM_SDQS3,      0x00000030},
 {MX6_IOM_DRAM_SDQS4,      0x00000030},
 {MX6_IOM_DRAM_SDQS5,      0x00000030},
 {MX6_IOM_DRAM_SDQS6,      0x00000030},
 {MX6_IOM_DRAM_SDQS7,      0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0P p. 2398 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 40 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT, read only in DL, write as 0 (disabled)in DQ
  *    |||| |||| |||| |||| |||| +-------------- write as 0
  *    |||| |||| |||| |||| |||+ --------------- PKE Pull/Keeper Disabled
  *    |||| |||| |||| |||| ||+----------------- PUE Keeper (don't care because PKE=0)
  *    |||| |||| |||| |||| ++------------------ PUS 100k PD (don't care because PKE=0)
  *    ++++ ++++ ++++ ++++--------------------- HYS, DDR_INPUT, DDR_SEL write all as 0
 */

 {MX6_IOM_GRP_B0DS,        0x00000030},
 {MX6_IOM_GRP_B1DS,        0x00000030},
 {MX6_IOM_GRP_B2DS,        0x00000030},
 {MX6_IOM_GRP_B3DS,        0x00000030},
 {MX6_IOM_GRP_B4DS,        0x00000030},
 {MX6_IOM_GRP_B5DS,        0x00000030},
 {MX6_IOM_GRP_B6DS,        0x00000030},
 {MX6_IOM_GRP_B7DS,        0x00000030},
 {MX6_IOM_GRP_ADDDS,       0x00000030},
 {MX6_IOM_GRP_CTLDS,       0x00000030},
 /* IOMUXC_SW_PAD_CTL_GRP_B0DS p. 2585 in [M48_DS_IMX_RMDQ_5]
  * IOMUXC_SW_PAD_CTL_GRP_ADDDS p. 2574 in [M48_DS_IMX_RMDQ_5]
  * IOMUXC_SW_PAD_CTL_GRP_CTLDS p. 2586 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 40 Ohm. Value determined with oscilloscope
  *    ++++ ++++ ++++ ++++ ++++ ++++ ++-------- write as 0
  *
  */

 {MX6_IOM_DRAM_DQM0,       0x00000030},
 {MX6_IOM_DRAM_DQM1,       0x00000030},
 {MX6_IOM_DRAM_DQM2,       0x00000030},
 {MX6_IOM_DRAM_DQM3,       0x00000030},
 {MX6_IOM_DRAM_DQM4,       0x00000030},
 {MX6_IOM_DRAM_DQM5,       0x00000030},
 {MX6_IOM_DRAM_DQM6,       0x00000030},
 {MX6_IOM_DRAM_DQM7,       0x00000030},

 {MX6_IOM_DRAM_CAS,        0x00000030},
 {MX6_IOM_DRAM_RAS,        0x00000030},
 {MX6_IOM_DRAM_SDCLK_0,    0x00000030},
 {MX6_IOM_DRAM_SDCLK_1,    0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0 p. 2400 in [M48_DS_IMX_RMDQ_5]
  * IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS_B p. 2368 in [M48_DS_IMX_RMDQ_5]
  * IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS_B p. 2374 in [M48_DS_IMX_RMDQ_5]
  * IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK0_P p. 2382 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 40 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 1000 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: The pins above are outputs of the i.MX6. It is unclear why input characteristics can be set for these pins.
  * Settings correspond with [M48_DS_IMX_SCR_Q].
 */

 {MX6_IOM_DRAM_RESET,      0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET p. 2376 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 40 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    |||| |||| |||| ++----------------------- DDR_SEL DDR3_LPDDR2 mode
  *    ++++ ++++ ++++ ------------------------- write as 0
  * Note: DRAM_RESET is an output of the i.MX6. It is unclear why input characteristics can be set for these pins.
  * Settings correspond with [M48_DS_IMX_SCR_Q].
 */

 {MX6_IOM_DRAM_SDCKE0,     0x00020000},
 {MX6_IOM_DRAM_SDCKE1,     0x00020000},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCKE0 p. 2386 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    2    0    0    0    0
  * 0b 0000 0000 0000 0010 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| ++++ ++++- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT differential input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: SDCKE0 is an output of the i.MX6. It is unclear why input characteristics can be set for these pins.
  * Settings correspond with [M48_DS_IMX_SCR_Q].
 */

 {MX6_IOM_DRAM_SDODT0,     0x00000030},
 {MX6_IOM_DRAM_SDODT1,     0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_ODT0 p. 2392 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 40 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: SDODT0 is an output of the i.MX6. It is unclear why input characteristics can be set for these pins.
  * Settings correspond with [M48_DS_IMX_SCR_Q].
 */

 {MX6_IOM_DDRMODE_CTL,     0x00020000},
 {MX6_IOM_GRP_DDRMODE,     0x00020000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL p. 2575 in [M48_DS_IMX_RMDQ_5]
  * IOMUXC_SW_PAD_CTL_GRP_DDRMODE p. 2582 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    2    0    0    0    0
  * 0b 0000 0000 0000 0010 0000 0000 0000 0000
  *    |||| |||| |||| |||+ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ||+---------------------- DDR_INPUT differential input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
 * Note: DDR_INPUT mode should be a don't care but sources in [M48_DS_IMX_CONF] say that the preferred setting is differential mode
  * Settings correspond with [M48_DS_IMX_SCR_Q].
  */

 {MX6_IOM_GRP_DDRPKE,      0x00000000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRPKE p. 2577 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| ++++ ++++ ++++- write as 0
  *    |||| |||| |||| |||| |||+---------------- PKE Pull/Keeper disabled
  *    ++++ ++++ ++++ ++++ +++----------------- write as 0
  * Settings correspond with [M48_DS_IMX_SCR_Q].
 */

 {MX6_IOM_DRAM_SDBA0,      0x00000000},
 {MX6_IOM_DRAM_SDBA1,      0x00000000},
 {MX6_IOM_DRAM_SDBA2,      0x00000000},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA0 p. 2378 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| ++++ ++++- write as 0
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 1000 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
 */

 {MX6_IOM_GRP_DDR_TYPE,    0x000C0000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE p. 2589 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    C    0    0    0    0
  * 0b 0000 0000 0000 1100 0000 0000 0000 0000
  *    |||| |||| |||| ||++ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ++----------------------- DDR_SEL DDR3 mode
  *    ++++ ++++ ++++-------------------------- write as 0
 */

 /***********************************/
 /* DDR Controller                  */
 /***********************************/

 /* MDSCR    configuration request */
 {MX6_MMDC_P0_MDSCR,       0x00008000},
 /* MMDCx_MDSCR p. 3878 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    8    0    0    0
  * 0b 0000 0000 0000 0000 1000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA bank address 0, should be don't care because of CMD=0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS CS0 used, should be don't care because of CMD=0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD 0: Normal operation
  *    |||| |||| |||| |||| |||| |||+ +--------- write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID read only, write as 0
  *    |||| |||| |||| |||| ||++ +-------------- write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ request to configure MMDC
  *    |||| |||| |||| ||||                      According to bullet 1 on p. 3892 in [M48_DS_IMX_RMDL_4] and
  *    |||| |||| |||| ||||                      p. 3794 in [M48_DS_IMX_RMDQ_5], there is no need to check for CON_ACK
  *    |||| |||| |||| ||||                      in the start up situation here.
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR 0, should be don't care because of CMD=0
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP 0, should be don't care because of CMD=0
  * Note: The user manuals of the i.MX6 show a block diagram of the MMDC with channel 0 and 1 but the registers are
  * numbered MMDC1_ and MMDC2_. We stay with the 0/1 scheme for the register names in this file because that is the
  * way it originally was done in Uboot.
  * We use DDR3 x64 configuration, so it should be enough to use only the register of channel 0. [M48_DS_IMX_SCR_Q] also
  * only uses the MMDC1_MDSCR and not MMDC2_MDSCR.
 */

 {MX6_MMDC_P0_MPZQHWCTRL,  0xA139001F},
 {MX6_MMDC_P1_MPZQHWCTRL,  0xA139001F},
 /* MMDCx_MPZQHWCTRL p. 3906 in [M48_DS_IMX_RMDQ_5]
  * 0x    A    1    3    9    0    0    1    F
  * 0b 1010 0001 0011 1001 0000 0000 0001 1111
  *    |||| |||| |||| |||| |||| |||| |||| ||++- ZQ_MODE 3:periodically and ZQ command and exit of self refresh
  *    |||| |||| |||| |||| |||| |||| ||++ ++--- ZQ_HW_PER ZQ cal every 128ms
  *    |||| |||| |||| |||| |||| |+++ ++-------- ZQ_HW_PU_RES read only, write as 0
  *    |||| |||| |||| |||| ++++ +-------------- ZQ_HW_PD_RES read only, write as 0
  *    |||| |||| |||| |||+--------------------- ZQ_HW_FOR force ZQ automatic calibration (ZQ_MODE must be 1 or 3)
  *    |||| |||| |||| +++---------------------- TZQ_INIT 512 cycles, JEDEC default for DDR3
  *    |||| |||| |+++-------------------------- TZQ_OPER 256 cycles, JEDEC default for DDR3
  *    |||| ||++ +----------------------------- TZQ_CS 128 cycles, default
  *    |||| |+--------------------------------- write as 0
  *    ++++ +---------------------------------- ZQ_EARLY_COMPARATOR_EN_TIMER 0x14: 21 cycles, default
  */
};
int ddr_setup_mx6q_size = ARRAY_SIZE(ddr_setup_mx6q);


m48_configuration ram_setup_mx6q [] =
{
 {MX6_MMDC_P0_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY3DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY3DL, 0x33333333},
 /* MMDCx_MPRDDQBY0DL p. 3922 in [M48_DS_IMX_RMDQ_5]
  * 0x    3    3    3    3    3    3    3    3
  * 0b 0011 0011 0011 0011 0011 0011 0011 0011
  *    |... |... |... |... |... |... |... |+++- rd_dqx_del 3: for all eight nibbles: Add 3 delay units. This is the
  *                                             recommended value even though the reset value is 0. [M48_DS_IMX_SCR_Q]
  */

  {0x021b08c0, 0x24921492}, /* MMDC1_MPDCCR */
  {0x021b48c0, 0x24921492}, /* MMDC2_MPDCCR */
  /* MMDCx_MPRDDQBY0DL p. 4069 in [M48_DS_IMX_RMDL_4], p. 3977 in [M48_DS_IMX_RMDQ_5]
   * 0x    2    4    9    2    1    4    9    2
   * 0b 0010 0100 1001 0010 0001 0100 1001 0010
   *    |||| |||| |||| |||| |||| |||| |||| |+++- WR_DQS0_FT_DCC: 2: 50% duty cycle
   *    |||| |||| |||| |||| |||| |||| ||++ +---- WR_DQS1_FT_DCC: 2: 50% duty cycle
   *    |||| |||| |||| |||| |||| |||+ ++-------- WR_DQS2_FT_DCC: 2: 50% duty cycle
   *    |||| |||| |||| |||| |||| +++------------ WR_DQS3_FT_DCC: 2: 50% duty cycle
   *    |||| |||| |||| |||| |+++---------------- WR_FT0_DCC: 1: 53% low, 47% high
   *    |||| |||| |||| |||| +------------------- reserved, write as 0
   *    |||| |||| |||| |+++--------------------- CK_FT1_DCC: 2: 50% duty cycle
   *    |||| |||| ||++ +------------------------ RD_DQS0_FT_DCC: 2: 50% duty cycle
   *    |||| |||+ ++---------------------------- RD_DQS1_FT_DCC: 2: 50% duty cycle
   *    |||| +++-------------------------------- RD_DQS2_FT_DCC: 2: 50% duty cycle
   *    |+++------------------------------------ RD_DQS3_FT_DCC: 2: 50% duty cycle
   *    +--------------------------------------- reserved, write as 0
   * Note: See [M48_DS_IMX_EIB817] for information about the preferred setting of these registers.
   * Note: Another note was found in the Uboot documentation: "A recent recommendation was added for i.mx6DQ Rev C or
   * later Devices. This (the change to 0x24921492) has been seen to improve measured duty cycle by setting fine tune
   * SDCLK duty cyc to low. If you are not experiencing issues, you could leave this commented out. (added by NXP)"
   */






  /* Times in the DDR3 RAMs are measured in clocks, CK. The assumed clock period is depending on the speed grade of the RAM
   * 1 CK of [M48_DS_DDR3_MICRON_R] is 1.07ns long. Speed bin 1866, 933MHz max. clock
   * 1 CK of [M48_DS_DDR3_ISSI_I1] is 1.25ns long. Speed bin 1600, 800MHz max. clock
   * 1 CK of [M48_DS_DDR3_ALLIANCE_1.1] 1.25ns long. Speed bin 1600, 800MHz max. clock
   * The times which are expressed in CK in the RAM data sheets need to be converted into clocks of the i.MX6 memory interface
   * which runs at 528MHz. So i.MX6 clocks are CK * [1.07 | 1.25] / 1.894 depending on the RAM.
   */

 {MX6_MMDC_P0_MDPDC,       0x00020036},
 /* MMDCx_MDPDC p. 3864 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    2    0    0    3    6
  * 0b 0000 0000 0000 0010 0000 0000 0011 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: max of 5CK or 10ns. 6 * 1.894ns is > 10ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 97
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: max of 5CK or 10ns. 6 * 1.894ns is > 10ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 87
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 0: independent. Only one CS is used anyway.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of DDR3
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 0: No Power Down Time-out for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 0: No Power Down Time-out for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2: 3 cycles: max of 3CK or 5ns. 3 * 1.894ns is > 5ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 97
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: 0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: 0: Disabled
  *    +--------------------------------------- reserved, write as 0
  * Note: PWDT_x: Why no power down time out? MDPDC is written again at the end of the initialisation. A timeout is set
  *       in this last write.
  */

 {MX6_MMDC_P0_MDCFG0,      0x898E7955},
 /* MMDCx_MDCFG0 p. 3869 in [M48_DS_IMX_RMDQ_5]
  * 0x    8    9    8    E    7    9    5    5
  * 0b 1000 1001 1000 1110 0111 1001 0101 0101
  *    |||| |||| |||| |||| |||| |||| |||| ++++- tCL: 5: 8 clocks: 8 * 1.894ns > 13.91ns. Must match with MR0 of DDR3
  *                                             [M48_DS_DDR3_MICRON_R] p. 1: 13.91ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 1: 13.75ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 2: 13.75ns
  *    |||| |||| |||| |||| |||| |||+ ++++------ tFAW: 0x15: 22 clocks: 22 * 1.894ns > 40ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 96, 2k page size: 35ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64, 2k page size: 40ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 34, 2k page size: 40ns
  *    |||| |||| |||| |||| |||+ +++------------ tXPDLL: 0xC: 13 clocks: max of 10CK or 24ns. 13 * 1.894ns is > 24ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 98
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 36
  *    |||| |||| |||| |||| +++----------------- tXP: 3: 4 cycles: max of 3CK or 6ns. 4 * 1.894 is > 6ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 98
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 36
  *    |||| |||| ++++ ++++--------------------- tXS: 0x8E: 143 clocks: max of 5CK or 270ns. 143 * 1.894ns is > 270ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 97:  tRFC + 10ns = 270ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64: tRFC + 10ns = 270ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35: tRFC + 10ns = 270ns
  *    ++++ ++++------------------------------- tRFC: 0x89, 138 clocks: 260ns. 138 * 1.894ns is > 260ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 33: 243 * 1.07ns = 260ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 55
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  */

 {MX6_MMDC_P0_MDCFG1,      0xFF328F64},
 /* MMDCx_MDCFG1 p. 3870 in [M48_DS_IMX_RMDQ_5]
  * 0x    F    F    3    2    8    F    6    4
  * 0b 1111 1111 0011 0010 1000 1111 0110 0100
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCWL: 4, 6 clocks: tCK = 1.894ns. Must match with MR2 of DDR3
  *                                             [M48_DS_DDR3_MICRON_R] p. 78: CL=7, CWL=6
  *                                             [M48_DS_DDR3_ISSI_I1] p. 56: CL=7, CWL=6
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: CL=7, CWL=6
  *    |||| |||| |||| |||| |||| |||| |||+ +---- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ +++------- tMRD: 0xB, 12 clocks: max of 12CK or 15ns. 12 * 1.894ns is > 15ns
  *                                             Set to max(tMRD, tMOD). tMOD is higher for all memories
  *                                             [M48_DS_DDR3_MICRON_R] p. 96: tMOD max(12CK, 15ns)
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64: tMOD max(12CK, 15ns)
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35: tMOD max(12CK, 15ns)
  *    |||| |||| |||| |||| |||| +++------------ tWR: 7, 8 clocks: 15ns. 8 * 1.894ns > 15ns. Must match with MR0 of DDR3
  *                                             [M48_DS_DDR3_MICRON_R] p. 96
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0x8, see Note.
  *    |||| |||| |||+ ++++--------------------- tRAS: 0x12, 19 clocks: 19 * 1.894ns > 35ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 81: 34ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 35ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 35ns
  *    |||| ||++ +++--------------------------- tRC: 0x19, 26 clocks: 26 * 1.894ns > 48.75ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 81: 47.91ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 48.75ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 48.75ns
  *    |||+ ++--------------------------------- tRP: 0x7, 8 clocks: 8 * 1.894ns > 13.91ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 81: 13.91ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 13.75ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 13.75ns
  *    +++------------------------------------- tRCD: 0x7, 8 clocks: 8 * 1.894ns > 13.91ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 81: 13.91ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 13.75ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 13.75ns
  * Note: [M48_DS_IMX_RMDQ_5] states a reset value of 0x8, but says in the table that it always has the value of 0. The
  * Dual Lite version writes a 0x8. A setting of 0xFF328F64 like in [M48_DS_IMX_SCR_Q] is desired.
  */

 {MX6_MMDC_P0_MDCFG2,      0x01FF00DB},
 /* MMDCx_MDCFG2 p. 3873 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    1    F    F    0    0    D    B
  * 0b 0000 0001 1111 1111 0000 0000 1101 1011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tRRD: 3, 4 clocks: max of 4CK or 7.5ns. 4 * 1.894ns > 7.5ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 96: max of 4CK or 6ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64: max of 4CK or 6ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 34: max of 4CK or 7.5ns
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tWTR: 3, 4 clocks: max of 4CK or 7.5ns. 4 * 1.894ns > 7.5ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 96: max of 4CK or 7.5ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64: max of 4CK or 7.5ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35: max of 4CK or 7.5ns
  *    |||| |||| |||| |||| |||| |||+ ++-------- tRTP: 3, 4 clocks: max of 4CK or 7.5ns. 4 * 1.894ns > 7.5ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 96: max of 4CK or 7.5ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64: max of 4CK or 7.5ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35: max of 4CK or 7.5ns
  *    |||| |||| |||| |||| ++++ +++------------ reserved, write as 0
  *    |||| |||+ ++++ ++++--------------------- tDLLK: 0x1FF, 512 clocks: default
  *                                             [M48_DS_DDR3_MICRON_R] p. 95
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    ++++ +++-------------------------------- reserved, write as 0
  * Note: tDLLK is given as 512CK in the memory data sheet, so we could use a lower value. But this is one time delay which doesn't
  * really affect RAM speed.
  */

 {MX6_MMDC_P0_MDOTC,       0x24444040},
 /* MMDCx_MDOTC p. 3867 in [M48_DS_IMX_RMDQ_5]
  * 0x    2    4    4    4    4    0    4    0
  * 0b 0010 0100 0100 0100 0100 0000 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| ++++- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ ++++ ----- tODT_idle_off: 4: 4 clocks: unclear, see note.
  *                                             [M48_DS_DDR3_MICRON_R]
  *                                             [M48_DS_DDR3_ISSI_I1]
  *                                             [M48_DS_DDR3_ALLIANCE_1.1]
  *    |||| |||| |||| |||| |||| +++------------ reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- tODTLon: 4: 4 clocks:
  *                                             [M48_DS_DDR3_MICRON_R] p. 98: CWL + AL - 2CK = 6 + 0 - 2 = 4
  *                                             [M48_DS_DDR3_ISSI_I1] p. unknown
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. unknown
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| ++++--------------------- tAXPD: 4: 5 clocks: CWL - 1 = 6 - 1 = 5
  *                                             [M48_DS_IMX_RMDQ_5] p. 3868
  *    |||| |||| ++++-------------------------- tANPD: 3, 4 clocks: CWL - 1 = 6 - 1 = 5
  *                                             [M48_DS_IMX_RMDQ_5] p. 3868
  *    |||| |+++------------------------------- tAONPD: 4, 5 clocks: 5 * 1.894ns > 8.5ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 198: 8.5ns max
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65: 8.5ns max
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 36: 8.5ns max
  *    ||++ +---------------------------------- tAOFPD: 4, 5 clocks: 5 * 1.894ns > 8.5ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 198: 8.5ns max
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65: 8.5ns max
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 36: 8.5ns max
  *    ++-------------------------------------- reserved, write as 0
  * Note: unclear how tODT_idle_off is to be determined.
  * Note: According to [M48_DS_IMX_SCR_Q] a value of 0x24444040 shall be used.
  */

 {MX6_MMDC_P0_MDRWD,       0x000026D2},
 /* MMDCx_MDRWD p. 3883 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    2    6    D    2
  * 0b 0000 0000 0000 0000 0010 0110 1101 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- RTR_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||| ||++ +---- RTW_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||+ ++-------- WTW_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| +++------------ WTR_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |+++---------------- RTW_SAME: 2, 2 clocks (default):
  *                                             [M48_DS_DDR3_MICRON_R] p. 164: Read to Write timing RTW is RL + tCCD - WL + 2tCK
  *                                             RL is AL + CL and WL is AL + CWL, so RTW = AL + CL + tCCD - CWL - AL + 2tCK
  *                                             = 0 + 8 + 4 - 6 - 0 + 2 = 8
  *                                             [M48_DS_DDR3_ISSI_I1] p.
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p.
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||+ ++++ ++++ ++++--------------------- tDAI: 0, 1 clocks: only relevant for LPDDR2
  *    +++------------------------------------- reserved, write as 0
  * Note: RTW_SAME might be too big, especially with RALAT=5. However it is a safe value and [M48_DS_IMX_SCR_Q] also uses the same.
  */

 {MX6_MMDC_P0_MDMISC,         0x0011740},
 /* MMDCx_MDMISC p. 3874 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    1    1    7    4    0
  * 0b 0000 0000 0000 0000 0001 0111 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- RST: 0: No Reset requested
  *    |||| |||| |||| |||| |||| |||| |||| |+----LPDDR2_2CH: 0: 1 channel mode (DDR3)
  *    |||| |||| |||| |||| |||| |||| |||+ +---- DDR_TYPE: 0: DDR3 is used
  *    |||| |||| |||| |||| |||| |||| ||+------- DDR_4_BANK: 0: 8 banks device is used.
  *    |||| |||| |||| |||| |||| |||+ ++-------- RALAT: 5, 5 clocks
  *    |||| |||| |||| |||| |||| |++- ---------- MIF3_MODE: 3: prediction on all possibilities
  *    |||| |||| |||| |||| |||| +-------------- LPDDR2_S2: 0: Should be cleared in DDR3 mode
  *    |||| |||| |||| |||| |||+---------------- BI_ON: 1: Banks are interleaved.
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- WALAT: 1, 1 clock additional delay
  *    |||| |||| |||| |+----------------------- LHD: 0: Latency hiding on, normal mode, debug feature off
  *    |||| |||| |||| +------------------------ ADDR_MIRROR: 0: Address mirroring disabled.
  *    |||| |||| |||+-------------------------- CALIB_PER_CS: 0: Calibration is targeted to CS0
  *    ||++ ++++ +++--------------------------- reserved, write as 0
  *    |+-------------------------------------- CS1_RDY: read only, write as 0
  *    +--------------------------------------- CS0_RDY: read only, write as 0
  * Note: RALAT is 5 clocks which corresponds to the output of [M48_DS_IMX_SCR_Q]. It might be possible to be faster because
  * we have a short distance between uC and DDR3.
  * Note: WALAT is 1 to be on the safe side in regard to WL_DL_ABS_OFFSETn. See comment in [M48_DS_IMX_RMQ_5] for WALAT.
  * [M48_DS_IMX_SCR_Q] also gives WALAT = 1.
  */

 {MX6_MMDC_P0_MDOR,        0x008E1023},
 /* MMDCx_MDOR p. 3885 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    8    E    1    0    2    3
  * 0b 0000 0000 1000 1110 0001 0000 0010 0011
  *    |||| |||| |||| |||| |||| |||| ||++ ++++- RST_to_CKE: 35, 33 clocks. 33 * 15.258us > 500us
  *    |||| |||| |||| |||| |||| |||| ++-------- reserved, write as 0
  *    |||| |||| |||| |||| ||++ ++++----------- SDE_to_RST: 16, 14 clocks. 14 * 15.258us > 200us
  *    |||| |||| |||| |||| ++------------------ reserved, write as 0
  *    |||| |||| ++++ ++++--------------------- tXPR: 0x8E, 143 clocks. 143 * 1.894ns > 270 ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 86: max of 5CK or tRFC + 10ns
  *                                             [M48_DS_DDR3_ISSI_I1] p. 64: max of 5CK or tRFC + 10ns
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35: max of 5CK or tRFC + 10ns
  *    ++++ ++++------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDASP,       0x00000047},
 /* MMDCx_MDASP p. 3891 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    4    7
  * 0b 0000 0000 0000 0000 0000 0000 0100 0111
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- CS0_END: 71 = 64 + 7 = DDR_CS_SIZE/32MB + 7.
  *    ++++ ++++ ++++ ++++ ++++ ++++ +--------- reserved, write as 0
  */

 /* configure density and burst length */
 {MX6_MMDC_P0_MDCTL,       0x041A0000},
 /* MMDCx_MDCTL p. 3862 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    4    1    A    0    0    0    0
  * 0b 0000 0100 0001 1010 0000 0000 0000 0000
  *    |||| |||| |||| |||| ++++ ++++ ++++ ++++- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- DSIZ: 2: 64-bit data bus
  *    |||| |||| |||| |+----------------------- reserved, write as 0
  *    |||| |||| |||| +------------------------ BL: 1: Burst length 8
  *    |||| |||| |+++ ------------------------- COL: 1: 10 bits column
  *                                             [M48_DS_DDR3_MICRON_R] p. 2 table 2
  *                                             [M48_DS_DDR3_ISSI_I1] p. 1 address table
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 2 Column address A0 to A9
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| |+++------------------------------- ROW: 4: 15 bits row
  *                                             [M48_DS_DDR3_MICRON_R] p. 2 table 2
  *                                             [M48_DS_DDR3_ISSI_I1] p. 1 address table
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 2 Row address A0 to A9
  *    ||++ +---------------------------------- reserved, write as 0
  *    |+-------------------------------------- SDE_1: 0: CS1 disabled
  *    +--------------------------------------- SDE_0: 0: CS0 disabled
  */

 /* start ZQ calibration */
 {MX6_MMDC_P0_MDCTL,       0x841A0000},
 /* MMDCx_MDCTL p. 3862 in [M48_DS_IMX_RMDQ_5]
  * 0x    8    4    1    A    0    0    0    0
  * 0b 1000 0100 0001 1010 0000 0000 0000 0000
  *    |||| |||| |||| |||| ++++ ++++ ++++ ++++- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- DSIZ: 2: 64-bit data bus
  *    |||| |||| |||| |+----------------------- reserved, write as 0
  *    |||| |||| |||| +------------------------ BL: 1: Burst length 8
  *    |||| |||| |+++ ------------------------- COL: 1: 10 bits column
  *                                             [M48_DS_DDR3_MICRON_R] p. 2 table 2
  *                                             [M48_DS_DDR3_ISSI_I1] p. 1 address table
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 2 Column address A0 to A9
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| |+++------------------------------- ROW: 4: 15 bits row
  *                                             [M48_DS_DDR3_MICRON_R] p. 2 table 2
  *                                             [M48_DS_DDR3_ISSI_I1] p. 1 address table
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 2 Row address A0 to A9
  *    ||++ +---------------------------------- reserved, write as 0
  *    |+-------------------------------------- SDE_1: 0: CS1 disabled
  *    +--------------------------------------- SDE_0: 1: CS0 enabled
  * Note: It might be unnecessary to do the write to MMDCx_MDCTL twice with only SDE_0 changed.
  */

 /* DDR3 MR2: */
 {MX6_MMDC_P0_MDSCR,       0x04088032},
 /* MMDCx_MDSCR p. 3878 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    4    0    8    8    0    3    2
  * 0b 0000 0100 0000 1000 1000 0000 0011 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 2: Bank address 2
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register Command
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 8: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 4: MSB of Command/Address
  *                                             write 0x0408 to MR2 (BA=2) of DDR3:
  *    |||| |||| |||| |\\\---------------------- reserved, write as 0
  *    |||| |||| ||\\ \------------------------- CWL: 1: 6CK. Must match with tCWL in MMDCx_MDCFG1.
  *    |||| |||| |\----------------------------- ASR: 0: Auto Self Refresh disabled
  *    |||| |||| \------------------------------ SRT: 0: Self Refresh Temperature normal
  *    |||| |||\-------------------------------- reserved, write as 0
  *    |||| |\\--------------------------------- RTT(WR): 2: RZQ/2 (120Ohm)
  *    \\\\ \----------------------------------- reserved, write as 0
  * Note: Differences to [M48_DS_IMX_SCR_Q] are because of different RTT settings.
  */

 /* DDR3 MR3: */
 {MX6_MMDC_P0_MDSCR,       0x00008033},
 /* MMDCx_MDSCR p. 3878 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    8    0    3    3
  * 0b 0000 0000 0000 0000 1000 0000 0011 0011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 3: Bank address 3
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register Command
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  *                                             write 0x0000 to MR3 (BA=3) of DDR3:
  *    |||| |||| |||| ||\\---------------------- MPR READ function: 0: Predefined Pattern
  *    |||| |||| |||| |\------------------------ MPR Enable: 0: Normal DRAM operation
  *    \\\\ \\\\ \\\\ \------------------------- reserved, write as 0
  */

 /* DDR3 MR1: */
 {MX6_MMDC_P0_MDSCR,       0x00428031},
 /* MMDCx_MDSCR p. 3878 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    4    2    8    0    3    1
  * 0b 0000 0000 0100 0010 1000 0000 0011 0001
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 1: Bank address 1
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register Command
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x42: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  *                                             write 0x0042 to MR1 (BA=1) of DDR3:
  *    |||| |||| |||| |||\---------------------- DLL Enable: 0: Enable (normal)
  *    |||| |||| ||\- --\----------------------- ODS: 1: RZQ/7 (34Ohm)
  *    |||| ||\- -\-- -\------------------------ RTT: 2: RZQ/2 (120Ohm)
  *    |||| |||| |||\ \------------------------- AL: 0: Disabled (AL=0)
  *    |||| |||| \------------------------------ WL: 0: Disabled (normal)
  *    |||| |\|\-------------------------------- reserved, write as 0
  *    |||| \----------------------------------- TDQS: 0: Disabled
  *    |||\------------------------------------- Q Off: 0: Enabled
  *    \\\-------------------------------------- reserved, write as 0
  * Note: Differences to [M48_DS_IMX_SCR_Q] are because of different RTT and ODS settings.
  */

 /* DDR3 MR0: */
 {MX6_MMDC_P0_MDSCR,       0x19408030},
 /* MMDCx_MDSCR p. 3878 in [M48_DS_IMX_RMDQ_5]
  * 0x    1    9    4    0    8    0    3    0
  * 0b 0001 1001 0100 0000 1000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register Command
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x40: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x19: MSB of Command/Address
  *                                             write 0x1940 to MR0 (BA=0) of DDR3:
  *    |||| |||| |||| ||\\---------------------- BL: 0: Fixed BL8
  *    |||| |||| |\\\ -\------------------------ CAS Latency: 8: CL=8. Must match with tCL in MMDCx_MDCFG0.
  *    |||| |||| |||| \------------------------- BT: 0: Burst Type Sequential
  *    |||| |||| \------------------------------ reserved, write as 0
  *    |||| |||\-------------------------------- DLL: 1: DLL Reset: Yes
  *    |||| \\\--------------------------------- WR: 4: Write Recovery 8. Must match with setting in MMDCx_MDCFG1
  *    |||\------------------------------------- PD: 1: DLL on (fast exit). Must match with SLOW_PD in MMDCx_MDPDC.
  *    \\\-------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDSCR,       0x04008040},
 /* MMDCx_MDSCR p. 3878 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    4    0    0    8    0    4    0
  * 0b 0000 0100 0000 0000 1000 0000 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 4: ZQ Calibration
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x00: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x04: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDPDC,       0x00025576},
 /* MMDCx_MDPDC p. 3864 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    2    5    5    7    6
  * 0b 0000 0000 0000 0010 0101 0101 0111 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: max of 5CK or 10ns. 6 * 1.894ns is > 10ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 97
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: max of 5CK or 10ns. 6 * 1.894ns is > 10ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 87
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 1: both chips selects idle.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of DDR3
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 5: 256 cycles for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 5: 256 cycles for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2, 3 cycles: max of 3CK or 5ns. 3 * 1.894ns is > 5ns
  *                                             [M48_DS_DDR3_MICRON_R] p. 97
  *                                             [M48_DS_DDR3_ISSI_I1] p. 65
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: Disabled
  *    +--------------------------------------- reserved, write as 0
  * Note: BOTH_CS_PD: Since we are using only one CS, this setting might be wrong here. [M48_DS_IMX_SCR_Q] also gives 0x00025576
  */

 {MX6_MMDC_P0_MAPSR,       0x00001006},
 /* MMDCx_MAPSR p. 3894 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    1    0    0    6
  * 0b 0000 0000 0000 0000 0001 0000 0000 0110
  *    |||| |||| |||| |||| |||| |||| |||| |||+- PSD: 0: Power saving enabled
  *    |||| |||| |||| |||| |||| |||| |||| +++-- reserved, write as 3
  *    |||| |||| |||| |||| |||| |||| |||+------ PSS: read only, write as 0
  *    |||| |||| |||| |||| |||| |||| ||+------- RIS: read only, write as 0
  *    |||| |||| |||| |||| |||| |||| |+-------- WIS: read only, write as 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| ++++ ++++----------- PST: 16: 1024 cycles
  *    |||| |||| |||| ++++--------------------- reserved, write as 0
  *    |||| |||| |||+-------------------------- LPMD: no lpmd request
  *    |||| |||| ||+--------------------------- DVFS: no DVFS/self refresh request
  *    |||| |||| ++---------------------------- reserved, write as 0
  *    |||| |||+------------------------------- LPACK: read only, write as 0
  *    |||| ||+-------------------------------- DVACK: read only, write as 0
  *    ++++ ++--------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDREF,       0x00001800},
 /* MMDCx_MDREF p. 3881 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    1    8    0    0
  * 0b 0000 0000 0000 0000 0001 1000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- START_REF: 0: Do not start a manual refresh cycle
  *    |||| |||| |||| |||| |||| |+++ ++++ +++-- reserved, write as 0
  *    |||| |||| |||| |||| ||++ +-------------- REFR: 3: 4 refreshes every cycle
  *    |||| |||| |||| |||| ++------------------ REF_SEL: 0: Periodic refresh cycles with 64kHz.
  *                                             4 refreshes every 1/64kHz seconds means a resulting refresh rate of
  *                                             15.625us / 4 = 3.906us. This is the necessary value for DDR3 operation up to
  *                                             a temperature of 95 C.
  *                                             [M48_DS_DDR3_MICRON_R] p. 97
  *                                             [M48_DS_DDR3_ISSI_I1] p. 55
  *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
  *    ++++ ++++ ++++ ++++--------------------- REF_CNT: 0: reserved
  * Note: REF_CNT with value 0 is reserved, but also reset value.
  */

 {MX6_MMDC_P0_MPODTCTRL,   0x00022227},
 {MX6_MMDC_P1_MPODTCTRL,   0x00022227},
 /* MMDCx_MPODTCTRL p. 3920 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    2    2    2    2    7
  * 0b 0000 0000 0000 0010 0010 0010 0010 0111
  *    |||| |||| |||| |||| |||| |||| |||| |||+- ODT_WR_PAS_EN: 1: Inactive CS ODT pin is enabled during write
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- ODT_WR_ACT_EN: 1: Active CS ODT pin is enabled during write
  *    |||| |||| |||| |||| |||| |||| |||| |+--- ODT_RD_PAS_EN: 1: Inactive CS ODT pin is enabled during read
  *    |||| |||| |||| |||| |||| |||| |||| +---- ODT_RD_ACT_EN: 0: Active CS ODT pin is disabled during read
  *    |||| |||| |||| |||| |||| |||| |+++------ ODT0_INT_RES: 2: Rtt_Nom 60 Ohms for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT1_INT_RES: 2: Rtt_Nom 60 Ohms for Byte 1
  *    |||| |||| |||| |||| |||| +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- ODT2_INT_RES: 2: Rtt_Nom 60 Ohms for Byte 2
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| |+++--------------------- ODT3_INT_RES: 2: Rtt_Nom 60 Ohms for Byte 3
  *    ++++ ++++ ++++ +------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MPMUR0,      0x00000800},
 {MX6_MMDC_P1_MPMUR0,      0x00000800},
 /* MMDCx_MPMUR0 p. 3974 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    8    0    0
  * 0b 0000 0000 0000 0000 0000 1000 0000 0000
  *    |||| |||| |||| |||| |||| ||++ ++++ ++++- MU_BYP_VAL: 0: No delay for bypass measurement (debug only)
  *    |||| |||| |||| |||| |||| |+------------- MU_BYP_EN: 0: Use MU_UNIT_DEL_NUM for delay
  *    |||| |||| |||| |||| |||| +-------------- FRC_MSR: 1: complete calibration, latch delay values in PHY.
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| ||++ ++++ ++++--------------------- MU_UNIT_DEL_NUM: read only, write as 0
  *    ++++ ++--------------------------------- reserved, write as 0
  */

 /* switch DDR3 to normal operation */
 {MX6_MMDC_P0_MDSCR,       0x00000000},
 /* MMDCx_MDSCR p. 3878 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 0: Normal operation
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 0: No Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x00: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x00: MSB of Command/Address
  */

};
int ram_setup_mx6q_size = ARRAY_SIZE(ram_setup_mx6q);

/* The first samples of M48 were not correctly built to do the DDRAM calibration. We had boards with revision index 6 and lower
 * which performed OK. The set in ram_calibration_mx6q_pre is used for these old boards.
 * Indices 06 and 08 refer to the values of the M48.1 boards which follow another numbering scheme as the M48.3 boards. To be
 * comparable, the same indication _06 and _08 is used as in the Dual Lite version.
 */
m48_configuration ram_calibration_mx6q_pre [] =
{

 {MX6_MMDC_P0_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x001F001F},
 /* MMDCx_MPWLDECTRLx p. 3913, 3915 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    0    1    F    0    0    1    F
  * 0b 0000 0000 0001 1111 0000 0000 0001 1111
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WL_DL_ABS_OFFSET0/2: 0x1F: write leveling delay offset for Byte 0/2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+----------- WL_HC_DEL0/2: 0: No delay is added
  *    |||| |||| |||| |||| |||| |++------------ WL_CYC_DEL0/2: 0: No delay is added
  *    |||| |||| |||| |||| ++++ +-------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WL_DL_ABS_OFFSET1/3: 0x1F: write leveling delay offset for Byte 1/3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| |||+------------------------------- WL_HC_DEL1/3: 0: No delay is added
  *    |||| |++-------------------------------- WL_CYC_DEL1/3 0: No delay is added
  *    ++++ +---------------------------------- reserved, write as 0
  */


 {MX6_MMDC_P0_MPDGCTRL0,   0x42640302},
 /* MMDCx_MPDGCTRL0 p. 3942 in [M48_DS_IMX_RMDQ_5]
  * 0x    4    2    6    4    0    3    0    2
  * 0b 0100 0010 0110 0100 0000 0011 0000 0010
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x02: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 3: 1.5 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x64: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 2: 1 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 0: Read DQS gating mechanism is enabled
  *    |+-------------------------------------- DG_CMP_CYC: 1: MMDC waits 32 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P0_MPDGCTRL1,   0x026A0266},
 /* MMDCx_MPDGCTRL1 p. 3944 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    2    6    A    0    2    6    6
  * 0b 0000 0010 0100 0100 0000 0010 0100 1100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x66: read gating delay offset for Byte 2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 2: 1 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x6A: read gating delay offset for Byte 3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 2: 1 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P1_MPDGCTRL0,   0x42640273},
 /* MMDCx_MPDGCTRL0 p. 3942 in [M48_DS_IMX_RMDQ_5]
  * 0x    4    2    6    4    0    2    7    3
  * 0b 0100 0010 0110 0100 0000 0010 0111 0011
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x73: read gating delay offset for Byte 4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 2: 1 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x64: read gating delay offset for Byte 5
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 2: 1 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 0: Read DQS gating mechanism is enabled
  *    |+-------------------------------------- DG_CMP_CYC: 1: MMDC waits 32 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P1_MPDGCTRL1,   0x02640264},
 /* MMDCx_MPDGCTRL1 p. 3944 in [M48_DS_IMX_RMDQ_5]
  * 0x    0    2    6    4    0    2    6    4
  * 0b 0000 0010 0011 0000 0000 0010 0100 0100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x64: read gating delay offset for Byte 6
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 2: 1 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x64: read gating delay offset for Byte 7
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 2: 1 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MPRDDLCTL,   0x463A383C},
 {MX6_MMDC_P1_MPRDDLCTL,   0x403C3848},
 /* MMDCx_MPRDDLCTL p. 3948 in [M48_DS_IMX_RMDQ_5]
  * 0x    4    6    3    A    3    8    3    C
  * 0b 0100 0110 0011 1010 0011 1000 0011 1100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- RD_DL_ABS_OFFSET0: 0x3C: read delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- RD_DL_ABS_OFFSET1: 0x38: read delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- RD_DL_ABS_OFFSET2: 0x3A: read delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- RD_DL_ABS_OFFSET3: 0x46: read delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MPWRDLCTL,   0x3C3C4240},
 {MX6_MMDC_P1_MPWRDLCTL,   0x46324A44},
 /* MMDCx_MPWRDLCTL p. 3950 in [M48_DS_IMX_RMDQ_5]
  * 0x    3    C    3    C    4    2    4    0
  * 0b 0001 0010 0001 0010 0011 0100 0011 0100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WR_DL_ABS_OFFSET0: 0x40: write delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- WR_DL_ABS_OFFSET1: 0x42: write delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WR_DL_ABS_OFFSET2: 0x3C: write delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- WR_DL_ABS_OFFSET3: 0x3C: write delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

};
int ram_calibration_mx6q_size_pre = ARRAY_SIZE(ram_calibration_mx6q_pre);

/* The set in ram_calibration_mx6q_prod is used for the boards beginning with revision index 8/2/2. The parameters have been
 * determined by runs on several different boards. Averaging of the measurements lead to the results below.
 */
m48_configuration ram_calibration_mx6q_prod [] =
{
 {MX6_MMDC_P0_MPWLDECTRL0, 0x004E0050},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x004C0041},
 {MX6_MMDC_P1_MPWLDECTRL0, 0x001D0033},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x0004000C},

 {MX6_MMDC_P0_MPDGCTRL0,   0x434A0352},
 {MX6_MMDC_P0_MPDGCTRL1,   0x033E033F},
 {MX6_MMDC_P1_MPDGCTRL0,   0x4327033E},
 {MX6_MMDC_P1_MPDGCTRL1,   0x03190242},
 {MX6_MMDC_P0_MPRDDLCTL,   0x42363839},
 {MX6_MMDC_P1_MPRDDLCTL,   0x3A3A3440},
 {MX6_MMDC_P0_MPWRDLCTL,   0x3A3E4440},
 {MX6_MMDC_P1_MPWRDLCTL,   0x45344741},
};
int ram_calibration_mx6q_size_prod = ARRAY_SIZE(ram_calibration_mx6q_prod);

