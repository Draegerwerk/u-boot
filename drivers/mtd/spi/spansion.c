/*
 * Copyright (C) 2009 Freescale Semiconductor, Inc.
 *
 * Author: Mingkai Hu (Mingkai.hu@freescale.com)
 * Based on stmicro.c by Wolfgang Denk (wd@denx.de),
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com),
 * and  Jason McMullan (mcmullan@netapp.com)
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

#define EXT_ADDR_WIDTH          4
#define EXT_CMD_READ_ARRAY   0x6C
#define EXT_CMD_ERASE_4K     0x21
#define EXT_CMD_ERASE_64K    0xDC
#define EXT_CMD_PAGE_PROGRAM 0x12

struct spansion_spi_flash_params {
    u16 idcode1;
    u16 idcode2;
    u16 pages_per_sector;
    u16 nr_sectors;
    const char *name;
};

static const struct spansion_spi_flash_params spansion_spi_flash_table[] = {
    {
        .idcode1 = 0x0213,
        .idcode2 = 0,
        .pages_per_sector = 256,
        .nr_sectors = 16,
        .name = "S25FL008A",
    },
    {
        .idcode1 = 0x0214,
        .idcode2 = 0,
        .pages_per_sector = 256,
        .nr_sectors = 32,
        .name = "S25FL016A",
    },
    {
        .idcode1 = 0x0215,
        .idcode2 = 0,
        .pages_per_sector = 256,
        .nr_sectors = 64,
        .name = "S25FL032A",
    },
    {
        .idcode1 = 0x0216,
        .idcode2 = 0,
        .pages_per_sector = 256,
        .nr_sectors = 128,
        .name = "S25FL064A",
    },
    {
        .idcode1 = 0x2018,
        .idcode2 = 0x0301,
        .pages_per_sector = 256,
        .nr_sectors = 256,
        .name = "S25FL128P_64K",
    },
    {
        .idcode1 = 0x2018,
        .idcode2 = 0x0300,
        .pages_per_sector = 1024,
        .nr_sectors = 64,
        .name = "S25FL128P_256K",
    },
    {
        .idcode1 = 0x0215,
        .idcode2 = 0x4d00,
        .pages_per_sector = 256,
        .nr_sectors = 64,
        .name = "S25FL032P",
    },
    {
        .idcode1 = 0x2018,
        .idcode2 = 0x4d01,
        .pages_per_sector = 256,
        .nr_sectors = 256,
        .name = "S25FL129P_64K",
    },
    {
        .idcode1 = 0x2019,
        .idcode2 = 0x4d01,
        .pages_per_sector = 256,
        .nr_sectors = 512,
        .name = "S25FL256S",
    },
    {
        .idcode1 = 0x0219,
        .idcode2 = 0x4d00,
        .pages_per_sector = 1024,
        .nr_sectors = 128,
        .name = "S25FL256S_256K",
    },
    {
        .idcode1 = 0x0219,
        .idcode2 = 0x4d01,
        .pages_per_sector = 256,
        .nr_sectors = 512,
        .name = "S25FL256S_64K",
    },
};

static inline int spi_flash_cmd_reset_spansion(struct spi_flash *flash)
{
    int result;
    if ((result = spi_flash_cmd(flash->spi, 0xFF, NULL, 0)) == 0 ) {
        result = spi_flash_cmd(flash->spi, 0xF0, NULL, 0);
    }
    return result;
}

int spi_flash_cmd_write_multi_spansion(struct spi_flash *flash, u32 offset,
        size_t len, const void *buf)
{
    unsigned long page_addr, byte_addr, page_size;
    size_t chunk_len, actual;
    int ret;
    u8 cmd[5];

    page_size = flash->page_size;
    page_addr = offset / page_size;
    byte_addr = offset % page_size;

    ret = spi_claim_bus(flash->spi);
    if (ret) {
        debug("SF: unable to claim SPI bus\n");
        return ret;
    }

    cmd[0] = EXT_CMD_PAGE_PROGRAM;
    for (actual = 0; actual < len; actual += chunk_len) {
        chunk_len = min(len - actual, page_size - byte_addr);

        cmd[1] = page_addr >> 16;
        cmd[2] = page_addr >> 8;
        cmd[3] = page_addr;
        cmd[4] = byte_addr;
        debug("PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x%02x } "
                "chunk_len = %zu\n", buf + actual, cmd[0],
                cmd[1], cmd[2], cmd[3], cmd[4], chunk_len);

        ret = spi_flash_cmd_write_enable(flash);
        if (ret < 0) {
            debug("SF: enabling write failed\n");
            break;
        }

        ret = spi_flash_cmd_write(flash->spi, cmd, EXT_ADDR_WIDTH + 1, buf + actual, chunk_len);
        if (ret < 0) {
            debug("SF: write failed\n");
            break;
        }

        ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
        if (ret)
            break;

        if (flash->poll_read_status) {
            ret = flash->poll_read_status(flash);
            if (ret)
                break;
        }

        page_addr++;
        byte_addr = 0;
    }

    debug("SF: program %s %zu bytes @ %#x\n",
          ret ? "failure" : "success", len, offset);

    spi_release_bus(flash->spi);
    return ret;
}

int spi_flash_cmd_erase_spansion(struct spi_flash *flash, u32 offset, size_t len)
{
    u32 start, end, erase_size;
    int ret;
    u8 cmd[5];

    erase_size = flash->sector_size;
    if (offset % erase_size || len % erase_size) {
        debug("SF: Erase offset/length not multiple of erase size\n");
        return -1;
    }

    ret = spi_claim_bus(flash->spi);
    if (ret) {
        debug("SF: Unable to claim SPI bus\n");
        return ret;
    }

    if (erase_size == 4096)
        cmd[0] = EXT_CMD_ERASE_4K;
    else
        cmd[0] = EXT_CMD_ERASE_64K;
    start = offset;
    end = start + len;

    cmd[5] = 0;

    while (offset < end) {

        cmd[1] = (u8) (offset >> 24);
        cmd[2] = (u8) (offset >> 16);
        cmd[3] = (u8) (offset >>  8);
        cmd[4] = (u8) (offset >>  0);
        offset += erase_size;

        debug("SF: erase %2x %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
              cmd[2], cmd[3], cmd[4], offset);

        ret = spi_flash_cmd_write_enable(flash);
        if (ret)
            goto out;

        /* address byte for extended is 4 */
        ret = spi_flash_cmd_write(flash->spi, cmd, EXT_ADDR_WIDTH + 1, NULL, 0);
        if (ret)
            goto out;

        ret = spi_flash_cmd_wait_ready(flash,
            SPI_FLASH_PAGE_ERASE_TIMEOUT);
        if (ret)
            goto out;

        if (flash->poll_read_status) {
            ret = flash->poll_read_status(flash);
            if (ret)
                goto out;
        }
    }

    debug("SF: Successfully erased %zu bytes @ %#x\n", len, start);

 out:
    spi_release_bus(flash->spi);
    return ret;
}

