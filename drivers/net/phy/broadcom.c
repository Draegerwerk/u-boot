// SPDX-License-Identifier: GPL-2.0+
/*
 * Broadcom PHY drivers
 *
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 * author Andy Fleming
 */
#include <common.h>
#include <malloc.h>
#include <asm-generic/gpio.h>
#include <phy.h>
#include <linux/delay.h>

/* Broadcom BCM54xx -- taken from linux sungem_phy */
#define MIIM_BCM54xx_AUXCNTL			0x18
#define MIIM_BCM54xx_AUXCNTL_ENCODE(val) (((val & 0x7) << 12)|(val & 0x7))
#define MIIM_BCM54xx_AUXSTATUS			0x19
#define MIIM_BCM54xx_AUXSTATUS_LINKMODE_MASK	0x0700
#define MIIM_BCM54xx_AUXSTATUS_LINKMODE_SHIFT	8

#define MIIM_BCM54XX_SHD			0x1c
#define MIIM_BCM54XX_SHD_WRITE			0x8000
#define MIIM_BCM54XX_SHD_VAL(x)			((x & 0x1f) << 10)
#define MIIM_BCM54XX_SHD_DATA(x)		((x & 0x3ff) << 0)
#define MIIM_BCM54XX_SHD_WR_ENCODE(val, data)	\
	(MIIM_BCM54XX_SHD_WRITE | MIIM_BCM54XX_SHD_VAL(val) | \
	 MIIM_BCM54XX_SHD_DATA(data))

#define MIIM_BCM54XX_EXP_DATA		0x15	/* Expansion register data */
#define MIIM_BCM54XX_EXP_SEL		0x17	/* Expansion register select */
#define MIIM_BCM54XX_EXP_SEL_SSD	0x0e00	/* Secondary SerDes select */
#define MIIM_BCM54XX_EXP_SEL_ER		0x0f00	/* Expansion register select */

#define MIIM_BCM_AUXCNTL_SHDWSEL_MISC	0x0007
#define MIIM_BCM_AUXCNTL_ACTL_SMDSP_EN	0x0800

#define MIIM_BCM_CHANNEL_WIDTH    0x2000

#define MII_RDB_ADDR_SEL_REG                        0x01E /* Selects the 12-bit RDB register address */
#define MII_RDB_DATA_RW_REG                         0x01F /* Register to Read/Write Data from RDB register */

#define MII_RDB_CLOCK_ALIGNMENT_CTRL_REG            0x013 /* RDB CLOCK_ALIGNMENT_CONTROL Register */
#define MII_RDB_MODE_CTRL_REG                       0x021 /* RDB MODE_CONTROL Register */
#define MII_RDB_COPPER_AUX_CTRL_REG                 0x028 /* RDB COPPER_AUXILIARY_CONTROL Register */
#define MII_RDB_10BASE_T_REG                        0x029 /* RDB 10BASE-T Register */
#define MII_RDB_COPPER_PWR_MII_CTRL_REG             0x02A /* RDB COPPER_POWER/MII_CONTROL Register */
#define MII_RDB_COPPER_MISC_TEST_REG                0x02C /* RDB COPPER_MISCELLANEOUS_TEST Register */
#define MII_RDB_COPPER_MISC_CTRL_REG                0x02F /* RDB COPPER_MISCELLANEOUS_CONTROL Register */
#define MII_RDB_RX_TX_PACKET_CNTR_REG               0x030 /* RDB RX_TX_PACKET_COUNTER Register */
#define MII_RDB_MULTICOLOR_LED_SELECTOR_REG         0x034 /* RDB MULTICOLOR_LED_SELECTOR Register */
#define MII_RDB_MULTICOLOR_LED_FLASH_RATE_CTRL_REG  0x035 /* RDB MULTICOLOR_LED_FLASH_RATE_CONTROL Register */
#define MII_RDB_MULTICOLOR_LED_PRGM_BLINK_CTRL_REG  0x036 /* RDB MULTICOLOR_LED_PROGRAMMABLE_BLINK_CONTROL Register */
#define MII_RDB_100BASE_FX_FAR_END_FAULT_REG        0x037 /* RDB 100BASE_FX_FAR_END_FAULT Register */
#define MII_RDB_TOP_LEVEL_CONFIG_REG                0x810 /* RDB TOP_LEVEL_CONFIGURATION Register */
#define MII_RDB_SGMII_SLAVE_REG                     0x235 /* RDB SGMII_SLAVE Register */
#define MII_RDB_AUTO_DET_MEDIUM_REG                 0x23E /* RDB AUTO_DETECT_MEDIUM Register */

/* Defines for RDB CLOCK_ALIGNMENT_CONTROL Register */
#define CLOCK_ALIGN_CTRL_SHD1C_SEL_POS              (10)
#define CLOCK_ALIGN_CTRL_SHD1C_SEL_DEFAULT          0x03

