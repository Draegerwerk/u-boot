/*
 * Copyright (C) 2015 Draegerwerk AG
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/video.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <fsl_sec.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/crm_regs.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <i2c.h>
#include <power/pmic.h>
#include <power/pfuze100_pmic.h>
#include <micrel.h>
#include <version.h>
#include <common.h>
#include <config.h>
#include <command.h>
#include <i2c.h>
#include <post.h>
#include <draeger_m48_pmstruct.h>
#include <asm/cache.h>
#include <malloc.h>

#define uint32_t int
#define AIPSTZ2_MPR (*(volatile uint32_t *)(0x0217C000))
#define AIPSTZ1_MPR (*(volatile uint32_t *)(0x0207C000))
#define AIPSTZ1_OPACR (*(volatile uint32_t *)(0x0207C040))
#define AIPSTZ1_OPACR1 (*(volatile uint32_t *)(0x0207C044))
#define AIPSTZ1_OPACR2 (*(volatile uint32_t *)(0x0207C048))
#define AIPSTZ1_OPACR3 (*(volatile uint32_t *)(0x0207C04C))
#define AIPSTZ1_OPACR4 (*(volatile uint32_t *)(0x0207C050))
#define AIPSTZ2_OPACR (*(volatile uint32_t *)(0x0217C040))
#define AIPSTZ2_OPACR1 (*(volatile uint32_t *)(0x0217C044))
#define AIPSTZ2_OPACR2 (*(volatile uint32_t *)(0x0217C048))
#define AIPSTZ2_OPACR3 (*(volatile uint32_t *)(0x0217C04C))
#define AIPSTZ2_OPACR4 (*(volatile uint32_t *)(0x0217C050))
#define CSU_BASE 0x021C0000
#define SNVS_HPSR (*(volatile uint32_t *)(0x020cc014))
#define CSU_HP0 (*(volatile uint32_t *)(CSU_BASE+0x200))
#define CSU_HPCONTROL0 (*(volatile uint32_t *)(CSU_BASE+0x358))
#define CSU_CSL5 (*(volatile uint32_t *)(CSU_BASE+0x14))
#define CSU_CSL17 (*(volatile uint32_t *)(CSU_BASE+0x44))
#define CSU_CSL26 (*(volatile uint32_t *)(CSU_BASE+0x68))
#define IOMUXC_GPR10 (*(volatile uint32_t *)(0x020E0028))
#define SRC_SBMR2 (*(volatile uint32_t *)(0x020D801C))
#define BASE_ADDR 0x02100000
#define JR0MIDR_MS (*(volatile uint32_t *)(BASE_ADDR+0x00000010))
#define JR1MIDR_MS (*(volatile uint32_t *)(BASE_ADDR+0x00000018))

#define JRCFGR0_MS (*(volatile uint32_t *)(BASE_ADDR+0x00001050))
#define IRBAR0 (*(volatile uint32_t *)(BASE_ADDR+0x00001004))
#define IRSAR0 (*(volatile uint32_t *)(BASE_ADDR+0x00001014))
#define IRSR0 (*(volatile uint32_t *)(BASE_ADDR+0x0000100C))
#define IRJAR0 (*(volatile uint32_t *)(BASE_ADDR+0x0000101C))
#define JROSR0 (*(volatile uint32_t *)(BASE_ADDR+0x00001044))
#define ORBAR0 (*(volatile uint32_t *)(BASE_ADDR+0x00001024))
#define ORSR0 (*(volatile uint32_t *)(BASE_ADDR+0x0000102C))
#define ORSFR0 (*(volatile uint32_t *)(BASE_ADDR+0x0000103C))
#define JR0MIDR_LS (*(volatile uint32_t *)(BASE_ADDR+0x00000014))
#define JR1MIDR_MS (*(volatile uint32_t *)(BASE_ADDR+0x00000018))
#define JR1MIDR_LS (*(volatile uint32_t *)(BASE_ADDR+0x0000001C))
#define RDSTA (*(volatile uint32_t *)(BASE_ADDR+0x000006C0))
#define MCFGR (*(volatile uint32_t *)(BASE_ADDR+0x00000004))
#define CSTA (*(volatile uint32_t *)(BASE_ADDR+0x00000fd4))
#define DECORR (*(volatile uint32_t *)(BASE_ADDR+0x0000009C))
#define DECO0MID_MS (*(volatile uint32_t *)(BASE_ADDR+0x000000A0))
#define DECO0MID_LS (*(volatile uint32_t *)(BASE_ADDR+0x000000A4))
#define SNVS_LPLR (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000034))
#define SNVS_LPMKCR (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x0000003C))
#define SNVS_HPLR (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000000))
#define SNVS_HPCOMR (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000004))
#define SNVS_LPZMKR0 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x0000006c))
#define SNVS_LPZMKR1 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000070))
#define SNVS_LPZMKR2 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000074))
#define SNVS_LPZMKR3 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000078))
#define SNVS_LPZMKR4 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x0000007c))
#define SNVS_LPZMKR5 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000080))
#define SNVS_LPZMKR6 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000084))
#define SNVS_LPZMKR7 (*(volatile uint32_t *)(SNVS_BASE_ADDR+0x00000088))

#define WDOG1_WCR (*(volatile uint32_t *)(0x020BC000))
DECLARE_GLOBAL_DATA_PTR;

PmBootData* m48PmData = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;
char*       m48PmUsrData = NULL;

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
        PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
        PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_100K_UP |			\
        PAD_CTL_SPEED_LOW | PAD_CTL_DSE_60ohm |			\
        PAD_CTL_SRE_SLOW  | PAD_CTL_HYS)

#define ENET_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
        PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED | \
        PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)

#define I2C_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
        PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
        PAD_CTL_ODE | PAD_CTL_SRE_FAST)

#define I2C_PMIC	1

#define I2C_PAD MUX_PAD_CTRL(I2C_PAD_CTRL)

#define RST_POR     0x00001
#define RST_CSU     0x00004
#define RST_IPP     0x00008
#define RST_WDOG    0x00010
#define RST_JTAG_Z  0x00020
#define RST_JTAG_SW 0x00040
#define RST_WARM    0x10000

#define COLDRST_MASK (RST_WDOG|RST_WARM)

extern int ipu_set_ldb_clock(int rate);
extern u32 get_reset_cause_num(void);
extern int drv_video_init(void);
extern int fdt_fixup_memory(void *blob, u64 start, u64 size);


int dram_init(void)
{
    gd->ram_size = get_ram_size((void *)PHYS_SDRAM, PHYS_SDRAM_SIZE);
    return 0;
}

iomux_v3_cfg_t const uart1_pads[] = {
    IOMUX_PADS(PAD_SD3_DAT7__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
    IOMUX_PADS(PAD_SD3_DAT6__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
};

iomux_v3_cfg_t const enet_pads3[] = {
        IOMUX_PADS(PAD_KEY_COL1__ENET_MDIO        | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_ENET_MDC__ENET_MDC      | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_TXC__RGMII_TXC    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_TD0__RGMII_TD0    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_TD1__RGMII_TD1    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_TD2__RGMII_TD2    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_TD3__RGMII_TD3    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_TX_CTL__RGMII_TX_CTL  | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_ENET_REF_CLK__ENET_TX_CLK   | MUX_PAD_CTRL(ENET_PAD_CTRL)),

        IOMUX_PADS(PAD_RGMII_RXC__RGMII_RXC    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_RD0__RGMII_RD0    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_RD1__RGMII_RD1    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_RD2__RGMII_RD2    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_RD3__RGMII_RD3    | MUX_PAD_CTRL(ENET_PAD_CTRL)),
        IOMUX_PADS(PAD_RGMII_RX_CTL__RGMII_RX_CTL  | MUX_PAD_CTRL(ENET_PAD_CTRL)),
};

static void setup_iomux_enet(void)
{
    SETUP_IOMUX_PADS(enet_pads3);
}

iomux_v3_cfg_t const usdhc3_pads[] = {
        IOMUX_PADS(PAD_SD3_CLK__SD3_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD3_CMD__SD3_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
};

iomux_v3_cfg_t const usdhc4_pads[] = {
        IOMUX_PADS(PAD_SD4_CLK__SD4_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD4_CMD__SD4_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD4_DAT0__SD4_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD4_DAT1__SD4_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD4_DAT2__SD4_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
        IOMUX_PADS(PAD_SD4_DAT3__SD4_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
};

iomux_v3_cfg_t const ecspi1_pads[] = {
        IOMUX_PADS(PAD_KEY_COL0__ECSPI1_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL)),
        IOMUX_PADS(PAD_KEY_COL1__ECSPI1_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL)),
        IOMUX_PADS(PAD_KEY_ROW0__ECSPI1_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL)),
        IOMUX_PADS(PAD_KEY_ROW1__GPIO4_IO09 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

static struct i2c_pads_info mx6q_i2c_pad_info1 = {
     .scl = {
             .i2c_mode = MX6Q_PAD_KEY_COL3__I2C2_SCL | I2C_PAD,
             .gpio_mode = MX6Q_PAD_KEY_COL3__GPIO4_IO12 | I2C_PAD,
             .gp = IMX_GPIO_NR(4, 12)
     },
     .sda = {
             .i2c_mode = MX6Q_PAD_KEY_ROW3__I2C2_SDA | I2C_PAD,
             .gpio_mode = MX6Q_PAD_KEY_ROW3__GPIO4_IO13 | I2C_PAD,
             .gp = IMX_GPIO_NR(4, 13)
     }
};
static struct i2c_pads_info mx6dl_i2c_pad_info1 = {
     .scl = {
             .i2c_mode = MX6DL_PAD_KEY_COL3__I2C2_SCL | I2C_PAD,
             .gpio_mode = MX6DL_PAD_KEY_COL3__GPIO4_IO12 | I2C_PAD,
             .gp = IMX_GPIO_NR(4, 12)
     },
     .sda = {
             .i2c_mode = MX6DL_PAD_KEY_ROW3__I2C2_SDA | I2C_PAD,
             .gpio_mode = MX6DL_PAD_KEY_ROW3__GPIO4_IO13 | I2C_PAD,
             .gp = IMX_GPIO_NR(4, 13)
     }
};

iomux_v3_cfg_t const lvds_pads[] = {
        IOMUX_PADS(PAD_EIM_A22__GPIO2_IO16 | MUX_PAD_CTRL(NO_PAD_CTRL)), /* DISPLAY_EN */
        IOMUX_PADS(PAD_EIM_A20__GPIO2_IO18 | MUX_PAD_CTRL(NO_PAD_CTRL)), /* BACKLIGHT_EN */
        IOMUX_PADS(PAD_EIM_A18__GPIO2_IO20 | MUX_PAD_CTRL(NO_PAD_CTRL)), /* DISP0_CONTRAST */
};

