/*
 *  (C) Copyright 2020 Draeger and Licensors, info@draeger.com
 *
 *  SPDX-License-Identifier: GPL-2.0+
 */


#include <common.h>
#include <cpu_func.h>
#include <post.h>
#include <u-boot/zlib.h>
#include <linux/ctype.h>
#include <mmc.h>
#include <env.h>
#include <net.h>
#include <image.h>
#include <bootstage.h>
#include <asm/global_data.h>

#include "draeger_m48_pmstruct.h"
#include "draeger_common.h"

#define MAX_TMP_IP_ADDR_SIZE        20

#define FIRST_BANK_INDEX            0

DECLARE_GLOBAL_DATA_PTR;

ulong post_word_load(void)
{
    PmBootData* data;
    data = (PmBootData*) CFG_SYS_PMSTRUCT_ADDR;

    return data->magicConstant;
}

void updateM48PmStructChecksum(void)
{
    PmBootData* data;
    data = (PmBootData*) CFG_SYS_PMSTRUCT_ADDR;
    data->data_size=sizeof(PmBootData)-offsetof(PmBootData, data_size);
    data->checkSum = crc32 (0, (const void *) &data->data_size, data->data_size);
    flush_cache((unsigned long) data, ALIGN(sizeof(PmBootData), CONFIG_SYS_CACHELINE_SIZE));
}

#ifndef CONFIG_SPL_BUILD

void post_word_store(ulong value)
{
    PmBootData* data;
    data = (PmBootData*) CFG_SYS_PMSTRUCT_ADDR;
    data->magicConstant = value;
    updateM48PmStructChecksum();
}

char cmdbuf[CMD_BUF_SIZE];

int check_name(char *imagepath_env, int size)
{
    /* allow only alphanumerical characters and /-. */
    int i;
    if (imagepath_env == NULL) {
        return 0;
    }

    for (i = 0; i < size; i++) {

        if (imagepath_env[i] == '\0') {
            return 1;
        }

        if ( ! (isalnum(imagepath_env[i]) ||
            imagepath_env[i] == '/' ||
            imagepath_env[i] == '.' ||
            imagepath_env[i] == '-' )
            ) {

            return 0;
        }
    }
    return 1;
}

char* get_image_path(const char *default_path)
{
    char *imagepath = (char*) default_path;
    char *imagepath_env = env_get("imagepath");
    if (check_name(imagepath_env, CMD_BUF_SIZE)) imagepath = imagepath_env;
    return imagepath;
}

char* get_image_name(const char *default_name)
{
    char *image_name = (char*) default_name;
    char *image_name_env = env_get("itb_name");
    if (check_name(image_name_env, CMD_BUF_SIZE)) image_name = image_name_env;
    return image_name;
}

char* get_legacy_image_name(const char *default_name)
{
    char *image_name = (char*) default_name;
    char *image_name_env = env_get("image_name");
    if (check_name(image_name_env, CMD_BUF_SIZE)) image_name = image_name_env;
    return image_name;
}

/* this test is part of the reset test in order to avoid an additional
 *  reboot of the board
 */
#define CHAR_BIT                               8
#define PERSISTEN_MEMORY_TEST_SIZE             (VXWORKS_PM_RESERVED_MEM)
#define PERSISTEN_MEMORY_TEST_PATTERN          0x96
#define PERSISTEN_MEMORY_TEST_MAX_FAILING_BITS 1

char* getPmMemoryRegionInDDR3(void)
{
    uint64_t endOfSdramAddr = 0;
    uint64_t pmAddr = 0;

    /*
     * PM address is dictated by VxWorks, and appears to be changing with every VxWorks release.
     * VxWorks uses first mememory bank for persistent memory.
     * Here, we are trying to keep U-Boot in sync with VxWorks.
     */

     /* Check if the first bank was configured */
    if (gd->bd->bi_dram[FIRST_BANK_INDEX].size != 0) {

        /* Calculate end address of SDRAM's first bank */
        endOfSdramAddr = gd->bd->bi_dram[FIRST_BANK_INDEX].start + gd->bd->bi_dram[FIRST_BANK_INDEX].size;

        /*
         * The VxWorks PM region is of size PM_RESERVED_MEM (which needs to be reserved via CONFIG_SYS_MEM_TOP_HIDE),
         * and until now it was the last entity of the first memory bank.
         * Another slice DMA32_HEAP_SIZE is reserved for DMA cache from the end of the memory offsetting the address of PM region.
         */
        pmAddr =  endOfSdramAddr - VXWORKS_PM_RESERVED_MEM - VXWORKS_DMA32_HEAP_SIZE;
    }

    if( (pmAddr <  gd->bd->bi_dram[FIRST_BANK_INDEX].start) || (pmAddr >= endOfSdramAddr)) {
        return (char*)NULL;
    } else {
        return (char*)((uint32_t)pmAddr);
    }
}