#define CLOCK_ALIGN_CTRL_GTXCLK_DELAY_EN            0x0200

/* Configuration of CLOCK_ALIGNMENT_CONTROL for RGMII-to-Copper */
#define CLOCK_ALIGN_SHD1C_SEL_CONFIG                (CLOCK_ALIGN_CTRL_SHD1C_SEL_DEFAULT << CLOCK_ALIGN_CTRL_SHD1C_SEL_POS)
#define CLOCK_ALIGN_CTRL_CONFIG                     (CLOCK_ALIGN_SHD1C_SEL_CONFIG | \
                                                     CLOCK_ALIGN_CTRL_GTXCLK_DELAY_EN)

/* Defines for RDB MODE_CONTROL Register */
#define MODE_CTRL_SHD1C_SEL_POS                     (10)
#define MODE_CTRL_SHD1C_SEL_DEFAULT                 0x1F

#define MODE_CTRL_MODE_SEL_POS                      (1)
#define MODE_CTRL_MODE_SEL_RGMII_TO_COPPER          0x0
#define MODE_CTRL_MODE_SEL_RGMII_TO_FIBER           0x1
#define MODE_CTRL_MODE_SEL_SGMII_TO_COPPER          0x2
#define MODE_CTRL_MODE_SEL_MEDIA_CONVERTER          0x3

#define MODE_CTRL_REG_1000X_EN                      0x0001
#define MODE_CTRL_SERDES_CAPABLE                    0x0008
#define MODE_CTRL_FIBER_SIGNAL_DET                  0x0010
#define MODE_CTRL_COPPER_ENERGY_DET                 0x0020
#define MODE_CTRL_FIBER_LINK_UP                     0x0040
#define MODE_CTRL_COPPER_LINK_UP                    0x0080
#define MODE_CTRL_DUAL_SERDES_CAPABLE               0x0200

/* Configuration of MODE_CONTROL for RGMII-to-Copper and RGMII-to-Fiber */
#define MODE_CTRL_SHD1C_SEL                         (MODE_CTRL_SHD1C_SEL_DEFAULT        << MODE_CTRL_SHD1C_SEL_POS)
#define MODE_CTRL_MODE_SEL_RGMII_TO_COPPER_CONFIG   (MODE_CTRL_MODE_SEL_RGMII_TO_COPPER << MODE_CTRL_MODE_SEL_POS)
#define MODE_CTRL_MODE_SEL_RGMII_TO_FIBER_CONFIG    (MODE_CTRL_MODE_SEL_RGMII_TO_FIBER  << MODE_CTRL_MODE_SEL_POS)

#define MODE_CTRL_RGMII_TO_COPPER_CONFIG_1          (MODE_CTRL_SHD1C_SEL                        | \
                                                     MODE_CTRL_MODE_SEL_RGMII_TO_COPPER_CONFIG  | \
                                                     MODE_CTRL_REG_1000X_EN)

#define MODE_CTRL_RGMII_TO_COPPER_CONFIG_2          (MODE_CTRL_SHD1C_SEL                        | \
                                                     MODE_CTRL_MODE_SEL_RGMII_TO_COPPER_CONFIG)

#define MODE_CTRL_RGMII_TO_FIBER_CONFIG_1           (MODE_CTRL_SHD1C_SEL                        | \
                                                     MODE_CTRL_MODE_SEL_RGMII_TO_FIBER_CONFIG)

#define MODE_CTRL_RGMII_TO_FIBER_CONFIG_2           (MODE_CTRL_SHD1C_SEL                        | \
                                                     MODE_CTRL_MODE_SEL_RGMII_TO_FIBER_CONFIG   | \
                                                     MODE_CTRL_REG_1000X_EN)

/*
 * Application notes are asking to set this register to 0x71E7,
 * but bits[15:12], 6 and [3:0] are being shown as RESERVED.
 */
#define COPPER_MISC_CTRL_RSRVD_BIT15                0x8000
#define COPPER_MISC_CTRL_RSRVD_BIT14                0x4000
#define COPPER_MISC_CTRL_RSRVD_BIT13                0x2000
#define COPPER_MISC_CTRL_RSRVD_BIT12                0x1000
#define COPPER_MISC_CTRL_ENABLE_PACKET_CNTR         0x0800
#define COPPER_MISC_CTRL_BYPASS_WIRESPEED_TIMER     0x0400
#define COPPER_MISC_CTRL_FORCE_AUTO_MDIX            0x0200
#define COPPER_MISC_CTRL_EN_RGMII_RXD_TO_RXC_SKEW   0x0100
#define COPPER_MISC_CTRL_RGMII_MODE                 0x0080
#define COPPER_MISC_CTRL_RSRVD_BIT06                0x0040
#define COPPER_MISC_CTRL_REG_RX_DATA_DURING_IPG     0x0020
#define COPPER_MISC_CTRL_WIRESPEED_EN               0x0010
#define COPPER_MISC_CTRL_RSRVD_BIT3                 0x0008
#define COPPER_MISC_CTRL_RSRVD_BIT2                 0x0004
#define COPPER_MISC_CTRL_RSRVD_BIT1                 0x0002
#define COPPER_MISC_CTRL_RSRVD_BIT0                 0x0001

