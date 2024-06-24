/*
 * mx6mvalenka_spl_cfg.c
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

#define CONFIG_MX6Q
#include <common.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6q-ddr.h>
#include "imx6m48_spl_cfg.h"

m48_configuration ddr_setup_valenka [] =
{
 {MX6_IOM_GRP_DDR_TYPE,    0x00080000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE
  * 0x    0    0    0    8    0    0    0    0
  * 0b 0000 0000 0000 1000 0000 0000 0000 0000
  *    |||| |||| |||| ||++ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ++----------------------- DDR_SEL LPDDR2 mode
  *    ++++ ++++ ++++-------------------------- write as 0
 */

 {MX6_IOM_GRP_DDRPKE,      0x00000000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRPKE
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| ++++ ++++ ++++- write as 0
  *    |||| |||| |||| |||| |||+---------------- PKE Pull/Keeper disabled
  *    ++++ ++++ ++++ ++++ +++----------------- write as 0
 */

 {MX6_IOM_DRAM_SDCLK_0,    0x00000020},
 {MX6_IOM_DRAM_SDCLK_1,    0x00000020},
 {MX6_IOM_DRAM_CAS,        0x00000020},
 {MX6_IOM_DRAM_RAS,        0x00000020},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS_B
  * IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS_B
  * IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK0_P
  * 0x    0    0    0    0    0    0    2    0
  * 0b 0000 0000 0000 0000 0000 0000 0010 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 1000 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: The pins above are outputs of the i.MX6. It is unclear why input characteristics can be set for these pins.
 */

 {MX6_IOM_GRP_ADDDS,       0x00000020},
 /* IOMUXC_SW_PAD_CTL_GRP_ADDDS
  * 0x    0    0    0    0    0    0    2    0
  * 0b 0000 0000 0000 0000 0000 0000 0010 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    ++++ ++++ ++++ ++++ ++++ ++++ ++-------- write as 0
  *
  */

 {MX6_IOM_DRAM_RESET,      0x00000020},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET
  * 0x    0    0    0    0    0    0    2    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    |||| |||| |||| ++----------------------- DDR_SEL DDR3_LPDDR2 mode
  *    ++++ ++++ ++++ ------------------------- write as 0
  * Note: DRAM_RESET is an output of the i.MX6. It is unclear why input characteristics can be set for these pins.
 */

 {MX6_IOM_DRAM_SDBA2,      0x00000000},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- read only
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    |||| |||| |||| ++----------------------- DDR_SEL DDR3_LPDDR2 mode
  *    ++++ ++++ ++++ ------------------------- write as 0
 */

 {MX6_IOM_DRAM_SDODT0,     0x00000020},
 {MX6_IOM_DRAM_SDODT1,     0x00000020},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_ODT0
  * 0x    0    0    0    0    0    0    2    0
  * 0b 0000 0000 0000 0000 0000 0000 0010 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: SDODT0 is an output of the i.MX6. It is unclear why input characteristics can be set for these pins.
 */

 {MX6_IOM_GRP_CTLDS,       0x00000020},
 /* IOMUXC_SW_PAD_CTL_GRP_CTLDS
  * 0x    0    0    0    0    0    0    2    0
  * 0b 0000 0000 0000 0000 0000 0000 0010 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    ++++ ++++ ++++ ++++ ++++ ++++ ++-------- write as 0
  *
  */

 {MX6_IOM_DDRMODE_CTL,     0x00020000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL
  * 0x    0    0    0    2    0    0    0    0
  * 0b 0000 0000 0000 0010 0000 0000 0000 0000
  *    |||| |||| |||| |||+ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ||+---------------------- DDR_INPUT differential input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
 * Note: DDR_INPUT mode should be a don't care but sources in [M48_DS_IMX_CONF] say that the preferred setting is differential mode
  */


 {MX6_IOM_DRAM_SDQS0,      0x00003020},
 {MX6_IOM_DRAM_SDQS1,      0x00003020},
 {MX6_IOM_DRAM_SDQS2,      0x00003020},
 {MX6_IOM_DRAM_SDQS3,      0x00003020},
 {MX6_IOM_DRAM_SDQS4,      0x00003020},
 {MX6_IOM_DRAM_SDQS5,      0x00003020},
 {MX6_IOM_DRAM_SDQS6,      0x00003020},
 {MX6_IOM_DRAM_SDQS7,      0x00003020},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0P
  * 0x    0    0    0    0    3    0    2    0
  * 0b 0000 0000 0000 0000 0011 0000 0010 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT, read only in DL, write as 0 (disabled)in DQ
  *    |||| |||| |||| |||| |||| +-------------- write as 0
  *    |||| |||| |||| |||| |||+ --------------- PKE Pull/Keeper Enabled
  *    |||| |||| |||| |||| ||+----------------- PUE Pull
  *    |||| |||| |||| |||| ++------------------ PUS 100k PD
  *    ++++ ++++ ++++ ++++--------------------- HYS, DDR_INPUT, DDR_SEL write all as 0
 */

 {MX6_IOM_GRP_DDRMODE,     0x00020000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRMODE
  * 0x    0    0    0    2    0    0    0    0
  * 0b 0000 0000 0000 0010 0000 0000 0000 0000
  *    |||| |||| |||| |||+ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ||+---------------------- DDR_INPUT differential input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
 * Note: DDR_INPUT mode should be a don't care but sources in [M48_DS_IMX_CONF] say that the preferred setting is differential mode
  */

 {MX6_IOM_GRP_B0DS,        0x00000020},
 {MX6_IOM_GRP_B1DS,        0x00000020},
 {MX6_IOM_GRP_B2DS,        0x00000020},
 {MX6_IOM_GRP_B3DS,        0x00000020},
 {MX6_IOM_GRP_B4DS,        0x00000020},
 {MX6_IOM_GRP_B5DS,        0x00000020},
 {MX6_IOM_GRP_B6DS,        0x00000020},
 {MX6_IOM_GRP_B7DS,        0x00000020},
 /* IOMUXC_SW_PAD_CTL_GRP_B0DS
  * IOMUXC_SW_PAD_CTL_GRP_ADDDS
  * IOMUXC_SW_PAD_CTL_GRP_CTLDS
  * 0x    0    0    0    0    0    0    2    0
  * 0b 0000 0000 0000 0000 0000 0000 0010 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    ++++ ++++ ++++ ++++ ++++ ++++ ++-------- write as 0
  *
  */

 {MX6_IOM_DRAM_DQM0,       0x00000020},
 {MX6_IOM_DRAM_DQM1,       0x00000020},
 {MX6_IOM_DRAM_DQM2,       0x00000020},
 {MX6_IOM_DRAM_DQM3,       0x00000020},
 {MX6_IOM_DRAM_DQM4,       0x00000020},
 {MX6_IOM_DRAM_DQM5,       0x00000020},
 {MX6_IOM_DRAM_DQM6,       0x00000020},
 {MX6_IOM_DRAM_DQM7,       0x00000020},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0
  * 0x    0    0    0    0    0    0    2    0
  * 0b 0000 0000 0000 0000 0000 0000 0010 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 60 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 1000 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: The pins above are outputs of the i.MX6. It is unclear why input characteristics can be set for these pins.
 */

 /***********************************/
 /* DDR Controller                  */
 /***********************************/

 /* MDSCR    configuration request */
 {MX6_MMDC_P0_MDSCR,       0x00008000},
 {MX6_MMDC_P1_MDSCR,       0x00008000},
 /* MMDCx_MDSCR
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
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR 0, should be don't care because of CMD=0
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP 0, should be don't care because of CMD=0
  * Note: The user manuals of the i.MX6 show a block diagram of the MMDC with channel 0 and 1 but the registers are
  * numbered MMDC1_ and MMDC2_. We stay with the 0/1 scheme for the register names in this file because that is the
  * way it originally was done in Uboot.
 */

 {MX6_MMDC_P0_MPZQLP2CTL, 0x1B4700C7},
 {MX6_MMDC_P1_MPZQLP2CTL, 0x1B4700C7},

 {MX6_MMDC_P0_MPZQHWCTRL,  0xA1390003},
 /* MMDCx_MPZQHWCTRL
  * 0x    A    1    3    9    0    0    0    3
  * 0b 1010 0001 0011 1001 0000 0000 0001 0011
  *    |||| |||| |||| |||| |||| |||| |||| ||++- ZQ_MODE 3:periodically and ZQ command and exit of self refresh
  *    |||| |||| |||| |||| |||| |||| ||++ ++--- ZQ_HW_PER ZQ cal every 4ms
  *    |||| |||| |||| |||| |||| |+++ ++-------- ZQ_HW_PU_RES read only, write as 0
  *    |||| |||| |||| |||| ++++ +-------------- ZQ_HW_PD_RES read only, write as 0
  *    |||| |||| |||| |||+--------------------- ZQ_HW_FOR force ZQ automatic calibration (ZQ_MODE must be 1 or 3)
  *    |||| |||| |||| +++---------------------- TZQ_INIT 512 cycles, JEDEC default for DDR3
  *    |||| |||| |+++-------------------------- TZQ_OPER 256 cycles, JEDEC default for DDR3
  *    |||| ||++ +----------------------------- TZQ_CS 128 cycles, default
  *    |||| |+--------------------------------- write as 0
  *    ++++ +---------------------------------- ZQ_EARLY_COMPARATOR_EN_TIMER 0x14: 21 cycles, default
  */

 {MX6_MMDC_P1_MPZQHWCTRL,  0xA1380003},
 /* MMDCx_MPZQHWCTRL
  * 0x    A    1    3    8    0    0    0    3
  * 0b 1010 0001 0011 1000 0000 0000 0001 0011
  *    |||| |||| |||| |||| |||| |||| |||| ||++- ZQ_MODE 3:periodically and ZQ command and exit of self refresh
  *    |||| |||| |||| |||| |||| |||| ||++ ++--- ZQ_HW_PER ZQ cal every 4ms
  *    |||| |||| |||| |||| |||| |+++ ++-------- ZQ_HW_PU_RES read only, write as 0
  *    |||| |||| |||| |||| ++++ +-------------- ZQ_HW_PD_RES read only, write as 0
  *    |||| |||| |||| |||+--------------------- ZQ_HW_FOR do not force ZQ automatic calibration (ZQ_MODE must be 1 or 3)
  *    |||| |||| |||| +++---------------------- TZQ_INIT 512 cycles, JEDEC default for DDR3
  *    |||| |||| |+++-------------------------- TZQ_OPER 256 cycles, JEDEC default for DDR3
  *    |||| ||++ +----------------------------- TZQ_CS 128 cycles, default
  *    |||| |+--------------------------------- write as 0
  *    ++++ +---------------------------------- ZQ_EARLY_COMPARATOR_EN_TIMER 0x14: 21 cycles, default
  */
};
int ddr_setup_valenka_size = ARRAY_SIZE(ddr_setup_valenka);


m48_configuration ram_setup_valenka [] =
{
 /* Channel 0 configuration */
 /* Perform software reset of MMDC prior to initializing its registers */
 {MX6_MMDC_P0_MDMISC,         0x00001602},
 /* MMDCx_MDMISC
  * 0x    0    0    0    0    1    6    0    2
  * 0b 0000 0000 0000 0000 0001 0110 0000 0010
  *    |||| |||| |||| |||| |||| |||| |||| |||+- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- RST: 1: Reset requested
  *    |||| |||| |||| |||| |||| |||| |||| |+----LPDDR2_2CH: 0: reset value
  *    |||| |||| |||| |||| |||| |||| |||+ +---- DDR_TYPE: 0: reset value
  *    |||| |||| |||| |||| |||| |||| ||+------- DDR_4_BANK: 0: reset value
  *    |||| |||| |||| |||| |||| |||+ ++-------- RALAT: 0: reset value
  *    |||| |||| |||| |||| |||| |++- ---------- MIF3_MODE: 3: reset value
  *    |||| |||| |||| |||| |||| +-------------- LPDDR2_S2: 0: reset value
  *    |||| |||| |||| |||| |||+---------------- BI_ON: 1: reset value
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- WALAT: 0: reset value
  *    |||| |||| |||| |+----------------------- LHD: 0: reset value
  *    |||| |||| |||| +------------------------ ADDR_MIRROR: 0: reset value
  *    |||| |||| |||+-------------------------- CALIB_PER_CS: 0: reset value
  *    ||++ ++++ +++--------------------------- reserved, write as 0
  *    |+-------------------------------------- CS1_RDY: read only, write as 0
  *    +--------------------------------------- CS0_RDY: read only, write as 0
  */

 {MX6_MMDC_P0_MDPDC,       0x00020036},
 /* MMDCx_MDPDC
  * 0x    0    0    0    2    0    0    3    6
  * 0b 0000 0000 0000 0010 0000 0000 0011 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 0: independent. Only one CS is used anyway.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of LPDDR2
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 0: No Power Down Time-out for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 0: No Power Down Time-out for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2: 3 cycles
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: 0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: 0: Disabled
  *    +--------------------------------------- reserved, write as 0
  * Note: PWDT_x: Why no power down time out? MDPDC is written again at the end of the initialisation. A timeout is set
  *       in this last write.
  */

 {MX6_MMDC_P0_MDOTC,       0x12272000},
 /* MMDCx_MDOTC
  * 0x    1    2    2    7    2    0    0    0
  * 0b 0001 0010 0010 0111 0010 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| ++++- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ ++++ ----- tODT_idle_off: Not used in case of LPDDR2
  *    |||| |||| |||| |||| |||| +++------------ reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- tODTLon: Not used in case of LPDDR2
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| ++++--------------------- tAXPD: 7: 8 clocks
  *    |||| |||| ++++-------------------------- tANPD: 2, 3 clocks
  *    |||| |+++------------------------------- tAONPD: 2, 3 clocks
  *    ||++ +---------------------------------- tAOFPD: 2, 3 clocks
  *    ++-------------------------------------- reserved, write as 0
  * Note: unclear how tODT_idle_off is to be determined.
  */

 {MX6_MMDC_P0_MDCFG0,      0x33374135},
 /* MMDCx_MDCFG0
  * 0x    3    3    3    7    4    1    3    5
  * 0b 0011 0011 0011 0111 0100 0001 0011 0101
  *    |||| |||| |||| |||| |||| |||| |||| ++++- tCL: 5: 8 clocks
  *    |||| |||| |||| |||| |||| |||+ ++++------ tFAW: 0x13: 19 clocks
  *    |||| |||| |||| |||| |||+ +++------------ Not used in case of LPDDR2
  *    |||| |||| |||| |||| +++----------------- tXP: 2: 3 cycles
  *    |||| |||| ++++ ++++--------------------- tXS: 0x37: 56 clocks
  *    ++++ ++++------------------------------- tRFC: 0x33, 52 clocks
  */

 {MX6_MMDC_P0_MDCFG1,      0x00100AC3},
 /* MMDCx_MDCFG1
  * 0x    0    0    1    0    0    A    C    3
  * 0b 0000 0000 0001 0000 0000 1010 1100 0011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCWL: 3, 5 clocks
  *    |||| |||| |||| |||| |||| |||| |||+ +---- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ +++------- tMRD: 0x6, 7 clocks
  *    |||| |||| |||| |||| |||| +++------------ tWR: 5, 6 clocks
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0x8, see Note.
  *    |||| |||| |||+ ++++--------------------- tRAS: 0x10, 17 clocks
  *    |||| ||++ +++--------------------------- tRC: Not used in case of LPDDR2 
  *    |||+ ++--------------------------------- tRP: Not used in case of LPDDR2 
  *    +++------------------------------------- tRCD: Not used in case of LPDDR2
  */

 {MX6_MMDC_P0_MDCFG2,      0x00000093},
 /* MMDCx_MDCFG2
  * 0x    0    0    0    0    0    0    9    3
  * 0b 0000 0000 0000 0000 0000 0000 1001 0011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tRRD: 3, 4 clocks
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tWTR: 2, 3 clocks
  *    |||| |||| |||| |||| |||| |||+ ++-------- tRTP: 1, 2 clocks
  *    |||| |||| |||| |||| ++++ +++------------ reserved, write as 0
  *    |||| |||+ ++++ ++++--------------------- tDLLK: Not used in case of LPDDR2
  *    ++++ +++-------------------------------- reserved, write as 0
  * Note: tDLLK is given as 512CK in the memory data sheet, so we could use a lower value. But this is one time delay which doesn't
  * really affect RAM speed.
  */

 {MX6_MMDC_P0_MDMISC,         0x0000174C},
 /* MMDCx_MDMISC
  * 0x    0    0    0    0    1    7    4    C
  * 0b 0000 0000 0000 0000 0001 0111 0100 1100
  *    |||| |||| |||| |||| |||| |||| |||| |||+- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- RST: 0: No Reset requested
  *    |||| |||| |||| |||| |||| |||| |||| |+----LPDDR2_2CH: 1: 2 channel mode (LPDDR2)
  *    |||| |||| |||| |||| |||| |||| |||+ +---- DDR_TYPE: 1: LPDDR2 is used
  *    |||| |||| |||| |||| |||| |||| ||+------- DDR_4_BANK: 0: 8 banks device is used.
  *    |||| |||| |||| |||| |||| |||+ ++-------- RALAT: 5, 5 clocks
  *    |||| |||| |||| |||| |||| |++- ---------- MIF3_MODE: 3: prediction on all possibilities
  *    |||| |||| |||| |||| |||| +-------------- LPDDR2_S2: 0: LPDDR2-S4 device is used
  *    |||| |||| |||| |||| |||+---------------- BI_ON: 1: Banks are interleaved.
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- WALAT: 0, 0 clock additional delay
  *    |||| |||| |||| |+----------------------- LHD: 0: Latency hiding on, normal mode, debug feature off
  *    |||| |||| |||| +------------------------ ADDR_MIRROR: 0: Address mirroring disabled.
  *    |||| |||| |||+-------------------------- CALIB_PER_CS: 0: Calibration is targeted to CS0
  *    ||++ ++++ +++--------------------------- reserved, write as 0
  *    |+-------------------------------------- CS1_RDY: read only, write as 0
  *    +--------------------------------------- CS0_RDY: read only, write as 0
  */

 /* Set the Configuration request bit during MMDC set up */
 {MX6_MMDC_P0_MDSCR,       0x00008000},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    0    0
  * 0b 0000 0000 0000 1000 1000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 0: Normal operation
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDRWD,       0x0F9F26D2},
 /* MMDCx_MDRWD
  * 0x    0    F    9    F    2    6    D    2
  * 0b 0000 1111 1001 1111 0010 0110 1101 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- RTR_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||| ||++ +---- RTW_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||+ ++-------- WTW_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| +++------------ WTR_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |+++---------------- RTW_SAME: 2, 2 clocks (default):
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||+ ++++ ++++ ++++--------------------- tDAI: 0xf9f, 4000 clocks
  *    +++------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDOR,        0x009F0E10},
 /* MMDCx_MDOR
  * 0x    0    0    9    F    0    E    1    0
  * 0b 0000 0000 1001 1111 0000 1110 0001 0000
  *    |||| |||| |||| |||| |||| |||| ||++ ++++- RST_to_CKE: 0x10, 14 clocks
  *    |||| |||| |||| |||| |||| |||| ++-------- reserved, write as 0
  *    |||| |||| |||| |||| ||++ ++++----------- SDE_to_RST: Not used in case of LPDDR2
  *    |||| |||| |||| |||| ++------------------ reserved, write as 0
  *    |||| |||| ++++ ++++--------------------- tXPR: Not used in case of LPDDR2
  *    ++++ ++++------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDCFG3LP,    0x001A0889},


 {MX6_MMDC_P0_MDASP,       0x0000004F},
 /* MMDCx_MDASP
  * 0x    0    0    0    0    0    0    4    F
  * 0b 0000 0000 0000 0000 0000 0000 0100 1111
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- CS0_END: 0x4f = 0x10 + 0x3f = DDR_CS_SIZE/32MB + 0x3f.
  *    ++++ ++++ ++++ ++++ ++++ ++++ +--------- reserved, write as 0
  */

 {MX6_MMDC_P0_MAARCR,      0x11420000},

 /* configure density and burst length */
 {MX6_MMDC_P0_MDCTL,       0xC3110000},
 /* MMDCx_MDCTL
  * 0x    C    3    1    1    0    0    0    0
  * 0b 1100 0011 0001 0001 0000 0000 0000 0000
  *    |||| |||| |||| |||| ++++ ++++ ++++ ++++- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- DSIZ: 1: 32-bit data bus
  *    |||| |||| |||| |+----------------------- reserved, write as 0
  *    |||| |||| |||| +------------------------ BL: 0: Burst length 4
  *    |||| |||| |+++ ------------------------- COL: 1: 10 bits column
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| |+++------------------------------- ROW: 3: 14 bits row
  *    ||++ +---------------------------------- reserved, write as 0
  *    |+-------------------------------------- SDE_1: 1: CS1 enabled
  *    +--------------------------------------- SDE_0: 1: CS0 enabled
  */

 /* Channel 1 configuration */
 /* Perform software reset of MMDC prior to initializing its registers */
 {MX6_MMDC_P1_MDMISC,         0x00001602},
 /* MMDCx_MDMISC
  * 0x    0    0    0    0    1    6    0    2
  * 0b 0000 0000 0000 0000 0001 0110 0000 0010
  *    |||| |||| |||| |||| |||| |||| |||| |||+- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- RST: 1: Reset requested
  *    |||| |||| |||| |||| |||| |||| |||| |+----LPDDR2_2CH: 0: reset value
  *    |||| |||| |||| |||| |||| |||| |||+ +---- DDR_TYPE: 0: reset value
  *    |||| |||| |||| |||| |||| |||| ||+------- DDR_4_BANK: 0: reset value
  *    |||| |||| |||| |||| |||| |||+ ++-------- RALAT: 0: reset value
  *    |||| |||| |||| |||| |||| |++- ---------- MIF3_MODE: 3: reset value
  *    |||| |||| |||| |||| |||| +-------------- LPDDR2_S2: 0: reset value
  *    |||| |||| |||| |||| |||+---------------- BI_ON: 1: reset value
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- WALAT: 0: reset value
  *    |||| |||| |||| |+----------------------- LHD: 0: reset value
  *    |||| |||| |||| +------------------------ ADDR_MIRROR: 0: reset value
  *    |||| |||| |||+-------------------------- CALIB_PER_CS: 0: reset value
  *    ||++ ++++ +++--------------------------- reserved, write as 0
  *    |+-------------------------------------- CS1_RDY: read only, write as 0
  *    +--------------------------------------- CS0_RDY: read only, write as 0
  */

 {MX6_MMDC_P1_MDPDC,       0x00020036},
 /* MMDCx_MDPDC
  * 0x    0    0    0    2    0    0    3    6
  * 0b 0000 0000 0000 0010 0000 0000 0011 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 0: independent. Only one CS is used anyway.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of LPDDR2
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 0: No Power Down Time-out for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 0: No Power Down Time-out for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2: 3 cycles
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: 0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: 0: Disabled
  *    +--------------------------------------- reserved, write as 0
  * Note: PWDT_x: Why no power down time out? MDPDC is written again at the end of the initialisation. A timeout is set
  *       in this last write.
  */

 {MX6_MMDC_P1_MDOTC,       0x12272000},
 /* MMDCx_MDOTC
  * 0x    1    2    2    7    2    0    0    0
  * 0b 0001 0010 0010 0111 0010 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| ++++- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ ++++ ----- tODT_idle_off: Not used in case of LPDDR2
  *    |||| |||| |||| |||| |||| +++------------ reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- tODTLon: Not used in case of LPDDR2
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| ++++--------------------- tAXPD: 7: 8 clocks
  *    |||| |||| ++++-------------------------- tANPD: 2, 3 clocks
  *    |||| |+++------------------------------- tAONPD: 2, 3 clocks
  *    ||++ +---------------------------------- tAOFPD: 2, 3 clocks
  *    ++-------------------------------------- reserved, write as 0
  * Note: unclear how tODT_idle_off is to be determined.
  */

 {MX6_MMDC_P1_MDCFG0,      0x33374135},
 /* MMDCx_MDCFG0
  * 0x    3    3    3    7    4    1    3    5
  * 0b 0011 0011 0011 0111 0100 0001 0011 0101
  *    |||| |||| |||| |||| |||| |||| |||| ++++- tCL: 5: 8 clocks
  *    |||| |||| |||| |||| |||| |||+ ++++------ tFAW: 0x13: 19 clocks
  *    |||| |||| |||| |||| |||+ +++------------ Not used in case of LPDDR2
  *    |||| |||| |||| |||| +++----------------- tXP: 2: 3 cycles
  *    |||| |||| ++++ ++++--------------------- tXS: 0x37: 56 clocks
  *    ++++ ++++------------------------------- tRFC: 0x33, 52 clocks
  */

 {MX6_MMDC_P1_MDCFG1,      0x00100AC3},
 /* MMDCx_MDCFG1
  * 0x    0    0    1    0    0    A    C    3
  * 0b 0000 0000 0001 0000 0000 1010 1000 0011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCWL: 3, 5 clocks
  *    |||| |||| |||| |||| |||| |||| |||+ +---- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ +++------- tMRD: 0x6, 7 clocks
  *    |||| |||| |||| |||| |||| +++------------ tWR: 5, 6 clocks
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0x8, see Note.
  *    |||| |||| |||+ ++++--------------------- tRAS: 0x10, 17 clocks
  *    |||| ||++ +++--------------------------- tRC: Not used in case of LPDDR2 
  *    |||+ ++--------------------------------- tRP: Not used in case of LPDDR2 
  *    +++------------------------------------- tRCD: Not used in case of LPDDR2
  */

 {MX6_MMDC_P1_MDCFG2,      0x00000093},
 /* MMDCx_MDCFG2
  * 0x    0    0    0    0    0    0    9    3
  * 0b 0000 0000 0000 0000 0000 0000 1001 0011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tRRD: 3, 4 clocks
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tWTR: 2, 3 clocks
  *    |||| |||| |||| |||| |||| |||+ ++-------- tRTP: 1, 2 clocks
  *    |||| |||| |||| |||| ++++ +++------------ reserved, write as 0
  *    |||| |||+ ++++ ++++--------------------- tDLLK: Not used in case of LPDDR2
  *    ++++ +++-------------------------------- reserved, write as 0
  * Note: tDLLK is given as 512CK in the memory data sheet, so we could use a lower value. But this is one time delay which doesn't
  * really affect RAM speed.
  */

 {MX6_MMDC_P1_MDMISC,         0x0000174C},
 /* MMDCx_MDMISC
  * 0x    0    0    0    0    1    7    4    C
  * 0b 0000 0000 0000 0000 0001 0111 0100 1100
  *    |||| |||| |||| |||| |||| |||| |||| |||+- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- RST: 0: No Reset requested
  *    |||| |||| |||| |||| |||| |||| |||| |+----LPDDR2_2CH: 1: 2 channel mode (LPDDR2)
  *    |||| |||| |||| |||| |||| |||| |||+ +---- DDR_TYPE: 1: LPDDR2 is used
  *    |||| |||| |||| |||| |||| |||| ||+------- DDR_4_BANK: 0: 8 banks device is used.
  *    |||| |||| |||| |||| |||| |||+ ++-------- RALAT: 5, 5 clocks
  *    |||| |||| |||| |||| |||| |++- ---------- MIF3_MODE: 3: prediction on all possibilities
  *    |||| |||| |||| |||| |||| +-------------- LPDDR2_S2: 0: LPDDR2-S4 device is used
  *    |||| |||| |||| |||| |||+---------------- BI_ON: 1: Banks are interleaved.
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- WALAT: 0, 0 clock additional delay
  *    |||| |||| |||| |+----------------------- LHD: 0: Latency hiding on, normal mode, debug feature off
  *    |||| |||| |||| +------------------------ ADDR_MIRROR: 0: Address mirroring disabled.
  *    |||| |||| |||+-------------------------- CALIB_PER_CS: 0: Calibration is targeted to CS0
  *    ||++ ++++ +++--------------------------- reserved, write as 0
  *    |+-------------------------------------- CS1_RDY: read only, write as 0
  *    +--------------------------------------- CS0_RDY: read only, write as 0
  */

 /* Set the Configuration request bit during MMDC set up */
 {MX6_MMDC_P1_MDSCR,       0x00008000},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    0    0
  * 0b 0000 0000 0000 1000 1000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 0: Normal operation
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P1_MDRWD,       0x0F9F26D2},
 /* MMDCx_MDRWD
  * 0x    0    F    9    F    2    6    D    2
  * 0b 0000 1111 1001 1111 0010 0110 1101 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- RTR_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||| ||++ +---- RTW_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||+ ++-------- WTW_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| +++------------ WTR_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |+++---------------- RTW_SAME: 2, 2 clocks (default):
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||+ ++++ ++++ ++++--------------------- tDAI: 0xf9f, 4000 clocks
  *    +++------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MDOR,        0x009F0E10},
 /* MMDCx_MDOR
  * 0x    0    0    9    F    0    E    1    0
  * 0b 0000 0000 1001 1111 0000 1110 0001 0000
  *    |||| |||| |||| |||| |||| |||| ||++ ++++- RST_to_CKE: 0x10, 14 clocks
  *    |||| |||| |||| |||| |||| |||| ++-------- reserved, write as 0
  *    |||| |||| |||| |||| ||++ ++++----------- SDE_to_RST: Not used in case of LPDDR2
  *    |||| |||| |||| |||| ++------------------ reserved, write as 0
  *    |||| |||| ++++ ++++--------------------- tXPR: Not used in case of LPDDR2
  *    ++++ ++++------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MDCFG3LP,    0x001A0889},


 {MX6_MMDC_P1_MDASP,       0x00000017},
 /* MMDCx_MDASP
  * 0x    0    0    0    0    0    0    1    7
  * 0b 0000 0000 0000 0000 0000 0000 0001 0111
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- CS0_END: 0x17 = 0x10 + 0x7 = DDR_CS_SIZE/32MB + 7.
  *    ++++ ++++ ++++ ++++ ++++ ++++ +--------- reserved, write as 0
  */

 {MX6_MMDC_P1_MAARCR,      0x11420000},

 {MX6_MMDC_P1_MDCTL,       0xC3110000},
 /* MMDCx_MDCTL
  * 0x    C    3    1    1    0    0    0    0
  * 0b 1100 0011 0001 0001 0000 0000 0000 0000
  *    |||| |||| |||| |||| ++++ ++++ ++++ ++++- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- DSIZ: 1: 32-bit data bus
  *    |||| |||| |||| |+----------------------- reserved, write as 0
  *    |||| |||| |||| +------------------------ BL: 0: Burst length 4
  *    |||| |||| |+++ ------------------------- COL: 1: 10 bits column
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| |+++------------------------------- ROW: 3: 14 bits row
  *    ||++ +---------------------------------- reserved, write as 0
  *    |+-------------------------------------- SDE_1: 1: CS1 enabled
  *    +--------------------------------------- SDE_0: 1: CS0 enabled
  */

 /* Precharge all */

 {MX6_MMDC_P0_MDSCR,       0x00008010},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    1    0
  * 0b 0000 0000 0000 0000 1000 0000 0001 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 1: Precharge all
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x00008018},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    1    0
  * 0b 0000 0000 0000 0000 1000 0000 0001 1000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 1: Chip select 1
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 1: Precharge all
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P1_MDSCR,       0x00008010},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    1    0
  * 0b 0000 0000 0000 0000 1000 0000 0001 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 1: Precharge all
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P1_MDSCR,       0x00008018},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    1    0
  * 0b 0000 0000 0000 0000 1000 0000 0001 1000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 1: Chip select 1
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 1: Precharge all
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 /* Configure DDR device channel 0 */

 {MX6_MMDC_P0_MDSCR,       0x003F8030}, /* BA=0, CS=0, MR_ADDR=63 MR_OP=0  */
 {MX6_MMDC_P0_MDSCR,       0xFF0A8030}, /* BA=0, CS=0, MR_ADDR=10 MR_OP=ff */
 {MX6_MMDC_P0_MDSCR,       0x82018030}, /* BA=0, CS=0, MR_ADDR=1  MR_OP=82 */
 {MX6_MMDC_P0_MDSCR,       0x06028030}, /* BA=0, CS=0, MR_ADDR=2  MR_OP=6  */
 {MX6_MMDC_P0_MDSCR,       0x01038030}, /* BA=0, CS=0, MR_ADDR=3  MR_OP=1  */
 {MX6_MMDC_P0_MDSCR,       0x003F8038}, /* BA=0, CS=1, MR_ADDR=63 MR_OP=0  */
 {MX6_MMDC_P0_MDSCR,       0xFF0A8038}, /* BA=0, CS=1, MR_ADDR=10 MR_OP=ff */
 {MX6_MMDC_P0_MDSCR,       0x82018038}, /* BA=0, CS=1, MR_ADDR=1  MR_OP=82 */
 {MX6_MMDC_P0_MDSCR,       0x06028038}, /* BA=0, CS=1, MR_ADDR=2  MR_OP=6  */
 {MX6_MMDC_P0_MDSCR,       0x01038038}, /* BA=0, CS=1, MR_ADDR=3  MR_OP=1  */
 /* MMDCx_MDSCR
  * 0x    x    x    x    x    8    0    3    0
  * 0b xxxx xxxx xxxx xxxx 1000 0000 0011 x000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: x: Chip select x
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register command
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 /* Configure DDR device channel 0 */

 {MX6_MMDC_P1_MDSCR,       0x003F8030}, /* BA=0, CS=0, MR_ADDR=63 MR_OP=0  */
 {MX6_MMDC_P1_MDSCR,       0xFF0A8030}, /* BA=0, CS=0, MR_ADDR=10 MR_OP=ff */
 {MX6_MMDC_P1_MDSCR,       0x82018030}, /* BA=0, CS=0, MR_ADDR=1  MR_OP=82 */
 {MX6_MMDC_P1_MDSCR,       0x06028030}, /* BA=0, CS=0, MR_ADDR=2  MR_OP=6  */
 {MX6_MMDC_P1_MDSCR,       0x01038030}, /* BA=0, CS=0, MR_ADDR=3  MR_OP=1  */
 {MX6_MMDC_P1_MDSCR,       0x003F8038}, /* BA=0, CS=1, MR_ADDR=63 MR_OP=0  */
 {MX6_MMDC_P1_MDSCR,       0xFF0A8038}, /* BA=0, CS=1, MR_ADDR=10 MR_OP=ff */
 {MX6_MMDC_P1_MDSCR,       0x82018038}, /* BA=0, CS=1, MR_ADDR=1  MR_OP=82 */
 {MX6_MMDC_P1_MDSCR,       0x06028038}, /* BA=0, CS=1, MR_ADDR=2  MR_OP=6  */
 {MX6_MMDC_P1_MDSCR,       0x01038038}, /* BA=0, CS=1, MR_ADDR=3  MR_OP=1  */
 /* MMDCx_MDSCR
  * 0x    x    x    x    x    8    0    3    0
  * 0b xxxx xxxx xxxx xxxx 1000 0000 0011 x000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: x: Chip select x
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register command
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P0_MPDGCTRL0,   0xA0000000},
 {MX6_MMDC_P0_MPDGCTRL0,   0xA0000000},
 {MX6_MMDC_P1_MPDGCTRL0,   0xA0000000},
 {MX6_MMDC_P1_MPDGCTRL0,   0xA0000000},
 /* MMDCx_MPDGCTRL0
  * 0x    A    0    0    0    0    0    0    0
  * 0b 1010 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x00: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 0: 0 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x00: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 0: 0 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 1: Read DQS gating mechanism is disabled
  *    |+-------------------------------------- DG_CMP_CYC: 0: MMDC waits 16 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 1: Reset read data FIFO
  */

 
 {MX6_MMDC_P0_MDREF,       0x00001800},
 {MX6_MMDC_P1_MDREF,       0x00001800},
 /* MMDCx_MDREF
  * 0x    0    0    0    0    1    8    0    0
  * 0b 0000 0000 0000 0000 0001 1000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- START_REF: 0: Do not start a manual refresh cycle
  *    |||| |||| |||| |||| |||| |+++ ++++ +++-- reserved, write as 0
  *    |||| |||| |||| |||| ||++ +-------------- REFR: 3: 4 refreshes every cycle
  *    |||| |||| |||| |||| ++------------------ REF_SEL: 0: Periodic refresh cycles with 64kHz.
  *    ++++ ++++ ++++ ++++--------------------- REF_CNT: 0: reserved
  * Note: REF_CNT with value 0 is reserved, but also reset value.
  */
 {MX6_MMDC_P0_MPODTCTRL,   0x00000000},
 {MX6_MMDC_P1_MPODTCTRL,   0x00000000},

 /* MMDCx_MPODTCTRL
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- ODT_WR_PAS_EN: 0: Inactive CS ODT pin is disabled during write
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- ODT_WR_ACT_EN: 0: Active CS ODT pin is disabled during write
  *    |||| |||| |||| |||| |||| |||| |||| |+--- ODT_RD_PAS_EN: 0: Inactive CS ODT pin is disabled during read
  *    |||| |||| |||| |||| |||| |||| |||| +---- ODT_RD_ACT_EN: 0: Active CS ODT pin is disabled during read
  *    |||| |||| |||| |||| |||| |||| |+++------ ODT0_INT_RES: 0: Rtt_Nom Disabled
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT1_INT_RES: 0: Rtt_Nom Disabled
  *    |||| |||| |||| |||| |||| +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- ODT2_INT_RES: 0: Rtt_Nom Disabled
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| |+++--------------------- ODT3_INT_RES: 0: Rtt_Nom Disabled
  *    ++++ ++++ ++++ +------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MDPDC,       0x00025576},
 {MX6_MMDC_P1_MDPDC,       0x00025576},
 /* MMDCx_MDPDC
  * 0x    0    0    0    2    5    5    7    6
  * 0b 0000 0000 0000 0010 0101 0101 0111 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 1: both chips selects idle.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of DDR3
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 5: 256 cycles for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 5: 256 cycles for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2, 3 cycles
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: Disabled
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MAPSR,       0x00011006},
 {MX6_MMDC_P1_MAPSR,       0x00011006},
 /* MMDCx_MAPSR
  * 0x    0    0    0    1    1    0    0    6
  * 0b 0000 0000 0000 0001 0001 0000 0000 0110
  *    |||| |||| |||| |||| |||| |||| |||| |||+- PSD: 0: Power saving enabled
  *    |||| |||| |||| |||| |||| |||| |||| +++-- reserved, write as 2
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


 /* switch LPDDR2 to normal operation */
 {MX6_MMDC_P0_MDSCR,       0x00000000},
 {MX6_MMDC_P1_MDSCR,       0x00000000},
 /* MMDCx_MDSCR
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
int ram_setup_valenka_size = ARRAY_SIZE(ram_setup_valenka);

m48_configuration ram_calibration_valenka [] =
{

 {MX6_MMDC_P0_MPPDCMPR2,   0x00400000},
 {MX6_MMDC_P1_MPPDCMPR2,   0x00400000},

 {MX6_MMDC_P0_MPRDDLCTL,   0x3F3B373E},
 /* MMDC0_MPRDDLCTL
  * 0x    3    F    3    B    3    7    3    E
  * 0b 0011 1111 0011 1011 0011 0111 0011 1110
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- RD_DL_ABS_OFFSET0: 0x3F: read delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- RD_DL_ABS_OFFSET1: 0x3B: read delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- RD_DL_ABS_OFFSET2: 0x37: read delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- RD_DL_ABS_OFFSET3: 0x3E: read delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MPRDDLCTL,   0x3F3B3C46},
 /* MMDC1_MPRDDLCTL
  * 0x    3    F    3    B    3    c    4    6
  * 0b 0011 1111 0011 1011 0011 1100 0100 0110
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- RD_DL_ABS_OFFSET0: 0x3F: read delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- RD_DL_ABS_OFFSET1: 0x3B: read delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- RD_DL_ABS_OFFSET2: 0x3C: read delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- RD_DL_ABS_OFFSET3: 0x46: read delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MPWRDLCTL,   0x36343B35},
 /* MMDC0_MPWRDLCTL
  * 0x    3    6    3    4    3    B    3    5
  * 0b 0011 0110 0011 0100 0011 1011 0011 0101
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WR_DL_ABS_OFFSET0: 0x36: write delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- WR_DL_ABS_OFFSET1: 0x34: write delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WR_DL_ABS_OFFSET2: 0x3B: write delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- WR_DL_ABS_OFFSET3: 0x35: write delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MPWRDLCTL,   0x3D2D3838},
 /* MMDC1_MPWRDLCTL
  * 0x    3    D    2    D    3    8    3    8
  * 0b 0011 1101 0010 1101 0011 1000 0011 1000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WR_DL_ABS_OFFSET0: 0x3D: write delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- WR_DL_ABS_OFFSET1: 0x2D: write delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WR_DL_ABS_OFFSET2: 0x3r8C: write delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- WR_DL_ABS_OFFSET3: 0x38: write delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MPDGCTRL0,   0x20000000},
 /* MMDCx_MPDGCTRL0
  * 0x    2    0    0    0    0    0    0    0
  * 0b 0010 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x00: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 0: 0 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x00: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 0: 0 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 1: Read DQS gating mechanism is disabled
  *    |+-------------------------------------- DG_CMP_CYC: 0: MMDC waits 16 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P0_MPDGCTRL1,   0x00000000},
 /* MMDCx_MPDGCTRL1
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x00: read gating delay offset for Byte 2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 0: 0 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x00: read gating delay offset for Byte 3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 0: 0 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P1_MPDGCTRL0,   0x20000000},
 /* MMDCx_MPDGCTRL0
  * 0x    2    0    0    0    0    0    0    0
  * 0b 0010 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x00: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 0: 0 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x00: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 0: 0 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 1: Read DQS gating mechanism is disabled
  *    |+-------------------------------------- DG_CMP_CYC: 0: MMDC waits 16 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P1_MPDGCTRL1,   0x00000000},
 /* MMDCx_MPDGCTRL1
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x00: read gating delay offset for Byte 2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 0: 0 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x00: read gating delay offset for Byte 3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 0: 0 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY3DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY3DL, 0x33333333},
 /* MMDCx_MPRDDQBY0DL
  * 0x    3    3    3    3    3    3    3    3
  * 0b 0011 0011 0011 0011 0011 0011 0011 0011
  *    |... |... |... |... |... |... |... |+++- wr_dqx_del 3: for all eight nibbles: Add 3 delay units. This is the
  *                                             recommended value even though the reset value is 0.
  */

 {MX6_MMDC_P0_MPWRDQBY0DL, 0xF3333333},
 {MX6_MMDC_P0_MPWRDQBY1DL, 0xF3333333},
 {MX6_MMDC_P0_MPWRDQBY2DL, 0xF3333333},
 {MX6_MMDC_P0_MPWRDQBY3DL, 0xF3333333},
 {MX6_MMDC_P1_MPWRDQBY0DL, 0xF3333333},
 {MX6_MMDC_P1_MPWRDQBY1DL, 0xF3333333},
 {MX6_MMDC_P1_MPWRDQBY2DL, 0xF3333333},
 {MX6_MMDC_P1_MPWRDQBY3DL, 0xF3333333},
 /* MMDCx_MPWRDQBY0DL
  * 0x    F    3    3    3    3    3    3    3
  * 0b 1111 0011 0011 0011 0011 0011 0011 0011
  *    |... |... |... |... |... |... |... |+++- rd_dqx_del 3: for all eight nibbles: Add 3 delay units. This is the
  *                                             recommended value even though the reset value is 0.
  *    ++-------------------------------------- wr_dm0_del 3: Add dm0 delay of 3 delay units 
  */

  {MX6_MMDC_P0_MPDCCR, 0x24921492}, /* MMDC1_MPDCCR */
  {MX6_MMDC_P1_MPDCCR, 0x24921492}, /* MMDC2_MPDCCR */
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


 {MX6_MMDC_P0_MPMUR0,      0x00000800},
 {MX6_MMDC_P1_MPMUR0,      0x00000800},
 /* MMDCx_MPMUR0
  * 0x    0    0    0    0    0    8    0    0
  * 0b 0000 0000 0000 0000 0000 1000 0000 0000
  *    |||| |||| |||| |||| |||| ||++ ++++ ++++- MU_BYP_VAL: 0: No delay for bypass measurement (debug only)
  *    |||| |||| |||| |||| |||| |+------------- MU_BYP_EN: 0: Use MU_UNIT_DEL_NUM for delay
  *    |||| |||| |||| |||| |||| +-------------- FRC_MSR: 1: complete calibration, latch delay values in PHY.
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| ||++ ++++ ++++--------------------- MU_UNIT_DEL_NUM: read only, write as 0
  *    ++++ ++--------------------------------- reserved, write as 0
  */

};
int ram_calibration_valenka_size = ARRAY_SIZE(ram_calibration_valenka);