void initializePMSaveMemory(void)
{
    char* m48PmUsrData;

    m48PmUsrData = getPmMemoryRegionInDDR3();
    post_log("\ninitializePMSaveMemory\n");
    if(m48PmUsrData) {
        post_log("start addr: %p\n", m48PmUsrData);
        post_log("size: %zu\n", PERSISTEN_MEMORY_TEST_SIZE);
        memset(m48PmUsrData, PERSISTEN_MEMORY_TEST_PATTERN, PERSISTEN_MEMORY_TEST_SIZE);
        flush_dcache_all();
    } else {
        post_log("failed\n");
    }
}

void checkPMSaveMemory(void)
{
    int i = 0;
    char bit;
    char mask;
    char count;
    char* m48PmUsrData;
    m48PmUsrData = getPmMemoryRegionInDDR3();
    if(m48PmUsrData) {
        for (i = 0; i < PERSISTEN_MEMORY_TEST_SIZE; i++) {
        	/* check if the byte is equal PERSISTEN_MEMORY_TEST_PATTERN */
            if (m48PmUsrData[i] != PERSISTEN_MEMORY_TEST_PATTERN) {
            	/* if not check number of mismatching bits */
            	for (count=0, bit = 0; bit < CHAR_BIT - 1; bit ++ )
            	{
            		mask = 1 << bit;
            		if ((m48PmUsrData[i] & mask) != (PERSISTEN_MEMORY_TEST_PATTERN & mask)) {
            			count ++;
            			if (count > PERSISTEN_MEMORY_TEST_MAX_FAILING_BITS) {
            				break;
            			}
            		}
            	}
            }
        }
    }
    if ((i == PERSISTEN_MEMORY_TEST_SIZE) && (m48PmUsrData != NULL)) {
        m48PmData->post_pmTest.result = M48_TS_PASS;
        post_log("\ncheckPMSaveMemory PASSED\n");
    } else {
        m48PmData->post_pmTest.result = M48_TS_FAIL;
        post_log("\ncheckPMSaveMemory, failed");
        if(m48PmUsrData) {
            post_log(" %08x+%08x data %02x\n", m48PmUsrData, i, m48PmUsrData[i]);
        } else {
            post_log("\n");
        }
    }
    m48PmData->post_pmTest.magic = PM_MEMORY_MAGIC;
    updateM48PmStructChecksum();

}

#ifdef CONFIG_POST
#ifdef CONFIG_SYS_POST_BSPEC3
int board_test_run_always (int flags) {

    if( m48PmData->timestamp_kernelloaded == CFG_SYS_ABS_TIME_OFFSET ) {
        /* We have not processed POST otherwise timestamp_kernelloaded contains the timestamp before last restart */
        m48PmData->timestamp_post = post_time_ms (0) + CFG_SYS_ABS_TIME_OFFSET;

    } else {
        m48PmData->timestamp_post += post_time_ms (0);

    }
    return 0;
}
#endif
#endif

#ifdef show_boot_progress
/*
 * In U-boot 2021.10, definition of show_boot_progress() got linked to CONFIG_BOOTSTAGE.
 * Enabling it causes other build issues, so decision was made to leave it disabled.
 * This causes, however, macro definition of show_boot_progress() in bootstage.h,
 * which has to be undefined here.
 *
 * Another workaround has been implemented in bootstage.h (See comment there)
 */
#undef show_boot_progress
#endif

void show_boot_progress(int val) {
    switch (val) {
        case BOOTSTAGE_ID_KERNEL_LOADED:
            /* HACK: the vxworks start of u-boot expects a legacy image,
             * in order to be able to boot from a fit image this flag is set here
             */
            images.legacy_hdr_valid = 1;
            break;

        case BOOTSTAGE_ID_RUN_OS:
            m48PmData->timestamp_kernelloaded += post_time_ms(0);
            updateM48PmStructChecksum();
            break;

        default: ;
    }
}

