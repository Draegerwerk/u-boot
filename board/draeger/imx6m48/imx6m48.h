/*
 * Copyright (C) 2015 Draegerwerk AG
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef UP1_SRC_U_BOOT_2014_07_BOARD_DRAEGER_MX6M48_MX6M48_H_
#define UP1_SRC_U_BOOT_2014_07_BOARD_DRAEGER_MX6M48_MX6M48_H_

#define MAX_PRELOADER_VERSION_SIZE 30

#define RST_POR         0x00001
#define RST_CSU         0x00004
#define RST_IPP         0x00008
#define RST_WDOG        0x00010
#define RST_JTAG_Z      0x00020
#define RST_JTAG_SW     0x00040
#define RST_WARM        0x10000
#define COLDRST_MASK    (RST_WDOG|RST_WARM)

/* Defines for WDOGx_WRSR Register */
#define WDOG_WRSR_SFTW  0x0001
#define WDOG_WRSR_TOUT  0x0002
#define WDOG_WRSR_POR   0x0010

#define PART_NUMBER_M48_1  "8421901"
#define PART_NUMBER_M48_2  "8421911"
#define PART_NUMBER_M48_3  "8421921"
#define PART_NUMBER_M48_4  "3722381"
#define PART_NUMBER_M48_1S "3729193"

typedef enum {
    M48_UNKNOWN,
    M48_1_PRE, /* M48.1 i.mx6 DL up to revision 7 */
    M48_2_PRE, /* M48.2 i.mx6 DL up to revision 1 without FPGA */
    M48_3_PRE, /* M48.3 i.mx6 Quad up to revision 1 */
    M48_1_PROD, /* M48.1 i.mx6 DL revision 8 or higher */
    M48_2_PROD, /* M48.2 i.mx6 DL without FPGA revision 2 or higher */
    M48_3_PROD, /* M48.3 i.mx6 Quad from revision 2 and higher */
    M48_3_QUADP, /* M48.4 i.mx6 Quad Plus in any revision */
    VALENKA_IMX6Q_SSPPM,
    VALENKA_IMX6Q_MOUNT
} M48_BOARD_VERSION;

typedef enum {
    VALENKA_UNKNOWN_SOM,
    VALENKA_2_SOM,
    VALENKA_3_2GB_SOM,
    VALENKA_3_4GB_SOM
} VALENKA_SOM_VERSION;

struct splHandoverT {
    char                preloaderVersion[MAX_PRELOADER_VERSION_SIZE];
    M48_BOARD_VERSION   boardVersion;
    VALENKA_SOM_VERSION somVersion;
};

extern struct splHandoverT * splHandoverData;

bool isValenka(void);

bool isM48withUp2 (void);

int getBootMmcDevice(void);

int power_init_board(void);

u16 get_imx_wdog_wrsr (void);


#endif /* UP1_SRC_U_BOOT_2014_07_BOARD_DRAEGER_MX6M48_MX6M48_H_ */
