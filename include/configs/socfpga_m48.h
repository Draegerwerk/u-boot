/*
 *  Copyright Altera Corporation (C) 2012-2013. All rights reserved
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms and conditions of the GNU General Public License,
 *  version 2, as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define CONFIG_SOCFPGA_CYCLONE5

#ifndef __CONFIG_H
#define __CONFIG_H

#include "../../board/draeger/socfpga_m48/build.h"
#include "../../board/draeger/socfpga_m48/pinmux_config.h"
#include "../../board/draeger/socfpga_m48/pll_config.h"
#include "../../board/draeger/socfpga_m48/sdram/sdram_config.h"
#include "../../board/draeger/socfpga_m48/reset_config.h"

#include <asm/arch/hardware.h>
#include <asm/arch/clock_manager.h>

/* Enabled for U-Boot debug message printout? */
/*#define DEBUG*/
/* if panic, will call hang as watchdog will come and trigger warm reset */
#if defined(DEBUG) && defined(CONFIG_SPL_BUILD)
#define CONFIG_PANIC_HANG
#endif

/* define this in order to make SPL wait in a while(1) for convenient JTAG-Debugger attaching
 * this will also deactivate the WATCHDOG */
#undef DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER

/*
 * Quick tips for debugging
 * > Before relocation, load elf for debugger as normal usage
 * > After relocation, you need to type below command at gdb
 * (gdb) symbol-file
 * (gdb) add-symbol-file u-boot <u-boot new address offset>
 *
 * > you can get the offset from "bdinfo" console command under reloc_off
 */

/*
 * High level configuration
 */
/* Running on virtual target? */
#undef CONFIG_SOCFPGA_VIRTUAL_TARGET
/* ARMv7 CPU Core */
#define CONFIG_ARMV7
/* Cache policy for CA-9 is write back */
#define CONFIG_SYS_CACHELINE_SIZE   32

/* Activate L2 Cache controller */
#ifndef CONFIG_SYS_L2CACHE_OFF
#define CONFIG_SYS_L2_PL310 y
#define CONFIG_SYS_PL310_BASE SOCFPGA_MPUL2_ADDRESS
#endif

/* activate ARM erratum */
#define CONFIG_ARM_ERRATA_845369 /*Under very rare timing circumstances, transitioning into streaming mode might create a data corruption*/

/* Support for IRQs - for ocram and SDRAM ECC */
#ifndef CONFIG_SOCFPGA_VIRTUAL_TARGET
#define CONFIG_USE_IRQ
#endif
/* miscellaneous platform dependent initialisations */
#define CONFIG_MISC_INIT_R
/* Use as single bootloader where no boot ROM and preloader */
#define CONFIG_SINGLE_BOOTLOADER
/* SOCFPGA Specific function */
#define CONFIG_SOCFPGA
/* base address for .text section */
#ifdef CONFIG_SOCFPGA_VIRTUAL_TARGET
#define CONFIG_SYS_TEXT_BASE        0x08000040
#else
#define CONFIG_SYS_TEXT_BASE        0x01000040
#endif
/* Default load address */
#define CONFIG_SYS_LOAD_ADDR        0x000FFFC0
#define CONFIG_STANDALONE_LOAD_ADDR	0xFFFF0000 /* OCRAM */

#ifdef CONFIG_USE_IRQ
/* Enable board late init for ECC setup if IRQ enabled */
#define CONFIG_BOARD_LATE_INIT
#endif
/* Enable THUMB2 mode to reduce software size which yield better boot time */
#define CONFIG_SYS_THUMB_BUILD

#define CONFIG_POST (CONFIG_SYS_POST_WATCHDOG | \
        CONFIG_SYS_POST_BSPEC1 | \
        CONFIG_SYS_POST_BSPEC3 | \
        CONFIG_SYS_POST_MEMORY | \
        CONFIG_SYS_POST_MEM_REGIONS)

#define CONFIG_POST_EXTERNAL_WORD_FUNCS
/* #define CONFIG_SYS_PMSTRUCT_ADDR 0x3ffff000 */
#define CONFIG_SYS_PMSTRUCT_ADDR 0xfffff800
#define CONFIG_PM_RESERVED_MEM (5412u * 4096u) /* needs to stay the same as setting PM_RESERVED_MEM in kernel configuration script */

#define CONFIG_SYS_PM_USR_ADDR   ((char *)((CONFIG_SYS_SDRAM_BASE)+(PHYS_SDRAM_1_SIZE)-(CONFIG_PM_RESERVED_MEM)))

#define CONFIG_POST_BSPEC1    {             \
    "Board reset test",             \
    "board_reset",                  \
    "This test verifies the functionality of the external board reset.",    \
    POST_RAM | POST_POWERON | POST_SLOWTEST | POST_MANUAL | POST_REBOOT,    \
    &board_reset_post_test,             \
    NULL,                       \
    NULL,                       \
    CONFIG_SYS_POST_BSPEC1              \
    }

#define CONFIG_POST_BSPEC3    {             \
    "M48 board specific tests (run always)",                \
    "boardtest_always",                 \
    "This test measures boot/post time.",   \
    POST_RAM | POST_POWERON | POST_NORMAL,  \
    &board_test_run_always,             \
    NULL,                       \
    NULL,                       \
    CONFIG_SYS_POST_BSPEC3              \
    }