/* Configuration of COPPER_MISCELLANEOUS_CONTROL for RGMII-to-Copper */
#define COPPER_MISC_CTRL_CONFIG                     (COPPER_MISC_CTRL_RSRVD_BIT14              | \
                                                     COPPER_MISC_CTRL_RSRVD_BIT13              | \
                                                     COPPER_MISC_CTRL_RSRVD_BIT12              | \
                                                     COPPER_MISC_CTRL_EN_RGMII_RXD_TO_RXC_SKEW | \
                                                     COPPER_MISC_CTRL_RGMII_MODE               | \
                                                     COPPER_MISC_CTRL_RSRVD_BIT06              | \
                                                     COPPER_MISC_CTRL_REG_RX_DATA_DURING_IPG   | \
                                                     COPPER_MISC_CTRL_RSRVD_BIT2               | \
                                                     COPPER_MISC_CTRL_RSRVD_BIT1               | \
                                                     COPPER_MISC_CTRL_RSRVD_BIT0)

#define TOP_LEVEL_CONFIG_RSRVD                       0x00B0
#define TOP_LEVEL_CONFIG_RGMII_SEL_1P8V_HSTL_POS     (0)
#define TOP_LEVEL_CONFIG_RGMII_SEL_3P3V_HSTL         0x0
#define TOP_LEVEL_CONFIG_RGMII_SEL_2P5V_HSTL         0x1
#define TOP_LEVEL_CONFIG_RGMII_SEL_1P8V_HSTL         0x3

/* Configuration of TOP_LEVEL_CONFIGURATION */
#define TOP_LEVEL_RGMII_SEL_1P8V_HSTL_CONFIG         (TOP_LEVEL_CONFIG_RGMII_SEL_1P8V_HSTL << TOP_LEVEL_CONFIG_RGMII_SEL_1P8V_HSTL_POS)
#define TOP_LEVEL_CONFIG_REG_CONFIG                  (TOP_LEVEL_CONFIG_RSRVD                     | \
                                                      TOP_LEVEL_RGMII_SEL_1P8V_HSTL_CONFIG)

/* Defines for RDB AUTO_DETECT_MEDIUM Register */
#define AUTO_DET_MED_RGMII_AUTO_DET_EN               0x001
#define AUTO_DET_MED_PRI_FIBER                       0x002
#define AUTO_DET_MED_DEFAULT_FIBER                   0x004
#define AUTO_DET_MED_FIBER_SD_SYNC_STATUS            0x020
#define AUTO_DET_MED_USE_FIBER_LED                   0x040
#define AUTO_DET_MED_FIBER_IN_USE_LED                0x080
#define AUTO_DET_MED_INV_FIBER_SD_FROM_PIN           0x100
#define AUTO_DET_MED_SGMII_AUTO_DET_EN               0x200

#define AUTO_DET_MED_SHD1C_SEL_POS                   (10)
#define AUTO_DET_MED_SHD1C_SEL_DEFAULT               0x1E

/* Configuration of MODE_CONTROL for RGMII-to-Copper and RGMII-to-Fiber */
#define AUTO_DET_MED_SHD1C_SEL                      (AUTO_DET_MED_SHD1C_SEL_DEFAULT << AUTO_DET_MED_SHD1C_SEL_POS)

#define AUTO_DET_MED_CONFIG                         (AUTO_DET_MED_SHD1C_SEL                    | \
                                                     AUTO_DET_MED_FIBER_IN_USE_LED             | \
                                                     AUTO_DET_MED_USE_FIBER_LED                | \
                                                     AUTO_DET_MED_FIBER_SD_SYNC_STATUS         | \
                                                     AUTO_DET_MED_DEFAULT_FIBER)

#define MII_MASSLA_CTRL_REG	0x9  /* MATER-SLAVE control register */
#define MII_MASSLA_STAT_REG	0xa  /* MATER-SLAVE status register */

/* MII Master-Slave Control register bit definition */
#define MII_MASSLA_CTRL_1000T_HD    0x100
#define MII_MASSLA_CTRL_1000T_FD    0x200
#define MII_MASSLA_CTRL_PORT_TYPE   0x400
#define MII_MASSLA_CTRL_CONFIG_VAL  0x800
#define MII_MASSLA_CTRL_CONFIG_EN   0x1000

/* MII Master-Slave Status register bit definition */
#define MII_MASSLA_STAT_LP1000T_HD  0x400
#define MII_MASSLA_STAT_LP1000T_FD  0x800
#define MII_MASSLA_STAT_REMOTE_RCV  0x1000
#define MII_MASSLA_STAT_LOCAL_RCV   0x2000
#define MII_MASSLA_STAT_CONF_RES    0x4000
#define MII_MASSLA_STAT_CONF_FAULT  0x8000