int spi_flash_cmd_read_fast_spansion(struct spi_flash *flash, u32 offset,
        size_t len, void *data)
{
    u8 cmd[6];

    cmd[0] = EXT_CMD_READ_ARRAY;
    cmd[1] = (u8)(offset >> 24);
    cmd[2] = (u8)(offset >> 16);
    cmd[3] = (u8)(offset >>  8);
    cmd[4] = (u8)(offset >>  0);
    cmd[5] = 0;

    return spi_flash_read_common(flash, cmd, EXT_ADDR_WIDTH + 2, data, len);
}


struct spi_flash *spi_flash_probe_spansion(struct spi_slave *spi, u8 *idcode)
{
    const struct spansion_spi_flash_params *params;
    struct spi_flash *flash;
    unsigned int i;
    unsigned short jedec, ext_jedec;

    jedec = idcode[1] << 8 | idcode[2];
    ext_jedec = idcode[3] << 8 | idcode[4];

    for (i = 0; i < ARRAY_SIZE(spansion_spi_flash_table); i++) {
        params = &spansion_spi_flash_table[i];
        if (params->idcode1 == jedec) {
            if (params->idcode2 == ext_jedec)
                break;
        }
    }

    if (i == ARRAY_SIZE(spansion_spi_flash_table)) {
        debug("SF: Unsupported SPANSION ID %04x %04x\n", jedec, ext_jedec);
        return NULL;
    }

    flash = malloc(sizeof(*flash));
    if (!flash) {
        debug("SF: Failed to allocate memory\n");
        return NULL;
    }

    flash->spi = spi;
    flash->name = params->name;

    flash->reset = spi_flash_cmd_reset_spansion;
    flash->page_size = 256;
    flash->sector_size = 256 * params->pages_per_sector;
    flash->size = flash->sector_size * params->nr_sectors;
    flash->poll_read_status = NULL;

    if (flash->size > 0x1000000) {
        int result;
        u8  cmd[20];
        u8  sr[20];

        memset(sr,0,20);

        flash->write = spi_flash_cmd_write_multi_spansion;
        flash->erase = spi_flash_cmd_erase_spansion;
        flash->read  = spi_flash_cmd_read_fast_spansion;

        spi_flash_cmd_write_enable(flash);

        cmd[0] = CMD_WRITE_STATUS;
        sr[0]  = 0x00; /* SR1 */
        sr[1]  = 0x02; /* CR1:1 - Quad */
        /*  sr[1]  |= 0xC0; CR1:6-7 - Latency Code <= 50 Mhz */
        result = spi_flash_cmd_write(spi, &cmd, 1, &sr, 2);

//        spi_flash_cmd_write_enable(flash);
//
//        cmd[0] = 0x17; /* CMD_BANK_REGISTER_WRITE */
//        sr [0] = 0x80; /* BAR:7 Extadd */
//        result = spi_flash_cmd_write(spi, &cmd, 1, &sr, 1);


#ifdef DEBUG
        cmd[0] = 0x9F; /* CMD_RDID */
        memset(sr,0,20);
        result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 3);
        debug("SF: ID: %02x %02x %02x result %d\n", sr[0], sr[1], sr[2], result);

        cmd[0] = 0x05; /* CMD_READ_STATUS */;
        memset(sr,0,20);
        result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 1);
        debug("SF: STATUS: %02x result %d\n", sr[0], result);

        cmd[0] = 0x35; /* CMD_READ_CONFIG */
        memset(sr,0,20);
        result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 1);
        debug("SF: CONFIG: %02x result %d\n", sr[0], result);

        cmd[0] = 0x16; /* CMD_BANK_REGISTER_READ */
        memset(sr,0,20);
        result = spi_flash_cmd_read(spi, &cmd, 1, &sr, 1);
        debug("SF: BRRD: %02x result %d\n", sr[0], result);
#endif

    } else {
        flash->write = spi_flash_cmd_write_multi;
        flash->erase = spi_flash_cmd_erase;
        flash->read = spi_flash_cmd_read_fast;
    }

    return flash;
}
