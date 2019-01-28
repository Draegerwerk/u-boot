/*
 *  Copyright Altera Corporation (C) 2013. All rights reserved
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
#include <asm/io.h>
#include <asm/arch/system_manager.h>
#include <asm/arch/reset_manager.h>
#include <asm/gpio.h>
#ifndef CONFIG_SPL_BUILD
#include <phy.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include "../../../drivers/net/designware.h"
#endif
#include <i2c.h>
#include <spi_flash.h>
#include <version.h>
#include <draeger_m48_pmstruct.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPL_BUILD
void spl_board_early_init(void)
{
    if (gpio_get_value(CONFIG_GPIO_DEBUG)) gd->flags |= (GD_FLG_SILENT | GD_FLG_DISABLE_CONSOLE);
}
#endif

/*
 * Initialization function which happen at early stage of c code
 */
int board_early_init_f(void)
{

#ifdef CONFIG_HW_WATCHDOG
    /* disable the watchdog when entering U-Boot */
    watchdog_disable();
#endif
    /* calculate the clock frequencies required for drivers */
    cm_derive_clocks_for_drivers();

    if (gpio_get_value(CONFIG_GPIO_DEBUG)) gd->flags |= (GD_FLG_SILENT | GD_FLG_DISABLE_CONSOLE);

    return 0;
}

/*
 * Miscellaneous platform dependent initialisations
 */
int board_init(void)
{
    /* adress of boot parameters for ATAG (if ATAG is used) */
    gd->bd->bi_boot_params = 0x00000100;


    /*
     * reinitialize the global variable for clock value as after
     * relocation, the global variable are cleared to zeroes
     */
    cm_derive_clocks_for_drivers();

    if (gd->flags & GD_FLG_DISABLE_CONSOLE) m48PmData->verboseBoot = 0;
    else                                    m48PmData->verboseBoot = 1;

    return 0;
}

static void setenv_ethaddr_eeprom(void)
{
    uint addr, alen;
    int linebytes;
    uchar chip, enetaddr[6], temp;

    /* configuration based on dev kit EEPROM */
    chip = 0x51;		/* slave ID for EEPROM */
    alen = 2;		/* dev kit using 2 byte addressing */
    linebytes = 6;		/* emac address stored in 6 bytes address */

#if (CONFIG_EMAC_BASE == CONFIG_EMAC0_BASE)
    addr = 0x16c;
#elif (CONFIG_EMAC_BASE == CONFIG_EMAC1_BASE)
    addr = 0x174;
#endif

    i2c_read(chip, addr, alen, enetaddr, linebytes);

    /* swapping endian to match board implementation */
    temp = enetaddr[0];
    enetaddr[0] = enetaddr[5];
    enetaddr[5] = temp;
    temp = enetaddr[1];
    enetaddr[1] = enetaddr[4];
    enetaddr[4] = temp;
    temp = enetaddr[2];
    enetaddr[2] = enetaddr[3];
    enetaddr[3] = temp;

    if (is_valid_ether_addr(enetaddr))
        eth_setenv_enetaddr("ethaddr", enetaddr);
    else
        puts("Skipped ethaddr assignment due to invalid "
            "EMAC address in EEPROM\n");
}

#ifdef CONFIG_BOARD_LATE_INIT
int mac_read_from_qspi(void);
int board_late_init(void)
{
    mac_read_from_qspi();
    return 0;
}
#endif

/* EMAC related setup and only supported in U-Boot */
#if !defined(CONFIG_SOCFPGA_VIRTUAL_TARGET) && \
!defined(CONFIG_SPL_BUILD)

int board_phy_config(struct phy_device *phydev)
{
	if((!phydev) || (!phydev->drv)) return -1;
	
    /* devadd=0x02 and mode=MII_KSZ9031_MOD_DATA_NO_POST_INC are explained in KSZ9031 manual */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_CTRL_SIG_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x0);
    /* min rx data delay */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_RX_DATA_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x0);
    /* min tx data delay */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_TX_DATA_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x0);
    /* max rx/tx clock delay, min rx/tx control */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_CLOCK_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x01FF);

    if (phydev->drv->config)
        phydev->drv->config(phydev);

    return 0;
}

#endif