#define SIGNAL_MODE_COPPER          0
#define SIGNAL_MODE_FIBER           1
#define SIGNAL_MODE_NONE_DETECTED  -1

#define SIGNAL_DETECTION_TIMEOUT_MS 2000    /* 2 seconds */
#define LINK_DETECTION_TIMEOUT_MS   5000    /* 5 seconds */

#define LINK_PROGRESS_ITERATION     500     /* Every 500 ms */

struct bcm54210s_phy_priv {
	int              mode;
	struct gpio_desc enable_optics_gpio;
};

static int bcm54210s_get_mode(struct phy_device *phydev);
static int bcm54210sWaitForSignal(struct phy_device *phydev);
static void bcm54210sWaitForLink(struct phy_device *phydev);

static void bcm_phy_write_misc(struct phy_device *phydev,
			       u16 reg, u16 chl, u16 value)
{
	int reg_val;

	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54xx_AUXCNTL,
		  MIIM_BCM_AUXCNTL_SHDWSEL_MISC);

	reg_val = phy_read(phydev, MDIO_DEVAD_NONE, MIIM_BCM54xx_AUXCNTL);
	reg_val |= MIIM_BCM_AUXCNTL_ACTL_SMDSP_EN;
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54xx_AUXCNTL, reg_val);

	reg_val = (chl * MIIM_BCM_CHANNEL_WIDTH) | reg;
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_SEL, reg_val);

	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_DATA, value);
}

/* Broadcom BCM5461S */
static int bcm5461_config(struct phy_device *phydev)
{
	genphy_config_aneg(phydev);

	phy_reset(phydev);

	return 0;
}

static int bcm54xx_parse_status(struct phy_device *phydev)
{
	unsigned int mii_reg;

	mii_reg = phy_read(phydev, MDIO_DEVAD_NONE, MIIM_BCM54xx_AUXSTATUS);

	switch ((mii_reg & MIIM_BCM54xx_AUXSTATUS_LINKMODE_MASK) >>
			MIIM_BCM54xx_AUXSTATUS_LINKMODE_SHIFT) {
	case 1:
		phydev->duplex = DUPLEX_HALF;
		phydev->speed = SPEED_10;
		break;
	case 2:
		phydev->duplex = DUPLEX_FULL;
		phydev->speed = SPEED_10;
		break;
	case 3:
		phydev->duplex = DUPLEX_HALF;
		phydev->speed = SPEED_100;
		break;
	case 5:
		phydev->duplex = DUPLEX_FULL;
		phydev->speed = SPEED_100;
		break;
	case 6:
		phydev->duplex = DUPLEX_HALF;
		phydev->speed = SPEED_1000;
		break;
	case 7:
		phydev->duplex = DUPLEX_FULL;
		phydev->speed = SPEED_1000;
		break;
	default:
		printf("Auto-neg error, defaulting to 10BT/HD\n");
		phydev->duplex = DUPLEX_HALF;
		phydev->speed = SPEED_10;
		break;
	}

	return 0;
}

static int bcm54xx_startup(struct phy_device *phydev)
{
	int ret;

	/* Read the Status (2x to make sure link is right) */
	ret = genphy_update_link(phydev);
	if (ret)
		return ret;

	return bcm54xx_parse_status(phydev);
}

/* Broadcom BCM5482S */
/*
 * "Ethernet@Wirespeed" needs to be enabled to achieve link in certain
 * circumstances.  eg a gigabit TSEC connected to a gigabit switch with
 * a 4-wire ethernet cable.  Both ends advertise gigabit, but can't
 * link.  "Ethernet@Wirespeed" reduces advertised speed until link
 * can be achieved.
 */
static u32 bcm5482_read_wirespeed(struct phy_device *phydev, u32 reg)
{
	return (phy_read(phydev, MDIO_DEVAD_NONE, reg) & 0x8FFF) | 0x8010;
}

static int bcm5482_config(struct phy_device *phydev)
{
	unsigned int reg;

	/* reset the PHY */
	reg = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR);
	reg |= BMCR_RESET;
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, reg);

	/* Setup read from auxilary control shadow register 7 */
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54xx_AUXCNTL,
			MIIM_BCM54xx_AUXCNTL_ENCODE(7));
	/* Read Misc Control register and or in Ethernet@Wirespeed */
	reg = bcm5482_read_wirespeed(phydev, MIIM_BCM54xx_AUXCNTL);
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54xx_AUXCNTL, reg);

	/* Initial config/enable of secondary SerDes interface */
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_SHD,
			MIIM_BCM54XX_SHD_WR_ENCODE(0x14, 0xf));
	/* Write intial value to secondary SerDes Contol */
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_SEL,
			MIIM_BCM54XX_EXP_SEL_SSD | 0);
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_DATA,
			BMCR_ANRESTART);
	/* Enable copper/fiber auto-detect */
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_SHD,
			MIIM_BCM54XX_SHD_WR_ENCODE(0x1e, 0x201));

	genphy_config_aneg(phydev);

	return 0;
}

