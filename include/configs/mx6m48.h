/*
 * mx6m48.h
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 * Configuration for Draeger M48 processor 1
 *
 */

#ifndef __MX6M48_CONFIG_H
#define __MX6M48_CONFIG_H

/* SPL */
#define CONFIG_SPL_MMC_SUPPORT

#include "imx6_spl.h"                  /* common IMX6 SPL configuration */

/* define this in order to make SPL wait in a while(1) for convenient JTAG-Debugger attaching */
#undef DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER

#ifdef CONFIG_SYS_SPL_MALLOC_SIZE
#undef CONFIG_SYS_SPL_MALLOC_SIZE
#endif
#define CONFIG_SYS_SPL_MALLOC_SIZE  0x1400   /* 5 kB */

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#define CONFIG_MACH_TYPE	3980
#define CONFIG_MXC_UART_BASE	UART1_BASE
#define CONFIG_SILENT_CONSOLE
#define CONFIG_DISABLE_CONSOLE
#define CONFIG_CONSOLE_DEV		"ttymxc0"
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"

#define CONFIG_DEFAULT_FDT_FILE_MX6Q "imx6q-m48.dtb"
#define CONFIG_DEFAULT_FDT_FILE_MX6DL "imx6dl-m48.dtb"
#define CONFIG_DEFAULT_FDT_FILE "invalid.dtb"

#define PHYS_SDRAM_SIZE		(2u * 1024 * 1024 * 1024)
#define CONFIG_PM_RESERVED_MEM (5412u * 4096u) /* needs to stay the same as setting PM_RESERVED_MEM in kernel configuration script */

#define CONFIG_MX6

#include "mx6_common.h"
#include <linux/sizes.h>

#define CONFIG_PRAM                 32768

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_LAST_STAGE_INIT	1

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_SYS_GENERIC_BOARD

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(10 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MXC_GPIO

#define CONFIG_MXC_UART


#undef CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR      0

#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#undef CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_FAT_WRITE
#define CONFIG_DOS_PARTITION

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define IMX_FEC_BASE			ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RGMII
#define CONFIG_ETHPRIME			"FEC"
#define CONFIG_FEC_MXC_PHYADDR		0

#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#define CONFIG_PHY_MICREL_KSZ9031

#undef CONFIG_CMD_SF
#ifdef CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_MXC_SPI
#define CONFIG_SF_DEFAULT_BUS		0
#define CONFIG_SF_DEFAULT_CS		(0 | (IMX_GPIO_NR(4, 9) << 8))
#define CONFIG_SF_DEFAULT_SPEED		20000000
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_0
#endif

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX              1
#define CONFIG_BAUDRATE                        115200

/* Command definition */
#include <config_cmd_default.h>

#undef CONFIG_CMD_BMODE
#undef CONFIG_CMD_BOOTZ
#define CONFIG_CMD_SETEXPR
#undef CONFIG_CMD_IMLS

#define CONFIG_CMD_GPIO
/* #define CONFIG_CMD_MEMTEST
 * #define CONFIG_SYS_ALT_MEMTEST
 */
#define CONFIG_CMD_MEMINFO
#define CONFIG_CMD_GETTIME
#define CONFIG_CMD_DIAG

#define CONFIG_POST (CONFIG_SYS_POST_WATCHDOG | \
        CONFIG_SYS_POST_BSPEC1 | \
        CONFIG_SYS_POST_BSPEC3 | \
        CONFIG_SYS_POST_MEMORY | \
        CONFIG_SYS_POST_MEM_REGIONS)

#define CONFIG_POST_EXTERNAL_WORD_FUNCS
/* #define CONFIG_SYS_PMSTRUCT_ADDR 0x4ffff000 */
#define CONFIG_SYS_PMSTRUCT_ADDR 0x00917F00

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

