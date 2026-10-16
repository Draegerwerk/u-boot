/*
 *  (C) Copyright 2020 Draeger and Licensors, info@draeger.com
 *
 *  SPDX-License-Identifier: GPL-2.0+
 */
#include <config.h>
#include <log.h>
#include <linux/delay.h>
#include <spl.h>
#include <hang.h>
#include <i2c.h>
#include <mmc.h>
#include <post.h>
#include <init.h>
#include <cpu_func.h>
#include <fsl_esdhc_imx.h>
#include <asm-generic/sections.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch-mx6/crm_regs.h>
#include <asm/arch-mx6/iomux.h>
#include <asm/arch-mx6/clock.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/mach-imx/sys_proto.h>
#include <asm/mach-imx/hab.h>
#include <asm/gpio.h>
#include <asm/cache.h>
#include <asm/system.h>

#include <dm/uclass.h>
#include <dm/uclass-internal.h>
#include <dm/platdata.h>
#include <dm/device-internal.h>

#include <power/pfuze100_pmic.h>

#include "../common/draeger_m48_pmstruct.h"

#include "imx6m48.h"
#include "imx6m48_spl_cfg.h"


/* #define DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER
 */

#define RESET_GPIO_BASE_ADDR GPIO1_BASE_ADDR
#define RESET_GPIO_INDEX     2


#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP | \
    PAD_CTL_SPEED_LOW | PAD_CTL_DSE_120ohm | \
    PAD_CTL_SRE_SLOW  | PAD_CTL_HYS)

#define I2C_PAD_CTRL  (PAD_CTL_PUS_100K_UP |             \
    PAD_CTL_SPEED_LOW | PAD_CTL_DSE_60ohm | PAD_CTL_HYS |\
    PAD_CTL_ODE | PAD_CTL_SRE_FAST)
#define I2C_PAD MUX_PAD_CTRL(I2C_PAD_CTRL)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |             \
    PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |              \
    PAD_CTL_SRE_FAST  | PAD_CTL_HYS)
#define USDHC_PAD MUX_PAD_CTRL(USDHC_PAD_CTRL)

#define USDHC4_PAD_CTRL (PAD_CTL_PUS_47K_UP |             \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm |              \
    PAD_CTL_SRE_FAST  | PAD_CTL_HYS)
#define USDHC4_PAD MUX_PAD_CTRL(USDHC4_PAD_CTRL)

#define SD2_USDHC_PAD_CLK_CTRL (PAD_CTL_PUS_47K_UP |     \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm |              \
    PAD_CTL_SRE_FAST  | PAD_CTL_HYS)
#define SD2_USDHC_CLK_PAD MUX_PAD_CTRL(SD2_USDHC_PAD_CLK_CTRL)

#define SD2_USDHC_PAD_CMD_CTRL (PAD_CTL_PUS_47K_UP |     \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_48ohm |              \
    PAD_CTL_SRE_FAST  | PAD_CTL_HYS)
#define SD2_USDHC_CMD_PAD MUX_PAD_CTRL(SD2_USDHC_PAD_CMD_CTRL)

#define SD2_USDHC_PAD_DATA_CTRL (PAD_CTL_PUS_47K_UP |         \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_60ohm |              \
    PAD_CTL_SRE_FAST  | PAD_CTL_HYS)
#define SD2_USDHC_DATA_PAD MUX_PAD_CTRL(SD2_USDHC_PAD_DATA_CTRL)

#define SD_VSELECT_PAD_CTRL (PAD_CTL_PUS_100K_UP |       \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm |              \
    PAD_CTL_SRE_SLOW | PAD_CTL_HYS)
#define SD_PAD MUX_PAD_CTRL(SD_VSELECT_PAD_CTRL)

DECLARE_GLOBAL_DATA_PTR;