/* Valenka3 2GB/4GB DDR3 */
m48_configuration ddr_setup_valenka3 [] =
{
 {MX6_IOM_GRP_DDR_TYPE,    0x000C0000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE
  * 0x    0    0    0    C    0    0    0    0
  * 0b 0000 0000 0000 1100 0000 0000 0000 0000
  *    |||| |||| |||| ||++ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ++----------------------- DDR_SEL DDR3 mode
  *    ++++ ++++ ++++-------------------------- write as 0
 */

 {MX6_IOM_GRP_DDRPKE,      0x00000000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRPKE
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| ++++ ++++ ++++- write as 0
  *    |||| |||| |||| |||| |||+---------------- PKE Pull/Keeper disabled
  *    ++++ ++++ ++++ ++++ +++----------------- write as 0
 */

 {MX6_IOM_DRAM_SDCLK_0,    0x00000030},
 {MX6_IOM_DRAM_SDCLK_1,    0x00000030},
 {MX6_IOM_DRAM_CAS,        0x00000030},
 {MX6_IOM_DRAM_RAS,        0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS_B
  * IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS_B
  * IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK0_P
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 80 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 1000 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: The pins above are outputs of the i.MX6. It is unclear why input characteristics can be set for these pins.
 */

 {MX6_IOM_GRP_ADDDS,       0x00000030},
 /* IOMUXC_SW_PAD_CTL_GRP_ADDDS
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 80 Ohm. Value determined with oscilloscope
  *    ++++ ++++ ++++ ++++ ++++ ++++ ++-------- write as 0
  *
  */

 {MX6_IOM_DRAM_RESET,      0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 80 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    |||| |||| |||| ++----------------------- DDR_SEL DDR3_LPDDR2 mode
  *    ++++ ++++ ++++ ------------------------- write as 0
  * Note: DRAM_RESET is an output of the i.MX6. It is unclear why input characteristics can be set for these pins.
 */

 {MX6_IOM_DRAM_SDBA2,      0x00000000},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2
  * 0x    0    0    0    0    0    0    0    0
  * 0b 0000 0000 0000 0000 0000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- read only
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    |||| |||| |||| ++----------------------- DDR_SEL DDR3_LPDDR2 mode
  *    ++++ ++++ ++++ ------------------------- write as 0
 */

 {MX6_IOM_DRAM_SDODT0,     0x00000030},
 {MX6_IOM_DRAM_SDODT1,     0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_ODT0
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 80 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT disabled
  *    |||| |||| |||| |||| ++++ +-------------- write as 0 (reset value is 0011 0)
  *    |||| |||| |||| |||+--------------------- HYS CMOS input mode
  *    |||| |||| |||| ||+---------------------- DDR_INPUT CMOS input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
  * Note: SDODT0 is an output of the i.MX6. It is unclear why input characteristics can be set for these pins.
 */

 {MX6_IOM_GRP_CTLDS,       0x00000030},
 /* IOMUXC_SW_PAD_CTL_GRP_CTLDS
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 80 Ohm. Value determined with oscilloscope
  *    ++++ ++++ ++++ ++++ ++++ ++++ ++-------- write as 0
  *
  */

 {MX6_IOM_DDRMODE_CTL,     0x00020000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL
  * 0x    0    0    0    2    0    0    0    0
  * 0b 0000 0000 0000 0010 0000 0000 0000 0000
  *    |||| |||| |||| |||+ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ||+---------------------- DDR_INPUT differential input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
 * Note: DDR_INPUT mode should be a don't care but sources in [M48_DS_IMX_CONF] say that the preferred setting is differential mode
  */


 {MX6_IOM_DRAM_SDQS0,      0x00000030},
 {MX6_IOM_DRAM_SDQS1,      0x00000030},
 {MX6_IOM_DRAM_SDQS2,      0x00000030},
 {MX6_IOM_DRAM_SDQS3,      0x00000030},
 {MX6_IOM_DRAM_SDQS4,      0x00000030},
 {MX6_IOM_DRAM_SDQS5,      0x00000030},
 {MX6_IOM_DRAM_SDQS6,      0x00000030},
 {MX6_IOM_DRAM_SDQS7,      0x00000030},
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0P
  * 0x    0    0    0    0    0    0    3    0
  * 0b 0000 0000 0000 0000 0000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- write as 0
  *    |||| |||| |||| |||| |||| |||| ||++ +---- DSE 40 Ohm. Value determined with oscilloscope
  *    |||| |||| |||| |||| |||| |||| ++-------- write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT, read only in DL, write as 0 (disabled)in DQ
  *    |||| |||| |||| |||| |||| +-------------- write as 0
  *    |||| |||| |||| |||| |||+ --------------- PKE Pull/Keeper Disabled
  *    |||| |||| |||| |||| ||+----------------- PUE Keeper
  *    |||| |||| |||| |||| ++------------------ PUS 100k PD
  *    ++++ ++++ ++++ ++++--------------------- HYS, DDR_INPUT, DDR_SEL write all as 0
 */

 {MX6_IOM_GRP_DDRMODE,     0x00020000},
 /* IOMUXC_SW_PAD_CTL_GRP_DDRMODE
  * 0x    0    0    0    2    0    0    0    0
  * 0b 0000 0000 0000 0010 0000 0000 0000 0000
  *    |||| |||| |||| |||+ ++++ ++++ ++++ ++++- write as 0
  *    |||| |||| |||| ||+---------------------- DDR_INPUT differential input mode
  *    ++++ ++++ ++++ ++----------------------- write as 0
 * Note: DDR_INPUT mode should be a don't care but sources in [M48_DS_IMX_CONF] say that the preferred setting is differential mode
  */

 {MX6_IOM_GRP_B0DS,        0x00000030},
 {MX6_IOM_GRP_B1DS,        0x00000030},
 {MX6_IOM_GRP_B2DS,        0x00000030},
 {MX6_IOM_GRP_B3DS,        0x00000030},
 {MX6_IOM_GRP_B4DS,        0x00000030},
 {MX6_IOM_GRP_B5DS,        0x00000030},
 {MX6_IOM_GRP_B6DS,        0x00000030},
 {MX6_IOM_GRP_B7DS,        0x00000030},
 /* IOMUXC_SW_PAD_CTL_GRP_B0DS
  * IOMUXC_SW_PAD_CTL_GRP_ADDDS
  * IOMUXC_SW_PAD_CTL_GRP_CTLDS
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
 /* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0
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
 */

 /***********************************/
 /* DDR Controller                  */
 /***********************************/

 {MX6_MMDC_P0_MPZQHWCTRL,  0xA1390003},
 /* MMDCx_MPZQHWCTRL
  * 0x    A    1    3    9    0    0    0    3
  * 0b 1010 0001 0011 1001 0000 0000 0001 0011
  *    |||| |||| |||| |||| |||| |||| |||| ||++- ZQ_MODE 3:periodically and ZQ command and exit of self refresh
  *    |||| |||| |||| |||| |||| |||| ||++ ++--- ZQ_HW_PER ZQ cal every 4ms
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
int ddr_setup_valenka3_size = ARRAY_SIZE(ddr_setup_valenka3);


m48_configuration ram_calibration_valenka3_2gb [] =
{

 {MX6_MMDC_P0_MPWLDECTRL0, 0x001A0023},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x00270027},

 {MX6_MMDC_P1_MPWLDECTRL0, 0x001E0023},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x00110021},

 {MX6_MMDC_P0_MPDGCTRL0,   0x4250025C},
 /* MMDCx_MPDGCTRL0
  * 0x    4    2    5    0    0    2    5    C
  * 0b 0100 0010 0101 0000 0000 0010 0101 1100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x5C: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 2: 1 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x50: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 2: 1 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 1: Read DQS gating mechanism is disabled
  *    |+-------------------------------------- DG_CMP_CYC: 0: MMDC waits 32 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P0_MPDGCTRL1,   0x024C0240},
 /* MMDCx_MPDGCTRL1
  * 0x    0    2    4    C    0    2    4    0
  * 0b 0000 0010 0100 1100 0000 0010 0100 0000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x40: read gating delay offset for Byte 2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 2: 1 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x4C: read gating delay offset for Byte 3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 2: 1 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P1_MPDGCTRL0,   0x42500254},
 /* MMDCx_MPDGCTRL0
  * 0x    4    2    5    0    0    2    5    4
  * 0b 0100 0010 0101 0000 0000 0010 0101 0100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x54: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 2: 1 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x50: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 2: 1 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 1: Read DQS gating mechanism is disabled
  *    |+-------------------------------------- DG_CMP_CYC: 1: MMDC waits 16 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P1_MPDGCTRL1,   0x0248022C},
 /* MMDCx_MPDGCTRL1
  * 0x    0    2    4    8    0    2    2    C
  * 0b 0000 0010 0100 1000 0000 0010 0010 1100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x2C: read gating delay offset for Byte 2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 2: 1 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x48: read gating delay offset for Byte 3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 2: 1 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MPRDDLCTL,   0x463A3A38},
 /* MMDC0_MPRDDLCTL
  * 0x    4    6    3    A    3    A    3    8
  * 0b 0100 0110 0011 1010 0011 1010 0011 1000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- RD_DL_ABS_OFFSET0: 0x38: read delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- RD_DL_ABS_OFFSET1: 0x3A: read delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- RD_DL_ABS_OFFSET2: 0x3A: read delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- RD_DL_ABS_OFFSET3: 0x46: read delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MPRDDLCTL,   0x3C3A3642},
 /* MMDC1_MPRDDLCTL
  * 0x    3    C    3    A    3    6    4    2
  * 0b 0011 1100 0011 1010 0011 0110 0100 0010
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- RD_DL_ABS_OFFSET0: 0x42: read delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- RD_DL_ABS_OFFSET1: 0x36: read delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- RD_DL_ABS_OFFSET2: 0x3A: read delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- RD_DL_ABS_OFFSET3: 0x3C: read delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MPWRDLCTL,   0x3A3A423A},
 /* MMDC0_MPWRDLCTL
  * 0x    3    A    3    A    4    2    3    A
  * 0b 0011 1010 0011 1010 0100 0010 0011 1010
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WR_DL_ABS_OFFSET0: 0x3A: write delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- WR_DL_ABS_OFFSET1: 0x42: write delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WR_DL_ABS_OFFSET2: 0x3A: write delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- WR_DL_ABS_OFFSET3: 0x3A: write delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MPWRDLCTL,   0x42344240},
 /* MMDC1_MPWRDLCTL
  * 0x    4    2    3    4    4    2    4    0
  * 0b 0100 0010 0011 0100 0100 0010 0100 0000
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WR_DL_ABS_OFFSET0: 0x40: write delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- WR_DL_ABS_OFFSET1: 0x42: write delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WR_DL_ABS_OFFSET2: 0x34: write delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- WR_DL_ABS_OFFSET3: 0x42: write delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY3DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY3DL, 0x33333333},
 /* MMDCx_MPRDDQBY0DL
  * 0x    3    3    3    3    3    3    3    3
  * 0b 0011 0011 0011 0011 0011 0011 0011 0011
  *    |... |... |... |... |... |... |... |+++- wr_dqx_del 3: for all eight nibbles: Add 3 delay units. This is the
  *                                             recommended value even though the reset value is 0.
  */

  {MX6_MMDC_P0_MPDCCR, 0x24921492}, /* MMDC1_MPDCCR */
  {MX6_MMDC_P1_MPDCCR, 0x24921492}, /* MMDC2_MPDCCR */
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

 {MX6_MMDC_P0_MPMUR0,      0x00000800},
 {MX6_MMDC_P1_MPMUR0,      0x00000800},
 /* MMDCx_MPMUR0
  * 0x    0    0    0    0    0    8    0    0
  * 0b 0000 0000 0000 0000 0000 1000 0000 0000
  *    |||| |||| |||| |||| |||| ||++ ++++ ++++- MU_BYP_VAL: 0: No delay for bypass measurement (debug only)
  *    |||| |||| |||| |||| |||| |+------------- MU_BYP_EN: 0: Use MU_UNIT_DEL_NUM for delay
  *    |||| |||| |||| |||| |||| +-------------- FRC_MSR: 1: complete calibration, latch delay values in PHY.
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| ||++ ++++ ++++--------------------- MU_UNIT_DEL_NUM: read only, write as 0
  *    ++++ ++--------------------------------- reserved, write as 0
  */

};
int ram_calibration_valenka3_2gb_size = ARRAY_SIZE(ram_calibration_valenka3_2gb);

m48_configuration ram_setup_valenka3_2gb [] =
{

 {MX6_MMDC_P0_MDPDC,       0x00020036},
 /* MMDCx_MDPDC
  * 0x    0    0    0    2    0    0    3    6
  * 0b 0000 0000 0000 0010 0000 0000 0011 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 0: independent. Only one CS is used anyway.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of LPDDR2
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 0: No Power Down Time-out for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 0: No Power Down Time-out for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2: 3 cycles
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: 0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: 0: Disabled
  *    +--------------------------------------- reserved, write as 0
  * Note: PWDT_x: Why no power down time out? MDPDC is written again at the end of the initialisation. A timeout is set
  *       in this last write.
  */

 {MX6_MMDC_P0_MDOTC,       0x24444040},
 /* MMDCx_MDOTC
  * 0x    2    4    4    4    4    0    4    0
  * 0b 0010 0100 0100 0100 0100 0000 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| ++++- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ ++++ ----- tODT_idle_off: 4 cycles
  *    |||| |||| |||| |||| |||| +++------------ reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- tODTLon: 4 cycles
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| ++++--------------------- tAXPD: 4: 5 clocks
  *    |||| |||| ++++-------------------------- tANPD: 4, 5 clocks
  *    |||| |+++------------------------------- tAONPD: 4, 5 clocks
  *    ||++ +---------------------------------- tAOFPD: 4, 5 clocks
  *    ++-------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDCFG0,      0x898E79A5},
 /* MMDCx_MDCFG0
  * 0x    8    9    8    E    7    9    A    5
  * 0b 1000 1001 1000 1110 0111 1001 1010 0101
  *    |||| |||| |||| |||| |||| |||| |||| ++++- tCL: 5: 8 clocks
  *    |||| |||| |||| |||| |||| |||+ ++++------ tFAW: 0x1A: 26 clocks
  *    |||| |||| |||| |||| |||+ +++------------ tXPDLL: 0xc: 12 clocks
  *    |||| |||| |||| |||| +++----------------- tXP: 3: 4 cycles
  *    |||| |||| ++++ ++++--------------------- tXS: 0x8E: 142 clocks
  *    ++++ ++++------------------------------- tRFC: 0x89, 137 clocks
  */

 {MX6_MMDC_P0_MDCFG1,      0xDB530F64},
 /* MMDCx_MDCFG1
  * 0x    D    B    5    3    0    F    6    4
  * 0b 1101 1011 0101 0011 0000 1111 0110 0100
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCWL: 4, 5 clocks
  *    |||| |||| |||| |||| |||| |||| |||+ +---- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ +++------- tMRD: 0xB, 11 clocks
  *    |||| |||| |||| |||| |||| +++------------ tWR: 7, 8 clocks
  *    |||| |||| |||| |||| |+++---------------- reserved, write as 0
  *    |||| |||| |||| |||| +------------------- tRPA: 0, equal to tRP
  *    |||| |||| |||+ ++++--------------------- tRAS: 0x13, 20 clocks
  *    |||| ||++ +++--------------------------- tRC: 0x1A, 27 clocks
  *    |||+ ++--------------------------------- tRP: 0x6, 7 clocks
  *    +++------------------------------------- tRCD: 0x6, 7 clocks
  */

 {MX6_MMDC_P0_MDCFG2,      0x01FF00DD},
 /* MMDCx_MDCFG2
  * 0x    0    1    F    F    0    0    D    D
  * 0b 0000 0001 1111 1111 0000 0000 1101 1101
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tRRD: 5, 6 clocks
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tWTR: 3, 4 clocks
  *    |||| |||| |||| |||| |||| |||+ ++-------- tRTP: 3, 4 clocks
  *    |||| |||| |||| |||| ++++ +++------------ reserved, write as 0
  *    |||| |||+ ++++ ++++--------------------- tDLLK: 0x1FF, 512 cycles
  *    ++++ +++-------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDMISC,         0x00011740},
 /* MMDCx_MDMISC
  * 0x    0    0    0    1    1    7    4    0
  * 0b 0000 0000 0000 0001 0001 0111 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- RST: 0: No Reset requested
  *    |||| |||| |||| |||| |||| |||| |||| |+----LPDDR2_2CH: 0: 1 channel mode (DDR3)
  *    |||| |||| |||| |||| |||| |||| |||+ +---- DDR_TYPE: 0: DDR3 is used
  *    |||| |||| |||| |||| |||| |||| ||+------- DDR_4_BANK: 0: 8 banks device is used.
  *    |||| |||| |||| |||| |||| |||+ ++-------- RALAT: 5, 5 clocks
  *    |||| |||| |||| |||| |||| |++- ---------- MIF3_MODE: 3: prediction on all possibilities
  *    |||| |||| |||| |||| |||| +-------------- LPDDR2_S2: 0: LPDDR2-S4 device is used
  *    |||| |||| |||| |||| |||+---------------- BI_ON: 1: Banks are interleaved.
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- WALAT: 1, 1 clock additional delay
  *    |||| |||| |||| |+----------------------- LHD: 0: Latency hiding on, normal mode, debug feature off
  *    |||| |||| |||| +------------------------ ADDR_MIRROR: 0: Address mirroring disabled.
  *    |||| |||| |||+-------------------------- CALIB_PER_CS: 0: Calibration is targeted to CS0
  *    ||++ ++++ +++--------------------------- reserved, write as 0
  *    |+-------------------------------------- CS1_RDY: read only, write as 0
  *    +--------------------------------------- CS0_RDY: read only, write as 0
  */

 /* Set the Configuration request bit during MMDC set up */
 {MX6_MMDC_P0_MDSCR,       0x00008000},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    0    0
  * 0b 0000 0000 0000 1000 1000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 0: Normal operation
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDRWD,       0x000026D2},
 /* MMDCx_MDRWD
  * 0x    0    0    0    0    2    6    D    2
  * 0b 0000 0000 0000 0000 0010 0110 1101 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- RTR_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||| ||++ +---- RTW_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||+ ++-------- WTW_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| +++------------ WTR_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |+++---------------- RTW_SAME: 2, 2 clocks (default):
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||+ ++++ ++++ ++++--------------------- tDAI: Not used for DDR3
  *    +++------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDOR,        0x008E1023},
 /* MMDCx_MDOR
  * 0x    0    0    8    E    1    0    2    3
  * 0b 0000 0000 1000 1110 0001 0000 0010 0011
  *    |||| |||| |||| |||| |||| |||| ||++ ++++- RST_to_CKE: 0x23, 33 clocks
  *    |||| |||| |||| |||| |||| |||| ++-------- reserved, write as 0
  *    |||| |||| |||| |||| ||++ ++++----------- SDE_to_RST: 0x10, 14 cycles
  *    |||| |||| |||| |||| ++------------------ reserved, write as 0
  *    |||| |||| ++++ ++++--------------------- tXPR: 0x8E, 140 cycles
  *    ++++ ++++------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDASP,       0x00000047},
 /* MMDCx_MDASP
  * 0x    0    0    0    0    0    0    4    7
  * 0b 0000 0000 0000 0000 0000 0000 0100 0111
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- CS0_END: 0x47 = 0x40 + 0x7 = DDR_CS_SIZE/32MB + 0x7.
  *    ++++ ++++ ++++ ++++ ++++ ++++ +--------- reserved, write as 0
  */

 /* configure density and burst length */
 {MX6_MMDC_P0_MDCTL,       0x841A0000},
 /* MMDCx_MDCTL
  * 0x    8    4    1    A    0    0    0    0
  * 0b 1000 0100 0001 1010 0000 0000 0000 0000
  *    |||| |||| |||| |||| ++++ ++++ ++++ ++++- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- DSIZ: 2: 64-bit data bus
  *    |||| |||| |||| |+----------------------- reserved, write as 0
  *    |||| |||| |||| +------------------------ BL: 1: Burst length 8
  *    |||| |||| |+++ ------------------------- COL: 1: 10 bits column
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| |+++------------------------------- ROW: 4: 15 bits row
  *    ||++ +---------------------------------- reserved, write as 0
  *    |+-------------------------------------- SDE_1: 0: CS1 disabled
  *    +--------------------------------------- SDE_0: 1: CS0 enabled
  */

 {MX6_MMDC_P0_MDSCR,       0x04088032},
 /* MMDCx_MDSCR
  * 0x    0    4    0    8    8    0    3    2
  * 0b 0000 0100 0000 1000 1000 0000 0011 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 2: Bank address 2
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x08: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x04: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x00008033},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    3    3
  * 0b 0000 0000 0000 0000 1000 0000 0011 0011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 3: Bank address 3
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x00: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x00: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x02068031},
 /* MMDCx_MDSCR
  * 0x    0    2    0    6    8    0    3    1
  * 0b 0000 0010 0000 0110 1000 0000 0011 0001
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 1: Bank address 1
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x06: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x02: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x19408030},
 /* MMDCx_MDSCR
  * 0x    1    9    4    0    8    0    3    0
  * 0b 0001 1001 0100 0000 1000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x40: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x19: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x04008040},
 /* MMDCx_MDSCR
  * 0x    0    4    0    0    8    0    4    0
  * 0b 0000 0100 0000 0000 1000 0000 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 4: ZQ calibration
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x00: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x04: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDREF,       0x00005800},
 /* MMDCx_MDREF
  * 0x    0    0    0    0    5    8    0    0
  * 0b 0000 0000 0000 0000 0101 1000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- START_REF: 0: Do not start a manual refresh cycle
  *    |||| |||| |||| |||| |||| |+++ ++++ +++-- reserved, write as 0
  *    |||| |||| |||| |||| ||++ +-------------- REFR: 3: 4 refreshes every cycle
  *    |||| |||| |||| |||| ++------------------ REF_SEL: 1: Periodic refresh cycles with 32kHz.
  *    ++++ ++++ ++++ ++++--------------------- REF_CNT: 0: reserved
  * Note: REF_CNT with value 0 is reserved, but also reset value.
  */

 {MX6_MMDC_P0_MPODTCTRL,   0x00022227},
 {MX6_MMDC_P1_MPODTCTRL,   0x00022227},
 /* MMDCx_MPODTCTRL
  * 0x    0    0    0    2    2    2    2    7
  * 0b 0000 0000 0000 0010 0010 0010 0010 0111
  *    |||| |||| |||| |||| |||| |||| |||| |||+- ODT_WR_PAS_EN: 1: Inactive CS ODT pin is enabled during write
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- ODT_WR_ACT_EN: 1: Active CS ODT pin is enabled during write
  *    |||| |||| |||| |||| |||| |||| |||| |+--- ODT_RD_PAS_EN: 1: Inactive CS ODT pin is enabled during read
  *    |||| |||| |||| |||| |||| |||| |||| +---- ODT_RD_ACT_EN: 0: Active CS ODT pin is disabled during read
  *    |||| |||| |||| |||| |||| |||| |+++------ ODT0_INT_RES: 2: Rtt_Nom 60 Ohm
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT1_INT_RES: 2: Rtt_Nom 60 Ohm
  *    |||| |||| |||| |||| |||| +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- ODT2_INT_RES: 2: Rtt_Nom 60 Ohm
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| |+++--------------------- ODT3_INT_RES: 2: Rtt_Nom 60 Ohm
  *    ++++ ++++ ++++ +------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MDPDC,       0x00025576},
 /* MMDCx_MDPDC
  * 0x    0    0    0    2    5    5    7    6
  * 0b 0000 0000 0000 0010 0101 0101 0111 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 1: both chips selects idle.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of DDR3
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 5: 256 cycles for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 5: 256 cycles for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2, 3 cycles
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: Disabled
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MAPSR,       0x00011006},
 /* MMDCx_MAPSR
  * 0x    0    0    0    1    1    0    0    6
  * 0b 0000 0000 0000 0001 0001 0000 0000 0110
  *    |||| |||| |||| |||| |||| |||| |||| |||+- PSD: 0: Power saving enabled
  *    |||| |||| |||| |||| |||| |||| |||| +++-- reserved, write as 2
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

 /* switch DDR3 to normal operation */
 {MX6_MMDC_P0_MDSCR,       0x00000000},
 /* MMDCx_MDSCR
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
int ram_setup_valenka3_2gb_size = ARRAY_SIZE(ram_setup_valenka3_2gb);


/* Valenka3 4GB DDR3 */
m48_configuration ram_calibration_valenka3_4gb [] =
{

 {MX6_MMDC_P0_MPWLDECTRL0, 0x00250025},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x002D002A},

 {MX6_MMDC_P1_MPWLDECTRL0, 0x00210026},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x001B002B},

 {MX6_MMDC_P0_MPDGCTRL0,   0x42580262},
 /* MMDCx_MPDGCTRL0
  * 0x    4    2    5    8    0    2    6    2
  * 0b 0100 0010 0101 1000 0000 0010 0110 0010
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x62: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 2: 1 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x58: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 2: 1 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 0: Read DQS gating mechanism is enabled
  *    |+-------------------------------------- DG_CMP_CYC: 1: MMDC waits 32 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P0_MPDGCTRL1,   0x0257024E},
 /* MMDCx_MPDGCTRL1
  * 0x    0    2    5    7    0    2    4    E
  * 0b 0000 0010 0101 0111 0000 0010 0100 1110
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x4E: read gating delay offset for Byte 2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 2: 1 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x57: read gating delay offset for Byte 3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 2: 1 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P1_MPDGCTRL0,   0x42580264},
 /* MMDCx_MPDGCTRL0
  * 0x    4    2    5    8    0    2    6    4
  * 0b 0100 0010 0101 1000 0000 0010 0110 0100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET0: 0x64: read gating delay offset for Byte 0
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL0: 2: 1 cycle delay
  *    |||| |||| |||| |||| |||+---------------- HW_DG_ERR: 0: read only, write as 0
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET0: 0x58: read gating delay offset for Byte 1
  *    |||| |||| +----------------------------- DG_EXT_UP: 0: extend upper boundary
  *    |||| ++++------------------------------- DG_HC_DEL1: 2: 1 cycle delay
  *    |||+------------------------------------ HW_DG_EN: 0: Disable automatic read DQS gating calibration
  *    ||+------------------------------------- DG_DIS: 0: Read DQS gating mechanism is disabled
  *    |+-------------------------------------- DG_CMP_CYC: 1: MMDC waits 16 DDR cycles
  *    +--------------------------------------- RST_RD_FIFO: 0: Do not reset read data FIFO
  */

 {MX6_MMDC_P1_MPDGCTRL1,   0x02550234},
 /* MMDCx_MPDGCTRL1
  * 0x    0    2    5    5    0    2    3    4
  * 0b 0000 0010 0101 0101 0000 0010 0011 0100
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- DG_DL_ABS_OFFSET2: 0x34: read gating delay offset for Byte 2
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ++++----------- DG_HC_DEL2: 2: 1 cycle delay
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- DG_DL_ABS_OFFSET3: 0x55: read gating delay offset for Byte 3
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| ++++------------------------------- DG_HC_DEL3: 2: 1 cycle delay
  *    ++++------------------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MPRDDLCTL,   0x453B3A3D},
 /* MMDC0_MPRDDLCTL
  * 0x    4    5    3    B    3    A    3    D
  * 0b 0100 0101 0011 1011 0011 1010 0011 1101
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- RD_DL_ABS_OFFSET0: 0x3D: read delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- RD_DL_ABS_OFFSET1: 0x3A: read delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- RD_DL_ABS_OFFSET2: 0x3B: read delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- RD_DL_ABS_OFFSET3: 0x45: read delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MPRDDLCTL,   0x403E3A43},
 /* MMDC1_MPRDDLCTL
  * 0x    4    0    3    E    3    A    4    3
  * 0b 0100 0000 0011 1110 0011 1010 0100 0011
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- RD_DL_ABS_OFFSET0: 0x43: read delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- RD_DL_ABS_OFFSET1: 0x3A: read delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- RD_DL_ABS_OFFSET2: 0x3E: read delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- RD_DL_ABS_OFFSET3: 0x40: read delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MPWRDLCTL,   0x383B413B},
 /* MMDC0_MPWRDLCTL
  * 0x    3    8    3    B    4    1    3    B
  * 0b 0011 1000 0011 1011 0100 0001 0011 1011
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WR_DL_ABS_OFFSET0: 0x3B: write delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- WR_DL_ABS_OFFSET1: 0x41: write delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WR_DL_ABS_OFFSET2: 0x3B: write delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- WR_DL_ABS_OFFSET3: 0x38: write delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P1_MPWRDLCTL,   0x4335443E},
 /* MMDC1_MPWRDLCTL
  * 0x    4    3    3    5    4    4    3    E
  * 0b 0100 0011 0011 0101 0100 0100 0011 1110
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- WR_DL_ABS_OFFSET0: 0x3E: write delay offset for Byte 0/4
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |+++ ++++----------- WR_DL_ABS_OFFSET1: 0x44: write delay offset for Byte 1/5
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |+++ ++++--------------------- WR_DL_ABS_OFFSET2: 0x35: write delay offset for Byte 2/6
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |+++ ++++------------------------------- WR_DL_ABS_OFFSET3: 0x43: write delay offset for Byte 3/7
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P0_MPRDDQBY3DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY0DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY1DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY2DL, 0x33333333},
 {MX6_MMDC_P1_MPRDDQBY3DL, 0x33333333},
 /* MMDCx_MPRDDQBY0DL
  * 0x    3    3    3    3    3    3    3    3
  * 0b 0011 0011 0011 0011 0011 0011 0011 0011
  *    |... |... |... |... |... |... |... |+++- wr_dqx_del 3: for all eight nibbles: Add 3 delay units. This is the
  *                                             recommended value even though the reset value is 0.
  */

  {MX6_MMDC_P0_MPDCCR, 0x24921492}, /* MMDC1_MPDCCR */
  {MX6_MMDC_P1_MPDCCR, 0x24921492}, /* MMDC2_MPDCCR */
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

 {MX6_MMDC_P0_MPMUR0,      0x00000800},
 {MX6_MMDC_P1_MPMUR0,      0x00000800},
 /* MMDCx_MPMUR0
  * 0x    0    0    0    0    0    8    0    0
  * 0b 0000 0000 0000 0000 0000 1000 0000 0000
  *    |||| |||| |||| |||| |||| ||++ ++++ ++++- MU_BYP_VAL: 0: No delay for bypass measurement (debug only)
  *    |||| |||| |||| |||| |||| |+------------- MU_BYP_EN: 0: Use MU_UNIT_DEL_NUM for delay
  *    |||| |||| |||| |||| |||| +-------------- FRC_MSR: 1: complete calibration, latch delay values in PHY.
  *    |||| |||| |||| |||| ++++---------------- reserved, write as 0
  *    |||| ||++ ++++ ++++--------------------- MU_UNIT_DEL_NUM: read only, write as 0
  *    ++++ ++--------------------------------- reserved, write as 0
  */

};
int ram_calibration_valenka3_4gb_size = ARRAY_SIZE(ram_calibration_valenka3_4gb);

m48_configuration ram_setup_valenka3_4gb [] =
{

 {MX6_MMDC_P0_MDPDC,       0x00020036},
 /* MMDCx_MDPDC
  * 0x    0    0    0    2    0    0    3    6
  * 0b 0000 0000 0000 0010 0000 0000 0011 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 0: independent. Only one CS is used anyway.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of LPDDR2
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 0: No Power Down Time-out for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 0: No Power Down Time-out for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2: 3 cycles
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: 0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: 0: Disabled
  *    +--------------------------------------- reserved, write as 0
  * Note: PWDT_x: Why no power down time out? MDPDC is written again at the end of the initialisation. A timeout is set
  *       in this last write.
  */

 {MX6_MMDC_P0_MDOTC,       0x24444040},
 /* MMDCx_MDOTC
  * 0x    2    4    4    4    4    0    4    0
  * 0b 0010 0100 0100 0100 0100 0000 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| ++++- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ ++++ ----- tODT_idle_off: 4 cycles
  *    |||| |||| |||| |||| |||| +++------------ reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- tODTLon: 4 cycles
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| ++++--------------------- tAXPD: 4: 5 clocks
  *    |||| |||| ++++-------------------------- tANPD: 4, 5 clocks
  *    |||| |+++------------------------------- tAONPD: 4, 5 clocks
  *    ||++ +---------------------------------- tAOFPD: 4, 5 clocks
  *    ++-------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDCFG0,      0xB8BE79A5},
 /* MMDCx_MDCFG0
  * 0x    B    8    B    E    7    9    A    5
  * 0b 1011 1000 1011 1110 0111 1001 1010 0101
  *    |||| |||| |||| |||| |||| |||| |||| ++++- tCL: 5: 8 clocks
  *    |||| |||| |||| |||| |||| |||+ ++++------ tFAW: 0x1A: 27 clocks
  *    |||| |||| |||| |||| |||+ +++------------ tXPDLL: 0xC: 13 clocks
  *    |||| |||| |||| |||| +++----------------- tXP: 3: 4 cycles
  *    |||| |||| ++++ ++++--------------------- tXS: 0xBE: 191 clocks
  *    ++++ ++++------------------------------- tRFC: 0xB8, 185 clocks
  */

 {MX6_MMDC_P0_MDCFG1,      0xDB530F64},
 /* MMDCx_MDCFG1
  * 0x    D    B    5    3    0    F    6    4
  * 0b 1101 1011 0101 0011 0000 1111 0110 0100
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCWL: 4, 5 clocks
  *    |||| |||| |||| |||| |||| |||| |||+ +---- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||+ +++------- tMRD: 0xB, 12 clocks
  *    |||| |||| |||| |||| |||| +++------------ tWR: 7, 8 clocks
  *    |||| |||| |||| |||| |+++---------------- reserved, write as 0
  *    |||| |||| |||| |||| +------------------- tRPA: 0, equal to tRP
  *    |||| |||| |||+ ++++--------------------- tRAS: 0x13, 20 clocks
  *    |||| ||++ +++--------------------------- tRC: 0x1A, 27 clocks
  *    |||+ ++--------------------------------- tRP: 0x6, 7 clocks
  *    +++------------------------------------- tRCD: 0x6, 7 clocks
  */

 {MX6_MMDC_P0_MDCFG2,      0x01FF00DD},
 /* MMDCx_MDCFG2
  * 0x    0    1    F    F    0    0    D    D
  * 0b 0000 0001 1111 1111 0000 0000 1101 1101
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tRRD: 5, 6 clocks
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tWTR: 3, 4 clocks
  *    |||| |||| |||| |||| |||| |||+ ++-------- tRTP: 3, 4 clocks
  *    |||| |||| |||| |||| ++++ +++------------ reserved, write as 0
  *    |||| |||+ ++++ ++++--------------------- tDLLK: 0x1FF, 512 cycles
  *    ++++ +++-------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDMISC,         0x00011740},
 /* MMDCx_MDMISC
  * 0x    0    0    0    1    1    7    4    0
  * 0b 0000 0000 0000 0001 0001 0111 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- reserved, write as 0
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- RST: 0: No Reset requested
  *    |||| |||| |||| |||| |||| |||| |||| |+----LPDDR2_2CH: 0: 1 channel mode (DDR3)
  *    |||| |||| |||| |||| |||| |||| |||+ +---- DDR_TYPE: 0: DDR3 is used
  *    |||| |||| |||| |||| |||| |||| ||+------- DDR_4_BANK: 0: 8 banks device is used.
  *    |||| |||| |||| |||| |||| |||+ ++-------- RALAT: 5, 5 clocks
  *    |||| |||| |||| |||| |||| |++- ---------- MIF3_MODE: 3: prediction on all possibilities
  *    |||| |||| |||| |||| |||| +-------------- LPDDR2_S2: 0: LPDDR2-S4 device is used
  *    |||| |||| |||| |||| |||+---------------- BI_ON: 1: Banks are interleaved.
  *    |||| |||| |||| |||| +++----------------- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- WALAT: 1, 1 clock additional delay
  *    |||| |||| |||| |+----------------------- LHD: 0: Latency hiding on, normal mode, debug feature off
  *    |||| |||| |||| +------------------------ ADDR_MIRROR: 0: Address mirroring disabled.
  *    |||| |||| |||+-------------------------- CALIB_PER_CS: 0: Calibration is targeted to CS0
  *    ||++ ++++ +++--------------------------- reserved, write as 0
  *    |+-------------------------------------- CS1_RDY: read only, write as 0
  *    +--------------------------------------- CS0_RDY: read only, write as 0
  */

 /* Set the Configuration request bit during MMDC set up */
 {MX6_MMDC_P0_MDSCR,       0x00008000},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    0    0
  * 0b 0000 0000 0000 0000 1000 0000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 0: Normal operation
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: read only, write as 0, not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDRWD,       0x000026D2},
 /* MMDCx_MDRWD
  * 0x    0    0    0    0    2    6    D    2
  * 0b 0000 0000 0000 0000 0010 0110 1101 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- RTR_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||| ||++ +---- RTW_DIFF: 2, 2 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| |||+ ++-------- WTW_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |||| +++------------ WTR_DIFF: 3, 3 clocks (default) should not apply because only CS0 is used
  *    |||| |||| |||| |||| |+++---------------- RTW_SAME: 2, 2 clocks (default):
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||+ ++++ ++++ ++++--------------------- tDAI: Not used for DDR3
  *    +++------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDOR,        0x00BE1023},
 /* MMDCx_MDOR
  * 0x    0    0    B    E    1    0    2    3
  * 0b 0000 0000 1011 1110 0001 0000 0010 0011
  *    |||| |||| |||| |||| |||| |||| ||++ ++++- RST_to_CKE: 0x23, 33 clocks
  *    |||| |||| |||| |||| |||| |||| ++-------- reserved, write as 0
  *    |||| |||| |||| |||| ||++ ++++----------- SDE_to_RST: 0x10, 14 cycles
  *    |||| |||| |||| |||| ++------------------ reserved, write as 0
  *    |||| |||| ++++ ++++--------------------- tXPR: 0xBE, 191 cycles
  *    ++++ ++++------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MDASP,       0x0000007F},
 /* MMDCx_MDASP
  * 0x    0    0    0    0    0    0    7    F
  * 0b 0000 0000 0000 0000 0000 0000 0111 1111
  *    |||| |||| |||| |||| |||| |||| |+++ ++++- CS0_END: 0x7F = 0x78 + 0x7 = DDR_CS_SIZE/32MB + 0x7. (Usable memory is roughly 3.75GB)
  *    ++++ ++++ ++++ ++++ ++++ ++++ +--------- reserved, write as 0
  */

 /* configure density and burst length */
 {MX6_MMDC_P0_MDCTL,       0x851A0000},
 /* MMDCx_MDCTL
  * 0x    8    5    1    A    0    0    0    0
  * 0b 1000 0101 0001 1010 0000 0000 0000 0000
  *    |||| |||| |||| |||| ++++ ++++ ++++ ++++- reserved, write as 0
  *    |||| |||| |||| ||++--------------------- DSIZ: 2: 64-bit data bus
  *    |||| |||| |||| |+----------------------- reserved, write as 0
  *    |||| |||| |||| +------------------------ BL: 1: Burst length 8
  *    |||| |||| |+++ ------------------------- COL: 1: 10 bits column
  *    |||| |||| +----------------------------- reserved, write as 0
  *    |||| |+++------------------------------- ROW: 5: 16 bits row
  *    ||++ +---------------------------------- reserved, write as 0
  *    |+-------------------------------------- SDE_1: 0: CS1 disabled
  *    +--------------------------------------- SDE_0: 1: CS0 enabled
  */

 {MX6_MMDC_P0_MDSCR,       0x04088032},
 /* MMDCx_MDSCR
  * 0x    0    4    0    8    8    0    3    2
  * 0b 0000 0100 0000 1000 1000 0000 0011 0010
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 2: Bank address 2
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: 0: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x08: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x04: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x00008033},
 /* MMDCx_MDSCR
  * 0x    0    0    0    0    8    0    3    3
  * 0b 0000 0000 0000 0000 1000 0000 0011 0011
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 3: Bank address 3
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x00: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x00: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x02068031},
 /* MMDCx_MDSCR
  * 0x    0    2    0    6    8    0    3    1
  * 0b 0000 0010 0000 0110 1000 0000 0011 0001
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 1: Bank address 1
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x06: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x02: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x19408030},
 /* MMDCx_MDSCR
  * 0x    1    9    4    0    8    0    3    0
  * 0b 0001 1001 0100 0000 1000 0000 0011 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 3: Load Mode Register
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x40: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x19: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDSCR,       0x04008040},
 /* MMDCx_MDSCR
  * 0x    0    4    0    0    8    0    4    0
  * 0b 0000 0100 0000 0000 1000 0000 0100 0000
  *    |||| |||| |||| |||| |||| |||| |||| |+++- CMD_BA: 0: Bank address 0
  *    |||| |||| |||| |||| |||| |||| |||| +---- CMD_CS: 0: Chip select 0
  *    |||| |||| |||| |||| |||| |||| |+++------ CMD: 4: ZQ calibration
  *    |||| |||| |||| |||| |||| |||+ +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| ||+------------ WL_EN: 0: stay in normal mode
  *    |||| |||| |||| |||| |||| |+------------- MRR_READ_DATA_VALID: not relevant for DDR3
  *    |||| |||| |||| |||| ||++ +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+------------------ CON_ACK: 0: read only, write as 0
  *    |||| |||| |||| |||| +------------------- CON_REQ: 1: Request to configure MMDC
  *    |||| |||| ++++ ++++--------------------- CMD_ADDR_LSB_MR_ADDR: 0x00: LSB of Command/Address
  *    ++++ ++++------------------------------- CMD_ADDR_MSB_MR_OP: 0x04: MSB of Command/Address
  */

 {MX6_MMDC_P0_MDREF,       0x00005800},
 /* MMDCx_MDREF
  * 0x    0    0    0    0    5    8    0    0
  * 0b 0000 0000 0000 0000 0101 1000 0000 0000
  *    |||| |||| |||| |||| |||| |||| |||| |||+- START_REF: 0: Do not start a manual refresh cycle
  *    |||| |||| |||| |||| |||| |+++ ++++ +++-- reserved, write as 0
  *    |||| |||| |||| |||| ||++ +-------------- REFR: 3: 4 refreshes every cycle
  *    |||| |||| |||| |||| ++------------------ REF_SEL: 1: Periodic refresh cycles with 32kHz.
  *    ++++ ++++ ++++ ++++--------------------- REF_CNT: 0: reserved
  * Note: REF_CNT with value 0 is reserved, but also reset value.
  */

 {MX6_MMDC_P0_MPODTCTRL,   0x00011117},
 {MX6_MMDC_P1_MPODTCTRL,   0x00011117},
 /* MMDCx_MPODTCTRL
  * 0x    0    0    0    1    1    1    1    7
  * 0b 0000 0000 0000 0001 0001 0001 0001 0111
  *    |||| |||| |||| |||| |||| |||| |||| |||+- ODT_WR_PAS_EN: 1: Inactive CS ODT pin is enabled during write
  *    |||| |||| |||| |||| |||| |||| |||| ||+-- ODT_WR_ACT_EN: 1: Active CS ODT pin is enabled during write
  *    |||| |||| |||| |||| |||| |||| |||| |+--- ODT_RD_PAS_EN: 1: Inactive CS ODT pin is enabled during read
  *    |||| |||| |||| |||| |||| |||| |||| +---- ODT_RD_ACT_EN: 0: Active CS ODT pin is disabled during read
  *    |||| |||| |||| |||| |||| |||| |+++------ ODT0_INT_RES: 1: Rtt_Nom 120 Ohm
  *    |||| |||| |||| |||| |||| |||| +--------- reserved, write as 0
  *    |||| |||| |||| |||| |||| |+++----------- ODT1_INT_RES: 1: Rtt_Nom 120 Ohm
  *    |||| |||| |||| |||| |||| +-------------- reserved, write as 0
  *    |||| |||| |||| |||| |+++---------------- ODT2_INT_RES: 1: Rtt_Nom 120 Ohm
  *    |||| |||| |||| |||| +------------------- reserved, write as 0
  *    |||| |||| |||| |+++--------------------- ODT3_INT_RES: 1: Rtt_Nom 120 Ohm
  *    ++++ ++++ ++++ +------------------------ reserved, write as 0
  */

 {MX6_MMDC_P0_MDPDC,       0x00025576},
 /* MMDCx_MDPDC
  * 0x    0    0    0    2    5    5    7    6
  * 0b 0000 0000 0000 0010 0101 0101 0111 0110
  *    |||| |||| |||| |||| |||| |||| |||| |+++- tCKSRE: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| ||++ +---- tCKSRX: 6: 6 cycles
  *    |||| |||| |||| |||| |||| |||| |+-------- BOTH_CS_PD: 1: both chips selects idle.
  *    |||| |||| |||| |||| |||| |||| +--------- SLOW_PD: 0: Fast mode. Must match with MR0 of DDR3
  *    |||| |||| |||| |||| |||| ++++----------- PWDT_0: 5: 256 cycles for CS0
  *    |||| |||| |||| |||| ++++---------------- PWDT_1: 5: 256 cycles for CS1
  *    |||| |||| |||| |+++--------------------- tCKE: 2, 3 cycles
  *    |||| |||| ++++ +------------------------ reserved, write as 0
  *    |||| |+++ ------------------------------ PRCT_0: Disabled
  *    |||| +---------------------------------- reserved, write as 0
  *    |+++------------------------------------ PRCT_1: Disabled
  *    +--------------------------------------- reserved, write as 0
  */

 {MX6_MMDC_P0_MAPSR,       0x00011006},
 /* MMDCx_MAPSR
  * 0x    0    0    0    1    1    0    0    6
  * 0b 0000 0000 0000 0001 0001 0000 0000 0110
  *    |||| |||| |||| |||| |||| |||| |||| |||+- PSD: 0: Power saving enabled
  *    |||| |||| |||| |||| |||| |||| |||| +++-- reserved, write as 2
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

 /* switch DDR3 to normal operation */
 {MX6_MMDC_P0_MDSCR,       0x00000000},
 /* MMDCx_MDSCR
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
int ram_setup_valenka3_4gb_size = ARRAY_SIZE(ram_setup_valenka3_4gb);