iomux_v3_cfg_t const di0_pads[] = {
        IOMUX_PADS(PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK),    /* DISP0_CLK */
        IOMUX_PADS(PAD_DI0_PIN2__IPU1_DI0_PIN02),       /* DISP0_HSYNC */
        IOMUX_PADS(PAD_DI0_PIN3__IPU1_DI0_PIN03),       /* DISP0_VSYNC */
};

static void setup_iomux_uart(void)
{
    SETUP_IOMUX_PADS(uart1_pads);
}

#ifdef CONFIG_FSL_ESDHC
struct fsl_esdhc_cfg usdhc_cfg[CONFIG_SYS_FSL_USDHC_NUM] = {
        {USDHC3_BASE_ADDR},
        {USDHC4_BASE_ADDR},
};

#define USDHC3_CD_GPIO	IMX_GPIO_NR(7, 7) /*lufeng changed for M48 20150407*/

#ifndef CONFIG_SPL_BUILD

int board_mmc_getcd(struct mmc *mmc)
{
    struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
    int ret = 0;

    switch (cfg->esdhc_base) {
    case USDHC3_BASE_ADDR:
        ret = !gpio_get_value(USDHC3_CD_GPIO);
        break;
    case USDHC4_BASE_ADDR:
        ret = 1; /* eMMC/uSDHC4 is always present */
        break;
    }


    return ret;
}