static int bcm_cygnus_startup(struct phy_device *phydev)
{
	int ret;

	/* Read the Status (2x to make sure link is right) */
	ret = genphy_update_link(phydev);
	if (ret)
		return ret;

	return genphy_parse_link(phydev);
}

static void bcm_cygnus_afe(struct phy_device *phydev)
{
	/* ensures smdspclk is enabled */
	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54xx_AUXCNTL, 0x0c30);

	/* AFE_VDAC_ICTRL_0 bit 7:4 Iq=1100 for 1g 10bt, normal modes */
	bcm_phy_write_misc(phydev, 0x39, 0x01, 0xA7C8);

	/* AFE_HPF_TRIM_OTHERS bit11=1, short cascode for all modes*/
	bcm_phy_write_misc(phydev, 0x3A, 0x00, 0x0803);

	/* AFE_TX_CONFIG_1 bit 7:4 Iq=1100 for test modes */
	bcm_phy_write_misc(phydev, 0x3A, 0x01, 0xA740);

	/* AFE TEMPSEN_OTHERS rcal_HT, rcal_LT 10000 */
	bcm_phy_write_misc(phydev, 0x3A, 0x03, 0x8400);

	/* AFE_FUTURE_RSV bit 2:0 rccal <2:0>=100 */
	bcm_phy_write_misc(phydev, 0x3B, 0x00, 0x0004);

	/* Adjust bias current trim to overcome digital offSet */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1E, 0x02);

	/* make rcal=100, since rdb default is 000 */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x17, 0x00B1);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x15, 0x0010);

	/* CORE_EXPB0, Reset R_CAL/RC_CAL Engine */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x17, 0x00B0);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x15, 0x0010);

	/* CORE_EXPB0, Disable Reset R_CAL/RC_CAL Engine */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x17, 0x00B0);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x15, 0x0000);
}

static int bcm_cygnus_config(struct phy_device *phydev)
{
	genphy_config_aneg(phydev);
	phy_reset(phydev);
	/* AFE settings for PHY stability */
	bcm_cygnus_afe(phydev);
	/* Forcing aneg after applying the AFE settings */
	genphy_restart_aneg(phydev);

	return 0;
}

/*******************************************************************************
*
* bcm54210sConfigInitFiber - config broadcom bcm54210s phy for fiber (optical)
*                            operation.
*
* This routine configures broadcom bcm54210s phy to RGMII-to-Fiber Mode based on
* application notes found in the Interface Mode Configuration Guide -
* 54210S-Interface_cfg.pdf Rev: 54210S-AN301-R.
*
* RETURNS: Success
*
* ERRNO: N/A
*/

int bcm54210sConfigInitFiber(struct phy_device *phydev, int speed)
{
	u16 miiVal = 0;
	struct bcm54210s_phy_priv *priv = (struct bcm54210s_phy_priv *)phydev->priv;

	printf("Initialize BCM54210 %d Mbit for Fiber\n", speed);

	/* Enable optics */
	dm_gpio_set_value(&priv->enable_optics_gpio, 1);

	/*
	 * More details on this procedure can be found on page 16 of
	 * the Interface Mode Configuration Guide.
	 */

	/*
	 * Use RDB Access Method to get access to supplementary registers.
	 * This is done by writting address of the supplemenetary register
	 * to register 0x1E. Then, you can read or write the supplementary
	 * register by making calls to 0x1F register.
	 */

	/* Step 1: Configure COPPER_MISCELLANEOUS_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_COPPER_MISC_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, COPPER_MISC_CTRL_CONFIG);

	/* Step 2: Configure CLOCK_ALIGNMENT_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_CLOCK_ALIGNMENT_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, CLOCK_ALIGN_CTRL_CONFIG);

	/* Step 3: Configure TOP_LEVEL_CONFIGURATION Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_TOP_LEVEL_CONFIG_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, TOP_LEVEL_CONFIG_REG_CONFIG);

	/* Step 4: Configure MODE_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_MODE_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, MODE_CTRL_RGMII_TO_FIBER_CONFIG_1);

	/* Step 5: Configure MII Control Register */
	if (speed == SPEED_1000)
	{
		miiVal = BMCR_ANENABLE | BMCR_PDOWN | BMCR_FULLDPLX | BMCR_SPEED1000;
	}
	else
	{
		miiVal = BMCR_ANENABLE | BMCR_PDOWN | BMCR_FULLDPLX | BMCR_SPEED100;
	}
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, miiVal);

	/* Step 6: Configure MODE_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_MODE_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, MODE_CTRL_RGMII_TO_FIBER_CONFIG_2);

	/* Step 7: Configure AUTO_DETECT_MEDIUM Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_AUTO_DET_MEDIUM_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, AUTO_DET_MED_CONFIG);

	/*
	 * Step 8: Configure MII Control Register:
	 * Enable Auto Negotiation, Full-duplex Mode, 1000 BASE,
	 * and restart auto negotiation
	 */
	if (speed == SPEED_1000)
	{
		miiVal = BMCR_ANRESTART | BMCR_ANENABLE | BMCR_FULLDPLX | BMCR_SPEED1000;
	}
	else
	{
		miiVal = BMCR_ANRESTART | BMCR_ANENABLE | BMCR_FULLDPLX | BMCR_SPEED100;
	}

	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, miiVal);

	return (0);
}