#define CONFIG_GPIO_BOARD_COLD_RESET 61 /* external cold reset GPIO61 */
#define CONFIG_GPIO_BOARD_WARM_RESET 24 /* external warm reset FPGA_LOANIO24 */
#define CONFIG_GPIO_DEBUG            59 /* Jumper 1 switch serial output on/off */
#define CONFIG_GPIO_FPGA_PRESENT     89 /* notify uP1 about loaded FPGA */

/* DesignWare APB GPIO*/
#define CONFIG_DW_APB_GPIO
#define CONFIG_SPL_GPIO_SUPPORT

/*
 * Display CPU and Board Info
 */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/*
 * Enable early stage initialization at C environment
 */
#define CONFIG_BOARD_EARLY_INIT_F

/*
 * Kernel Info
 */
/* flat device tree */
#define CONFIG_OF_LIBFDT
/* skip updating the FDT blob */
#define CONFIG_FDT_BLOB_SKIP_UPDATE
/* Initial Memory map size for Linux, minus 4k alignment for DFT blob */
#define CONFIG_SYS_BOOTMAPSZ        (64 * 1024 * 1024)

/*
 * Memory allocation (MALLOC)
 */
/* Room required on the stack for the environment data */
#define CONFIG_ENV_SIZE         8192
/* Size of DRAM reserved for malloc() use */
#define CONFIG_SYS_MALLOC_LEN       (CONFIG_ENV_SIZE + 256 * 1024)

/*
 * Stack setup
 */
#ifdef CONFIG_USE_IRQ
/* IRQ stack */
#define CONFIG_STACKSIZE_IRQ        (1 << 10)
/* FIQ stack */
#define CONFIG_STACKSIZE_FIQ        (1 << 10)
#endif
/* SP location before relocation, must use scratch RAM */
#define CONFIG_SYS_INIT_RAM_ADDR    0xFFFF0000
/* Reserving 0x100 space at back of scratch RAM for debug info */
#define CONFIG_SYS_INIT_RAM_SIZE    (0x10000 - 0x100)
/* Stack pointer prior relocation, must situated at on-chip RAM */
#define CONFIG_SYS_INIT_SP_ADDR     (CONFIG_SYS_INIT_RAM_ADDR + \
                     CONFIG_SYS_INIT_RAM_SIZE - \
                     GENERATED_GBL_DATA_SIZE)

/*
 * Command line configuration.
 */
#define CONFIG_SYS_NO_FLASH
#include <config_cmd_default.h>
/* FAT file system support */
#define CONFIG_CMD_FAT
/* Enable FAT write support */
#define CONFIG_FAT_WRITE
/* EXT2 file system support */
#undef CONFIG_CMD_EXT2
/* MMC support */
#define CONFIG_CMD_MMC
#define CONFIG_CMD_DIAG
/* switch on/off cache */
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_GPIO
/*
 * Misc
 */
/* Enable DOS partition */
#define CONFIG_DOS_PARTITION            1


/* OSE operating system support */
#define CONFIG_BOOTM_OSE

/* Support loading of zImage */
#undef CONFIG_CMD_BOOTZ

/* Support loading of uVxWorks */
#define CONFIG_BOOTM_VXWORKS

/*
 * Environment setup
 */

/* Delay before automatically booting the default image */
#define CONFIG_BOOTDELAY        0
/* allow to abort autoboot with 3 times CTRL+C */
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_STOP_STR	"\x03\x03\x03"
#define CONFIG_ZERO_BOOTDELAY_CHECK
/* write protection for vendor parameters is completely disabled */
#define CONFIG_ENV_OVERWRITE
/* Enable auto completion of commands using TAB */
#define CONFIG_AUTO_COMPLETE
/* Enable editing and history functions for interactive CLI operations */
#define CONFIG_CMDLINE_EDITING
/* Additional help message */
#define CONFIG_SYS_LONGHELP
/* use "hush" command parser */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2  "> "
#define CONFIG_CMD_RUN

#ifdef CONFIG_SOCFPGA_VIRTUAL_TARGET
#define CONFIG_BOOTCOMMAND "run ramboot"
#else
#define CONFIG_BOOTCOMMAND "run autoboot"
#endif

/* TODO remove fpgaloadlegacy when new format has been rolled out
 * :: block comments please !!!
 */

/*
 * arguments passed to the bootz command. The value of
 * CONFIG_BOOTARGS goes into the environment value "bootargs".
 * Do note the value will overide also the chosen node in FDT blob.
 */
#define CONFIG_BOOTARGS ""