/* We know all the init functions have been run now */
int board_eth_init(bd_t *bis)
{
#if !defined(CONFIG_SOCFPGA_VIRTUAL_TARGET) && \
!defined(CONFIG_SPL_BUILD)

    /* Initialize EMAC */

    /*
     * Putting the EMAC controller to reset when configuring the PHY
     * interface select at System Manager
    */
    emac0_reset_enable(1);
    emac1_reset_enable(1);

    /* Clearing emac0 PHY interface select to 0 */
    clrbits_le32(CONFIG_SYSMGR_EMAC_CTRL,
        (SYSMGR_EMACGRP_CTRL_PHYSEL_MASK <<
#if (CONFIG_EMAC_BASE == CONFIG_EMAC0_BASE)
        SYSMGR_EMACGRP_CTRL_PHYSEL0_LSB));
#elif (CONFIG_EMAC_BASE == CONFIG_EMAC1_BASE)
        SYSMGR_EMACGRP_CTRL_PHYSEL1_LSB));
#endif

    /* configure to PHY interface select choosed */
    setbits_le32(CONFIG_SYSMGR_EMAC_CTRL,
#if (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_GMII)
        (SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_GMII_MII <<
#elif (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_MII)
        (SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_GMII_MII <<
#elif (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_RGMII)
        (SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RGMII <<
#elif (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_RMII)
        (SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RMII <<
#endif
#if (CONFIG_EMAC_BASE == CONFIG_EMAC0_BASE)
        SYSMGR_EMACGRP_CTRL_PHYSEL0_LSB));
    /* Release the EMAC controller from reset */
    emac0_reset_enable(0);
#elif (CONFIG_EMAC_BASE == CONFIG_EMAC1_BASE)
        SYSMGR_EMACGRP_CTRL_PHYSEL1_LSB));
    /* Release the EMAC controller from reset */
    emac1_reset_enable(0);
#endif

    /* initialize and register the emac */
    int rval = designware_initialize(CONFIG_EMAC_BASE,
#if (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_GMII)
        PHY_INTERFACE_MODE_GMII);
#elif (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_MII)
        PHY_INTERFACE_MODE_MII);
#elif (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_RGMII)
        PHY_INTERFACE_MODE_RGMII);
#elif (CONFIG_PHY_INTERFACE_MODE == SOCFPGA_PHYSEL_ENUM_RMII)
        PHY_INTERFACE_MODE_RMII);
#endif
    debug("board_eth_init %d\n", rval);
    return rval;
#else
    return 0;
#endif
}

#define CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET 0x120000
#define CONFIG_SYS_QSPI_BOARD_REV_OFFSET 28

int mac_read_from_qspi(void)
{
    uchar buf[28];
    char str[18];
    unsigned int crc = 0;
    struct spi_flash *flash;
    uint32_t crc_buf;

    /* initialize the Quad SPI controller */
    flash = spi_flash_probe(0, 0,
        CONFIG_SF_DEFAULT_SPEED, SPI_MODE_3);
    if (!flash)
    {
        printf("SPI probe failed.\n");
        return 0;
    }

    /* Read MAC addresses from EEPROM */
    spi_flash_read(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET,
        28, buf);

    memcpy(&crc_buf, &buf[24], sizeof(uint32_t));

    if (crc32(crc, buf, 24) == crc_buf)
    {
        printf("Reading MAC from EEPROM\n");
        if (memcmp(buf, "\0\0\0\0\0\0", 6))
        {
            sprintf(str,
                "%02X:%02X:%02X:%02X:%02X:%02X",
                buf[0], buf[1],
                buf[2], buf[3],
                buf[4], buf[5]);
            setenv("ethaddr", str);
        }
    }
    else
    {
        printf("QSPI MAC CRC failed\n");
    }

    spi_flash_reset(flash);

    return 0;
}