#define ENV_KEY_KERNEL_SIZE "kernel_size"
#define ENV_KEY_FDT_SIZE    "fdt_size"

void set_ramboot_env(ulong kernel_size, ulong fdt_size, const char *fdt_name)
{
	env_set_hex(ENV_KEY_KERNEL_SIZE, kernel_size);
	env_set_hex(ENV_KEY_FDT_SIZE, fdt_size);
}

void get_ramboot_env(ulong* kernel_size, ulong* fdt_size)
{
	*kernel_size = env_get_hex(ENV_KEY_KERNEL_SIZE, 0);
	*fdt_size = env_get_hex(ENV_KEY_FDT_SIZE, 0);
}


int start_legacy_image(void)
{
    snprintf(cmdbuf, CMD_BUF_SIZE, "bootm %lx - %lx" , (ulong) CONFIG_SYS_LOAD_ADDR, (ulong) CFG_SYS_FDT_ADDR);
    puts (cmdbuf);
    puts ("\n");
    if (run_command(cmdbuf, 0)) return -1;
    return 0;
}

static int load_kernel_from_partition(int partition, const char *kernel_name, const char *fdt_name, bool loadonly)
{
    char *imagepath = get_image_path(IMAGE_PATH);
    ulong kernel_size = 0;
    ulong fdt_size = 0;
    int   mmc_device;

    mmc_device = getBootMmcDevice();

    const char loadcmd[] = "fatload mmc %d:%d %lx %s/%s";

    snprintf(cmdbuf, CMD_BUF_SIZE, loadcmd, mmc_device, partition, (ulong) CONFIG_SYS_LOAD_ADDR, imagepath, kernel_name);
    puts (cmdbuf);
    puts ("\n");

    if (run_command(cmdbuf, 0)) return -1;
    kernel_size = env_get_hex("filesize", 0);

    if (fdt_name != 0) {
        snprintf(cmdbuf, CMD_BUF_SIZE, loadcmd, mmc_device, partition, (ulong) CFG_SYS_FDT_ADDR, imagepath, fdt_name);
        puts (cmdbuf);
        puts ("\n");
        if (run_command(cmdbuf, 0)) return -1;

        fdt_size = env_get_hex("filesize", 0);
    }
    
    if (load_extra_images_from_partition(partition)) return -1;

    m48PmData->bootmode=M48_BM_FAT | partition;

    if ( ! loadonly) {
    	return start_kernel_image(kernel_size, fdt_size);
    } else {
    	set_ramboot_env(kernel_size, fdt_size, fdt_name);
    }
    return 1;
}

static int load_kernel_from_tftp(const char *kernel_name, const char *fdt_name, bool loadonly)
{
    char *imagepath = get_image_path(IMAGE_PATH);
    ulong kernel_size = 0;
    ulong fdt_size = 0;

    const char tftpcmd[] = "tftp %lx %s/%s";

    snprintf(cmdbuf, CMD_BUF_SIZE, tftpcmd, (ulong) CONFIG_SYS_LOAD_ADDR, imagepath, kernel_name);
    if (run_command(cmdbuf, 0)) return -1;
    kernel_size = env_get_hex("filesize", 0);

    if (fdt_name != 0) {
        snprintf(cmdbuf, CMD_BUF_SIZE, tftpcmd, (ulong) CFG_SYS_FDT_ADDR, imagepath, fdt_name);
        if (run_command(cmdbuf, 0)) return -1;

        fdt_size = env_get_hex("filesize", 0);
    }

    if (load_extra_images_from_tftp()) return -1;

    m48PmData->bootmode=M48_BM_TFTP;

    if ( ! loadonly) {
    	return start_kernel_image(kernel_size, fdt_size);
    } else {
    	set_ramboot_env(kernel_size, fdt_size, fdt_name);
    }
    return 1;
}