#define CONFIG_EXTRA_ENV_SETTINGS \
    "autoboot=" \
           "setenv mmcpart 1; " \
        "run defaultargs; " \
        "devtype;" \
        "run bridge_disable; "\
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
            "else "\
                "echo Booting from QSPI; " \
                "run loadqspi; " \
                "halt;"\
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
        "if fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${imagepath}/${image_name} && fatload mmc ${mmcdev}:${mmcpart} ${fdtaddr} ${imagepath}/${fdt_name}; then "\
            "run fpgaloadprimary; " \
        "fi\0" \
    "loadtftp="\
        "if tftp ${loadaddr} ${imagepath}/${image_name} && tftp ${fdtaddr} ${imagepath}/${fdt_name}; then "\
            "run fpgaloadtftp; " \
        "fi\0" \
    "loadqspi=run fpgaloadqspi\0" \
    "image_name=uVxWorks2\0" \
    "imagepath=boot\0" \
    "fdt_name=altsoc-m48.dtb\0" \
    "fdtaddr=0xf000000\0" \
    "ipaddr=192.168.0.45\0" \
    "netmask=ffffff00\0" \
    "serverip=192.168.0.1\0" \
    "gwip=192.168.0.1\0" \
    "defaultargs=setenv bootargs emac(0,0)host:vxWorks " \
        "h=${serverip} " \
        "e=${ipaddr}:${netmask} " \
        "g=${gwip} " \
        "u=target pw=vxTarget\0" \
    "netretry=20\0" \
    "mmcdev=0\0" \
    "mmcpart=1\0" \
    "ramboot=bootm ${loadaddr} - ${fdtaddr}\0" \
    "loadaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
    "fpgaloadprimary=" \
        "if fatload mmc ${mmcdev}:${mmcpart} ${fpgadata} ${imagepath}/${fpganame} && run fpgacheck && fpga loadmk 0 ${fpgadata}; then " \
            "run bridge_enable_handoff; " \
            "run fpgaenableremoteaccess; " \
        "else " \
            "run fpgaloadlegacy; " \
        "fi\0" \
    "fpgaloadlegacy=" \
        "if fatload mmc ${mmcdev}:${mmcpart} ${fpgadata} m48_default_system.rbf && fpga load 0 ${fpgadata}; then " \
            "run bridge_enable_handoff; " \
            "run fpgaenableremoteaccess; " \
        "fi\0" \
    "fpgaloadqspi=" \
        "sf probe ${qspiloadcs}; sf read ${fpgadata} ${qspifpgaoffset} ${fpgadatasize}; sf reset; "\
        "if run fpgacheck && fpga loadmk 0 ${fpgadata}; then " \
            "run bridge_enable_handoff; " \
            "run fpgaenableremoteaccess; " \
        "fi\0" \
    "fpgaloadqspidefault="\
        "sf probe ${qspiloadcs}; sf read ${fpgadata} ${qspifpgadefault} ${fpgadatasize}; sf reset; "\
        "if fpga loadmk 0 ${fpgadata}; then " \
            "run bridge_enable_handoff; " \
            "run fpgaenableremoteaccess; " \
        "fi\0" \
    "fpgaloadtftp=" \
        "if tftp ${fpgadata} ${imagepath}/${fpganame} && run fpgacheck && fpga loadmk 0 ${fpgadata}; then " \
            "run bridge_enable_handoff; " \
            "run fpgaenableremoteaccess; " \
        "else " \
            "run fpgaloadlegacy; " \
        "fi\0" \
    "fpgadata=0x02000000\0" \
    "fpgadatasize=0x40C580\0" \
    "fpganame=m48_default_system.img\0" \
    "fpgaenableremoteaccess="\
        "mw.l ${fpgasecurityregl4main} ${fpgasecurity_allenabled} 32;\0" \
    "fpgacheck=" \
        "if test ${fpgaverify} = n -o ${fpgaverify} = no ; then " \
            "echo FPGA check disabled;"\
            "true; " \
        "else " \
            "if cktype ${fpgadata}; then " \
                "echo found correct FPGA;"\
                "true; " \
            "else " \
                "echo FPGA version wrong;" \
                "false; " \
            "fi; " \
        "fi\0" \
    "fpga2sdram=ffc25080\0" \
    "fpga2sdram_handoff=0x00000000\0" \
    "fpga2sdram_apply=3df81bd4\0" \
    "fpgasecurityregl4main=0xFF800008\0" \
    "fpgasecurity_alldisabled=0x00000000\0" \
    "fpgasecurity_allenabled=0xFFFFFFFFF\0" \
    "fpgaintf=ffd08028\0" \
    "fpgaverify=yes\0" \
    "fpgaintf_handoff=0x00000000\0" \
    "qspiloadcs=0\0" \
    "qspifpgaoffset=0x00900000\0" \
    "qspifpgadefault=0x00200000\0" \
    "fpga=0\0" \
    "axibridge=ffd0501c\0" \
    "axibridge_handoff=0x00000000\0" \
    "l3remap=ff800000\0" \
    "l3remap_handoff=0x00000019\0" \
    "bridge_disable="\
        "mw $fpgaintf 0; "\
        "mw $fpga2sdram 0; "\
        "go $fpga2sdram_apply; "\
        "mw $axibridge 0; "\
        "mw $l3remap 0x1\0" \
    "bridge_enable_handoff="\
        "mw $fpgaintf ${fpgaintf_handoff}; "\
        "go $fpga2sdram_apply; "\
        "mw $fpga2sdram ${fpga2sdram_handoff}; "\
        "mw $axibridge ${axibridge_handoff}; "\
        "mw $l3remap ${l3remap_handoff}\0" \
    CONFIG_KSZ9021_CLK_SKEW_ENV "=" \
        __stringify(CONFIG_KSZ9021_CLK_SKEW_VAL) "\0" \
    CONFIG_KSZ9021_DATA_SKEW_ENV "=" \
        __stringify(CONFIG_KSZ9021_DATA_SKEW_VAL) "\0" \


    /* "post_poweron=board_reset watchdog\0" */