/*******************************************************************************
*
* bcm54210sConfigInitCopper - config broadcom bcm54210s phy for copper operation.
*
* This routine configures broadcom bcm54210s phy to RGMII-to-Copper Mode based on
* application notes found in the Interface Mode Configuration Guide -
* 54210S-Interface_cfg.pdf Rev: 54210S-AN301-R.
*
* RETURNS: Success
*
* ERRNO: N/A
*/

int bcm54210sConfigInitCopper(struct phy_device *phydev, int speed)
{
	u16 miiVal = 0;

	printf("Initialize BCM54210 %d Mbit for Copper\n", speed);

	/*
	 * Use RDB Access Method to get access to supplementary registers.
	 * This is done by writting address of the supplemenetary register
	 * to register 0x1E. Then, you can read or write the supplementary
	 * register by making calls to 0x1F register.
	 */

	/* Step 1: Configure COPPER_MISCELLANEOUS_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_COPPER_MISC_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, COPPER_MISC_CTRL_CONFIG);

	/* Step 2: Configure CLOCK_ALIGNMENT_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_CLOCK_ALIGNMENT_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, CLOCK_ALIGN_CTRL_CONFIG);

	/* Step 3: Configure TOP_LEVEL_CONFIGURATION Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_TOP_LEVEL_CONFIG_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, TOP_LEVEL_CONFIG_REG_CONFIG);

	/* Step 4: Configure MODE_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_MODE_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, MODE_CTRL_RGMII_TO_COPPER_CONFIG_1);

	/* Step 5: Configure MII Control Register */
	miiVal = BMCR_PDOWN | BMCR_FULLDPLX;

	if (speed == SPEED_1000)
	{
	  miiVal |= BMCR_SPEED1000 | BMCR_ANENABLE;
	}
	else
	{
	  miiVal |= BMCR_SPEED100;
	}
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, miiVal);

	/* Step 6: Configure MODE_CONTROL Register */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_MODE_CTRL_REG);
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG, MODE_CTRL_RGMII_TO_COPPER_CONFIG_2);

	/* Enable advertisement of gigE modes. */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_MASSLA_CTRL_REG, MII_MASSLA_CTRL_1000T_FD | MII_MASSLA_CTRL_1000T_HD);

	/* Step 8: Optional - skipped */

	/* Step 9: Configure MII Control Register:
	 * Enable Auto Negotiation, Full-duplex Mode, 1000 BASE,
	 * and restart auto negotiation
	 */
	miiVal = BMCR_FULLDPLX;
	if (speed == SPEED_1000)
	{
	  miiVal |= BMCR_ANRESTART | BMCR_SPEED1000 | BMCR_ANENABLE;
	}
	else
	{
	  miiVal |= BMCR_SPEED100;
	}
	phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, miiVal);

	return (0);

}

/*******************************************************************************
*
* bcm54210s_config - config broadcom bcm54210s phy.
*
* This routine configures broadcom bcm54210s phy to RGMII-to-Copper Mode based on
* application notes found in the Interface Mode Configuration Guide -
* 54210S-Interface_cfg.pdf Rev: 54210S-AN301-R.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static int bcm54210s_config(struct phy_device *phydev)
{
	struct udevice            *dev = phydev->dev;
	struct bcm54210s_phy_priv *priv;
	int                        ret;

	if (phydev->priv)
	{
		printf("Already configured\n");
		return 0;
	}

	priv = malloc(sizeof(*priv));

	if (priv == NULL)
	{
		return 1;
	}

	phydev->priv = priv;

	ret = gpio_request_by_name(dev, "enable-optics-gpios", 0,
	                           &priv->enable_optics_gpio, GPIOD_IS_OUT);

	if (ret)
	{
		printf("%s: no enable-optics-gpios defined!\n", dev->name);
		return 1;
	}

	return 0;
}

/*******************************************************************************
*
* bcm54210s_startup - stratup routine for broadcom bcm54210s phy.
*
* This routine detects if PHY is used in Copper or Fiber mode
* and then invokes appropriate phy startup routine.
*
* RETURNS: success
*
* ERRNO: N/A
*/
static int bcm54210s_startup(struct phy_device *phydev)
{
	int retVal = 0;
	int mode = SIGNAL_MODE_NONE_DETECTED;

	/*
	 * Detect signal mode: fiber or copper.
	 * Sometimes it takes few milliseconds,
	 * so give it a chance.
	 */
	mode = bcm54210sWaitForSignal(phydev);

	/* If copper energy detected, configure for copper */
	if(SIGNAL_MODE_COPPER == mode)
	{
		bcm54210sConfigInitCopper(phydev, SPEED_1000);

		/*
		 * Use Generic PHY Startup
		 */
		retVal = genphy_startup(phydev);
	}
	else
	{
		/* else configure for Fiber (Optical) */
		bcm54210sConfigInitFiber(phydev, SPEED_1000);

		phydev->speed = SPEED_1000;
		phydev->duplex = DUPLEX_FULL;

		bcm54210sWaitForLink(phydev);
	}


	return retVal;
}