int board_mmc_init(bd_t *bis)
{
    s32 status = 0;
    int i;

    /*
     * According to the board_mmc_init() the following map is done:
     * (U-boot device node)    (Physical Port)
     * mmc0                    SD3
     * mmc1                    SD4
     */
    for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
        switch (i) {
        case 0:
            SETUP_IOMUX_PADS( usdhc3_pads);
            gpio_direction_input(USDHC3_CD_GPIO);
            usdhc_cfg[i].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
            usdhc_cfg[i].max_bus_width = 4;
            break;
        case 1:
            SETUP_IOMUX_PADS(usdhc4_pads);
            usdhc_cfg[i].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);
            usdhc_cfg[i].max_bus_width = 4;
            break;
        default:
            printf("Warning: you configured more USDHC controllers"
                    "(%d) then supported by the board (%d)\n",
                    i + 1, CONFIG_SYS_FSL_USDHC_NUM);
            return status;
        }

        status |= fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
    }

    return status;
}
#endif

#endif


int board_phy_config(struct phy_device *phydev)
{
    /* mx6_rgmii_rework(phydev); */

    /* devadd=0x02 and mode=MII_KSZ9031_MOD_DATA_NO_POST_INC are explained in KSZ9031 manual */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_CTRL_SIG_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x0);
    /* min rx data delay */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_RX_DATA_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x0);
    /* min tx data delay */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_TX_DATA_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x0);
    /* max rx/tx clock delay, min rx/tx control */
    ksz9031_phy_extended_write(phydev, 0x02, MII_KSZ9031_EXT_RGMII_CLOCK_SKEW, MII_KSZ9031_MOD_DATA_NO_POST_INC, 0x03F7);

    if (phydev->drv->config)
        phydev->drv->config(phydev);

    return 0;
}

#if defined(CONFIG_VIDEO_IPUV3)