/*
 * Environment setup
 */
/* using environment setting for stdin, stdout, stderr */
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
/* Enable the call to overwrite_console() */
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
/* Enable overwrite of previous console environment settings */
#define CONFIG_SYS_CONSOLE_ENV_OVERWRITE
/* Environment will save to boot device/flash */
#if (CONFIG_PRELOADER_BOOT_FROM_SDMMC == 1)
/* Store envirnoment in MMC card */
#define CONFIG_ENV_IS_IN_MMC
#elif (CONFIG_PRELOADER_BOOT_FROM_NAND == 1)
/* Store envirnoment in NAND flash */
#define CONFIG_ENV_IS_IN_NAND
#else
/* Store envirnoment in SPI flash */
#define CONFIG_ENV_IS_IN_SPI_FLASH
#endif

/* environment setting for SPI flash */
#ifdef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET       (0x00040000)
#define CONFIG_ENV_SECT_SIZE        (64 * 1024)
#define CONFIG_ENV_SPI_BUS      0
#define CONFIG_ENV_SPI_CS       0
#define CONFIG_ENV_SPI_MODE     SPI_MODE_3
#define CONFIG_ENV_SPI_MAX_HZ       CONFIG_SF_DEFAULT_SPEED
#endif

/* environment setting for MMC */
#ifdef CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV      0   /* device 0 */
#define CONFIG_ENV_OFFSET		((1024 * 1024) - CONFIG_ENV_SIZE)
#endif

/* environment setting for NAND */
#ifdef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET       (0x00080000)
#endif

/*
 * Console setup
 */
#define CONFIG_SILENT_CONSOLE
#define CONFIG_DISABLE_CONSOLE
/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE       256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE       (CONFIG_SYS_CBSIZE + \
                    sizeof(CONFIG_SYS_PROMPT) + 16)
/* max number of command args */
#define CONFIG_SYS_MAXARGS      16

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE     CONFIG_SYS_CBSIZE

/*
 * Hardware drivers
 */

/*
 * SDRAM Memory Map
 */
/* We have 1 bank of DRAM */
#define CONFIG_NR_DRAM_BANKS        1
/* SDRAM Bank #1 */
#define CONFIG_SYS_SDRAM_BASE       0x00000000
/* SDRAM memory size */
/*
 * Enable auto calculation of sdram size. Code will ignore PHYS_SDRAM_1_SIZE
 * Auto calculation can be disabled for use case where user want to
 * utilize portion of SDRAM only (PHYS_SDRAM_1_SIZE will be used then)
 */
#define CONFIG_SDRAM_CALCULATE_SIZE
/*
 * Remain PHYS_SDRAM_1_SIZE as its still used by U-Boot memory test (mtest)
 * when user didn't specify mtest end address in console
 */
#ifdef CONFIG_SOCFPGA_VIRTUAL_TARGET
#define PHYS_SDRAM_1_SIZE       0x80000000
#else
#define PHYS_SDRAM_1_SIZE       0x40000000
#endif
/* SDRAM Bank #1 base address */
#define PHYS_SDRAM_1            CONFIG_SYS_SDRAM_BASE
/* U-Boot memtest setup */
/* Begin and end addresses of the area used by the simple memory test.c */
#define CONFIG_SYS_MEMTEST_START    0x00000000
#define CONFIG_SYS_MEMTEST_END      0x02000000


/*
 * L2 PL-310
 */
/* for configuring L2 address filtering start address in assembly */
#define SOCFPGA_MPUL2_ADRFLTR_START (0xC00)

/*
 * Generic Interrupt Controller from ARM
 */
#define SOCFPGA_GIC_CPU_IF      (SOCFPGA_MPUSCU_ADDRESS + 0x100)
#define SOCFPGA_GIC_DIC         (SOCFPGA_MPUSCU_ADDRESS + 0x1000)

/*
 * SCU Non-secure Access Control
 */
#define SOCFPGA_SCU_SNSAC       (SOCFPGA_MPUSCU_ADDRESS + 0x54)

/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#ifdef CONFIG_SYS_NS16550
#define UART0_BASE          SOCFPGA_UART0_ADDRESS
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE -4
#define CONFIG_CONS_INDEX               1
#define CONFIG_SYS_NS16550_COM1     UART0_BASE
#define CONFIG_SYS_BAUDRATE_TABLE {4800, 9600, 19200, 38400, 57600, 115200}
#if defined(CONFIG_SOCFPGA_VIRTUAL_TARGET)
#define CONFIG_SYS_NS16550_CLK      1000000
#else
#define CONFIG_SYS_NS16550_CLK      (cm_l4_sp_clock)
#endif
#define CONFIG_BAUDRATE         115200
#endif /* CONFIG_SYS_NS16550 */