#define I2C_RTC_ADDR_M48                0x68
#define I2C_RTC_ADDR_VAL                0x32
#define I2C_PMIC_ADDR                   0x08 /* iMX6 I2C2 */
#define I2C_MUX_ADDR                    0x70 /* iMX6 I2C3 */
#define I2C_PI_EEPROM_ADDR              0x53 /* iMX6 I2C3 */

#define PMIC_SW34_MASK  (0x3F)
#define PMIC_SW34_1P35V (0x26)

#define PMIC_SW2_MASK   (0x3F)
#define PMIC_SW2_3P3V   (0x32)

#ifdef BUILD_TAG
#  define DECLARE_GLOBAL_BUILD_ID  const char *global_build_id = BUILD_TAG
#else
#  define DECLARE_GLOBAL_BUILD_ID  const char *global_build_id = "private"
#endif

#define M48_PART_NO_SIZE  7
#define M48_REVISION_SIZE 2

struct m48_board_info {
    u8 partno[M48_PART_NO_SIZE];     /* 0x1e: part number */
    u8 revision[M48_REVISION_SIZE];  /* 0x0C: revision */
    u8 end;                          /* string delimiter */
};

static iomux_v3_cfg_t const uart1_pads[] =
{
 IOMUX_PADS(PAD_SD3_DAT7__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
 IOMUX_PADS(PAD_SD3_DAT6__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
};

static struct i2c_pads_info mx6q_i2c_pad_info =
{
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

static struct i2c_pads_info mx6dl_i2c_pad_info =
{
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


static iomux_v3_cfg_t const spl_usdhc3_pads[] =
{
 IOMUX_PADS(PAD_SD3_CLK__SD3_CLK   | USDHC_PAD),
 IOMUX_PADS(PAD_SD3_CMD__SD3_CMD   | USDHC_PAD),
 IOMUX_PADS(PAD_SD3_DAT0__SD3_DATA0 | USDHC_PAD),
 IOMUX_PADS(PAD_SD3_DAT1__SD3_DATA1 | USDHC_PAD),
 IOMUX_PADS(PAD_SD3_DAT2__SD3_DATA2 | USDHC_PAD),
 IOMUX_PADS(PAD_SD3_DAT3__SD3_DATA3 | USDHC_PAD),
};

static struct fsl_esdhc_cfg usdhc_cfg[CFG_FSL_USDHC_NUM];


int post_bootmode_get(unsigned int *last_test);

static void setup_iomux_uart(void)
{
    SETUP_IOMUX_PADS(uart1_pads);
}

void setup_m48_i2c(void)
{
    struct i2c_pads_info *p;

    if (is_cpu_type(MXC_CPU_MX6Q) || is_cpu_type(MXC_CPU_MX6QP))
        p = &mx6q_i2c_pad_info;
    else
        p = &mx6dl_i2c_pad_info;

    setup_i2c(CONFIG_SYS_EEPROM_I2C_BUS_NUM_M48, CONFIG_SYS_I2C_SPEED, 0x7f, p);
}

int board_mmc_getcd(struct mmc *mmc)
{
    return 1;
}

int board_mmc_init(struct bd_info *bis)
{
    s32 status = 0;

    SETUP_IOMUX_PADS(spl_usdhc3_pads);

    usdhc_cfg[0].esdhc_base = USDHC3_BASE_ADDR;
    usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
    usdhc_cfg[0].max_bus_width = 4;

    status = fsl_esdhc_initialize(bis, &usdhc_cfg[0]);

    return status;
}

M48_BOARD_VERSION read_eeprom(int bus, struct m48_board_info *info)
{
    int ret;
    int revision = 0;

    uint8_t *buf = (uint8_t *)info;
    bool     infoValid = true;
    uint32_t failedAddr = 0;

    M48_BOARD_VERSION board_model = M48_UNKNOWN;

    ret = (0 != i2c_set_bus_num(CONFIG_SYS_EEPROM_I2C_BUS_NUM_M48) ||
            0 != i2c_probe(CONFIG_SYS_I2C_EEPROM_ADDR));
    if (! ret)
        ret = i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR, CFG_SYS_I2C_PART_NO_OFFSET_M48,
                        CONFIG_SYS_EEPROM_MEMADDR_LEN_M48, buf, sizeof(struct m48_board_info));

    if (ret)
    {
        failedAddr = CONFIG_SYS_I2C_EEPROM_ADDR;
    }
    else
    {
        info->end = 0;

        revision = simple_strtoul((const char*) info->revision, NULL, 10);

        switch(get_cpu_type())
        {
            case MXC_CPU_MX6QP:
                board_model = M48_4_QUADP; /* Set default even if EEPROM not programmed */

                if (memcmp(info->partno, PART_NUMBER_M48_4, sizeof(info->partno)) == 0)
                {
                    board_model = M48_4_QUADP;
                }
                else
                {
                    infoValid = false;
                }
            break;

            case MXC_CPU_MX6Q:
            {
                board_model = M48_3_PROD;   /* Set default even if EEPROM not programmed */

                if (memcmp(info->partno, PART_NUMBER_M48_3, sizeof(info->partno)) == 0)
                {
                    if (revision < 2)  board_model = M48_3_PRE;    /* Old PCB: Force to use old RAM timing */
                    else               board_model = M48_3_PROD;   /* New PCB */
                }
                else
                {
                    infoValid = false;
                }
            }
            break;

            case MXC_CPU_MX6DL:
            {
                board_model = M48_1_PROD; /* Set default even if EEPROM not programmed */

                if (memcmp(info->partno, PART_NUMBER_M48_2, sizeof(info->partno)) == 0)
                {
                    if (revision < 2)  board_model = M48_2_PRE;     /* Old PCB: Force to use old RAM timing */
                    else               board_model = M48_2_PROD;    /* New PCB */
                }
                else if (memcmp(info->partno, PART_NUMBER_M48_1, sizeof(info->partno)) == 0)
                {
                    if (revision < 8)  board_model = M48_1_PRE;     /* Old PCB: Force to use old RAM timing */
                    else               board_model = M48_1_PROD;    /* New PCB */
                }
                else if (memcmp(info->partno, PART_NUMBER_M48_1S, sizeof(info->partno)) == 0)
                {
                    /* For M48.1S there are no "bad" revisions that need old RAM timings */
                    board_model = M48_1_PROD;    /* New PCB */
                }                    
                else
                {
                    infoValid = false;
                }
            }
            break;

            default:
                board_model = M48_3_PROD; /* default setting for all unknown boards */
        }
    }

    /* Check if reading of EEPROM(s) failed */
    if (failedAddr != 0)
    {
        infoValid = false;
        printf("\nEEPROM @ 0x%02x read FAILED!!!\n", failedAddr);
    }

    if (infoValid == false)
    {
        printf("\nUnexpected SOM Part# '%.7s'. EEPROM is either blank or not programmed correctly!\n",
                info->partno);
        splHandoverData->eepromInfoValid = false;
    }
    else
    {
        splHandoverData->eepromInfoValid = true;
    }

    return board_model;
}


void get_board_model(void)
{
    struct m48_board_info m48_info;
    M48_BOARD_VERSION board_model = M48_UNKNOWN;

    char revision_0;

    /** read/validate EEPROM info to determine board model and SDRAM cfg **/
    memset(&m48_info, '?', sizeof(m48_info));
    m48_info.end = 0;
    board_model = read_eeprom(1, &m48_info);

    if (board_model == M48_UNKNOWN) {
        /** As already stated in read_eeprom, the read of eeprom failed! **/
        printf("Unknown board model! Will halt now.\n");
        hang();
    }

    revision_0 = m48_info.revision[0];
    m48_info.revision[0] = 0;
    printf("PartNo: %s-%c%c model %d\n",
        m48_info.partno,
        revision_0,
        m48_info.revision[1],
        board_model);

    splHandoverData->boardVersion = board_model;
}

static void configure_registers(m48_configuration *reg_list, int count)
{
    int i;
    volatile u32 *address;

    for (i=0; i<count; i++) {
        address = (u32*) reg_list[i].address;
        *address = reg_list[i].value;
        asm("DSB");
    }
}

static void spl_dram_init(M48_BOARD_VERSION board_model)
{
    switch (board_model) {
        case M48_3_PROD:
            configure_registers(ddr_setup_mx6q, ddr_setup_mx6q_size);
            configure_registers(ram_calibration_mx6q_prod, ram_calibration_mx6q_size_prod);
            configure_registers(ram_setup_mx6q, ram_setup_mx6q_size);
            break;
        case M48_2_PROD: /* fallthrough */
        case M48_1_PROD:
            configure_registers(ddr_setup_mx6dl, ddr_setup_mx6dl_size);
            configure_registers(ram_calibration_mx6dl_prod, ram_calibration_mx6dl_size_prod);
            configure_registers(ram_setup_mx6dl, ram_setup_mx6dl_size);
            break;
        case M48_3_PRE:
            configure_registers(ddr_setup_mx6q, ddr_setup_mx6q_size);
            configure_registers(ram_calibration_mx6q_pre, ram_calibration_mx6q_size_pre);
            configure_registers(ram_setup_mx6q, ram_setup_mx6q_size);
            break;
        case M48_2_PRE: /* fallthrough */
        case M48_1_PRE:
            configure_registers(ddr_setup_mx6dl, ddr_setup_mx6dl_size);
            configure_registers(ram_calibration_mx6dl_pre, ram_calibration_mx6dl_size_pre);
            configure_registers(ram_setup_mx6dl, ram_setup_mx6dl_size);
            break;
        case M48_4_QUADP:
            configure_registers(ddr_setup_mx6q, ddr_setup_mx6q_size);
            configure_registers(ram_setup_mx6qp, ram_setup_mx6qp_size);
            configure_registers(ram_calibration_mx6q_prod, ram_calibration_mx6q_size_prod);
            break;
        default:
            break;
    }

    /* wait for one complete refresh cycle */
    mdelay(70);
}

/* is also done in Device Configuration Data (DCD)
 * board/draeger/imx6m48/imx6m48spl.cfg
 * Function is used to be able to load it in debugger
 */
static void clock_init(void)
{
    struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

    /* set the default clock gate to save power */
    writel(0x00C03F3F, &ccm->CCGR0);
    writel(0x0030FC03, &ccm->CCGR1);
    writel(0x0FFFF000, &ccm->CCGR2);
    writel(0x3FF00003, &ccm->CCGR3);
    writel(0xFFFFF300, &ccm->CCGR4);
    writel(0x0F0000C3, &ccm->CCGR5);
    writel(0x000003FF, &ccm->CCGR6);

    /* enable AXI cache for VDOA/VPU/IPU */
    writel( 0xF00000CF, MX6_IOMUXC_GPR4);

    /* set IPU AXI-id0 Qos=0xf(bypass) AXI-id1 Qos=0x7 */
        writel( 0x007F007F, MX6_IOMUXC_GPR6);
        writel( 0x007F007F, MX6_IOMUXC_GPR7);

        /*
         * Setup CCM_CCOSR register as follows:
         *
         * cko1_en  = 1	   --> CKO1 enabled
         * cko1_div = 111  --> divide by 8
         * cko1_sel = 1011 --> ahb_clk_root
         *
         * This sets CKO1 at ahb_clk_root/8 = 132/8 = 16.5 MHz
         */
        writel( 0x000000fb, CCM_CCOSR);

}

/**
 * @brief Deactivate SWBST voltage as it is not used.
 *
 * This is done to have less periphery parts around the PMIC in future HW revisions.
 */
static int pmic_disable_swbst(void)
{
    /* to disable SWBST (see. section 6.4.5.1) set complete register to 0 */
    uchar state = 0x00;

    i2c_set_bus_num(CONFIG_SYS_EEPROM_I2C_BUS_NUM_M48);
    if (i2c_probe(I2C_PMIC_ADDR))
        return 1;

    if (i2c_write(I2C_PMIC_ADDR, PFUZE100_SWBSTCON1, 1, &state, 1))
        return 2;

    return 0;
}


/**
 * @brief Enables the PWM/PWM mode in the SW4Mode regsiter
 *
 */
static int pmic_enable_sw4mode(void)
{
    uchar value = 0x00;

    i2c_set_bus_num(CONFIG_SYS_EEPROM_I2C_BUS_NUM_M48);
    if (i2c_probe(I2C_PMIC_ADDR))
        return 1;

    if(i2c_read(I2C_PMIC_ADDR, PFUZE100_SW4MODE, 1, &value, 1))
        return 2;

    value &= ~(SW_MODE_MASK);
	value |= PWM_PWM;    

    if (i2c_write(I2C_PMIC_ADDR, PFUZE100_SW4MODE, 1, &value, 1))
        return 2;

    return 0;
}

void checkWarmstart(void)
{
    u32              cause;
    u16              wdog_wrsr = 0;
    int              post_boot_mode;
    struct gpio_regs *regs;

    cause = get_imx_reset_cause();

    printf("RESET: %08x\n", cause);

    if (cause & RST_POR)     puts("RESET: POWER_UP - IPP_RESET_B\n");
    if (cause & RST_CSU)     puts("RESET: COLD     - CSU_RESET_B\n");
    if (cause & RST_JTAG_Z)  puts("RESET: JTAG     - JTAG_RST_B\n");
    if (cause & RST_JTAG_SW) puts("RESET: JTAG-SW  - JTAG_SW_RST\n");
    if (cause & RST_WARM)    puts("RESET: WARM     - WARM_BOOT\n");
    if (cause & RST_IPP)     puts("RESET: WARM - IPP_USER_RESET\n");
    if (cause & RST_WDOG)
    {
        puts("RESET: WDOG     - WDOG_RST_B\n");
        wdog_wrsr = get_imx_wdog_wrsr();
        if (wdog_wrsr & WDOG_WRSR_SFTW)     puts("WDOG_WRSR: SW RESET\n");
        if (wdog_wrsr & WDOG_WRSR_TOUT)     puts("WDOG_WRSR: TIMEOUT\n");
        if (wdog_wrsr & WDOG_WRSR_POR)      puts("WDOG_WRSR: POWER ON RESET\n");
    }

    if ( !(cause & (RST_IPP | RST_POR | RST_WARM )))
    {
        post_boot_mode = post_bootmode_get(0);

        /* do not reset the other controller when in POST */
        if ( post_boot_mode != 0 && ! (post_boot_mode & POST_POWERTEST)) {

            puts("Resetting uP2\n");
            udelay(250);

            regs = (struct gpio_regs *) RESET_GPIO_BASE_ADDR;

            writel((0 << RESET_GPIO_INDEX), &regs->gpio_dr);
            writel((1 << RESET_GPIO_INDEX), &regs->gpio_dir);
            udelay(500);
            writel((1 << RESET_GPIO_INDEX), &regs->gpio_dr);
            writel((0 << RESET_GPIO_INDEX), &regs->gpio_dir);
            puts("Resetting uP2 done\n");

        } else {
            printf("No uP2 reset due to POST %x\n", post_boot_mode);
        }
    }

}


/******************************************************************************
 * u-boot hooks
 *****************************************************************************/

/* board_init_f is needed in order to configure the DRAM */
void board_init_f(ulong dummy)
{
#ifdef DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER
    volatile ulong debugger_is_attached;
    debugger_is_attached = 0;
    while(debugger_is_attached == 0)
    { /* attach with your debugger and manually set variable to 1 */ }
#endif

    /* setup clock gating and AXI */
    /* (done in DCD)	          */
    clock_init();

    /* setup AIPS and disable watchdog */
    arch_cpu_init();

    /* setup GP timer */
    timer_init();

    /* iomux and setup of i2c */
    setup_m48_i2c();

    /* iomux and setup of uart */
    setup_iomux_uart();

    board_early_init_f();

    if (!(gd->flags & GD_FLG_SILENT))
    {
        preloader_console_init();
    }

    get_board_model();

    pmic_enable_sw4mode();
    pmic_disable_swbst();

    /* configure MMDC for SDRAM width/size and calibration */
    spl_dram_init(splHandoverData->boardVersion);

    /* Set up early MMU tables at the beginning of DRAM and start d-cache */
    gd->arch.tlb_addr = MMDC0_ARB_BASE_ADDR + SZ_32M;
    gd->arch.tlb_size = PGTABLE_SIZE;
    enable_caches();
}

int board_fdt_blob_setup(void **fdtp)
{
    int err = -EEXIST;
    *fdtp = _image_binary_end;

    return err;
}

void spl_display_print(void)
{
#if defined(BUILD_TAG)
    puts("Build: " BUILD_TAG "\n");
#endif
}

void spl_board_init(void)
{
    DECLARE_GLOBAL_BUILD_ID;

    strncpy(splHandoverData->preloaderVersion, global_build_id, sizeof(splHandoverData->preloaderVersion));
    splHandoverData->preloaderVersion[sizeof(splHandoverData->preloaderVersion) - 1] = '\0';
}

void board_boot_order(u32 *spl_boot_list)
{
    checkWarmstart();

    if (getBootMmcDevice() == 0)
    {
        spl_boot_list[0] = BOOT_DEVICE_MMC1;
    }
    else
    {
        spl_boot_list[0] = BOOT_DEVICE_MMC2;
    }
}

/* mark imx6 memory ranges cachable (ROM, internal and external RAM)
 * We only use 512MB of the external RAM for caching. This way we do not have to deal with
 * different memory banks and stuff. U-Boot will make the whole memory cachable once relocated.
 * This method is called from board_init_f() -> enable_caches() -> mmu_setup()
 */
void dram_bank_mmu_setup(int bank) {

    set_section_dcache(ROMCP_ARB_BASE_ADDR >> MMU_SECTION_SHIFT, DCACHE_DEFAULT_OPTION);
    set_section_dcache(IRAM_BASE_ADDR >> MMU_SECTION_SHIFT, DCACHE_DEFAULT_OPTION);

    for (int i = (MMDC0_ARB_BASE_ADDR >> MMU_SECTION_SHIFT);
        i < ((MMDC0_ARB_BASE_ADDR >> MMU_SECTION_SHIFT) + (SZ_512M >> MMU_SECTION_SHIFT));
        i++)
        {
            set_section_dcache(i, DCACHE_DEFAULT_OPTION);
        }
}

/* This method is copied from arch/arm/mach-imx/spl.c where it is declared weak
 * We override it here in order to disable the cache when leaving SPL.
 * U-Boot will later properly initialize MMU and cache.
 */
void __noreturn jump_to_image(struct spl_image_info *spl_image)
{
    typedef void __noreturn (*image_entry_noargs_t)(void);
    uint32_t offset;

    image_entry_noargs_t image_entry =
        (image_entry_noargs_t)(unsigned long)spl_image->entry_point;

    debug("image entry point: 0x%lX\n", spl_image->entry_point);

    if (spl_image->flags & SPL_FIT_FOUND) {
        dcache_disable();
        image_entry();
    } else {
        /*
         * HAB looks for the CSF at the end of the authenticated
         * data therefore, we need to subtract the size of the
         * CSF from the actual filesize
         */
        offset = spl_image->size - CONFIG_CSF_SIZE;
        if (!imx_hab_authenticate_image(spl_image->load_addr,
                        offset + IVT_SIZE +
                        CSF_PAD_SIZE, offset)) {
            dcache_disable();
            image_entry();
        } else {
            panic("spl: ERROR:  image authentication fail\n");
        }
    }
}

void spl_board_prepare_for_boot(void)
{
    /* This method is called right before jump_to_image */
    flush_dcache_all();
}
