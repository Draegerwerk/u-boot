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

#include <common.h>
#include <version.h>
#include <i2c.h>
#include <linux/ctype.h>
#include <watchdog.h>
#include <post.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/reset_manager.h>
#include <asm/arch/system_manager.h>
#include <draeger_m48_pmstruct.h>

DECLARE_GLOBAL_DATA_PTR;

#define DEVICE_TYPE_MNEMONIC   7
#define DEVICE_TYPE_DEFAULT   "M48Test"

#define     PM_BSP_MAGIC              0x07101973
PmBootData* m48PmData = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;

#if (CONFIG_SPL_BUILD && ((CONFIG_PRELOADER_WARMRST_SKIP_CFGIO == 1) || \
(CONFIG_HPS_RESET_WARMRST_HANDSHAKE_SDRAM == 1)))
extern u32 rst_mgr_status;
void checkWarmstart()
{
    int i;
	int post_boot_mode;

    printf("RESET: %08x\n", rst_mgr_status);

    if (rst_mgr_status & RSTMGR_COLDRST_MASK) puts("RESET: COLD mask\n");
    if (rst_mgr_status & RSTMGR_WARMRST_MASK) puts("RESET: WARM mask\n");
    if (rst_mgr_status & RSTMGR_STAT_NRSTPINRST_MASK) puts("RESET: WARM reset from Pin\n");

    if (!(rst_mgr_status & (RSTMGR_STAT_NRSTPINRST_MASK | RSTMGR_COLDRST_MASK)))
    {
    	post_boot_mode = post_bootmode_get(0);
    	/* do not reset the other controller when in POST */
    	if ( post_boot_mode != 0 && ! (post_boot_mode & POST_POWERTEST)) {
    		WATCHDOG_RESET();
    		puts("reseting uP1\n");
    		gpio_direction_output(CONFIG_GPIO_BOARD_COLD_RESET, 0);
    		for (i=0; i< 800; i++) {
    			udelay(1000);
    		}
    		WATCHDOG_RESET();
    		gpio_direction_output(CONFIG_GPIO_BOARD_COLD_RESET, 1);
    		puts("reseting uP1 done\n");
    	} else {
    		printf("No uP1 reset due to POST %x\n", post_boot_mode);
    	}
    }

}
#else
void checkWarmstart() {}
#endif



void updateM48PmStructChecksum(void)
{
    PmBootData* data;
    data = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;
    data->data_size=sizeof(PmBootData) - offsetof(PmBootData, data_size);
    data->checkSum = crc32 (0, (const void *) &data->data_size, data->data_size);
    flush_cache(data, sizeof(PmBootData));
}

#if (CONFIG_SPL_BUILD)
#define BOOTMODE_MAGIC	0xDEAD0000

int post_bootmode_get(unsigned int *last_test)
{
    unsigned long word = post_word_load();
    int bootmode;

    if ((word & 0xFFFF0000) != BOOTMODE_MAGIC)
        return 0;

    bootmode = word & 0x7F;

    if (last_test && (bootmode & POST_POWERTEST))
        *last_test = (word >> 8) & 0xFF;

    return bootmode;
}
#endif

void storeM48UbootVersion(void)
{
	int post_boot_mode;
    unsigned long reg;
    char *vers;
    int len;
    const char *token = "-draeger_";
    vers = strstr(U_BOOT_VERSION, token);

#if (CONFIG_SPL_BUILD)
    vers += 9; /* strlen(token); */
    snprintf(m48PmData->uboot_version, sizeof(m48PmData->uboot_version), "SPL %s,", vers);

	post_boot_mode = post_bootmode_get(0);
	if (post_boot_mode == 0 ||
			(post_boot_mode & (POST_POWERON | POST_POWERTEST)) ==
					(POST_POWERON | POST_POWERTEST)) {
    	m48PmData->startType = M48_START_TYPE_POWERUP;
	} else {

		if (rst_mgr_status & RSTMGR_WARMRST_MASK) {
			m48PmData->startType = M48_START_TYPE_WARM_REBOOT;
		} else {
			m48PmData->startType = M48_START_TYPE_COLD_REBOOT;
		}
	}
#else
    vers += 1;
    len = strnlen(m48PmData->uboot_version, sizeof(m48PmData->uboot_version));
    if (len > 16) len = 16;
    memcpy(m48PmData->uboot_version + len, vers, 32 - len);
    m48PmData->uboot_version[sizeof(m48PmData->uboot_version)-1] = '\0';
#endif

    updateM48PmStructChecksum();

    reg = readl(CONFIG_SYSMGR_ECC_OCRAM);
    if (reg & SYSMGR_ECC_OCRAM_SERR)
        writel(SYSMGR_ECC_OCRAM_SERR | SYSMGR_ECC_OCRAM_EN,
            CONFIG_SYSMGR_ECC_OCRAM);
    if (reg & SYSMGR_ECC_OCRAM_DERR)
        writel(SYSMGR_ECC_OCRAM_DERR  | SYSMGR_ECC_OCRAM_EN,
            CONFIG_SYSMGR_ECC_OCRAM);

}