/*
 * FLASH
 */
#define CONFIG_SYS_NO_FLASH

/*
 * USB
 */
#undef CONFIG_SYS_USB_ADDRESS /* SOCFPGA_USB1_ADDRESS */
#undef CONFIG_CMD_USB
#undef CONFIG_USB_DWC2_OTG
#undef CONFIG_USB_STORAGE
#undef CONFIG_USB_HOST_ETHER
#undef CONFIG_USB_ETHER_ASIX

/*
 * L4 OSC1 Timer 0
 */
/* This timer use eosc1 where the clock frequency is fixed
 * throughout any condition */
#define CONFIG_SYS_TIMERBASE        SOCFPGA_OSC1TIMER0_ADDRESS
/* reload value when timer count to zero */
#define TIMER_LOAD_VAL          0xFFFFFFFF
/* Timer info */
#define CONFIG_SYS_HZ           1000
/* Clocks source frequency to timer */
#if defined(CONFIG_SOCFPGA_VIRTUAL_TARGET)
#define CONFIG_TIMER_CLOCK_KHZ      2400
#else
/* Preloader and U-Boot need to know the clock source frequency from handoff*/
#define CONFIG_TIMER_CLOCK_KHZ      (CONFIG_HPS_CLK_OSC1_HZ / 1000)
#endif

/*
 * L4 Watchdog
 */
#define CONFIG_HW_WATCHDOG
#define CONFIG_HW_WATCHDOG_TIMEOUT_MS   (2000)
#define CONFIG_DESIGNWARE_WATCHDOG
#define CONFIG_DW_WDT_BASE      SOCFPGA_L4WD0_ADDRESS

#if defined(DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER)
#undef CONFIG_PRELOADER_WATCHDOG_ENABLE
#define CONFIG_PRELOADER_WATCHDOG_ENABLE    (0)
#endif

/* Clocks source frequency to watchdog timer */
#if defined(CONFIG_SOCFPGA_VIRTUAL_TARGET)
#define CONFIG_DW_WDT_CLOCK_KHZ     2400
#else
/* Preloader and U-Boot need to know the clock source frequency from handoff*/
#define CONFIG_DW_WDT_CLOCK_KHZ     (CONFIG_HPS_CLK_OSC1_HZ / 1000)
#endif

/*
 * network support
 */
#ifndef CONFIG_SOCFPGA_VIRTUAL_TARGET
#define CONFIG_DESIGNWARE_ETH       1
#endif

#ifdef CONFIG_DESIGNWARE_ETH
#define CONFIG_EMAC0_BASE       SOCFPGA_EMAC0_ADDRESS
#define CONFIG_EMAC1_BASE       SOCFPGA_EMAC1_ADDRESS
/* console support for network */
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
/* designware */
#define CONFIG_NET_MULTI
#define CONFIG_DW_ALTDESCRIPTOR
#define CONFIG_DW_SEARCH_PHY
#define CONFIG_MII
#define CONFIG_PHY_GIGE
#define CONFIG_DW_AUTONEG
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
/* phy */
#define CONFIG_PHY_ADDR                0
#define CONFIG_KSZ9021_CLK_SKEW_ENV    "micrel-ksz9021-clk-skew"
#define CONFIG_KSZ9021_CLK_SKEW_VAL    0xf0f0
#define CONFIG_KSZ9021_DATA_SKEW_ENV   "micrel-ksz9021-data-skew"
#define CONFIG_KSZ9021_DATA_SKEW_VAL   0x0
/* Type of PHY available */
#define SOCFPGA_PHYSEL_ENUM_GMII    0x0
#define SOCFPGA_PHYSEL_ENUM_MII     0x1
#define SOCFPGA_PHYSEL_ENUM_RGMII   0x2
#define SOCFPGA_PHYSEL_ENUM_RMII    0x3
#endif  /* CONFIG_DESIGNWARE_ETH */

/*
 * MMC support
 */
#define CONFIG_MMC
#ifdef CONFIG_MMC
#define CONFIG_SDMMC_BASE       (SOCFPGA_SDMMC_ADDRESS)
#define CONFIG_SDMMC_HOST_HS
#define CONFIG_GENERIC_MMC      1
#define CONFIG_DWMMC            1
#define CONFIG_ALTERA_DWMMC     1
#define CONFIG_DWMMC_FIFO_DEPTH     1024
/* using smaller max blk cnt to avoid flooding the limited stack we have */
#define CONFIG_SYS_MMC_MAX_BLK_COUNT     256
#define CONFIG_DWMMC_BUS_HZ     (cm_sdmmc_clock)
#if defined(CONFIG_SPL_BUILD)
#define CONFIG_DWMMC_BUS_WIDTH      CONFIG_HPS_SDMMC_BUSWIDTH
#else
#define CONFIG_DWMMC_BUS_WIDTH      4
#endif  /* CONFIG_SPL_BUILD */
#endif  /* CONFIG_MMC */

/*
 * MTD
 */
#undef CONFIG_CMD_MTDPARTS     /* Enable MTD parts commands */
#undef CONFIG_MTD_DEVICE       /* needed for mtdparts commands */
#undef CONFIG_MTD_PARTITIONS

