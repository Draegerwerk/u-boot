// SPDX-License-Identifier: GPL-2.0+ */
/*
 *  (C) Copyright 2020 Draeger and Licensors, info@draeger.com
 *
 */
#ifndef __CONFIG_SOCFPGA_CYCLONE5_M48_H__
#define __CONFIG_SOCFPGA_CYCLONE5_M48_H__

/* define this in order to make SPL/UBoot wait in a while(1) for convenient JTAG-Debugger attaching
#define DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER
#define DEBUG_WAIT_FOR_JTAG_DEBUGGER
*/

/* CONFIG_TARGET_SOCFPGA_CYCLONE5_M48 is defined by kconfig */

#include <asm/arch/base_addr_ac5.h>
#include <linux/sizes.h>

/* Define machine type for Cyclone 5 */
#define CONFIG_MACH_TYPE 4251

#define CONFIG_ARM_ERRATA_743622
#define CONFIG_ARM_ERRATA_751472
#define CONFIG_ARM_ERRATA_845369

#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV      0
#endif

/* Memory configurations */
#define PHYS_SDRAM_SIZE                 SZ_1G

#define CONFIG_STANDALONE_LOAD_ADDR 0xFFFF0000 /* OCRAM */

/* VXWORKS_PM_RESERVED_MEM is the setting in VxWorks PM_RESERVED_MEM which
 * reserves memory for power save memory */
#define VXWORKS_PM_RESERVED_MEM (5412u * 4096u)
/* VXWORKS_DMA32_HEAP_SIZE is the setting in VxWorks DMA32_HEAP_SIZE which
 * reserves space for DMA cache in kernel */
#define VXWORKS_DMA32_HEAP_SIZE 0
/* both values define the offset of the power save memory area;
 * do not access it by u-boot*/
#ifdef CONFIG_SYS_MEM_TOP_HIDE
#undef CONFIG_SYS_MEM_TOP_HIDE
#endif
#define CONFIG_SYS_MEM_TOP_HIDE (VXWORKS_PM_RESERVED_MEM + VXWORKS_DMA32_HEAP_SIZE)

#ifdef CONFIG_SPL_BUILD
#   define CFG_MALLOC_F_ADDR         0xFFFFD000 /* malloc in OCRAM */
/* OCRAM memory map:
 * FFFF0000..FFFFCFFF text
 * FFFFD000..FFFFDFFF 4k malloc area     CFG_MALLOC_F_ADDR CONFIG_SPL_SYS_MALLOC_F_LEN
 * FFFFE000..FFFFEFFF 4k reserved OCRAM
 * FFFFF800..FFFFF900 256 Byte PMSTRUCT  CFG_SYS_PMSTRUCT_ADDR
 * FFFFF900..FFFFFF20 stack
 * FFFFFF20..FFFFFFFF gd
 */
#endif

#define CONFIG_ENV_OVERWRITE
#define CFG_ENV_FLAGS_LIST_STATIC "netmask:x"

#ifndef __ASSEMBLY__
int board_reset_post_test(int);
int board_test_run_always(int);
#endif

#ifndef CONFIG_SPL_BUILD

#define CFG_POST ( \
    CFG_SYS_POST_WATCHDOG | \
    CFG_SYS_POST_BSPEC1 | \
    CFG_SYS_POST_BSPEC3 | \
    CFG_SYS_POST_MEMORY | \
    CFG_SYS_POST_MEM_REGIONS)

#define CFG_POST_BSPEC1    {             \
    "Board reset test",             \
    "board_reset",                  \
    "This test verifies the functionality of the external board reset.",    \
    POST_RAM | POST_POWERON | POST_MANUAL | POST_REBOOT,    \
    &board_reset_post_test,             \
    NULL,                       \
    NULL,                       \
    CFG_SYS_POST_BSPEC1              \
    }

#define CFG_POST_BSPEC3    {             \
    "M48 board specific tests (run always)",                \
    "boardtest_always",                 \
    "This test measures boot/post time.",   \
    POST_RAM | POST_POWERON | POST_NORMAL,  \
    &board_test_run_always,             \
    NULL,                       \
    NULL,                       \
    CFG_SYS_POST_BSPEC3              \
    }
#else
#   define CFG_POST 0
#endif

#define CFG_POST_EXTERNAL_WORD_FUNCS

#ifdef LEGACY_BOOT
#   define CONFIG_BOOTCOMMAND "startM48;errorMsg"
#else
#   define CONFIG_BOOTCOMMAND "startKernel;errorMsg"
#endif

/******************************************************************************
 * M48 special defines
 *****************************************************************************/
#define CFG_SYS_PMSTRUCT_ADDR 0xfffff800

/* absolute ballpark figure for time offset in ms up to the internal clock is running */
#define CFG_SYS_ABS_TIME_OFFSET 450

#define CFG_SYS_ITB_IMAGE_NAME  "vxWorks2.itb"
#define CFG_SYS_LEGACY_IMAGE    "uVxWorks2"
#define CFG_SYS_ENET_NAME       "emac"
#define CFG_SYS_M48_MAX_PARTITIONS 3

#define CFG_SYS_FDT_ADDR        0x0F000000

#define CFG_EXTRA_ENV_SETTINGS \
    "use_tftp=no\0"\
    "loadimage=startM48 loadonly\0" \
	"ramboot=bootRam\0" \
    "imagepath=boot\0" \
    "image_name=" CFG_SYS_LEGACY_IMAGE "\0" \
    "itb_name=" CFG_SYS_ITB_IMAGE_NAME "\0" \
    "netretry=20\0" \
    /** "post_poweron=board_reset watchdog\0" **/

/* The rest of the configuration is shared */
#include <configs/socfpga_common.h>

#endif	/* __CONFIG_SOCFPGA_CYCLONE5_H__ */
