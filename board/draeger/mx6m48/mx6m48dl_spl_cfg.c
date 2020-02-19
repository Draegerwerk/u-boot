/*
 * mx6m48dl_spl_cfg.c
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

#define CONFIG_MX6DL
#include <common.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6dl-ddr.h>
#include "mx6m48_spl_cfg.h"

m48_configuration ddr_setup_mx6dl [] =
{
 /***********************************/
 /* IOMUX                           */
 /***********************************/
  {MX6_IOM_DRAM_SDQS0, 0x00000030},
 {MX6_IOM_DRAM_SDQS1, 0x00000030},
 {MX6_IOM_DRAM_SDQS2, 0x00000030},
 {MX6_IOM_DRAM_SDQS3, 0x00000030},
 {MX6_IOM_DRAM_SDQS4, 0x00000030},
 {MX6_IOM_DRAM_SDQS5, 0x00000030},
 {MX6_IOM_DRAM_SDQS6, 0x00000030},
 {MX6_IOM_DRAM_SDQS7, 0x00000030},

 {MX6_IOM_GRP_B0DS, 0x00000030},
 {MX6_IOM_GRP_B1DS, 0x00000030},
 {MX6_IOM_GRP_B2DS, 0x00000030},
 {MX6_IOM_GRP_B3DS, 0x00000030},
 {MX6_IOM_GRP_B4DS, 0x00000030},
 {MX6_IOM_GRP_B5DS, 0x00000030},
 {MX6_IOM_GRP_B6DS, 0x00000030},
 {MX6_IOM_GRP_B7DS, 0x00000030},
 {MX6_IOM_GRP_ADDDS, 0x00000030},
 {MX6_IOM_GRP_CTLDS, 0x00000030},

 {MX6_IOM_DRAM_DQM0, 0x00000030},
 {MX6_IOM_DRAM_DQM1, 0x00000030},
 {MX6_IOM_DRAM_DQM2, 0x00000030},
 {MX6_IOM_DRAM_DQM3, 0x00000030},
 {MX6_IOM_DRAM_DQM4, 0x00000030},
 {MX6_IOM_DRAM_DQM5, 0x00000030},
 {MX6_IOM_DRAM_DQM6, 0x00000030},
 {MX6_IOM_DRAM_DQM7, 0x00000030},

 {MX6_IOM_DRAM_CAS, 0x00000030},
 {MX6_IOM_DRAM_RAS, 0x00000030},
 {MX6_IOM_DRAM_SDCLK_0, 0x00000030},
 {MX6_IOM_DRAM_SDCLK_1, 0x00000030},

 {MX6_IOM_DRAM_RESET, 0x00000030},
 {MX6_IOM_DRAM_SDCKE0, 0x00020000},
 {MX6_IOM_DRAM_SDCKE1, 0x00020000},

 {MX6_IOM_DRAM_SDODT0, 0x00000030},
 {MX6_IOM_DRAM_SDODT1, 0x00000030},

 {MX6_IOM_DDRMODE_CTL, 0x00020000},
 {MX6_IOM_GRP_DDRMODE, 0x00020000},
 {MX6_IOM_GRP_DDRPKE, 0x00000000},
 {MX6_IOM_DRAM_SDBA0, 0x00000000},
 {MX6_IOM_DRAM_SDBA1, 0x00000000},
 {MX6_IOM_DRAM_SDBA2, 0x00000000},
 {MX6_IOM_GRP_DDR_TYPE, 0x000C0000},

 /***********************************/
 /* DDR Controller                  */
 /***********************************/

 /* MDSCR    con_req */
 {MX6_MMDC_P0_MDSCR, 0x00008000},

 {MX6_MMDC_P0_MPZQHWCTRL, 0xA139001F},
 {MX6_MMDC_P1_MPZQHWCTRL, 0xA139001F},