/*
 * QSPI support
 */
#define CONFIG_CADENCE_QSPI
#define CONFIG_CQSPI_BASE       (SOCFPGA_QSPIREGS_ADDRESS)
#define CONFIG_CQSPI_AHB_BASE       (SOCFPGA_QSPIDATA_ADDRESS)
#ifdef CONFIG_CADENCE_QSPI
#define CONFIG_SPI_FLASH        /* SPI flash subsystem */
#define CONFIG_SPI_FLASH_STMICRO    /* Micron/Numonyx flash */
#define CONFIG_SPI_FLASH_SPANSION   /* Spansion flash */
#define CONFIG_SPI_FLASH_MACRONIX   /* Macronix flash */
#define CONFIG_CMD_SF           /* Serial flash commands */
/* Flash device info */
#define CONFIG_SF_DEFAULT_SPEED     (50000000)
#define CONFIG_SF_DEFAULT_MODE      SPI_MODE_3
#define CONFIG_SPI_FLASH_QUAD       (1)
/* QSPI reference clock */
#define CONFIG_CQSPI_REF_CLK        (cm_qspi_clock)
/* QSPI page size and block size */
#define CONFIG_CQSPI_PAGE_SIZE      (256)
#define CONFIG_CQSPI_BLOCK_SIZE     (16)
/* QSPI Delay timing */
#define CONFIG_CQSPI_TSHSL_NS       (200)
#define CONFIG_CQSPI_TSD2D_NS       (255)
#define CONFIG_CQSPI_TCHSH_NS       (20)
#define CONFIG_CQSPI_TSLCH_NS       (20)
#define CONFIG_CQSPI_DECODER        (0)
#endif  /* CONFIG_CADENCE_QSPI */

/* NAND */
#undef CONFIG_NAND_DENALI
#ifdef CONFIG_NAND_DENALI
#undef CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE  1
#define CONFIG_SYS_NAND_USE_FLASH_BBT
#define CONFIG_SYS_NAND_REGS_BASE   SOCFPGA_NAND_REGS_ADDRESS
#define CONFIG_SYS_NAND_DATA_BASE   SOCFPGA_NAND_DATA_ADDRESS
#define CONFIG_SYS_NAND_BASE        CONFIG_SYS_NAND_REGS_BASE
#define CONFIG_SYS_NAND_ONFI_DETECTION
/* How many bytes need to be skipped at the start of spare area */
#define CONFIG_NAND_DENALI_SPARE_AREA_SKIP_BYTES    (2)
/* The ECC size which either 512 or 1024 */
#define CONFIG_NAND_DENALI_ECC_SIZE         (512)
#endif /* CONFIG_NAND_DENALI */

/*
 * FPGA support
 */
/* Enables FPGA subsystem */
#define CONFIG_FPGA
/* Altera FPGA */
#define CONFIG_FPGA_ALTERA
/* Family type */
#define CONFIG_FPGA_SOCFPGA
/* Only support single device */
#define CONFIG_FPGA_COUNT       (1)
/* Enable FPGA command at console */
#define CONFIG_CMD_FPGA

/*
 * DMA support
 */
#define CONFIG_PL330_DMA
#define CONFIG_SPL_DMA_SUPPORT

/*
 * I2C support
 */
#define CONFIG_HARD_I2C
#define CONFIG_DW_I2C
#define CONFIG_SYS_I2C_EEPROM_ADDR 0x50
#define CONFIG_SYS_I2C_DEV_TYPE_OFF 0

#define CONFIG_SYS_I2C_BASE     SOCFPGA_I2C0_ADDRESS
/* using standard mode which the speed up to 100Kb/s) */
#define CONFIG_SYS_I2C_SPEED        (100000)
/* address of device when used as slave */
#define CONFIG_SYS_I2C_SLAVE        (0x02)
/* clock supplied to I2C controller in unit of MHz */
#define IC_CLK              (cm_l4_sp_clock / 1000000)
#define CONFIG_CMD_I2C

/*
 * SPL "Second Program Loader" aka Preloader
 */

/* Enable building of SPL globally */
/*#define CONFIG_SPL*/

/* Enable the SPL framework under common */
#define CONFIG_SPL_FRAMEWORK

/*
 * Disable cache for SPL. By default, SPL didn't enable cache. But some
 * peripheral controller driver such as SDMMC driver might call cache
 * invalidation function after using internal DMA to transfer the read data.
 */
#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_DCACHE_OFF
#endif  /* CONFIG_SPL_BUILD */

/* TEXT_BASE for linking the SPL binary */
#ifndef CONFIG_PRELOADER_EXE_ON_FPGA
#error "CONFIG_PRELOADER_EXE_ON_FPGA must be defined"
#endif
#if (CONFIG_PRELOADER_EXE_ON_FPGA == 1)
#define CONFIG_SPL_TEXT_BASE        0xC0000000
#else
#define CONFIG_SPL_TEXT_BASE        0xFFFF0000
#endif

/* SPL max size */
#define CONFIG_SPL_MAX_SIZE     (64 * 1024)