/*******************************************************************************
*
* bcm54210s_get_mode - check if bcm54210s phy is connected to fiber or copper.
*
* This routine checks Mode Control register to see if fiber or copper
* signal is detected.
*
* RETURNS: 0 - if Copper energy detected
*          1 - if Fiber signal detected
*         -1 - if no Copper or Fiber signal detected
*
* ERRNO: N/A
*/
static int bcm54210s_get_mode(struct phy_device *phydev)
{
	unsigned int reg = 0;
	int mode = SIGNAL_MODE_NONE_DETECTED;

	/*
	 * Read Mode Control register which contains bits
	 * specifing if copper or fiber connection was detected.
	 */
	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_MODE_CTRL_REG);

	reg = phy_read(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG);

	/* Check if Copper Energy is detected or Fiber Signal */
	if(reg & MODE_CTRL_COPPER_ENERGY_DET)
	{
		mode = SIGNAL_MODE_COPPER;
	}
	else if (reg & MODE_CTRL_FIBER_SIGNAL_DET)
	{
		mode = SIGNAL_MODE_FIBER;
	}

	return mode;
}

/*******************************************************************************
*
* bcm54210sWaitForLink - wait for link
*
* This routine keeps on checking link status until link is up or
* timeout occurs.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

static void bcm54210sWaitForLink(struct phy_device *phydev)
{
	int i = 0;
	unsigned int reg = 0;

	printf("Waiting for Link Status");

	/* Get initial state of the status register */
	reg = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);

	/* Loop until link is detected or timeout occurs */
	while (!(reg & BMSR_LSTATUS))
	{
		/* Timeout reached ? */
		if (i > LINK_DETECTION_TIMEOUT_MS)
		{
			printf(" TIMEOUT !\n");
			phydev->link = 0;
			return;
		}

		if ((i++ % LINK_PROGRESS_ITERATION) == 0)
		{
			printf(".");
		}

		/* Delays 1 ms */
		mdelay(1);

		/* Read status register again */
		reg = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);
	}

	printf(" Link obtained in %d ms\n", i);
	phydev->link = 1;
}

/*******************************************************************************
*
* bcm54210sWaitForSignal - waits to detect copper or fiber signal.
*
* This routine waits to figure out if the broadcom bcm54210s phy is connected
* to Copper or Fiber. Once signal is detected, it returns the signal mode.
*
* RETURNS: 0 - if Copper energy detected
*          1 - if Fiber signal detected
*         -1 - if no Copper or Fiber signal detected
*
* ERRNO: N/A
*/
static int bcm54210sWaitForSignal(struct phy_device *phydev)
{
	int i = 0;
	int mode = SIGNAL_MODE_NONE_DETECTED;

	mode = bcm54210s_get_mode(phydev);

	/* Loop until a copper signal is detected or timeout occurs */
	while (SIGNAL_MODE_COPPER != mode)
	{
		/* Timeout reached ? */
		if (i++ > SIGNAL_DETECTION_TIMEOUT_MS)
		{
			if (mode == SIGNAL_MODE_NONE_DETECTED)
			{
				/* Neither a copper nor a fiber signal detected */
				printf(" TIMEOUT !\n");
			}

			return mode;
		}

		/* Delays 1 ms */
		mdelay(1);

		mode = bcm54210s_get_mode(phydev);
	}

	return mode;
}

bool bcm54210_dump_regs(struct phy_device *phydev)
{
	u16 misr, sgmiisr;

	phy_write(phydev, MDIO_DEVAD_NONE, MII_RDB_ADDR_SEL_REG, MII_RDB_SGMII_SLAVE_REG);
	sgmiisr = phy_read(phydev, MDIO_DEVAD_NONE, MII_RDB_DATA_RW_REG);

	misr = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);

	printf("BCM54:\t\tMIICR:\t0x%04x\tMIISR:\t0x%04x\tSGMII:\t0x%04x\n",
	       phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR), misr, sgmiisr);

	return (misr & BMSR_LSTATUS);
}