/* MMDCx_MPZQHWCTRL p. 3998 in [M48_DS_IMX_RMDL_4]
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
int ddr_setup_mx6dl_size = ARRAY_SIZE(ddr_setup_mx6dl);


m48_configuration ram_setup_mx6dl [] =
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
 {0x021b08c0, 0x24922492}, /* MMDC1_MPDCCR */
 {0x021b48c0, 0x24922492}, /* MMDC2_MPDCCR */

 /* CL: 13,91 ns * 396 MHZ = 5,51 -> 6 CK */

 {MX6_MMDC_P0_MDPDC,  0x0002002D},

 /* tRFC(4GB)=243*1.07ns=260ns */
 /* RFC=260ns / 396 MHz = 103 */
 /* RFC=103; XS=107; XP=3; XPDLL=10; FAW=16; CL=6 -> SDRAM MR0;  */
 {MX6_MMDC_P0_MDCFG0, 0x666B52F3},

 {MX6_MMDC_P0_MDCFG1, 0xB66D8B63},
/* MMDCx_MDCFG1 p. 3962 in [M48_DS_IMX_RMDL_4]
 * 0x    B    6    6    D    8    B    6    3
 * 0b 1011 0110 0110 1101 1000 1011 0110 0011
 *    |||| |||| |||| |||| |||| |||| |||| |+++- tCWL: 3, 5 clocks: tCK = 2.525ns. Must match with MR2 of DDR3
 *                                             [M48_DS_DDR3_MICRON_R] p. 78: CL=6, CWL=5
 *                                             [M48_DS_DDR3_ISSI_I1] p. 56: CL=6, CWL=5
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: CL=6, CWL=5
 *    |||| |||| |||| |||| |||| |||| |||+ +---- reserved, write as 0
 *    |||| |||| |||| |||| |||| |||+ +++------- tMRD: 0xB, 12 clocks: max of 12CK or 15ns. 12 * 2.525ns is > 15ns
 *                                             Set to max(tMRD, tMOD). tMOD is higher for all memories
 *                                             [M48_DS_DDR3_MICRON_R] p. 96: tMOD max(12CK, 15ns)
 *                                             [M48_DS_DDR3_ISSI_I1] p. 64: tMOD max(12CK, 15ns)
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35: tMOD max(12CK, 15ns)
 *    |||| |||| |||| |||| |||| +++------------ tWR: 5, 6 clocks: 15ns. 6 * 2.525 > 15ns. Must match with MR0 of DDR3
 *                                             [M48_DS_DDR3_MICRON_R] p. 96
 *                                             [M48_DS_DDR3_ISSI_I1] p. 64
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35
 *    |||| |||| |||| |||| ++++---------------- reserved, write as 0x8, see Note.
 *    |||| |||| |||+ ++++--------------------- tRAS: 0xD, 14 clocks: 14 * 2.525 > 35ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 81: 34ns
 *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 35ns
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 35ns
 *    |||| ||++ +++--------------------------- tRC: 0x13, 20 clocks: 20 * 2.525 > 48.75ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 81: 47.91ns
 *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 48.75ns
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 48.75ns
 *    |||+ ++--------------------------------- tRP: 0x5, 6 clocks: 6 * 2.525 > 13.91ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 81: 13.91ns
 *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 13.75ns
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 13.75ns
 *    +++------------------------------------- tRCD: 0x5, 6 clocks: 6 * 2.525 > 13.91ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 81: 13.91ns
 *                                             [M48_DS_DDR3_ISSI_I1] p. 56: 13.75ns
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 32: 13.75ns
 * Note: [M48_DS_IMX_RMDL_4] states a reset value of 0x8 for the reserved bits, but says in the table that it always has
 * the value of 0. [M48_DS_IMX_SCR_DL] also writes a 0x8.
 */

 {MX6_MMDC_P0_MDCFG2, 0x01FF00DB},