/* Support for drivers/serial in SPL binary */
#ifndef CONFIG_PRELOADER_SERIAL_SUPPORT
#error "CONFIG_PRELOADER_SERIAL_SUPPORT must be defined"
#endif
#if (CONFIG_PRELOADER_SERIAL_SUPPORT == 1)
#define CONFIG_SPL_SERIAL_SUPPORT
#endif

/* Enable spl_board_init function call */
#define CONFIG_SPL_BOARD_INIT

/*
 * define the chunk size for crc32_wd().
 * Current define smaller to avoid watchdog timeout
 */
#define CHUNKSZ_CRC32           (1 * 1024)

#define CONFIG_CRC32_VERIFY

/*
 * Linker script for SPL
 */
#if (CONFIG_PRELOADER_EXE_ON_FPGA == 1)
#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/armv7/socfpga/u-boot-spl-fpga.lds"
#else
#define CONFIG_SPL_LDSCRIPT "arch/arm/cpu/armv7/socfpga/u-boot-spl.lds"
#endif

/*
 * Support for common/libcommon.o in SPL binary
 */
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT

/*
 * Support for drivers/watchdog.o in SPL binary
 */
#ifndef CONFIG_PRELOADER_WATCHDOG_ENABLE
#error "CONFIG_PRELOADER_WATCHDOG_ENABLE must be defined"
#endif
#if (CONFIG_PRELOADER_WATCHDOG_ENABLE == 1)
#define CONFIG_SPL_WATCHDOG_SUPPORT
#else
#ifdef CONFIG_SPL_BUILD
#undef CONFIG_HW_WATCHDOG
#endif  /* CONFIG_SPL_BUILD */
#endif

/*
 * Boot from SDMMC
 */
#ifndef CONFIG_PRELOADER_BOOT_FROM_SDMMC
#error "CONFIG_PRELOADER_BOOT_FROM_SDMMC must be defined"
#endif
#if (CONFIG_PRELOADER_BOOT_FROM_SDMMC == 1)
/* Support for drivers/mmc/libmmc.o in SPL binary */
#define CONFIG_SPL_MMC_SUPPORT
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR \
    (CONFIG_PRELOADER_SDMMC_NEXT_BOOT_IMAGE / 512)
#endif

/*
 * Boot from QSPI
 */
#ifndef CONFIG_PRELOADER_BOOT_FROM_QSPI
#error "CONFIG_PRELOADER_BOOT_FROM_QSPI must be defined"
#endif
#if (CONFIG_PRELOADER_BOOT_FROM_QSPI == 1)
/* Enable spi_spl_load.c */
#define CONFIG_SPL_SPI_LOAD
/* Support for drivers/mtd/spi/libspi_flash.o in SPL binary */
#define CONFIG_SPL_SPI_FLASH_SUPPORT
/* Support for drivers/spi/libspi.o in SPL binary */
#define CONFIG_SPL_SPI_SUPPORT
/* Support for XiP */
#undef CONFIG_SPL_SPI_XIP
/* the XiP address that SPL will jump to */
#define CONFIG_SPL_SPI_XIP_ADDR     0xFFA00040
/* SPL SPI flash Chip select */
#define CONFIG_SPL_SPI_CS       0
/* SPL SPI flash Bus Number */
#define CONFIG_SPL_SPI_BUS      0
/* offset of U-Boot with spi flash */
#define CONFIG_SYS_SPI_U_BOOT_OFFS  CONFIG_PRELOADER_QSPI_NEXT_BOOT_IMAGE
#endif

/*
 * Boot from RAM
 */
#ifndef CONFIG_PRELOADER_BOOT_FROM_RAM
#error "CONFIG_PRELOADER_BOOT_FROM_RAM must be defined"
#endif
#if (CONFIG_PRELOADER_BOOT_FROM_RAM == 1)
#define CONFIG_SPL_RAM_DEVICE
#endif

/*
 * Boot from NAND
 */
#ifndef CONFIG_PRELOADER_BOOT_FROM_NAND
#error "CONFIG_PRELOADER_BOOT_FROM_NAND must be defined"
#endif
#if (CONFIG_PRELOADER_BOOT_FROM_NAND == 1)
/* Support for drivers/mmc/libmmc.o in SPL binary */
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_SIMPLE
#define CONFIG_SPL_NAND_DRIVERS
#define CONFIG_SYS_NAND_U_BOOT_OFFS CONFIG_PRELOADER_NAND_NEXT_BOOT_IMAGE
#define CONFIG_SYS_NAND_PAGE_SIZE   2048
#define CONFIG_SYS_NAND_OOBSIZE     64
#define CONFIG_SYS_NAND_BLOCK_SIZE  (128 * 1024)
/* number of pages per block */
#define CONFIG_SYS_NAND_PAGE_COUNT  64
/* location of bad block marker within OOB */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS   0
/* to use the ecc.read_page and ecc.read_page_raw */
#define CONFIG_SPL_USE_ECC_READ
/* To allocate buffer size for Denali driver. Cannot use standard as its
 * too large which bloated Preloader a lot
 */
#define NAND_MAX_OOBSIZE    CONFIG_SYS_NAND_OOBSIZE
#define NAND_MAX_PAGESIZE   CONFIG_SYS_NAND_PAGE_SIZE
#endif