/*
 * Print Board information
 */
int checkboard(void)
{
#ifdef CONFIG_SOCFPGA_VIRTUAL_TARGET
    puts("BOARD : Altera VTDEV5XS1 Virtual Board\n");
#else
    checkWarmstart();
#endif
    storeM48UbootVersion();
    return 0;
}

ulong post_word_load(void)
{
    PmBootData* data;
    data = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;
    return data->magicConstant;
}

void post_word_store(ulong value)
{
    PmBootData* data;
    data = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;
    data->magicConstant = value;
    updateM48PmStructChecksum();
}

/* This function will be called after verification of the kernel image and before jumping into it */
void arch_preboot_os(void)
{
	/* inform uP1 about a successful load of the FPGA before jumping to kernel */
    gpio_direction_output(CONFIG_GPIO_FPGA_PRESENT, 0);
}

static void do_get_device_type (cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    PmBootData* data;
    data = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;
    char buf[M48_DEVICE_TYPE_SIZE];
    char printb[M48_DEVICE_TYPE_SIZE];
    int i;

    /* todo use separate command ? */
    /* kill fpga */
    fpga_load(0, printb, 0);

    /* Read device_type from EEPROM */
    if ( (i2c_probe(CONFIG_SYS_I2C_EEPROM_ADDR) != 0) ||
            (i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_DEV_TYPE_OFF, 1, buf, M48_DEVICE_TYPE_SIZE) != 0) ) {
        printf("\nEEPROM @ 0x%02x read FAILED!!!\n",
               CONFIG_SYS_I2C_EEPROM_ADDR);
    } else {
        memcpy(data->device_type, buf, M48_DEVICE_TYPE_SIZE);
        updateM48PmStructChecksum();

        for (i=0; i<DEVICE_TYPE_MNEMONIC; i++) {
            if (isprint(buf[i]))
                printb[i] = data->device_type[i];
            else
                printb[i] = '\0';
        }
        printb[DEVICE_TYPE_MNEMONIC] = '\0';
        printf("device type '%s'\n", printb);

    }

}

static int do_check_device_type (cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{

    PmBootData* data = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;
    struct image_header *header;
    char *image_name;
    unsigned int offset = 0;
    char printb[DEVICE_TYPE_MNEMONIC];
    int i;

    offset = simple_strtoul(argv[1], NULL, 16);
    header = (struct image_header *) offset;

    if (image_get_magic(header) == IH_MAGIC) {
        image_name = image_get_name(header);

        for (i=0; i<DEVICE_TYPE_MNEMONIC; i++) {
            if (isprint(image_name[i]))
                printb[i] = image_name[i];
            else
                printb[i] = '\0';
        }
        printb[DEVICE_TYPE_MNEMONIC] = '\0';
        printf("FPGA type '%s'\n", printb);


        if ( (memcmp(image_name, data->device_type, DEVICE_TYPE_MNEMONIC) == 0) ||
        		(memcmp(image_name, DEVICE_TYPE_DEFAULT, DEVICE_TYPE_MNEMONIC) == 0)) {
        	return 0;
        }
    }

    return 1;
}

U_BOOT_CMD(
    devtype, 1,  1,  do_get_device_type,
    "Store device type.",
    "Read device type from EEPROM and set into post mortem boot data.\n"
    "getdevtype"
);

U_BOOT_CMD(
    cktype, 2,  1,  do_check_device_type,
    "Check device type vs. FPGA type.",
    "Check the device type in the post mortem boot data vs. FPGA device header.\n"
    "checkdevtype imageaddress"
);

