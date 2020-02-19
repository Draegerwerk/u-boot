/*
 * mx6m48q_spl_cfg.c
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

#define CONFIG_MX6Q
#include <common.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6q-ddr.h>
#include "mx6m48_spl_cfg.h"

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

 {MX6_IOM_DRAM_RESET,      0x00000030},
 {MX6_IOM_DRAM_SDCKE0,     0x00020000},
 {MX6_IOM_DRAM_SDCKE1,     0x00020000},

 {MX6_IOM_DRAM_SDODT0,     0x00000030},
 {MX6_IOM_DRAM_SDODT1,     0x00000030},

 {MX6_IOM_DDRMODE_CTL,     0x00020000},
 {MX6_IOM_GRP_DDRMODE,     0x00020000},
 {MX6_IOM_GRP_DDRPKE,      0x00000000},
 {MX6_IOM_DRAM_SDBA0,      0x00000000},
 {MX6_IOM_DRAM_SDBA1,      0x00000000},
 {MX6_IOM_DRAM_SDBA2,      0x00000000},
 {MX6_IOM_GRP_DDR_TYPE,    0x000C0000},

 /***********************************/
 /* DDR Controller                  */
 /***********************************/

 /* MDSCR    con_req */
 {MX6_MMDC_P0_MDSCR,       0x00008000},

 {MX6_MMDC_P0_MPZQHWCTRL,  0xA139001F},
 {MX6_MMDC_P1_MPZQHWCTRL,  0xA139001F},

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

 /* A recent recommendation was added for i.mx6DQ Rev C or later Devices
 This has been seen to improve measured duty cycle by setting fine tune SDCLK duty cyc to low.
 If you are not experiencing issues, you could leave this commented out. (added by NXP)
 */
  {0x021b08c0, 0x24921492}, /* MMDC1_MPDCCR */
  {0x021b48c0, 0x24921492}, /* MMDC2_MPDCCR */

 /* CL: 13,91 ns * 528 MHZ = 7,34 -> 8 CK */

 {MX6_MMDC_P0_MDPDC,       0x00020036},

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

 /* RCD=8; RP=8; RC=26; RAS=20; RPA=1; WR=8->SDRAM MR0; */
 /* MRD=12; CWL=6 -> SDRAM MR2 */
 {MX6_MMDC_P0_MDCFG1,      0xFF328F64},
 /* RRD=4; WTR=4; RTP=4; DLLK=512 */
 {MX6_MMDC_P0_MDCFG2,      0x01FF00DB},
 /* ODT_idle_off=4 (AL=0); ODTLon=4; AXPD=5 (CWL-1) */
 /* ANPD=5 (CWL-1); AONPD=3; AOFPD=3 */
 {MX6_MMDC_P0_MDOTC,       0x24444040},

 /* RL=AL+CL */
 /* RTW=AL+CL+CCD-CWL+2CK=0+8+4-6+2=8  */
 /* RTW_SAME = RTW-AL-(CL-CWL)-BL/2 = 8-0-(8-6)-(8/2) = 2 */
 {MX6_MMDC_P0_MDRWD,       0x000026D2},

 /* CALIB_PER_CS=0 (CS0); ADDR_MIRROR=1; LHD=0; WALAT=0 */
 /* BI_ON=1; LPDDR2_S2=0; MIF3_MODE=3; RALAT=0 (AL) */
 /* DDR_4_BANK=0; DDR_TYPE=0; LPDDR2_2CH=0; RST=0; */
 {MX6_MMDC_P0_MDMISC,         0x0011740},

 /* XPR=RFC+10ns=138+6=144; SDE_to_RST=14(JEDEC); RST_toCKE=33 (JEDEC); */
 {MX6_MMDC_P0_MDOR,        0x008E1023},
 {MX6_MMDC_P0_MDASP,       0x00000047},

 /* configure density and burst length */
 {MX6_MMDC_P0_MDCTL,       0x041A0000},

 /* start ZQ calibration */
 {MX6_MMDC_P0_MDCTL,       0x841A0000},
 {MX6_MMDC_P0_MDSCR,       0x04088032},
 {MX6_MMDC_P0_MDSCR,       0x00008033},
 /* SDRAM MR1: AL=0 == MDCFG2 */
 {MX6_MMDC_P0_MDSCR,       0x00428031},
 /* SDRAM MR0: CL=8 == MDPDC */
 {MX6_MMDC_P0_MDSCR,       0x19408030},
 {MX6_MMDC_P0_MDSCR,       0x04008040},

 {MX6_MMDC_P0_MDPDC,       0x00025576},
 {MX6_MMDC_P0_MAPSR,       0x00001006},

 {MX6_MMDC_P0_MDREF,       0x00001800},

 {MX6_MMDC_P0_MPODTCTRL,   0x00022227},
 {MX6_MMDC_P1_MPODTCTRL,   0x00022227},

 {MX6_MMDC_P0_MPMUR0,      0x00000800},
 {MX6_MMDC_P1_MPMUR0,      0x00000800},

 {MX6_MMDC_P0_MDSCR,       0x00000000},
};
int ram_setup_mx6q_size = ARRAY_SIZE(ram_setup_mx6q);


m48_configuration ram_calibration_mx6q_06 [] =
{

 {MX6_MMDC_P0_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x001F001F},

 {MX6_MMDC_P0_MPDGCTRL0,   0x42640302},
 {MX6_MMDC_P0_MPDGCTRL1,   0x026A0266},
 {MX6_MMDC_P1_MPDGCTRL0,   0x42640273},
 {MX6_MMDC_P1_MPDGCTRL1,   0x02640264},
 {MX6_MMDC_P0_MPRDDLCTL,   0x463A383C},
 {MX6_MMDC_P1_MPRDDLCTL,   0x403C3848},
 {MX6_MMDC_P0_MPWRDLCTL,   0x3C3C4240},
 {MX6_MMDC_P1_MPWRDLCTL,   0x46324A44},
};
int ram_calibration_mx6q_size_06 = ARRAY_SIZE(ram_calibration_mx6q_06);

m48_configuration ram_calibration_mx6q_08 [] =
{
 {MX6_MMDC_P0_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x001F001F},

 {MX6_MMDC_P0_MPDGCTRL0,   0x434A0352},
 {MX6_MMDC_P0_MPDGCTRL1,   0x033E033F},
 {MX6_MMDC_P1_MPDGCTRL0,   0x4327033E},
 {MX6_MMDC_P1_MPDGCTRL1,   0x03190242},
 {MX6_MMDC_P0_MPRDDLCTL,   0x42363839},
 {MX6_MMDC_P1_MPRDDLCTL,   0x3A3A3440},
 {MX6_MMDC_P0_MPWRDLCTL,   0x3A3E4440},
 {MX6_MMDC_P1_MPWRDLCTL,   0x45344741},
};
int ram_calibration_mx6q_size_08 = ARRAY_SIZE(ram_calibration_mx6q_08);

