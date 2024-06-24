/*
 *  (C) Copyright 2020 Draeger and Licensors, info@draeger.com
 *
 *  SPDX-License-Identifier: GPL-2.0+
 */

#include <asm/arch/imx-regs.h>
#include <linux/sizes.h>

#include "imx6_spl.h" /* common IMX6 SPL configuration */

/* CONFIG_TARGET_IMX6M48 is defined by kconfig */

/* define this in order to make SPL wait in a while(1) for convenient JTAG-Debugger attaching */
#undef DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER
/* define this in order to enable all (!) debug messages
#define DEBUG
 */

#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV		0	/* SDHC3 */
#endif

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_FLAGS_LIST_STATIC "netmask:x"

#ifndef __ASSEMBLY__
int board_reset_post_test(int);
int board_test_run_always(int);
#endif

#ifndef CONFIG_SPL_BUILD

#define CONFIG_POST (CONFIG_SYS_POST_WATCHDOG | \
        CONFIG_SYS_POST_BSPEC1 | \
        CONFIG_SYS_POST_BSPEC3 | \
        CONFIG_SYS_POST_MEMORY | \
        CONFIG_SYS_POST_MEM_REGIONS)

#define CONFIG_POST_BSPEC1    {				\
    "Board reset test",				\
    "board_reset",					\
    "This test verifies the functionality of the external board reset.",	\
    POST_RAM | POST_POWERON | POST_MANUAL | POST_REBOOT,	\
    &board_reset_post_test,				\
    NULL,						\
    NULL,						\
    CONFIG_SYS_POST_BSPEC1				\
    }

#define CONFIG_POST_BSPEC3    {				\
    "M48 board specific tests (run always)",				\
    "boardtest_always",					\
    "This test measures boot/post time.",	\
    POST_RAM | POST_POWERON | POST_NORMAL,	\
    &board_test_run_always,				\
    NULL,						\
    NULL,						\
    CONFIG_SYS_POST_BSPEC3				\
    }
#else
#   define CONFIG_POST 0
#endif

#define CONFIG_POST_EXTERNAL_WORD_FUNCS

#define PHYS_SDRAM_SIZE					SZ_2G
#define PHYS_SDRAM                     MMDC0_ARB_BASE_ADDR
#define CONFIG_SYS_SDRAM_BASE          PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR       IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE       IRAM_SIZE
#define CONFIG_SYS_INIT_SP_OFFSET \
    (CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
    (CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* VXWORKS_PM_RESERVED_MEM is the setting in VxWorks PM_RESERVED_MEM which
 * reserves memory for power save memory */
#define VXWORKS_PM_RESERVED_MEM (5412u * 4096u)
/* VXWORKS_DMA32_HEAP_SIZE is the setting in VxWorks DMA32_HEAP_SIZE which
 * reserves space for DMA cache in kernel */
#define VXWORKS_DMA32_HEAP_SIZE 0x4000000
/* both values define the offset of the power save memory area;
 * do not access it by u-boot*/
#ifdef CONFIG_SYS_MEM_TOP_HIDE
#undef CONFIG_SYS_MEM_TOP_HIDE
#endif
#define CONFIG_SYS_MEM_TOP_HIDE (VXWORKS_PM_RESERVED_MEM + VXWORKS_DMA32_HEAP_SIZE)

/* max allowed size of uboot defined in imx6_spl.h:59 */
#ifdef CONFIG_SYS_MONITOR_LEN
#undef CONFIG_SYS_MONITOR_LEN
#endif
#define CONFIG_SYS_MONITOR_LEN          SZ_1M

#ifndef CONFIG_SYS_L2CACHE_OFF
#  define CONFIG_SYS_L2_PL310
#  define CONFIG_SYS_PL310_BASE	L2_PL310_BASE
#endif

#define CONFIG_MXC_UART_BASE	UART1_BASE
#define CONFIG_SYS_BOOTM_LEN	0x8000000


/* I2C Configs */
#define CONFIG_SYS_I2C_SPEED		100000

#ifdef CONFIG_SPL_BUILD
#  define CONFIG_SYS_I2C_LEGACY
#  undef  CONFIG_DM_I2C
#  define CONFIG_SYS_I2C
#  undef  CONFIG_DM_PMIC
#  undef  CONFIG_DM_PMIC_PFUZE100
#  undef  CONFIG_CMD_PMIC
#endif


/* EEPROM Configuration */
#define CONFIG_SYS_EEPROM_BUS_NUM  1 /* i2c2 */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1

#define CONFIG_IMX_HDMI
#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_FB_16BPP_565RGB

#define CONFIG_SYS_FSL_ESDHC_ADDR      0

#define CONFIG_BOOTCOMMAND "startM48"

#define PHY_ANEG_TIMEOUT 20000
#define CONFIG_IPADDR    192.168.0.42
#define CONFIG_SERVERIP  192.168.0.1
#define CONFIG_GATEWAYIP 192.168.0.1
#define CONFIG_NETMASK   ffffff00

/******************************************************************************
 * M48 special defines
 *****************************************************************************/

#define CFG_FSL_USDHC_NUM	2

#define CFG_DEFAULT_FDT_FILE_MX6Q "imx6q-m48.dtb"
#define CFG_DEFAULT_FDT_FILE_MX6DL "imx6dl-m48.dtb"

#define CFG_GPIO_BOARD_COLD_RESET IMX_GPIO_NR(1, 2)  /* external cold reset GPIO1_IO02 */
#define CFG_GPIO_BOARD_WARM_RESET IMX_GPIO_NR(4, 30) /* external warm reset GPIO1_IO027 */
#define CFG_GPIO_DEBUG	          IMX_GPIO_NR(5, 9)  /* Jumper 1 switch serial output on/off */

#ifdef CONFIG_HAB_REVOCATION_TEST_IMAGE
#   define CFG_SYS_M48_MAX_PARTITIONS 1
#else
#   define CFG_SYS_M48_MAX_PARTITIONS 4
#endif

/* 0 -> 28 : 4 MAC addresses (24B) and one 32bit crc */
#define CFG_SYS_I2C_MAC_OFFSET  0
/* 28 -> 29 : Board revision 1B Major, 1B Minor */
#define CFG_SYS_I2C_BOARD_REV_OFFSET 28
/* 30 -> 39 : Part number + revision */
#define CFG_SYS_I2C_PART_NO_OFFSET   30

#define CFG_SYS_PMSTRUCT_ADDR 0x00917F00

/* absolute ballpark figure for time offset in ms up to the internal clock is running */
#define CFG_SYS_ABS_TIME_OFFSET 450

#define CFG_SYS_ITB_IMAGE_NAME "vxWorks1.itb"
#define CFG_SYS_LEGACY_IMAGE   "uVxWorks1"
#define CFG_SYS_FDT_ADDR        0x18000000
#define CFG_SYS_ENET_NAME       "enet"

#define CONFIG_EXTRA_ENV_SETTINGS \
    "use_tftp=no\0"\
    "loadimage=startM48 loadonly\0" \
	"ramboot=bootRam\0" \
    "imagepath=boot\0" \
    "image_name=" CFG_SYS_LEGACY_IMAGE "\0" \
    "itb_name=" CFG_SYS_ITB_IMAGE_NAME "\0" \
    "netretry=20\0" \
    "splashpos=m,m\0" \
    "fdt_high=0xffffffff\0"   \
    "initrd_high=0xffffffff\0" \
    /** "post_poweron=board_reset watchdog\0" **/

