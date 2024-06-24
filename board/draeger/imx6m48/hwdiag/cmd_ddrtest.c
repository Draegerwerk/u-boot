/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <env.h>
#include <errno.h>
#include <image.h>
#include <dm/uclass.h>
#include <thermal.h>


DECLARE_GLOBAL_DATA_PTR;

/*
 * NOTE:
 * While developing the DDR tests and running them on 2GB of RAM,
 * the following was observed:
 *		0x50000000 maps to 0x30000000
 *		0x60000000 maps to 0x40000000
 *		0x70000000 maps to 0x30000000
 *
 *		0xC0000000 maps to 0xA0000000
 *		0xD0000000 maps to 0xB0000000
 *		0xE0000000 maps to 0xA0000000
 *		0xF0000000 maps to 0xB0000000
 */


static int do_ddr_test(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{

	struct udevice *thermal_dev;
	int cpu_tmp, retcpu;
	int ret = 0;
	int loop = 0;
	int loop_load = false;
	struct cmd_tbl *fatload_cmd, *iminfo_cmd;
	char addr_str[16];
	char *load_argv[6] =
	{
		/* fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${imagepath}/${image_name} */
		"fatload", "mmc", "0:1", addr_str, "boot/uVxWorks1",
		NULL
	};
	char *info_argv[3] =
	{
		/* iminfo ${loadaddr} */
		"iminfo", addr_str,
		NULL
	};

	if (env_get("loadaddr") == 0)
	{
		printf("DDR Test: Environment variable \'loadaddr\" is not set\n");
		return(1);
	}

	fatload_cmd = find_cmd("fatload");
	if (!fatload_cmd)
	{
		printf("DDR Test: 'fatload' command not present.\n");
		return(1);
	}

	iminfo_cmd = find_cmd("iminfo");
	if (!iminfo_cmd)
	{
		printf("DDR Test: 'iminfo' command not present.\n");
		return(1);
	}

	if (2 == argc)
	{
		if ('l' == argv[1][0])
		{
			loop_load = true;
		}
	}

	strcpy(addr_str, env_get("loadaddr"));
	if (0 != fatload_cmd->cmd(fatload_cmd, 0, 5, load_argv))
	{
		printf("DDR Test: Kernel image load failed.\n");
		return(1);
	}


#if DEBUG_CRC_ERRORS
	crc_err = 0;
	dup_crc_err = 0;
#endif
	while (1)
	{
		loop++;

		retcpu = uclass_get_device(UCLASS_THERMAL, 0, &thermal_dev);
		if (!retcpu) {
			retcpu = thermal_get_temp(thermal_dev, &cpu_tmp);

			if (retcpu)
				printf("CPU:   Temperature: invalid sensor data\n");
		}
		else
		{
			printf("CPU:   Temperature: Can't find sensor device\n");
		}

		printf("\n");
		printf("=================================================================\n");
#if DEBUG_CRC_ERRORS
		printf(">>> Loop: %d,  CRC Errors: %d,  Dup CRC Errors: %d, CPU Temp=%dC\n",
			   loop, crc_err, dup_crc_err, cpu_tmp);
#else
		printf(">>> Loop: %d\n", loop);
#endif
		if (0 != iminfo_cmd->cmd(iminfo_cmd, 0, 2, info_argv))
		{
			printf("DDR Test: Kernel image info failed.\n");
			ret = 1;
			break;
		}

		if (tstc())
		{
			if (getchar() == 'q')
			{
				printf("\n");
				printf("DDR Test: Quitting...\n");
				break;
			}
		}

		/* Optional: re-load kernel from SD card */
		if (loop_load)
		{
			if (0 != fatload_cmd->cmd(fatload_cmd, 0, 5, load_argv))
			{
				printf("DDR Test: Kernel image load failed.\n");
				ret = 1;
				break;
			}
		}
	}

	printf("\n");
#if DEBUG_CRC_ERRORS
	printf(">>> Loops: %d,  CRC Errors: %d,  Dup CRC Errors: %d\n",
		   loop, crc_err, dup_crc_err);
#else
	printf(">>> Loops: %d\n", loop);
#endif
	printf("\n");

	return(ret);

}

U_BOOT_CMD(
	ddr_test,	CONFIG_SYS_MAXARGS,	0,	do_ddr_test,
	"Loop: Load kernel from uSD card and verify",
	"No arguments."
);



static int image_info_test(ulong addr)
{
	int ret = 0;

	void *hdr = (void *)addr;

	printf("\n## Checking Image at %08lx ...\n", addr);

	switch (genimg_get_format(hdr)) {
#if defined(CONFIG_LEGACY_IMAGE_FORMAT)
	case IMAGE_FORMAT_LEGACY:
		puts("   Legacy image found\n");
		if (!image_check_magic(hdr)) {
			puts("   Bad Magic Number\n");
			return 1;
		}

		if (!image_check_hcrc(hdr)) {
			puts("   Bad Header Checksum\n");
			return 1;
		}

		image_print_contents(hdr);


		puts("   Verifying Kernel Image Checksum ... ");
		if (!image_check_dcrc(hdr))
		{
			printf("Bad Kernel Image CRC\n");
			ret = 1;
		}
		else
		{
			puts("OK\n");
		}


		return(ret);
#endif
	default:
		puts("Unknown image format!\n");
		break;
	}

	return 1;
}



#define BANK0_START       0x10000000
#define BANK0_END         0x3FFFFFFC
#define BANK0_RESERVED    0x40000000 /* uBoot lives here */
#define BANK1_START       0x80000000
#define BANK1_END         0xBFFFFFFC

/*
 * NOTE The region size must be >= 2MB
 */
#define REGION_SIZE       (128 * 1024 * 1024)
#define REGIONS_PER_BANK  (0x40000000 / REGION_SIZE)


/*
 * Data-Is-Address Test
 *
 * Fill each bank with the data-is-address and verify.
 * All of DRF is filled, except for the reserved region that uBoot
 * is executing out of.
 * The 'Regions' here are only used here to provide occassional
 * visual feedback of progress.
 */
static int do_ddr_da(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{

	struct udevice *thermal_dev;
	int i, r, cpu_tmp, retcpu;

	int iterations = 0;
	int passed = true;
	uint32_t *p;


	while (passed)
	{
		iterations++;

		retcpu = uclass_get_device(UCLASS_THERMAL, 0, &thermal_dev);
		if (!retcpu) {
			retcpu = thermal_get_temp(thermal_dev, &cpu_tmp);

			if (retcpu)
				printf("CPU:   Temperature: invalid sensor data\n");
		}
		else
		{
			printf("CPU:   Temperature: Can't find sensor device\n");
		}

		printf("\n");
		printf("=================================================================\n");
		printf(">>> Iteration %d, CPU Temperature %dC\n", iterations, cpu_tmp);

		printf("\n");
		printf("Writing Bank 0 1GB, Data is Addr, 0x10000000-3FFFFFFF\n");
		printf("Skip 0x40000000-4FFFFFFF: uBoot lives there!\n");
		p = (uint32_t *)BANK0_START;
		for (r = 0; (r < REGIONS_PER_BANK) && ((uint32_t)p < BANK0_RESERVED); r++)
		{
			/* Periodic output to show test is still running. */
			printf("\tWriting Data-is-Addr @ %08X\n", (uint32_t)p);

			i = 0;
			while (i < REGION_SIZE)
			{
				*p = (uint32_t)p;
				p++;
				i += sizeof(uint32_t);
			}
		}

		printf("Writing Bank 1 1GB, Data is Addr, 0x80000000-BFFFFFFF\n");
		p = (uint32_t *)BANK1_START;
		for (r = 0; r < REGIONS_PER_BANK; r++)
		{
			/* Periodic output to show test is still running. */
			printf("\tWriting Data-is-Addr @ %08X\n", (uint32_t)p);

			i = 0;
			while (i < REGION_SIZE)
			{
				*p = (uint32_t)p;
				p++;
				i += sizeof(uint32_t);
			}
		}

		printf("DONE! DDR written with data-is-address.\n");


		printf("\n");
		printf("----------\n");
		printf("\n");


		printf("Verifying Bank 0 1GB, Data is Addr, 0x10000000-0x3FFFFFFF\n");
		p = (uint32_t *)BANK0_START;
		for (r = 0; (r < REGIONS_PER_BANK) && ((uint32_t)p < BANK0_RESERVED); r++)
		{
			/* Periodic output to show test is still running. */
			printf("\tVerifying Data-is-Addr @ %08X\n", (uint32_t)p);

			i = 0;
			while (i < REGION_SIZE)
			{
				if (*p != (uint32_t)p)
				{
					printf("Bank0 Data-is-Address MISMATCH: Expect=0x%08X, Actual=0x%08X\n",
						   (uint32_t)p, *p);
					passed = false;
				}
				p++;
				i += sizeof(uint32_t);
			}
		}

		printf("Verifying Bank 1 1GB, Data is Addr, 0x80000000-0xBFFFFFFF\n");
		p = (uint32_t *)BANK1_START;
		for (r = 0; r < REGIONS_PER_BANK; r++)
		{
			/* Periodic output to show test is still running. */
			printf("\tVerifying Data-is-Addr @ %08X\n", (uint32_t)p);

			i = 0;
			while (i < REGION_SIZE)
			{
				if (*p != (uint32_t)p)
				{
					printf("Bank0 Data-is-Address MISMATCH: Expect=0x%08X, Actual=0x%08X\n",
						   (uint32_t)p, *p);
					passed = false;
				}
				p++;
				i += sizeof(uint32_t);
			}
		}


		printf("\n");
		printf(">>> Iteration %d: Data-is-Address Test %s!\n",
			   iterations, passed ? "PASSED" : "FAILED");
		printf("\n");


		if (tstc())
		{
			if (getchar() == 'q')
			{
				printf("Quitting...\n");
				printf("\n");
				break;
			}
		}
	}


	return(0);

}

U_BOOT_CMD(
	ddr_da,	CONFIG_SYS_MAXARGS,	0,	do_ddr_da,
	"Fill and verify memory with data-is-address",
	"<no args>"
);



/*
 * CRC/Compare Test
 *
 * The original version of this test was used in March-May 2020 to screen
 * Val2 SOMs that were encountering intermittent CRC failures after loading
 * the VxWorks kernel from uSD card.
 *
 * Back then the test would load the kernel image twice, to 2 different
 * locations in Bank 0. Both kernels were CRC'd, and also the images were
 * compared.
 * Sometimes 1 or both or none of the CRCs failed.
 * Sometimes the compare would report 1 or more mismatches.
 * Sometimes both and sometimes none.
 * Eventually those failures were resolved once EB817 was applied to the
 * Valenka2 LPDDR2 DCD.
 *
 * This "CC" test is basically the orignal test expanded. Many more copies
 * are made and more of memory is used, including Bank1.
 * The REGION_SIZE determines how many copies are made and therefore how
 * many CRCs and compares are performed. The smaller the region the more
 * copies and CRCs and compares.
 *
 * This test was used during the "IS8 Stability" effort when some
 * systems encountered exceptions that led to the Valenka2 LPDDR2 DDR
 * configuration being updated, and a proper DQS/delay-lines
 * calibration performed.
 *
 */
static int do_ddr_cc(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{

	struct udevice *thermal_dev;
	int cpu_tmp, retcpu;

	ulong copy_addr;

	int r;
	int do_cmp = true;
	int do_crc = true;
	int passed = true;
	int ret = 0;
	int iterations = 0;
	struct cmd_tbl *fatload_cmd;
	char addr_str[16];
	char *load_argv[6] =
	{
		/* fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${imagepath}/${image_name} */
		"fatload", "mmc", "0:1", addr_str, "boot/uVxWorks1",
		NULL
	};


	if (2 == argc)
	{
		if (0 == strcmp("crc", argv[1]))
		{
			do_cmp = false;
		}
		else if (0 == strcmp("cmp", argv[1]))
		{
			do_crc = false;
		}
		else
		{
			printf("Unknown argument '%s' ignored.\n", argv[1]);
		}
	}


	fatload_cmd = find_cmd("fatload");
	if (!fatload_cmd)
	{
		printf("ERROR: 'fatload' command not present.\n");
		return(1);
	}


	/*
	 * Load the Vxworks kernel into the defined number of regions
	 * per bank. The load is only done once.
	 *
	 * Bank 0 0x10000000 - 0x4FFFFFFF
	 * Skip 40000000: uBoot lives there!
	 */
	copy_addr = BANK0_START;
	for (r = 0; (r < REGIONS_PER_BANK) && (copy_addr < BANK0_RESERVED); r++)
	{
		sprintf(addr_str, "%lx", copy_addr);
		ret = fatload_cmd->cmd(fatload_cmd, 0, 5, load_argv);
		printf("Kernel image load @ 0x%08lX %s!\n",
			   copy_addr, ret == 0 ? "SUCCESS" : "ERROR");
		printf("\n");
		if (ret)
		{
			passed = false;
		}

		copy_addr += (REGION_SIZE);
	}

	/* Bank 1 0x80000000 - 0xBFFFFFFF */
	copy_addr = BANK1_START;
	for (r = 0; r < REGIONS_PER_BANK; r++)
	{
		sprintf(addr_str, "%lx", copy_addr);
		ret = fatload_cmd->cmd(fatload_cmd, 0, 5, load_argv);
		printf("Kernel image load @ 0x%08lX %s!\n",
			   copy_addr, ret == 0 ? "SUCCESS" : "ERROR");
		printf("\n");
		if (ret)
		{
			passed = false;
		}

		copy_addr += (REGION_SIZE);
	}


	while (true == passed)
	{
		iterations++;

		retcpu = uclass_get_device(UCLASS_THERMAL, 0, &thermal_dev);
		if (!retcpu) {
			retcpu = thermal_get_temp(thermal_dev, &cpu_tmp);

			if (retcpu)
				printf("CPU:   Temperature: invalid sensor data\n");
		}
		else
		{
			printf("CPU:   Temperature: Can't find sensor device\n");
		}

		printf("\n");
		printf("=================================================================\n");
		printf(">>> Iteration %d, CPU Temperature %dC\n", iterations, cpu_tmp);


		/*
		 * Bank 0 0x10000000 - 0x4FFFFFFF
		 * Skip 40000000: uBoot lives there!
		 */
		copy_addr = BANK0_START;
		for (r = 0; (r < REGIONS_PER_BANK) && (copy_addr < BANK0_RESERVED); r++)
		{
			if (true == do_crc)
			{
				ret = image_info_test(copy_addr);
				printf("Kernel image CRC Test @ 0x%08lX: %s!\n",
					   copy_addr, ret == 0 ? "PASSED" : "FAILED");
				if (ret)
				{
					passed = false;
				}
			}
			printf("\n");


			if (true == do_cmp)
			{
				/* First image is what all other regions compare to. */
				if (r > 0)
				{
					/* Compare each to first image at 0x10000000 */
					printf("   Compare Kernel Images @ 0x%08lX and 0x%08X ... \n",
						   copy_addr, BANK0_START);
					{
						int i;
						ulong *ptr = (ulong *)copy_addr;
						ulong *ptr2 = (ulong *)BANK0_START;
						image_header_t *hdr = (image_header_t *)ptr2;

						for (i = 0; i < image_get_data_size(hdr) / sizeof(ulong); i++)
						{
							if (ptr[i] != ptr2[i])
							{
								printf("*** MISMATCH @ offset [0x%08X]: 0x%08lX != 0x%08lX\n",
									   i * sizeof(ulong), ptr[i], ptr2[i]);
								passed = false;
							}
						}
					}
					puts("   Compare Kernel Images DONE \n");
				}
				else
				{
					/* Compare first copy to second copy. */
					printf("   Compare Kernel Images @ 0x%08lX and 0x%08X ... \n",
						   copy_addr, BANK0_START + REGION_SIZE);
					{
						int i;
						ulong *ptr = (ulong *)copy_addr;
						ulong *ptr2 = (ulong *)BANK0_START;
						image_header_t *hdr = (image_header_t *)ptr2;

						for (i = 0; i < image_get_data_size(hdr) / sizeof(ulong); i++)
						{
							if (ptr[i] != ptr2[i])
							{
								printf("*** MISMATCH @ offset [0x%08X]: 0x%08lX != 0x%08lX\n",
									   i * sizeof(ulong), ptr[i], ptr2[i]);
								passed = false;
							}
						}
					}
					puts("   Compare Kernel Images DONE \n");
				}
			}

			copy_addr += (REGION_SIZE);
		}


		/*
		 * Bank 1 0x80000000 - 0xBFFFFFFF
		 */
		copy_addr = BANK1_START;
		for (r = 0; r < REGIONS_PER_BANK; r++)
		{
			if (true == do_crc)
			{
				ret = image_info_test(copy_addr);
				printf("Kernel image CRC Test @ 0x%08lX: %s!\n",
					   copy_addr, ret == 0 ? "PASSED" : "FAILED");
				if (ret)
				{
					passed = false;
				}
			}
			printf("\n");


			if (true == do_cmp)
			{
				/* Compare to first image at 0x10000000 */
				printf("   Compare Kernel Images @ 0x%08lX and 0x%08X ... \n",
					   copy_addr, BANK0_START);
				{
					int i;
					ulong *ptr = (ulong *)copy_addr;
					ulong *ptr2 = (ulong *)BANK0_START;
					image_header_t *hdr = (image_header_t *)ptr2;

					for (i = 0; i < image_get_data_size(hdr) / sizeof(ulong); i++)
					{
						if (ptr[i] != ptr2[i])
						{
							printf("*** MISMATCH @ offset [0x%08X]: 0x%08lX != 0x%08lX\n",
								   i * sizeof(ulong), ptr[i], ptr2[i]);
							passed = false;
						}
					}
				}
				puts("   Compare Kernel Images DONE \n");
			}

			copy_addr += (REGION_SIZE);
		}


		printf("\n");
		printf(">>> Iteration %d: %s!\n", iterations, passed ? "PASSED" : "FAILED");
		printf("\n");

		if (tstc())
		{
			if (getchar() == 'q')
			{
				printf("Quitting...\n");
				printf("\n");
				break;
			}
		}
	}

	return(0);

}


U_BOOT_CMD(
	ddr_cc,	CONFIG_SYS_MAXARGS,	0,	do_ddr_cc,
	"Load Vxworks kernel every 256MB, CRC and/or Compare them ALL",
	"[crc | cmp]"
);