#define PLL5_MAX_DIV 54
#define PLL5_MIN_DIV 27
void enable_pll5 (int pixclock)
{
    struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *) CCM_BASE_ADDR;
    int reg;
    int clkrate;
    int timeout;
    int video_div = 43;
    int video_num = 983000;
    int vid_denom = 1000000;
    int video_post_div = 0;
    int i;

    clkrate = PICOS2KHZ(pixclock); /* rate in kHz */

    for (i = 1; i <= 4; i *= 2) {
        /* VCO=24 000 kHz ldb_di0_ipu_div=7 */
        video_div = clkrate * 7 * i / 24000;
        if (video_div >= PLL5_MIN_DIV && video_div <= PLL5_MAX_DIV) {
            video_post_div = i;
            break;
        }
    }

    if (video_post_div <= 0) return;

    video_num= (((clkrate * 7 * video_div) - (video_div * 24000)) * 1000000) / 24000000;
    video_post_div = 0b100 >> video_post_div; /* :1->10, :2->01, :4->00  8-)  */

    /* set PLL5 clock */
    reg = readl(&ccm->analog_pll_video);
    reg |= BM_ANADIG_PLL_VIDEO_POWERDOWN;
    writel(reg, &ccm->analog_pll_video);

    /* set PLL5 clkrate */
    reg &= ~BM_ANADIG_PLL_VIDEO_DIV_SELECT;
    reg |= BF_ANADIG_PLL_VIDEO_DIV_SELECT(video_div);
    reg &= ~BM_ANADIG_PLL_VIDEO_POST_DIV_SELECT;
    reg |= BF_ANADIG_PLL_VIDEO_POST_DIV_SELECT(video_post_div);
    writel(reg, &ccm->analog_pll_video);

    writel(BF_ANADIG_PLL_VIDEO_NUM_A(video_num),
            &ccm->analog_pll_video_num);
    writel(BF_ANADIG_PLL_VIDEO_DENOM_B(vid_denom),
            &ccm->analog_pll_video_denom);

    reg &= ~BM_ANADIG_PLL_VIDEO_POWERDOWN;
    writel(reg, &ccm->analog_pll_video);

    /* --> ERRATA LDB Clock Switch Procedure & i.MX6 Asynchronous Clock Switching Guidelines, Rev. 0, 06/2016 */

    /* disable the MMDC_CH1 handshake */
    reg = readl(&ccm->ccdr);
    reg |= MXC_CCM_CCDR_MMDC_CH1_HS_MASK;
    writel(reg, &ccm->ccdr);

    /* Set periph2_clk2_sel to be sourced from pll3_sw_clk */
    reg = readl(&ccm->cbcmr);
    reg |= MXC_CCM_CBCMR_PERIPH2_CLK2_SEL;
    writel(reg, &ccm->cbcmr);

    /* Switch periph2_clk_sel to periph2_clk2 clock source */
    reg = readl(&ccm->cbcdr);
    reg |= MXC_CCM_CBCDR_PERIPH2_CLK_SEL;
    writel(reg, &ccm->cbcdr);

    /* Wait for the periph2_clk_sel_busy bit to clear */
    timeout = 100000;
    while (timeout--)
        if ( ! (readl(&ccm->cdhipr) & MXC_CCM_CDHIPR_PERIPH2_CLK_SEL_BUSY))
            break;
    if (timeout < 0)
        printf("Warning: periph2 clk sel timeout!\n");

    /* switch pll3_sw_clk to bypass source */
    reg = readl(&ccm->ccsr);
    reg |= MXC_CCM_CCSR_PLL3_SW_CLK_SEL;
    writel(reg, &ccm->ccsr);

    /* set LDB0, LDB1 clk select to 000/000 (PLL5 clock) */
    reg = readl(&ccm->cs2cdr);
    reg |= (4 << MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET) | (4 << MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET);
    writel(reg, &ccm->cs2cdr);
    reg &= ~(MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK | MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK);
    /* this code is only valid if multiplexer channel 0 is selected.
     * please review above stated application note for further information
     */
    reg |= (4 << MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET) | (4 << MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET);
    writel(reg, &ccm->cs2cdr);
    reg &= ~(MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK | MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK);
    /* reg |= (0 << MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET) | (0 << MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET); */
    writel(reg, &ccm->cs2cdr);

    reg = readl(&ccm->cscmr2);
    reg |= MXC_CCM_CSCMR2_LDB_DI0_IPU_DIV;
    writel(reg, &ccm->cscmr2);

    reg = readl(&ccm->chsccdr);
    reg |= (CHSCCDR_CLK_SEL_LDB_DI0
            << MXC_CCM_CHSCCDR_IPU1_DI0_CLK_SEL_OFFSET);
    writel(reg, &ccm->chsccdr);


    /* Switch periph2_clk_sel back to pll2_main_clk */
    reg = readl(&ccm->cbcdr);
    reg &= ~MXC_CCM_CBCDR_PERIPH2_CLK_SEL;
    writel(reg, &ccm->cbcdr);
    /* wait for the periph2_clk_sel_busy bit to clear */
    timeout = 100000;
    while (timeout--)
        if ( ! (readl(&ccm->cdhipr) & MXC_CCM_CDHIPR_PERIPH2_CLK_SEL_BUSY))
            break;
    if (timeout < 0)
        printf("Warning: periph2 clk sel back timeout!\n");

    /* Set periph2_clk2_sel to be sourced from PLL2 */
    reg = readl(&ccm->cbcmr);
    reg &= ~MXC_CCM_CBCMR_PERIPH2_CLK2_SEL;
    writel(reg, &ccm->cbcmr);

    /* switch pll3_sw_clk back from bypass source */
    reg = readl(&ccm->ccsr);
    reg &= ~MXC_CCM_CCSR_PLL3_SW_CLK_SEL;
    writel(reg, &ccm->ccsr);

    /* Enable MMDC_CH1 handshake */
    reg = readl(&ccm->ccdr);
    reg &= ~MXC_CCM_CCDR_MMDC_CH1_HS_MASK;
    writel(reg, &ccm->ccdr);

   /* <-- END ERRATA LDB Clock Switch Procedure & i.MX6 Asynchronous Clock Switching Guidelines, Rev. 0, 06/2016 */


    timeout = 100000;
    while (timeout--)
        if (readl(&ccm->analog_pll_video) & BM_ANADIG_PLL_VIDEO_LOCK)
            break;
    if (timeout < 0)
        printf("Warning: video pll lock timeout!\n");

    reg = readl(&ccm->analog_pll_video);
    reg |= BM_ANADIG_PLL_VIDEO_ENABLE;
    reg &= ~BM_ANADIG_PLL_VIDEO_BYPASS;
    writel(reg, &ccm->analog_pll_video);

    clkrate *= 1000;
    ipu_set_ldb_clock(clkrate);

    enable_ipu_clock();

    /* Turn on LDB0, LDB1, IPU,IPU DI0 clocks */
    reg = readl(&ccm->CCGR3);
    reg |=  MXC_CCM_CCGR3_LDB_DI0_MASK | MXC_CCM_CCGR3_LDB_DI1_MASK;
    writel(reg, &ccm->CCGR3);


}


static void disable_lvds(struct display_info_t const *dev)
{
    struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

    int reg = readl(&iomux->gpr[2]);

    reg &= ~(IOMUXC_GPR2_LVDS_CH0_MODE_MASK |
            IOMUXC_GPR2_LVDS_CH1_MODE_MASK);

    writel(reg, &iomux->gpr[2]);
}

static void do_enable_hdmi(struct display_info_t const *dev)
{
    disable_lvds(dev);
    imx_setup_hdmi();
    imx_enable_hdmi_phy();
}

static void enable_lvds(struct display_info_t const *dev)
{
    struct iomuxc *iomux = (struct iomuxc *) IOMUXC_BASE_ADDR;
    u32 reg = readl(&iomux->gpr[2]);
    reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
            | IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
            | IOMUXC_GPR2_LVDS_CH1_MODE_DISABLED
            | IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI0;
    if (dev->pixfmt == IPU_PIX_FMT_RGB24) {
        reg |= IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT;
    }
    writel(reg, &iomux->gpr[2]);
    reg = readl(&iomux->gpr[3]);
    reg = (reg & ~IOMUXC_GPR3_LVDS0_MUX_CTL_MASK)
                               | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
                                       << IOMUXC_GPR3_LVDS0_MUX_CTL_OFFSET);
    writel(reg, &iomux->gpr[3]);

    SETUP_IOMUX_PADS(lvds_pads);

    /* DISPLAY_EN */
    gpio_direction_output(IMX_GPIO_NR(2, 16), 1);
}