#define CONFIG_GPIO_BOARD_COLD_RESET IMX_GPIO_NR(1, 2)  /* external cold reset GPIO1_IO02 */
#define CONFIG_GPIO_BOARD_WARM_RESET IMX_GPIO_NR(4, 30) /* external warm reset GPIO1_IO027 */
#define CONFIG_GPIO_DEBUG	         IMX_GPIO_NR(5, 9)  /* Jumper 1 switch serial output on/off */

#define CONFIG_BOOTDELAY               0
/* allow to abort autoboot with CTRL+C */
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_STOP_STR	"\x03"
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_LOADADDR                0x100FFFC0
#define CONFIG_SYS_TEXT_BASE           0x17800000

#define CONFIG_EXTRA_ENV_SETTINGS \
    "autoboot="\
           "setenv mmcpart 1; " \
        "run defaultargs; "\
        "if test ${use_tftp} != force; then " \
            "mmc dev ${mmcdev}; " \
            "if mmc rescan ; then " \
                "for partition in 1 2 3 4; do " \
                    "setenv mmcpart ${partition}; " \
                    "echo Booting from partition ${mmcpart}; " \
                        "if run loadimage; then " \
                            "run ramboot; " \
                        "fi; " \
                "done;" \
            "fi; " \
        "fi; " \
        "if test ${use_tftp} = yes -o ${use_tftp} = y -o ${use_tftp} = force ; then " \
            "echo Trying to boot from tftp; " \
            "if run loadtftp; then " \
                "run ramboot;" \
            "fi; " \
        "fi\0" \
    "use_tftp=no\0"\
    "loadimage="\
        "fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${imagepath}/${image_name} && setexpr auth_krnl  ${filesize} - 2020" \
        "&& fatload mmc ${mmcdev}:${mmcpart} ${fdtaddr} ${imagepath}/${fdt_name} && setexpr auth_fdt  ${filesize} - 2020\0" \
    "loadtftp="\
        "tftp ${loadaddr} ${imagepath}/${image_name} && setexpr auth_krnl  ${filesize} - 2020" \
        "&& tftp ${fdtaddr} ${imagepath}/${fdt_name} && setexpr auth_fdt  ${filesize} - 2020\0" \
    "image_name=uVxWorks1\0" \
    "imagepath=boot\0" \
    "fdt_name=" CONFIG_DEFAULT_FDT_FILE "\0" \
    "fdtaddr=0x18000000\0" \
    "ipaddr=192.168.0.42\0" \
    "netmask=ffffff00\0" \
    "serverip=192.168.0.1\0" \
    "gwip=192.168.0.1\0" \
    "defaultargs=setenv bootargs enet(0,0)host:vxWorks " \
        "h=${serverip} " \
        "e=${ipaddr}:${netmask} " \
        "g=${gwip} " \
        "u=target pw=vxTarget\0" \
    "netretry=20\0" \
    "splashpos=m,m\0" \
    "fdt_high=0xffffffff\0"   \
    "initrd_high=0xffffffff\0" \
    "mmcdev=" __stringify(CONFIG_SYS_MMC_ENV_DEV) "\0" \
    "mmcpart=1\0" \
    "ramboot=if hab_auth_img ${loadaddr} ${auth_krnl} || hab_auth_img ${fdtaddr} ${auth_fdt}; then ;"\
        "else "\
            "bootm ${loadaddr} - ${fdtaddr};"\
        "fi\0"

    /* "post_poweron=board_reset watchdog\0" */

#define CONFIG_BOOTCOMMAND \
    "run autoboot; logo"

#ifdef CONFIG_HAB_REVOCATION_TEST_IMAGE
/* make sure that only a kernel and dtb signed by us gets booted */
#undef CONFIG_BOOTCOMMAND
/* addresses hard coded to prevent environment tinkering */
#define CONFIG_BOOTCOMMAND \
    "echo Booting SRK revocation test kernel \
    && fatload mmc 0:1 0x100FFFC0 boot/uVxWorks1 && setexpr auth_krnl  ${filesize} - 2020 \
    && fatload mmc 0:1 0x18000000 boot/${fdt_name} && setexpr auth_fdt  ${filesize} - 2020 \
    && if hab_auth_img 0x100FFFC0 ${auth_krnl} || hab_auth_img 0x18000000 ${auth_fdt}; then halt; \
       else bootm 0x100FFFC0 - 0x18000000; fi;"