void setEnvBootArgs(const char *netDeviceName)
{
#ifndef CONFIG_HAB_REVOCATION_TEST_IMAGE

    char serverip_str[MAX_TMP_IP_ADDR_SIZE] = {0};
    char ipaddr_str[MAX_TMP_IP_ADDR_SIZE] = {0};
    char gwip_str[MAX_TMP_IP_ADDR_SIZE] = {0};
    char netmask_str[MAX_TMP_IP_ADDR_SIZE] = {0};

    struct in_addr serverip, ipaddr, gwip;
    ulong netmask;

    serverip = env_get_ip("serverip");

    if (serverip.s_addr) {
        ip_to_string(serverip, serverip_str);
    }

    ipaddr = env_get_ip("ipaddr");
	ip_to_string(ipaddr, ipaddr_str);

	gwip = env_get_ip("gatewayip");
    if (gwip.s_addr) {
        ip_to_string(gwip, gwip_str);
    }
    netmask = env_get_hex("netmask", 0);
    if (netmask != 0) {
        snprintf(netmask_str,sizeof(netmask_str), "%0lx", netmask);
    }

    snprintf(cmdbuf, CMD_BUF_SIZE, "setenv bootargs \"%s(0,0)host:vxWorks h=%s e=%s:%s g=%s u=target pw=vxTarget\"",
    		netDeviceName, serverip_str, ipaddr_str, netmask_str, gwip_str);

    run_command(cmdbuf, 0);

#endif
}

int startKernel (const char *kernel_name, const char *fdt_name, bool loadonly)
{
    const char *use_tftp = NULL;
    int partition;
    int result = 0;

#ifdef CONFIG_HAB_REVOCATION_TEST_IMAGE
    printf("Booting SRK revocation test kernel\n");
#else
    use_tftp = env_get("use_tftp");
#endif

    if ((use_tftp == NULL) || (strcmp(use_tftp, "force") != 0)) {

        snprintf(cmdbuf, CMD_BUF_SIZE, "mmc dev %d", getBootMmcDevice());
        if (run_command(cmdbuf, 0) == 0 &&
            run_command("mmc rescan", 0) == 0)
        {
            for (partition = 1; partition <= CFG_SYS_M48_MAX_PARTITIONS; partition++) {
                if (env_set_ulong("mmcpart", partition) ) {
                    result = -1;
                    break;
                }
                printf("Booting from partition %d\n", partition);

                if ((result = load_kernel_from_partition(partition, kernel_name, fdt_name, loadonly)) >= 0) {
                	break;
                }
            }
        }

    }
    if ((result <= 0) && (use_tftp != NULL) &&
        ((strcmp(use_tftp, "force") == 0) || (strcmp(use_tftp, "yes") == 0) || (strcmp(use_tftp, "y") == 0)) ) {
        result = load_kernel_from_tftp(kernel_name, fdt_name, loadonly);
    }

    return display_error_message();
}

void fdt_fixup_serial_output(void *fdt)
{
    int offset;

    /* find offset of chosen node */
    offset = fdt_path_offset(fdt, "/chosen");

    /* delete the stdout-path property */
    (void) fdt_delprop(fdt, offset, "stdout-path");
}

int do_startKernel (struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	bool loadonly = false;
    char *image_name = get_image_name(CFG_SYS_ITB_IMAGE_NAME);

	if (argc >= 2) {
		loadonly = (strcmp("loadonly", argv[1]) == 0);
	}

    setEnvBootArgs(CFG_SYS_ENET_NAME);

    return startKernel(image_name, NULL, loadonly);
}

int do_startM48 (struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	bool loadonly = false;
    char *image_name = get_legacy_image_name(CFG_SYS_LEGACY_IMAGE);

	if (argc >= 2) {
		loadonly = (strcmp("loadonly", argv[1]) == 0);
	}
    setEnvBootArgs(CFG_SYS_ENET_NAME);

    return startKernel(image_name, get_ftd_name(), loadonly);
}

int do_bootKernelImage (struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	ulong kernel_size;
	ulong fdt_size;

    get_ramboot_env(&kernel_size, &fdt_size);
    return start_kernel_image(kernel_size, fdt_size);
}

U_BOOT_CMD(
    startKernel, 2, 1, do_startKernel,
    "Start boot process",
    "Starts Draeger's special boot sequence\n"
	"    [loadonly] - do not execute"
);


U_BOOT_CMD(
    startM48, 2, 1, do_startM48,
    "Start legacy boot process",
    "Starts Draeger's legacy special boot sequence\n"
	"    [loadonly] - do not execute"
);

U_BOOT_CMD(
    bootRam, 1, 1, do_bootKernelImage,
    "Start image from RAM",
    "Start image which was formerly loaded to RAM"
);

#else

/* empty function to remove linker error for SPL */
void show_boot_progress(int mark){};

/* Post will not be compiled for SPL */
#define BOOTMODE_MAGIC  0xDEAD0000
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