struct display_info_t const displays[] = {
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_LVDS666,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "1024x768-24@60",
                        .refresh        = 60,
                        .xres           = 1024,
                        .yres           = 768,   /* Timing: All values in pixclocks, except pixclock (of course) */
                        .pixclock       = 15385, /* pixel clock in ps (pico seconds) */
                        .left_margin    = 220,   /* time from sync to picture        */
                        .right_margin   = 40,    /* time from picture to sync        */
                        .upper_margin   = 21,    /* time from sync to picture        */
                        .lower_margin   = 7,     /* length of horizontal sync        */
                        .hsync_len      = 60,    /* length of vertical sync          */
                        .vsync_len      = 10,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_RGB24,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "1920x1080-32@60",
                        .refresh        = 60,
                        .xres           = 1920,
                        .yres           = 1080,
                        .pixclock       = 13351,
                        .left_margin    = 30,
                        .right_margin   = 30,
                        .upper_margin   = 10,
                        .lower_margin   =  3,
                        .hsync_len      = 30,
                        .vsync_len      = 37,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_RGB24,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "1920x1080-32@50",
                        .refresh        = 50,
                        .xres           = 1920,
                        .yres           = 1080,
                        .pixclock       = 16026,
                        .left_margin    = 30,
                        .right_margin   = 30,
                        .upper_margin   = 10,
                        .lower_margin   =  3,
                        .hsync_len      = 30,
                        .vsync_len      = 37,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_RGB24,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "1366x768-32@60",
                        .refresh        = 60,
                        .xres           = 1376,
                        .yres           = 768,
                        .pixclock       = 13265,
                        .left_margin    = 70,
                        .right_margin   = 70,
                        .upper_margin   = 10,
                        .lower_margin   =  3,
                        .hsync_len      = 54,
                        .vsync_len      = 25,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_RGB24,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "1366x768-32@50",
                        .refresh        = 50,
                        .xres           = 1376,
                        .yres           = 768,
                        .pixclock       = 15915,
                        .left_margin    = 70,
                        .right_margin   = 70,
                        .upper_margin   = 10,
                        .lower_margin   = 3,
                        .hsync_len      = 54,
                        .vsync_len      = 25,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_RGB24,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "1024x768-32@60",
                        .refresh        = 60,
                        .xres           = 1024,
                        .yres           = 768,
                        .pixclock       = 15385,
                        .left_margin    = 90,
                        .right_margin   = 90,
                        .upper_margin   = 29,
                        .lower_margin   = 3,
                        .hsync_len      = 144,
                        .vsync_len      = 6,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_LVDS666,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "1280x800-24@60",
                        .refresh        = 60,
                        .xres           = 1280,
                        .yres           = 800,
                        .pixclock       = 14084,
                        .left_margin    = 40,
                        .right_margin   = 40,
                        .upper_margin   = 3,
                        .lower_margin   = 5,
                        .hsync_len      = 80,
                        .vsync_len      = 5,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_RGB24,
                .detect = NULL,
                .enable = do_enable_hdmi,
                .mode   = {
                        .name           = "800x600-32@60",
                        .refresh        = 60,
                        .xres           = 800,
                        .yres           = 600,
                        .pixclock       = 25000,
                        .left_margin    = 90,
                        .right_margin   = 90,
                        .upper_margin   = 10,
                        .lower_margin   = 10,
                        .hsync_len      = 76,
                        .vsync_len      = 9,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus    = -1,
                .addr   = 0,
                .pixfmt = IPU_PIX_FMT_LVDS666,
                .detect = NULL,
                .enable = enable_lvds,
                .mode   = {
                        .name           = "640x480-24@60",
                        .refresh        = 60,
                        .xres           = 640,
                        .yres           = 480,
                        .pixclock       = 39682,
                        .left_margin    = 80,
                        .right_margin   = 80,
                        .upper_margin   = 21,
                        .lower_margin   = 7,
                        .hsync_len      = 60,
                        .vsync_len      = 10,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        },
        {
                .bus	= -1,
                .addr	= 0,
                .pixfmt	= IPU_PIX_FMT_RGB24,
                /* .detect	= detect_hdmi, */
                .detect	= NULL,
                .enable	= do_enable_hdmi,
                .mode	= {
                        .name           = "HDMI",
                        .refresh        = 60,
                        .xres           = 1024,
                        .yres           = 768,
                        .pixclock       = 15385,
                        .left_margin    = 220,
                        .right_margin   = 40,
                        .upper_margin   = 21,
                        .lower_margin   = 7,
                        .hsync_len      = 60,
                        .vsync_len      = 10,
                        .sync           = FB_SYNC_EXT,
                        .vmode          = FB_VMODE_NONINTERLACED
                }
        }
};

size_t display_count = 0;

static void setup_display(void)
{
    struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
    int reg;

    /* Setup HSYNC, VSYNC, DISP_CLK for debugging purposes */
    SETUP_IOMUX_PADS(di0_pads);

    reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
            | IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_LOW
            | IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
            | IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
            | IOMUXC_GPR2_DATA_WIDTH_CH1_24BIT
            | IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
            | IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT
            | IOMUXC_GPR2_LVDS_CH1_MODE_DISABLED
            | IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI0;
    writel(reg, &iomux->gpr[2]);

    reg = readl(&iomux->gpr[3]);
    reg = (reg & ~(IOMUXC_GPR3_LVDS1_MUX_CTL_MASK
            | IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
                | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
                        << IOMUXC_GPR3_LVDS1_MUX_CTL_OFFSET);
    writel(reg, &iomux->gpr[3]);
}
#endif /* CONFIG_VIDEO_IPUV3 */

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
    return 1;
}

/* int board_eth_init(bd_t *bis) */
/* { */
/*	setup_iomux_enet(); */
/*	setup_pcie(); */

/*	return cpu_eth_init(bis); */
/* } */

int board_eth_init(bd_t *bis)
{

    uint32_t base = IMX_FEC_BASE;
    struct mii_dev *bus = NULL;
    struct phy_device *phydev = NULL;
    int ret;

    setup_iomux_enet();

#ifdef CONFIG_FEC_MXC
    bus = fec_get_miibus(base, -1);
    if (!bus)
        return 0;
    /* scan phy 0,1,2,3,4,5,6,7 */
    phydev = phy_find_by_mask(bus, (0xf << 0), PHY_INTERFACE_MODE_RGMII);
    if (!phydev) {
        free(bus);
        return 0;
    }
    /*
    printf("using phy at %d\n", phydev->addr);
    printf("\nphy info:\n speed: %d\n duplex: %d\n link: %d\n port: %d\n advertising: %u\n supported: %u\n mmds: %u\n autoneg: %d\n addr: %d\n pause: %d\n asym_pause: %d\n phy_id: %u\n flags: %u\n",
            phydev->speed,
            phydev->duplex,
            phydev->link,
            phydev->port,
            phydev->advertising,
            phydev->supported,
            phydev->mmds,
            phydev->autoneg,
            phydev->addr,
            phydev->pause,
            phydev->asym_pause,
            phydev->phy_id,
            phydev->flags);

    printf("\nphy driver info:\n name: %s\n uid: %u\n mask: %u\n mmds: %u\n\n",phydev->drv->name, phydev->drv->uid, phydev->drv->mask, phydev->drv->mmds);
     */

    ret  = fec_probe(bis, -1, base, bus, phydev);
    if (ret) {
        printf("FEC MXC: %s:failed\n", __func__);
        free(phydev);
        free(bus);
    }
#endif

    return 0;
}

int board_early_init_f(void)
{

    if (gpio_get_value(CONFIG_GPIO_DEBUG)) gd->flags |= (GD_FLG_SILENT | GD_FLG_DISABLE_CONSOLE);

    setup_iomux_uart();
    return 0;
}

int board_init(void)
{
    int post_boot_mode;
    u32 cause;
    char *vers;
    const char *token = "-draeger_";
    vers = strstr(U_BOOT_VERSION, token);
    vers +=1;
    m48PmUsrData = (char*)      (gd->ram_size + PHYS_SDRAM - 0x00A02000);

    snprintf((char*) m48PmData->uboot_version, sizeof(m48PmData->uboot_version), "%s", vers);
    updateM48PmStructChecksum();

    /* address of boot parameters */
    gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_MXC_SPI
    setup_spi();
#endif

#define SLAVE_ADDR 0x7f /* does not matter, parameter unused in mxc_i2c driver */
    if (is_cpu_type(MXC_CPU_MX6Q)) {
        setup_i2c(1, CONFIG_SYS_I2C_SPEED, SLAVE_ADDR, &mx6q_i2c_pad_info1);
    }
    else {
        setup_i2c(1, CONFIG_SYS_I2C_SPEED, SLAVE_ADDR, &mx6dl_i2c_pad_info1);
    }

    /* currently not connected: */
    /* check pinmux before use! */
    /* setup_i2c(0, CONFIG_SYS_I2C_SPEED, SLAVE_ADDR, &i2c_pad_info2); */
    /* setup_i2c(2, CONFIG_SYS_I2C_SPEED, SLAVE_ADDR, &i2c_pad_info3); */
    /* 4 i2c busses currently not supported by mxc_i2c driver in this uboot?! */

    post_boot_mode = post_bootmode_get(0);
    if (post_boot_mode == 0	|| (post_boot_mode & POST_POWERON)) {
        m48PmData->startType = M48_START_TYPE_POWERUP;
    } else {
        cause = get_reset_cause_num();

        if (cause & (RST_WARM | RST_WDOG | RST_JTAG_SW)) {
            m48PmData->startType = M48_START_TYPE_WARM_REBOOT;
        } else {
            m48PmData->startType = M48_START_TYPE_COLD_REBOOT;
        }
    }
    updateM48PmStructChecksum();

    if (gd->flags & GD_FLG_DISABLE_CONSOLE) m48PmData->verboseBoot = 0;
    else                                    m48PmData->verboseBoot = 1;

    return 0;
}

static int pfuze_init(void)
{
    struct pmic *p;
    int ret;
    unsigned int reg;

    ret = power_pfuze100_init(I2C_PMIC);
    if (ret)
        return ret;

    p = pmic_get("PFUZE100_PMIC");
    ret = pmic_probe(p);
    if (ret)
        return ret;

    pmic_reg_read(p, PFUZE100_DEVICEID, &reg);
    printf("PMIC:  PFUZE100 ID=0x%02x\n", reg);

    /* Increase VGEN3 from 2.5 to 2.8V */
    pmic_reg_read(p, PFUZE100_VGEN3VOL, &reg);
    reg &= ~0xf;
    reg |= 0xa;
    pmic_reg_write(p, PFUZE100_VGEN3VOL, reg);

    /* Increase VGEN5 from 2.8 to 3V */
    pmic_reg_read(p, PFUZE100_VGEN5VOL, &reg);
    reg &= ~0xf;
    reg |= 0xc;
    pmic_reg_write(p, PFUZE100_VGEN5VOL, reg);

    /* Set SW1AB stanby volage to 0.975V */
    pmic_reg_read(p, PFUZE100_SW1ABSTBY, &reg);
    reg &= ~0x3f;
    reg |= 0x1b;
    pmic_reg_write(p, PFUZE100_SW1ABSTBY, reg);

    /* Set SW1AB/VDDARM step ramp up time from 16us to 4us/25mV */
    pmic_reg_read(p, PUZE_100_SW1ABCONF, &reg);
    reg &= ~0xc0;
    reg |= 0x40;
    pmic_reg_write(p, PUZE_100_SW1ABCONF, reg);

    /* Set SW1C standby voltage to 0.975V */
    pmic_reg_read(p, PFUZE100_SW1CSTBY, &reg);
    reg &= ~0x3f;
    reg |= 0x1b;
    pmic_reg_write(p, PFUZE100_SW1CSTBY, reg);

    /* Set SW1C/VDDSOC step ramp up time from 16us to 4us/25mV */
    pmic_reg_read(p, PFUZE100_SW1CCONF, &reg);
    reg &= ~0xc0;
    reg |= 0x40;
    pmic_reg_write(p, PFUZE100_SW1CCONF, reg);

    return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
        /* 4 bit bus width */
        {"sd2",	 MAKE_CFGVAL(0x40, 0x28, 0x00, 0x00)},
        {"sd3",	 MAKE_CFGVAL(0x40, 0x30, 0x00, 0x00)},
        /* 8 bit bus width */
        {"emmc", MAKE_CFGVAL(0x40, 0x38, 0x00, 0x00)},
        {NULL,	 0},
};
#endif

int board_late_init(void)
{
#ifdef CONFIG_CMD_BMODE
    add_board_boot_modes(board_boot_modes);
#endif
    /* Enable CAAM clocks */
    *((volatile uint32_t *) CCM_CCGR0) |=(1<<13)|(1<<12)|(1<<11)|(1<<10)|(1<<9)|(1<<8);

    pfuze_init();
    /* Write master priviledge registers to enable read and write into CAAM */
    AIPSTZ2_MPR|=(1<<23)|(1<<22)|(1<<21)|(1<<31)|(1<<30)|(1<<29)|(1<<27)|(1<<26)|(1<<25)|(1<<20)|(1<<28);
    AIPSTZ1_MPR|=(1<<23)|(1<<22)|(1<<21)|(1<<31)|(1<<30)|(1<<29)|(1<<27)|(1<<26)|(1<<25)|(1<<20)|(1<<28);
    AIPSTZ1_MPR&=~(1<<20);
    AIPSTZ2_MPR&=~(1<<20);
    AIPSTZ2_OPACR=0x88888888;
    AIPSTZ2_OPACR1=0x88888888;
    AIPSTZ2_OPACR2=0x88888888;
    AIPSTZ2_OPACR3=0x88888888;
    AIPSTZ2_OPACR4=0x88888888;
    AIPSTZ1_OPACR=0x88888888;
    AIPSTZ1_OPACR1=0x88888888;
    AIPSTZ1_OPACR2=0x88888888;
    AIPSTZ1_OPACR3=0x88888888;
    AIPSTZ1_OPACR4=0x88888888;

    /* Config security level register -  permission access*/
    CSU_CSL5&=~(1<<24);
    CSU_CSL5|=(1<<23)|(1<<22)|(1<<21)|(1<<20)|(1<<19)|(1<<18)|(1<<17)|(1<<16);
    CSU_CSL5|=(1<<24);
    CSU_CSL17&=~(1<<24);
    CSU_CSL17|=(1<<23)|(1<<22)|(1<<21)|(1<<20)|(1<<19)|(1<<18)|(1<<17)|(1<<16);
    CSU_CSL17|=(1<<24);
    CSU_CSL26&=~(1<<24);
    CSU_CSL26|=(1<<23)|(1<<22)|(1<<21)|(1<<20)|(1<<19)|(1<<18)|(1<<17)|(1<<16);
    CSU_CSL26|=(1<<24);

    CSU_HP0&=~(1<<15);
    CSU_HP0|=(1<<14);
    CSU_HP0|=(1<<15);
    CSU_HPCONTROL0&=~(1<<15);
    CSU_HPCONTROL0|=(1<<14);
    CSU_HPCONTROL0|=(1<<15);

    /* Job ring MID register. Specify MID value CAAm DMA asserts when reading or writing memory */
    JR0MIDR_MS|=(1<<16)|(1<<0);
    JR0MIDR_MS&=~(1<<2)&~(1<<3)&~(1<<1);
    JR0MIDR_LS&=~(1<<0)&~(1<<2)&~(1<<3)&~(1<<19)&~(1<<18)&~(1<<16)&~(1<<17);
    JR0MIDR_MS|=(1<<31)|(1<<16);


    JR1MIDR_MS|=(1<<16)|(1<<0);
    JR1MIDR_MS&=~~(1<<2)&~(1<<3)&~(1<<1);
    JR1MIDR_LS&=~(1<<0)&~(1<<2)&~(1<<3)&~(1<<19)&~(1<<18)&~(1<<17);
    JR1MIDR_MS|=(1<<31)|(1<<16);



    return 0;
}

int	last_stage_init(void)
{

#if defined(CONFIG_VIDEO_IPUV3)

    display_count = ARRAY_SIZE(displays);
    setup_display();
    drv_video_init();

#endif

    if (is_cpu_type(MXC_CPU_MX6Q)) {
        setenv("fdt_name", CONFIG_DEFAULT_FDT_FILE_MX6Q);
    } else {
        setenv("fdt_name", CONFIG_DEFAULT_FDT_FILE_MX6DL);
    }

    return 0;
}

int checkboard(void)
{
    u32 cause = get_reset_cause_num();
    int post_boot_mode;

    printf("RESET: %08x\n", cause);
    if (cause & RST_POR)     puts("RESET: POWER_UP - IPP_RESET_B\n");
    if (cause & RST_CSU)     puts("RESET: COLD     - CSU_RESET_B\n");
    if (cause & RST_WDOG)    puts("RESET: WDOG     - WDOG_RST_B\n");
    if (cause & RST_JTAG_Z)  puts("RESET: JTAG     - JTAG_RST_B\n");
    if (cause & RST_JTAG_SW) puts("RESET: JTAG-SW  - JTAG_SW_RST\n");
    if (cause & RST_WARM)    puts("RESET: WARM     - WARM_BOOT\n");

    if (cause & RST_IPP) puts("RESET: WARM - IPP_USER_RESET\n");

    if (!(cause & (RST_IPP | RST_POR | RST_WARM )))
    {
        post_boot_mode = post_bootmode_get(0);
        /* do not reset the other controller when in POST */
        if ( post_boot_mode != 0 && ! (post_boot_mode & POST_POWERTEST)) {

            printf("reseting uP2 %x\n", post_boot_mode);
            gpio_direction_output(CONFIG_GPIO_BOARD_COLD_RESET, 0);
            udelay(500);
            gpio_direction_output(CONFIG_GPIO_BOARD_COLD_RESET, 1);

        } else {
            printf("No uP2 reset due to POST %x\n", post_boot_mode);
        }
    }

    return 0;
}

void updateM48PmStructChecksum(void)
{
    PmBootData* data;
    data = (PmBootData*) CONFIG_SYS_PMSTRUCT_ADDR;
    data->data_size=sizeof(PmBootData)-offsetof(PmBootData, data_size);
    data->checkSum = crc32 (0, (const void *) &data->data_size, data->data_size);
    flush_cache((unsigned long) data, sizeof(PmBootData));
}

ulong post_word_load(void)
{
    return m48PmData->magicConstant;
}

void post_word_store(ulong value)
{
    m48PmData->magicConstant = value;
    updateM48PmStructChecksum();
}

void ft_board_setup(void *blob, bd_t *bd)
{
    fdt_fixup_memory(blob, (u64)PHYS_SDRAM, (u64)gd->ram_size);
}

#if defined(CONFIG_SYS_I2C_MAC_OFFSET)
int mac_read_from_eeprom(void)
{
    uchar buf[28];
    char str[18];
    unsigned int crc = 0;

    i2c_set_bus_num(1);

    /* Read MAC addresses from EEPROM */
    if (eeprom_read(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_MAC_OFFSET, buf, 28))
    {
        printf("\nEEPROM @ 0x%02x read FAILED!!!\n",
                CONFIG_SYS_I2C_EEPROM_ADDR);
    }
    else
    {
        uint32_t crc_buf;

        memcpy(&crc_buf, &buf[24], sizeof(uint32_t));


        if (crc32(crc, buf, 24) == crc_buf)
        {
            printf("Reading MAC from EEPROM\n");
            memset(str, 0xFF, 6);
            if (memcmp(buf, str, 6))
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
            printf("EEPROM MAC CRC failed\n");
        }
    }

    return 0;
}

static int do_setmacaddr (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int offset = 0;
    uchar buf[28];
    unsigned char str[18];

    if (argc != 3)
        return -1;

    if ((offset = simple_strtoul(argv[1], NULL, 10)) > 3)
    {
        printf ("MAC offset %d too high (valid values 0 -3)\n", offset);
        return 0;
    }

    eth_parse_enetaddr (argv[2], str);

    i2c_set_bus_num(1);

    /* Read MAC addresses from EEPROM */
    if (eeprom_read(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_MAC_OFFSET, buf, 28))
    {
        printf("\nEEPROM @ 0x%02x read FAILED!!!\n",
                CONFIG_SYS_I2C_EEPROM_ADDR);
    }
    else
    {
        uint32_t crc_buf;

        memcpy(&crc_buf, &buf[24], sizeof(uint32_t));
        if (crc32(0, buf, 24) != crc_buf)
        {
            printf ("The EEPROM MAC area is corrupted...erasing\n");
            memset (buf, 0, 28);
        }

        memcpy ((void *)(buf + 6 * offset), (void *)str, 6);
        crc_buf = crc32(0, buf, 24);
        memcpy ((void *)(buf + 24), (void *)(&crc_buf), sizeof(uint32_t));

        if (eeprom_write(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_MAC_OFFSET, buf, 28))
        {
            printf("\nEEPROM @ 0x%02x write FAILED!!!\n",
                    CONFIG_SYS_I2C_EEPROM_ADDR);
        }
        else
        {
            printf("MAC %s was written at offset %d\n", argv[2], offset);
        }
    }

    return 0;
}

U_BOOT_CMD(
        setmacaddr,	3,	1,	do_setmacaddr,
        "Set one of the 4 MAC addresses in EEPROM.",
        "Set one of the 4 MAC addresses in EEPROM. \n"
        "Only 4 MAC adresses can be written in EEPROM \n"
        "The one at offset 0 will be used as the interface MAC \n"
        "setmacaddr offset value"
);
#endif				/* CONFIG_I2C_MAC_OFFSET */

static int boardrev (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    uint16_t ver;

    i2c_set_bus_num(1);
    if (eeprom_read(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_BOARD_REV_OFFSET, (uchar *) &ver, 2))
    {
        printf("\nEEPROM @ 0x%02x write FAILED!!!\n",
                CONFIG_SYS_I2C_EEPROM_ADDR + CONFIG_SYS_I2C_BOARD_REV_OFFSET);
        return 0;
    }

    printf ("Board revision %d.%d\n", *((char *)(&ver)), *((char *)(&ver) + 1));

    return 0;
}

static int setboardrev (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned char ver;

    if (argc != 3)
        return -1;

    if (simple_strtoul(argv[1], NULL, 10) > 0xff || simple_strtoul(argv[2], NULL, 10) > 0xff)
    {
        printf ("Major or Minor numbers must be lower than 255\n");
        return 0;
    }

    ver = (unsigned char)simple_strtoul(argv[1], NULL, 10);
    i2c_set_bus_num(1);
    if (eeprom_write(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_BOARD_REV_OFFSET, &ver, 1))
    {
        printf("\nEEPROM @ 0x%02x write FAILED!!!\n",
                CONFIG_SYS_I2C_EEPROM_ADDR + CONFIG_SYS_I2C_BOARD_REV_OFFSET);
        return 0;
    }

    ver = (unsigned char)simple_strtoul(argv[2], NULL, 10);
    if (eeprom_write(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_BOARD_REV_OFFSET + 1, &ver, 1))
    {
        printf("\nEEPROM @ 0x%02x write FAILED!!!\n",
                CONFIG_SYS_I2C_EEPROM_ADDR + CONFIG_SYS_I2C_BOARD_REV_OFFSET + 1);
        return 0;
    }

    return 0;
}

static int do_halt (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    disable_interrupts();
    asm("	wfi");
    return 0;
}

U_BOOT_CMD(
        boardrev,	1,	1,	boardrev,
        "Board Revision Number.",
        "Board Revision Number. \n"
        "boardrev"
);

U_BOOT_CMD(
        setboardrev,	3,	1,	setboardrev,
        "Set Board Revision Number.",
        "Set Board Revision Number. \n"
        "setboardrev major minor"
);

U_BOOT_CMD(
        halt,	1,	1,	do_halt,
        "Halt the processor\n",
        ""
);