/*
 * Find out if PHY is in copper or serdes mode by looking at Expansion Reg
 * 0x42 - "Operating Mode Status Register"
 */
static int bcm5482_is_serdes(struct phy_device *phydev)
{
	u16 val;
	int serdes = 0;

	phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_SEL,
			MIIM_BCM54XX_EXP_SEL_ER | 0x42);
	val = phy_read(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_DATA);

	switch (val & 0x1f) {
	case 0x0d:	/* RGMII-to-100Base-FX */
	case 0x0e:	/* RGMII-to-SGMII */
	case 0x0f:	/* RGMII-to-SerDes */
	case 0x12:	/* SGMII-to-SerDes */
	case 0x13:	/* SGMII-to-100Base-FX */
	case 0x16:	/* SerDes-to-Serdes */
		serdes = 1;
		break;
	case 0x6:	/* RGMII-to-Copper */
	case 0x14:	/* SGMII-to-Copper */
	case 0x17:	/* SerDes-to-Copper */
		break;
	default:
		printf("ERROR, invalid PHY mode (0x%x\n)", val);
		break;
	}

	return serdes;
}

/*
 * Determine SerDes link speed and duplex from Expansion reg 0x42 "Operating
 * Mode Status Register"
 */
static u32 bcm5482_parse_serdes_sr(struct phy_device *phydev)
{
	u16 val;
	int i = 0;

	/* Wait 1s for link - Clause 37 autonegotiation happens very fast */
	while (1) {
		phy_write(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_SEL,
				MIIM_BCM54XX_EXP_SEL_ER | 0x42);
		val = phy_read(phydev, MDIO_DEVAD_NONE, MIIM_BCM54XX_EXP_DATA);

		if (val & 0x8000)
			break;

		if (i++ > 1000) {
			phydev->link = 0;
			return 1;
		}

		udelay(1000);	/* 1 ms */
	}

	phydev->link = 1;
	switch ((val >> 13) & 0x3) {
	case (0x00):
		phydev->speed = 10;
		break;
	case (0x01):
		phydev->speed = 100;
		break;
	case (0x02):
		phydev->speed = 1000;
		break;
	}

	phydev->duplex = (val & 0x1000) == 0x1000;

	return 0;
}

/*
 * Figure out if BCM5482 is in serdes or copper mode and determine link
 * configuration accordingly
 */
static int bcm5482_startup(struct phy_device *phydev)
{
	int ret;

	if (bcm5482_is_serdes(phydev)) {
		bcm5482_parse_serdes_sr(phydev);
		phydev->port = PORT_FIBRE;
		return 0;
	}

	/* Wait for auto-negotiation to complete or fail */
	ret = genphy_update_link(phydev);
	if (ret)
		return ret;

	/* Parse BCM54xx copper aux status register */
	return bcm54xx_parse_status(phydev);
}

static struct phy_driver BCM5461S_driver = {
	.name = "Broadcom BCM5461S",
	.uid = 0x2060c0,
	.mask = 0xfffff0,
	.features = PHY_GBIT_FEATURES,
	.config = &bcm5461_config,
	.startup = &bcm54xx_startup,
	.shutdown = &genphy_shutdown,
};

static struct phy_driver BCM5464S_driver = {
	.name = "Broadcom BCM5464S",
	.uid = 0x2060b0,
	.mask = 0xfffff0,
	.features = PHY_GBIT_FEATURES,
	.config = &bcm5461_config,
	.startup = &bcm54xx_startup,
	.shutdown = &genphy_shutdown,
};

static struct phy_driver BCM5482S_driver = {
	.name = "Broadcom BCM5482S",
	.uid = 0x143bcb0,
	.mask = 0xffffff0,
	.features = PHY_GBIT_FEATURES,
	.config = &bcm5482_config,
	.startup = &bcm5482_startup,
	.shutdown = &genphy_shutdown,
};

static struct phy_driver BCM_CYGNUS_driver = {
	.name = "Broadcom CYGNUS GPHY",
	.uid = 0xae025200,
	.mask = 0xfffff0,
	.features = PHY_GBIT_FEATURES,
	.config = &bcm_cygnus_config,
	.startup = &bcm_cygnus_startup,
	.shutdown = &genphy_shutdown,
};

static struct phy_driver BCM54210S_driver = {
	.name = "Broadcom BCM54210S",
	.uid = 0x600D8590,
	.mask = 0xffffff0,
	.features = PHY_GBIT_FEATURES,
	.config = &bcm54210s_config,
	.startup = &bcm54210s_startup,
	.shutdown = &genphy_shutdown,
};

int phy_broadcom_init(void)
{
	phy_register(&BCM5482S_driver);
	phy_register(&BCM5464S_driver);
	phy_register(&BCM5461S_driver);
	phy_register(&BCM54210S_driver);
	phy_register(&BCM_CYGNUS_driver);

	return 0;
}