/* MMDCx_MDCFG2 p. 3965 in [M48_DS_IMX_RMDL_4]
 * 0x    0    1    F    F    0    0    D    B
 * 0b 0000 0001 1111 1111 0000 0000 1101 1011
 *    |||| |||| |||| |||| |||| |||| |||| |+++- tRRD: 3, 4 clocks: max of 4CK or 7.5ns. 4 * 2.525ns > 7.5ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 96: max of 4CK or 6ns
 *                                             [M48_DS_DDR3_ISSI_I1] p. 64: max of 4CK or 6ns
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 34: max of 4CK or 7.5ns
 *    |||| |||| |||| |||| |||| |||| ||++ +---- tWTR: 3, 4 clocks: max of 4CK or 7.5ns. 4 * 2.525ns > 7.5ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 96: max of 4CK or 7.5ns
 *                                             [M48_DS_DDR3_ISSI_I1] p. 64: max of 4CK or 7.5ns
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 35: max of 4CK or 7.5ns
 *    |||| |||| |||| |||| |||| |||+ ++-------- tRTP: 3, 4 clocks: max of 4CK or 7.5ns. 4 * 2.525ns > 7.5ns
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

 {MX6_MMDC_P0_MDOTC,  0x1B333040},
/* MMDCx_MDOTC p. 3959 in [M48_DS_IMX_RMDL_4]
 * 0x    1    B    3    3    3    0    4    0
 * 0b 0001 1011 0011 0011 0011 0000 0100 0000
 *    |||| |||| |||| |||| |||| |||| |||| ++++- reserved, write as 0
 *    |||| |||| |||| |||| |||| |||+ ++++ ----- tODT_idle_off: 4, 4 clocks: unclear, see note.
 *                                             [M48_DS_DDR3_MICRON_R]
 *                                             [M48_DS_DDR3_ISSI_I1]
 *                                             [M48_DS_DDR3_ALLIANCE_1.1]
 *    |||| |||| |||| |||| |||| +++------------ reserved, write as 0
 *    |||| |||| |||| |||| |+++---------------- tODTLon: 3, 3 clocks:
 *                                             [M48_DS_DDR3_MICRON_R] p. 98: CWL + AL - 2CK = 5 + 0 - 2 = 3
 *                                             [M48_DS_DDR3_ISSI_I1] p. unknown
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. unknown
 *    |||| |||| |||| |||| +------------------- reserved, write as 0
 *    |||| |||| |||| ++++--------------------- tAXPD: 3, 4 clocks: CWL - 1
 *                                             [M48_DS_IMX_RMDL_4] p. 3960
 *    |||| |||| ++++-------------------------- tANPD: 3, 4 clocks: CWL - 1
 *                                             [M48_DS_IMX_RMDL_4] p. 3959
 *    |||| |+++------------------------------- tAONPD: 3, 4 clocks: 4 * 2.525ns > 8.5ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 198: 8.5ns max
 *                                             [M48_DS_DDR3_ISSI_I1] p. 65: 8.5ns max
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 36: 8.5ns max
 *    ||++ +---------------------------------- tAOFPD: 3, 4 clocks: 4 * 2.525ns > 8.5ns
 *                                             [M48_DS_DDR3_MICRON_R] p. 198: 8.5ns max
 *                                             [M48_DS_DDR3_ISSI_I1] p. 65: 8.5ns max
 *                                             [M48_DS_DDR3_ALLIANCE_1.1] p. 36: 8.5ns max
 *    ++-------------------------------------- reserved, write as 0
 * Note: unclear how tODT_idle_off is to be determined.
 * Note: According to [M48_DS_IMX_SCR_DL] a value of 0x1B333040 shall be used.
 */

 /* RL=AL+CL */
 /* RTW=AL+CL+CCD-CWL+2CK=0+6+4-6+2=6  */
 /* RTW_SAME = RTW-AL-(CL-CWL)-BL/2 = 6-0-(6-4)-(8/2) = 2 */
 {MX6_MMDC_P0_MDRWD, 0x000026D2},

 /* CALIB_PER_CS=0 (CS0); ADDR_MIRROR=1; LHD=0; WALAT=1*/
 /* BI_ON=1; LPDDR2_S2=0; MIF3_MODE=3; RALAT=0 (AL) */
 /* DDR_4_BANK=0; DDR_TYPE=0; LPDDR2_2CH=0; RST=0; */
 {MX6_MMDC_P0_MDMISC, 0x00011740},
 /* XPR=RFC+10ns=103+4=107; SDE_to_RST=14(JEDEC); RST_toCKE=33(JEDEC); */
 {MX6_MMDC_P0_MDOR,  0x006A1023},
 {MX6_MMDC_P0_MDASP, 0x00000047},

 /* configure density and burst lenght */
 {MX6_MMDC_P0_MDCTL, 0x041A0000},

 /* start ZQ calibration */
 {MX6_MMDC_P0_MDCTL, 0x841A0000},
 {MX6_MMDC_P0_MDSCR, 0x04008032},
 {MX6_MMDC_P0_MDSCR, 0x00008033},
 /* SDRAM MR1: AL=0 == MDCFG2 */
 {MX6_MMDC_P0_MDSCR, 0x00428031},
 /* SDRAM MR0: CL=6 == MDPDC */
 {MX6_MMDC_P0_MDSCR, 0x13208030},
 {MX6_MMDC_P0_MDSCR, 0x04008040},

 {MX6_MMDC_P0_MDPDC, 0x0002556D},
 {MX6_MMDC_P0_MAPSR, 0x00001006},


 {MX6_MMDC_P0_MDREF, 0x00001800},

 {MX6_MMDC_P0_MPODTCTRL, 0x00022227},
 {MX6_MMDC_P1_MPODTCTRL, 0x00022227},

 {MX6_MMDC_P0_MPMUR0, 0x00000800},
 {MX6_MMDC_P1_MPMUR0, 0x00000800},

 {MX6_MMDC_P0_MDSCR, 0x00000000},
};
int ram_setup_mx6dl_size = ARRAY_SIZE(ram_setup_mx6dl);

