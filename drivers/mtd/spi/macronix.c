/*
 * Copyright 2009(C) Marvell International Ltd. and its affiliates
 * Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Based on drivers/mtd/spi/stmicro.c
 *
 * Copyright 2008, Network Appliance Inc.
 * Jason McMullan <mcmullan@netapp.com>
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*#define DEBUG*/
#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

struct macronix_spi_flash_params {
	u16 idcode;
	u16 nr_blocks;
	const char *name;
};

static const struct macronix_spi_flash_params macronix_spi_flash_table[] = {
	{
		.idcode = 0x2013,
		.nr_blocks = 8,
		.name = "MX25L4005",
	},
	{
		.idcode = 0x2014,
		.nr_blocks = 16,
		.name = "MX25L8005",
	},
	{
		.idcode = 0x2015,
		.nr_blocks = 32,
		.name = "MX25L1605D",
	},
	{
		.idcode = 0x2016,
		.nr_blocks = 64,
		.name = "MX25L3205D",
	},
	{
		.idcode = 0x2017,
		.nr_blocks = 128,
		.name = "MX25L6405D",
	},
	{
		.idcode = 0x2018,
		.nr_blocks = 256,
		.name = "MX25L12805D",
	},
	{
		.idcode = 0x2019,
		.nr_blocks = 512,
		.name = "MX25L25635F",
	},
	{
		.idcode = 0x2618,
		.nr_blocks = 256,
		.name = "MX25L12855E",
	},
};

static inline int spi_flash_cmd_reset_macronix(struct spi_flash *flash)
{
	int result;
	if ((result = spi_flash_cmd(flash->spi, 0x66, NULL, 0)) == 0 ) {
		result = spi_flash_cmd(flash->spi, 0x99, NULL, 0);
	}
	return result;
}


struct spi_flash *spi_flash_probe_macronix(struct spi_slave *spi, u8 *idcode)
{
	const struct macronix_spi_flash_params *params;
	struct spi_flash *flash;
	unsigned int i;
	u16 id = idcode[2] | idcode[1] << 8;

	for (i = 0; i < ARRAY_SIZE(macronix_spi_flash_table); i++) {
		params = &macronix_spi_flash_table[i];
		if (params->idcode == id)
			break;
	}

	if (i == ARRAY_SIZE(macronix_spi_flash_table)) {
		debug("SF: Unsupported Macronix ID %04x\n", id);
		return NULL;
	}

	flash = malloc(sizeof(*flash));
	if (!flash) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	flash->spi = spi;
	flash->name = params->name;

	flash->write = spi_flash_cmd_write_multi;
	flash->erase = spi_flash_cmd_erase;
	flash->read = spi_flash_cmd_read_fast;
	flash->reset= spi_flash_cmd_reset_macronix;
	flash->page_size = 256;
	flash->sector_size = 256 * 16 * 16;
	flash->size = flash->sector_size * params->nr_blocks;
	flash->poll_read_status = NULL;
	/* Clear BP# bits for read-only flash */
	spi_flash_cmd_write_status(flash, 0);

	if (flash->size > 0x1000000) {
		int result;
		u8  cmd[20];
		u8  sr[20];

		memset(sr,0,20);

#ifdef DEBUG
		cmd[0] = 0x9F; /* CMD_RDID */
		result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 3);
		debug("SF: ID: %02x %02x %02x result %d\n", sr[0], sr[1], sr[2], result);

		cmd[0] = CMD_READ_STATUS;
		result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 1);
		debug("SF: STATUS: %02x result %d\n", sr[0], result);

		cmd[0] = 0x15; /* CMD_READ_CONFIG */
		result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 1);
		debug("SF: CONFIG: %02x result %d\n", sr[0], result);
#endif
		spi_flash_cmd_write_enable(flash);
		cmd[0] = 0xB7; /* EN4B enable 4Byte mode */
		result = spi_flash_cmd_write(spi, &cmd, 1, NULL, 0);

		spi_flash_cmd_write_enable(flash);
		cmd[0] = CMD_WRITE_STATUS;
		sr[0]  =  0x40; /* SR1 */
		sr[1]  =  0x27; /* CR1 */
		result = spi_flash_cmd_write(spi, &cmd, 1, &sr, 2);

#ifdef DEBUG
		cmd[0] = CMD_READ_STATUS;
		result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 1);
		debug("SF: STATUS: %02x result %d\n", sr[0], result);

		cmd[0] = 0x15; /* CMD_READ_CONFIG */
		result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 1);
		debug("SF: CONFIG: %02x result %d\n", sr[0], result);
#endif

	}


	return flash;
}