/*
 * Support for checksum in SPL binary
 */
#ifndef CONFIG_PRELOADER_CHECKSUM_NEXT_IMAGE
#error "CONFIG_PRELOADER_CHECKSUM_NEXT_IMAGE must be defined"
#endif
#if (CONFIG_PRELOADER_CHECKSUM_NEXT_IMAGE == 1)
#define CONFIG_SPL_CHECKSUM_NEXT_IMAGE
#endif

/*
 * Preloader State Register address. Write STATE_VALID to STATE_REG to
 * tell BootROM that Preloader run successfully.
 */
#ifndef CONFIG_PRELOADER_STATE_REG_ENABLE
#error "CONFIG_PRELOADER_STATE_REG_ENABLE must be defined"
#endif
#if (CONFIG_PRELOADER_STATE_REG_ENABLE == 1)
#define CONFIG_PRELOADER_STATE_REG  (0xFFD080C8)
#define CONFIG_PRELOADER_STATE_VALID    (0x49535756)
#endif

/*
 * Support for ARM semihosting in SPL
 */
#ifndef CONFIG_PRELOADER_SEMIHOSTING
#error "CONFIG_PRELOADER_SEMIHOSTING must be defined"
#endif
#if (CONFIG_PRELOADER_SEMIHOSTING == 1) && defined(CONFIG_SPL_BUILD)
#define CONFIG_SPL_SEMIHOSTING_SUPPORT
#endif

/*
 * Support for FAT partition if boot from SDMMC
 */
#if (CONFIG_PRELOADER_BOOT_FROM_SDMMC == 1 && CONFIG_PRELOADER_FAT_SUPPORT == 1)
/* MMC with FAT partition support */
#define CONFIG_SPL_FAT_SUPPORT
#endif

#ifdef CONFIG_SPL_FAT_SUPPORT
#define CONFIG_SPL_LIBDISK_SUPPORT
#define CONFIG_SYS_MMC_SD_FAT_BOOT_PARTITION    \
                CONFIG_PRELOADER_FAT_BOOT_PARTITION
#define CONFIG_SPL_FAT_LOAD_PAYLOAD_NAME    \
                CONFIG_PRELOADER_FAT_LOAD_PAYLOAD_NAME
#endif /* CONFIG_SPL_FAT_SUPPORT */

/*
 * Stack setup
 */
#define CONFIG_SPL_STACK        CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SYS_SPL_MALLOC_START ((unsigned long) (&__malloc_start))
#define CONFIG_SYS_SPL_MALLOC_SIZE  (&__malloc_end - &__malloc_start)

/* Stack size for SPL */
#ifdef CONFIG_SPL_FAT_SUPPORT
#define CONFIG_SPL_STACK_SIZE       (5 * 1024)
/*
 * New stack size which located within SDRAM after SDRAM is available.
 * FYI, 128kB stack can support fatload up to 7MB bootloader image
 */
#define CONFIG_SPL_SDRAM_STACK_SIZE (128 * 1024)
#else
#define CONFIG_SPL_STACK_SIZE       (4 * 1024)
#endif

/* MALLOC size for SPL */
#define CONFIG_SPL_MALLOC_SIZE      (5 * 1024)

/*
 * FPGA programming support with SPL
 * FPGA RBF file source (with mkimage header) is located within the same
 * boot device which stored the subsequent boot image (U-Boot).
 */
/* enabled program the FPGA */
#undef CONFIG_SPL_FPGA_LOAD
/* location of FPGA RBF image within QSPI */
#define CONFIG_SPL_FPGA_QSPI_ADDR   (0x800000)
/* RBF file name if its located within SD card */
#define CONFIG_SPL_FPGA_FAT_NAME    "fpga.rbf"

/* ensure FAT is defined if CONFIG_SPL_FPGA_LOAD is defined */
#ifdef CONFIG_SPL_FPGA_LOAD
#if (CONFIG_PRELOADER_BOOT_FROM_SDMMC == 1 && !defined(CONFIG_SPL_FAT_SUPPORT))
#error "CONFIG_SPL_FAT_SUPPORT required for  CONFIG_SPL_FPGA_LOAD"
#endif  /* CONFIG_SPL_FAT_SUPPORT */
#endif  /* CONFIG_SPL_FPGA_LOAD */

/*
 * Enable memory padding if SDRAM ECC is enabled
 */
#if (CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_ECCEN == 1)
#define CONFIG_SPL_SDRAM_ECC_PADDING    32
#endif

#ifdef CONFIG_SPL_BUILD
#include "../../board/altera/socfpga/iocsr_config_cyclone5.h"
#endif

/*
 * Console setup
 */
/* Monitor Command Prompt */
#define CONFIG_SYS_PROMPT		"M48_UP2# "

/* EMAC controller and PHY used */
#define CONFIG_EMAC_BASE		CONFIG_EMAC0_BASE
#define CONFIG_PHY_INTERFACE_MODE	SOCFPGA_PHYSEL_ENUM_RGMII

/* Define machine type for Cyclone 5 */
#define CONFIG_MACH_TYPE 4251

#define CONFIG_PRAM                 32768

#endif	/* __CONFIG_H */