m48_configuration ram_calibration_mx6dl_06 [] =
{
 {MX6_MMDC_P0_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL0, 0x001F001F},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x001F001F},
 {MX6_MMDC_P0_MPDGCTRL0, 0x42580258},
 {MX6_MMDC_P0_MPDGCTRL1, 0x0244024C},
 {MX6_MMDC_P1_MPDGCTRL0, 0x424C0250},
 {MX6_MMDC_P1_MPDGCTRL1, 0x02300244},
 {MX6_MMDC_P0_MPRDDLCTL, 0x444A484A},
 {MX6_MMDC_P1_MPRDDLCTL, 0x44484C40},
 {MX6_MMDC_P0_MPWRDLCTL, 0x12123434},
 {MX6_MMDC_P1_MPWRDLCTL, 0x3C34142E},
};
int ram_calibration_mx6dl_size_06 = ARRAY_SIZE(ram_calibration_mx6dl_06);

m48_configuration ram_calibration_mx6dl_08 [] =
{
 {MX6_MMDC_P0_MPWLDECTRL0, 0x00660071},
 {MX6_MMDC_P0_MPWLDECTRL1, 0x0057005B},
 {MX6_MMDC_P1_MPWLDECTRL0, 0x00490050},
 {MX6_MMDC_P1_MPWLDECTRL1, 0x0039004D},
 {MX6_MMDC_P0_MPDGCTRL0, 0x42570254},
 {MX6_MMDC_P0_MPDGCTRL1, 0x0234023C},
 {MX6_MMDC_P1_MPDGCTRL0, 0x42420248},
 {MX6_MMDC_P1_MPDGCTRL1, 0x02250234},
 {MX6_MMDC_P0_MPRDDLCTL, 0x44454847},
 {MX6_MMDC_P1_MPRDDLCTL, 0x46474843},
 {MX6_MMDC_P0_MPWRDLCTL, 0x38363031},
 {MX6_MMDC_P1_MPWRDLCTL, 0x3C363831},
};
int ram_calibration_mx6dl_size_08 = ARRAY_SIZE(ram_calibration_mx6dl_08);