static int do_setmacaddr (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int offset = 0;
    uchar *buf;
    unsigned char str[18];
    struct spi_flash *flash;
    uint32_t crc_buf;

    if (argc != 3)
        return -1;

    if ((offset = simple_strtoul(argv[1], NULL, 10)) > 3)
    {
        printf ("MAC offset %d too high (valid values 0 -3)\n", offset);
        return 0;
    }

    eth_parse_enetaddr (argv[2], str);

    /* initialize the Quad SPI controller */
    flash = spi_flash_probe(0, 0,
        CONFIG_SF_DEFAULT_SPEED, SPI_MODE_3);
    if (!flash)
    {
        printf("SPI probe failed.\n");
        return 0;
    }

    buf = (uchar *)malloc (flash->sector_size);
    if (NULL == buf)
    {
        printf ("setmacaddr: Could not allocate read buffer\n");
        return 0;
    }
    /* Read MAC addresses from QSPI Flash */
    spi_flash_read(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET,
        flash->sector_size, buf);

    memcpy(&crc_buf, &buf[24], sizeof(uint32_t));
    if (crc32(0, buf, 24) != crc_buf)
    {
        printf ("The QSPI Flash MAC area is corrupted...erasing\n");
        memset (buf, 0, 28);
    }

    memcpy ((void *)(buf + 6 * offset), (void *)str, 6);
    crc_buf = crc32(0, buf, 24);
    memcpy ((void *)(buf + 24), (void *)(&crc_buf), sizeof(uint32_t));

    /* Update MAC addresses from QSPI Flash */
    spi_flash_erase(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET,
        flash->sector_size);
    spi_flash_write(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET,
        flash->sector_size, buf);
    printf("MAC %s was written at offset %d\n", argv[2], offset);
    free (buf);
    spi_flash_reset(flash);
    return 0;
}

U_BOOT_CMD(
    setmacaddr, 3,  1,  do_setmacaddr,
    "Set one of the 4 MAC addresses in QSPI Flash.",
    "Set one of the 4 MAC addresses in QSPI Flash. \n"
    "Only 4 MAC adresses can be written in QSPI Flash \n"
    "The one at offset 0 will be used as the interface MAC \n"
    "setmacaddr offset value"
);

static int boardrev (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    uint16_t ver;
    struct spi_flash *flash;

    /* initialize the Quad SPI controller */
    flash = spi_flash_probe(0, 0,
        CONFIG_SF_DEFAULT_SPEED, SPI_MODE_3);
    if (!flash)
    {
        printf("SPI probe failed.\n");
        return 0;
    }

    /* Read MAC addresses from QSPI Flash */
    spi_flash_read(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET + CONFIG_SYS_QSPI_BOARD_REV_OFFSET,
        2, &ver);

    printf ("Board revision %d.%d\n", *((char *)(&ver)), *((char *)(&ver) + 1));
    spi_flash_reset(flash);
    return 0;
}

static int setboardrev (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    uchar * buf;;
    struct spi_flash *flash;

    if (argc != 3)
        return -1;

    if (simple_strtoul(argv[1], NULL, 10) > 0xff || simple_strtoul(argv[2], NULL, 10) > 0xff)
    {
        printf ("Major or Minor numbers must be lower than 255\n");
        return 0;
    }

    /* initialize the Quad SPI controller */
    flash = spi_flash_probe(0, 0,
        CONFIG_SF_DEFAULT_SPEED, SPI_MODE_3);
    if (!flash)
    {
        printf("SPI probe failed.\n");
        return 0;
    }

    buf = (uchar *)malloc (flash->sector_size);
    if (NULL == buf)
    {
        printf ("setmacaddr: Could not allocate read buffer\n");
        return 0;
    }

    /* Read whole QSPI Flash sector */
    spi_flash_read(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET,
        flash->sector_size, buf);

    buf[CONFIG_SYS_QSPI_BOARD_REV_OFFSET] = (unsigned char)simple_strtoul(argv[1], NULL, 10);
    buf[CONFIG_SYS_QSPI_BOARD_REV_OFFSET + 1] = (unsigned char)simple_strtoul(argv[2], NULL, 10);

    /* Update board revision in QSPI Flash */
    spi_flash_erase(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET,
        flash->sector_size);
    spi_flash_write(flash, CONFIG_SYS_QSPI_CUSTOM_AREA_OFFSET,
        flash->sector_size, buf);

    spi_flash_reset(flash);

    printf("Board revision %d.%d was written in QSPI at offset %d\n",
            buf[CONFIG_SYS_QSPI_BOARD_REV_OFFSET],
            buf[CONFIG_SYS_QSPI_BOARD_REV_OFFSET + 1],
            CONFIG_SYS_QSPI_BOARD_REV_OFFSET);
    free (buf);

    return 0;
}

static int do_halt (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    disable_interrupts();
    asm("   wfi");
    return 0;
}

U_BOOT_CMD(
    boardrev,   1,  1,  boardrev,
    "Board Revision Number.",
    "Board Revision Number. \n"
    "boardrev"
);

U_BOOT_CMD(
    setboardrev,    3,  1,  setboardrev,
    "Set Board Revision Number.",
    "Set Board Revision Number. \n"
    "setboardrev major minor"
);

U_BOOT_CMD(
    halt,   1,  1,  do_halt,
    "Halt the processor\n",
    ""
);
