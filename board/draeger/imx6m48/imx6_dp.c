/*
 *  (C) Copyright 2020 Draeger and Licensors, info@draeger.com
 *
 *  SPDX-License-Identifier: GPL-2.0+
 */

#include <log.h>
#include <display.h>
#include <video.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <ipu_pixfmt.h>
#include <env.h>

#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/clock.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/gpio.h>

#include <asm/mach-imx/video.h>
#include <asm/mach-imx/gpio.h>


struct imx6_dp_priv {
	u32 di_no;
	s32 timing_no;
	u32 pixfmt;
	struct display_timing timing;
};

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

    debug("M48 enable_pll5 %d kHz\n", clkrate);

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
    debug("24000 * (video_div(%d) + video_num(%d) / vid_denom(%d) / video_post_div(%d)) / 7 = %d kHz\n",
        video_div, video_num, vid_denom, video_post_div,
        (24000 * (video_div +  video_num / vid_denom) / video_post_div / 7));

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

static void setup_display(void)
{
    struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
    int reg;

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

static void disable_lvds(struct display_info_t const *dev)
{
    struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

    int reg = readl(&iomux->gpr[2]);

    reg &= ~(IOMUXC_GPR2_LVDS_CH0_MODE_MASK |
            IOMUXC_GPR2_LVDS_CH1_MODE_MASK);

    writel(reg, &iomux->gpr[2]);
}

static void enable_hdmi(struct display_info_t const *dev)
{
	debug("M48 enable_hdmi\n");

	setup_display();
	disable_lvds(dev);
    imx_setup_hdmi();
    imx_enable_hdmi_phy();
	enable_pll5 (dev->mode.pixclock);
}

static void enable_lvds(struct display_info_t const *dev)
{
    struct iomuxc *iomux = (struct iomuxc *) IOMUXC_BASE_ADDR;

	debug("M48 enable_lvds\n");

	enable_pll5 (dev->mode.pixclock);
	setup_display();

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

	/* Enable the DISPLAY_EN, active high */
	gpio_request(IMX_GPIO_NR(2, 16), "display_en");
    gpio_direction_output(IMX_GPIO_NR(2, 16), 1);

}

static int imx6_dp_probe(struct udevice *dev)
{
	debug("M48 imx6_dp_probe\n");
	return 0;
}

int imx6_dp_read_timing(struct udevice *dev, struct display_timing *timing)
{
	struct imx6_dp_priv *priv = dev_get_priv(dev);
	memcpy(timing, &priv->timing, sizeof(struct display_timing));
	return 0;
}

int imx6_dp_enable (struct udevice *dev, int panel_bpp,
    const struct display_timing *timing)
{
    struct imx6_dp_priv *priv = dev_get_priv(dev);
    int ret;

    /* needs to be static because it's used later for ipu init */
    static struct display_info_t display_info =
    {
     .bus    = -1,
     .addr   = 0,
     .detect = NULL,
     .mode   =
     {
      .sync     = FB_SYNC_EXT,
      .vmode    = 0,
      .flag     = 0,
     }
    };

    if (priv->timing_no < 0) {
        return -ENODEV;
    }

    /* gmode.name;	optional */
    /* gmode.refresh;  optional */

    display_info.mode.xres = priv->timing.hactive.typ;
    display_info.mode.yres = priv->timing.vactive.typ;
    display_info.mode.pixclock = KHZ2PICOS(priv->timing.pixelclock.typ / 1000); /* pixclock in Hz -> wavelen in ps */
    display_info.mode.left_margin = priv->timing.hfront_porch.typ;
    display_info.mode.right_margin = priv->timing.hback_porch.typ;
    display_info.mode.upper_margin = priv->timing.vfront_porch.typ;
    display_info.mode.lower_margin = priv->timing.vback_porch.typ;
    display_info.mode.hsync_len = priv->timing.hsync_len.typ;
    display_info.mode.vsync_len = priv->timing.vsync_len.typ;
    display_info.mode.sync |= (priv->timing.flags & DISPLAY_FLAGS_HSYNC_LOW) ? 0 : FB_SYNC_HOR_HIGH_ACT;
    display_info.mode.vmode |= (priv->timing.flags & DISPLAY_FLAGS_INTERLACED) ? 0 : FB_VMODE_NONINTERLACED;
    display_info.pixfmt = priv->pixfmt;

    ret = ipuv3_fb_init(&display_info.mode, priv->di_no, priv->pixfmt);

    if (priv->timing.hdmi_monitor) {
        enable_hdmi(&display_info);
    } else {
        enable_lvds(&display_info);
    }

    if (ret < 0)
        return ret;

    return 0;
}


static int imx6_dp_bind(struct udevice *dev)
{

	debug("M48 imx6_dp_bind\n");


	return 0;
}

static int imx6_lvds_dp_ofdata_to_platdata(struct udevice *dev)
{
	struct imx6_dp_priv *priv = dev_get_priv(dev);
	int ret;
	ofnode channel_node;
	u32 datawidth;
	ofnode timings_node, tmp_node;
	const char *display;
	int i;

	channel_node = ofnode_find_subnode(dev_ofnode(dev), "lvds-channel");
	if (!ofnode_valid(channel_node)) {
		dev_err(dev, "required node lvds-channel isn't provided\n");
		return -EINVAL;
	}

	timings_node = ofnode_find_subnode(channel_node, "display-timings");
	if (!ofnode_valid(timings_node))
		return -EINVAL;

	/* get timing from environment variable */
	display = env_get("panel");
	priv->timing_no = -1;
	i = 0;
	ofnode_for_each_subnode(tmp_node, timings_node) {
		if (!ofnode_valid(tmp_node))
			return -EINVAL;
		if (strcmp(ofnode_get_name(tmp_node), display) == 0) {
			priv->timing_no = i;
			break;
		}
		i++;
	}

	if (priv->timing_no >= 0) {

		ret = ofnode_read_u32(channel_node, "fsl,data-width", &datawidth);
		if (ret) {
			dev_err(dev, "required fsl,data-width property isn't provided\n");
			return -EINVAL;
		}

		switch (datawidth) {
		case 18:
			priv->pixfmt = IPU_PIX_FMT_LVDS666;
			break;
		case 24:
			priv->pixfmt = IPU_PIX_FMT_RGB24;
			break;
		default:
			priv->pixfmt = IPU_PIX_FMT_RGB24;
		}

		ret = ofnode_decode_display_timing(channel_node, priv->timing_no, &priv->timing);
		if (ret) {
			dev_err(dev, "failed to get any display timings\n");
			return -EINVAL;
		}
	}

	return 0;
}

void initialize_without_display(void)
{
    enable_pll5 (KHZ2PICOS(25175000 / 1000)); /* pixclock 640x480 in Hz -> wavelen in ps */
}

static const struct dm_display_ops imx6_dp_ops = {
	.enable      = imx6_dp_enable,
	.read_timing = imx6_dp_read_timing,
};

static const struct udevice_id imx6_lvds_dp_ids[] = {
	{ .compatible = "fsl,imx6q-ldb" },
	{ }
};

U_BOOT_DRIVER(imx6_lvds_dp) = {
	.name	= "imx6_lvds_dp",
	.id	= UCLASS_DISPLAY,
	.of_match = imx6_lvds_dp_ids,
	.bind	= imx6_dp_bind,
	.probe	= imx6_dp_probe,
	.ops	= &imx6_dp_ops,
	.of_to_plat	= imx6_lvds_dp_ofdata_to_platdata,
	.priv_auto	= sizeof(struct imx6_dp_priv),
};

static int imx6_hdmi_dp_ofdata_to_platdata(struct udevice *dev)
{
	struct imx6_dp_priv *priv = dev_get_priv(dev);
	ofnode timings_node, tmp_node;
	const char *display;
	int i;
	int ret;

	timings_node = ofnode_find_subnode(dev_ofnode(dev), "display-timings");
	if (!ofnode_valid(timings_node))
		return -EINVAL;

	/* get timing from environment variable */
	display = env_get("panel");
	priv->timing_no = -1;
	i = 0;
	ofnode_for_each_subnode(tmp_node, timings_node) {
		if (!ofnode_valid(tmp_node))
			return -EINVAL;
		if (strcmp(ofnode_get_name(tmp_node), display) == 0) {
			priv->timing_no = i;
			break;
		}
		i++;
	}

	if (priv->timing_no >= 0) {

		ret = ofnode_decode_display_timing(dev_ofnode(dev), priv->timing_no, &priv->timing);
		if (ret) {
			dev_err(dev, "failed to get any display timings\n");
			return -EINVAL;
		}
		priv->timing.hdmi_monitor = true;
		priv->pixfmt = IPU_PIX_FMT_RGB24;
	}

	return 0;
}

static const struct udevice_id imx6_hdmi_dp_ids[] = {
		{ .compatible = "fsl,imx6q-hdmi" },
		{ .compatible = "fsl,imx6dl-hdmi" },
		{ }
};

U_BOOT_DRIVER(imx6_hdmi_dp) = {
	.name	= "imx6_hdmi_dp",
	.id	= UCLASS_DISPLAY,
	.of_match = imx6_hdmi_dp_ids,
	.bind	= imx6_dp_bind,
	.probe	= imx6_dp_probe,
	.ops	= &imx6_dp_ops,
	.of_to_plat	= imx6_hdmi_dp_ofdata_to_platdata,
	.priv_auto	= sizeof(struct imx6_dp_priv),
};