/* HAB revocation image shall not be interruptible */
#undef CONFIG_AUTOBOOT_KEYED
#undef CONFIG_ZERO_BOOTDELAY_CHECK
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY  0
#endif

#define CONFIG_ARP_TIMEOUT     200UL

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2     "> "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE              256

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS             16
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START       0x10000000
#define CONFIG_SYS_MEMTEST_END         0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH     0x10800000

#define CONFIG_SYS_LOAD_ADDR           CONFIG_LOADADDR

#define CONFIG_CMDLINE_EDITING
#define CONFIG_STACKSIZE               (128 * 1024)

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS           1
#define PHYS_SDRAM                     MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE          PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR       IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE       IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
    (CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
    (CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_ENV_SIZE			(8 * 1024)

#define CONFIG_ENV_IS_IN_MMC

#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_ENV_OFFSET		((1024 * 1024) - CONFIG_ENV_SIZE)
#endif

#define CONFIG_OF_LIBFDT
#define CONFIG_OF_BOARD_SETUP

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

#define CONFIG_SYS_PROMPT		"M48_UP1# "

#define CONFIG_SYS_FSL_USDHC_NUM	2
#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV		0	/* SDHC3 */
#endif

/* Framebuffer */
#define CONFIG_VIDEO
#define CONFIG_VIDEO_IPUV3
#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define CONFIG_BMP_16BPP
#define CONFIG_CMD_BMP

#define CONFIG_VIDEO_LOGO
#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_LOGO_DEFERED

#define CONFIG_IPUV3_CLK 264000000
#define CONFIG_IMX_HDMI
#define CONFIG_IMX_VIDEO_SKIP
#define CONFIG_SYS_CONSOLE_BG_COL 0xFF
#define CONFIG_SYS_CONSOLE_FG_COL 0x00


#undef CONFIG_CMD_PCI
#ifdef CONFIG_CMD_PCI
#define CONFIG_PCI
#define CONFIG_PCI_PNP
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_PCIE_IMX
#define CONFIG_PCIE_IMX_PERST_GPIO	IMX_GPIO_NR(7, 12)
#define CONFIG_PCIE_IMX_POWER_GPIO	IMX_GPIO_NR(3, 19)
#endif

/* I2C Configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED		  100000

/* EEPROM Configuration */
#define CONFIG_CMD_EEPROM
#define CONFIG_SYS_EEPROM_SIZE	0x400
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	10

/* 0 -> 28 : 4 MAC addresses (24B) and one 32bit crc */
#define CONFIG_SYS_I2C_MAC_OFFSET  0
/* 28 -> 29 : Board revision 1B Major, 1B Minor */
#define CONFIG_SYS_I2C_BOARD_REV_OFFSET 28
/* 30 -> 39 : Part number + revision */
#define CONFIG_SYS_I2C_PART_NO_OFFSET   30

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE100
#define CONFIG_POWER_PFUZE100_I2C_ADDR	0x08

#define CONFIG_SYS_FSL_SEC_COMPAT 4
#define CONFIG_SYS_FSL_SEC_LE
#define CONFIG_SYS_FSL_SEC_BE

#define CONFIG_SECURE_BOOT
/* SECURE BOOT
#define CONFIG_SYS_FSL_SEC_LE
#define CONFIG_SYS_FSL_SEC_BE
#define CONFIG_SYS_FSL_SEC_COMPAT 4
#define CONFIG_FSL_CAAM
#define CONFIG_CMD_DEKBLOB
#define CONFIG_FAT_WRITE
#define CONFIG_FIT
#define CONFIG_FIT_SIGNATURE
#define CONFIG_RSA
#define CONFIG_OF_CONTROL
#define CONFIG_OF_SEPARATE
*/
#endif                         /* __MX6M48_CONFIG_H */
