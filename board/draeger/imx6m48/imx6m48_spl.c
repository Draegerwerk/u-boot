/*
 *  (C) Copyright 2020 Draeger and Licensors, info@draeger.com
 *
 *  SPDX-License-Identifier: GPL-2.0+
 */
#include <common.h>
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


#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			 \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			     \
    PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

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

#define SD2_USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |         \
    PAD_CTL_SPEED_LOW | PAD_CTL_DSE_48ohm |              \
    PAD_CTL_SRE_FAST  | PAD_CTL_HYS)
#define SD2_USDHC_PAD MUX_PAD_CTRL(SD2_USDHC_PAD_CTRL)

#define SD_VSELECT_PAD_CTRL (PAD_CTL_PUS_100K_UP |       \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_80ohm |              \
    PAD_CTL_SRE_SLOW | PAD_CTL_HYS)
#define SD_PAD MUX_PAD_CTRL(SD_VSELECT_PAD_CTRL)

DECLARE_GLOBAL_DATA_PTR;

#define I2C_RTC_ADDR 0x68
#define I2C_MUX_ADDR 0x70
#define I2C_PMIC_ADDR 0x08

#define VALENKA_EEPROM_BUS_NUM  2
#define VALENKA_I2C_EEPROM_ADDR 0x53

#define VALENKA_I2C_PART_NO_OFFSET    17
#define VALENKA_I2C_FORMAT_OFFSET      3
#define VALENKA_ELV8_FORMAT           '8'
#define VALENKA_I2C_HW_VERSION_OFFSET  9

#define PMIC_SW34_MASK  (0x3F)
#define PMIC_SW34_1P35V (0x26)

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

static iomux_v3_cfg_t const uart1_valenka_pads[] =
{
 IOMUX_PADS(PAD_CSI0_DAT10__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
 IOMUX_PADS(PAD_CSI0_DAT11__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
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

/* I2C3 */
static struct i2c_pads_info valenka_i2c_pad_info =
{
 .scl = {
         .i2c_mode = MX6Q_PAD_GPIO_3__I2C3_SCL | I2C_PAD,
         .gpio_mode = MX6Q_PAD_GPIO_3__GPIO1_IO03 | I2C_PAD,
         .gp = IMX_GPIO_NR(1, 3)
 },
 .sda = {
         .i2c_mode = MX6Q_PAD_GPIO_6__I2C3_SDA | I2C_PAD,
         .gpio_mode = MX6Q_PAD_GPIO_6__GPIO1_IO06 | I2C_PAD,
         .gp = IMX_GPIO_NR(1, 6)
 }
};

static iomux_v3_cfg_t const spl_usdhc2_pads[] =
{
 IOMUX_PADS(PAD_KEY_ROW2__SD2_VSELECT | SD_PAD),
 IOMUX_PADS(PAD_SD2_CLK__SD2_CLK	| SD2_USDHC_PAD),
 IOMUX_PADS(PAD_SD2_CMD__SD2_CMD	| SD2_USDHC_PAD),
 IOMUX_PADS(PAD_SD2_DAT0__SD2_DATA0 | SD2_USDHC_PAD),
 IOMUX_PADS(PAD_SD2_DAT1__SD2_DATA1 | SD2_USDHC_PAD),
 IOMUX_PADS(PAD_SD2_DAT2__SD2_DATA2 | SD2_USDHC_PAD),
 IOMUX_PADS(PAD_SD2_DAT3__SD2_DATA3 | SD2_USDHC_PAD),
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

static iomux_v3_cfg_t const spl_usdhc4_pads[] =
{
 IOMUX_PADS(PAD_SD4_CLK__SD4_CLK   | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_CMD__SD4_CMD   | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT0__SD4_DATA0 | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT1__SD4_DATA1 | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT2__SD4_DATA2 | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT3__SD4_DATA3 | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT4__SD4_DATA4 | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT5__SD4_DATA5 | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT6__SD4_DATA6 | USDHC4_PAD),
 IOMUX_PADS(PAD_SD4_DAT7__SD4_DATA7 | USDHC4_PAD),
};

static struct fsl_esdhc_cfg usdhc_cfg[CFG_FSL_USDHC_NUM];


int post_bootmode_get(unsigned int *last_test);

bool isValenka(void)
{
    static int aValenkaBoard = -1;

    if (aValenkaBoard < 0)
    {
        i2c_set_bus_num(CONFIG_SYS_EEPROM_BUS_NUM);

        /* If a RTC device is connected to EEPROM I2C bus it is assumed */
        /* that we running at a M48 platform else we assume a Valenka   */
        /* platform.                                                    */
        if (0 == i2c_probe(I2C_RTC_ADDR))
        {
            aValenkaBoard = 0;
        }
        else
        {
            aValenkaBoard = 1;
        }
    }

    return aValenkaBoard;
}

static void setup_iomux_uart(void)
{
    if (isValenka())
    {
        SETUP_IOMUX_PADS(uart1_valenka_pads);
    }
    else
    {
        SETUP_IOMUX_PADS(uart1_pads);
    }
}

void setup_m48_i2c(void)
{
    struct i2c_pads_info *p;

    if (is_cpu_type(MXC_CPU_MX6Q) || is_cpu_type(MXC_CPU_MX6QP))
        p = &mx6q_i2c_pad_info;
    else
        p = &mx6dl_i2c_pad_info;

    setup_i2c(CONFIG_SYS_EEPROM_BUS_NUM, CONFIG_SYS_I2C_SPEED, 0x7f, p);

    if (isValenka())
    {
        p = &valenka_i2c_pad_info;
        setup_i2c(VALENKA_EEPROM_BUS_NUM, CONFIG_SYS_I2C_SPEED, 0x7f, p);
    }
}

int board_mmc_getcd(struct mmc *mmc)
{
    return 1;
}

int board_mmc_init(struct bd_info *bis)
{
    s32 status = 0;

    if (isValenka())
    {
        SETUP_IOMUX_PADS(spl_usdhc2_pads);

        usdhc_cfg[0].esdhc_base = USDHC2_BASE_ADDR;
        usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);

        status = fsl_esdhc_initialize(bis, &usdhc_cfg[0]);

        if (status)
            return status;

        SETUP_IOMUX_PADS(spl_usdhc4_pads);

        usdhc_cfg[1].esdhc_base = USDHC4_BASE_ADDR;
        usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);
        usdhc_cfg[1].max_bus_width = 8;

        status = fsl_esdhc_initialize(bis, &usdhc_cfg[1]);
    }
    else
    {
        SETUP_IOMUX_PADS(spl_usdhc3_pads);

        usdhc_cfg[0].esdhc_base = USDHC3_BASE_ADDR;
        usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
        usdhc_cfg[0].max_bus_width = 4;

        status = fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
    }

    return status;
}

M48_BOARD_VERSION read_eeprom(int bus, struct m48_board_info *info)
{
    int ret;
    int revision = 0;

    uint8_t *buf = (uint8_t *)info;
    uint8_t  mask;
    uint8_t  elvFormat;
    uint16_t hwVersion = 0;
    uint8_t  hwVersionStr[2] = { '0' };

    M48_BOARD_VERSION board_model = M48_UNKNOWN;

    splHandoverData->somVersion = VALENKA_UNKNOWN_SOM;

    if (isValenka())
    {
        int                 i;

        ret = i2c_set_bus_num(VALENKA_EEPROM_BUS_NUM);

        /* Read the SOM EEPROM to determine whether we have a Valenka 3 */
        /* SOM. The SOM EEPROM is connected directly to I2C bus 3       */
        ret = i2c_probe(CONFIG_SYS_I2C_EEPROM_ADDR);

        if (! ret)
            ret = i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR, VALENKA_I2C_PART_NO_OFFSET, 2, buf, M48_PART_NO_SIZE);

        if (ret) {
            printf("\nSOM EEPROM @ 0x%02x read FAILED!!!\n",
                CONFIG_SYS_I2C_EEPROM_ADDR);
            return M48_UNKNOWN;
        }

        for (i = 0; i < sizeof(info->partno); i++)
        {
            debug("%c", info->partno[i]);
        }
        debug("\n");

        if (memcmp(info->partno, "MS40605", sizeof(info->partno)) == 0) {
            splHandoverData->somVersion = VALENKA_3_4GB_SOM;
        } else if (memcmp(info->partno, "MS40411", sizeof(info->partno)) == 0) {
            splHandoverData->somVersion = VALENKA_3_2GB_SOM;
        } else {
            splHandoverData->somVersion = VALENKA_2_SOM;
        }

        /* Read the PI EEPROM to determine whether we are a SSPPM or Mount */
        /* The PI EEPROM is connected to leg 4 of the I2C mux at I2C bus 3 */
        if (! ret)
            ret = i2c_probe(I2C_MUX_ADDR);

        if (! ret) {
            mask = 1 << (4 -1);
            ret = i2c_write(I2C_MUX_ADDR, 0x00, 0, &mask, sizeof(mask));
        }

        if (! ret)
            ret = i2c_probe(VALENKA_I2C_EEPROM_ADDR);

        /* Read the EEPROM format to see where the hwRev is */
        /* The If parts of the two "if (elfFormat < VALENKA_ELV8_FORMAT)" can be removed before the PPM is released */
        if (! ret)
            ret = i2c_read(VALENKA_I2C_EEPROM_ADDR, VALENKA_I2C_FORMAT_OFFSET, 2, &elvFormat, sizeof(elvFormat));

        if (! ret)
            ret = i2c_read(VALENKA_I2C_EEPROM_ADDR, VALENKA_I2C_PART_NO_OFFSET, 2, buf, M48_PART_NO_SIZE);

        if (! ret)
        {
            if (elvFormat < VALENKA_ELV8_FORMAT)
                ret = i2c_read(VALENKA_I2C_EEPROM_ADDR, VALENKA_I2C_HW_VERSION_OFFSET, 2, (char *) &hwVersion, sizeof(hwVersion));
            else
                ret = i2c_read(VALENKA_I2C_EEPROM_ADDR, VALENKA_I2C_HW_VERSION_OFFSET, 2, hwVersionStr, sizeof(hwVersionStr));
        }

        if (ret) {
            printf("\nPI EEPROM @ 0x%02x read FAILED!!!\n",
                VALENKA_I2C_EEPROM_ADDR);
            return M48_UNKNOWN;
        }

        if (elvFormat < VALENKA_ELV8_FORMAT)
        {
            snprintf((char*)&info->revision, sizeof(info->revision) + 1,
                     "%02d", __be16_to_cpu(hwVersion));
        }
        else
        {
            snprintf((char*)&info->revision, sizeof(info->revision) + 1,
                     "%c%c", hwVersionStr[0], hwVersionStr[1]);
        }

        for (i = 0; i < sizeof(info->partno); i++)
        {
            debug("%c", info->partno[i]);
        }
        debug("\n");

        if (memcmp(info->partno, "MS34821", sizeof(info->partno)) == 0) {
            board_model = VALENKA_IMX6Q_SSPPM;
        } else if (memcmp(info->partno, "MS34381", sizeof(info->partno)) == 0) {
            board_model = VALENKA_IMX6Q_MOUNT;
        } else {
            printf("\nUnexpected part no: %.7s, expected MS34821 or MS34381\n",
                    info->partno);
        }
        return board_model;
    }

    ret = ( 0 != i2c_set_bus_num(CONFIG_SYS_EEPROM_BUS_NUM) ||
        0 != i2c_probe(CONFIG_SYS_I2C_EEPROM_ADDR));
    if (! ret)
        ret = i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR, CFG_SYS_I2C_PART_NO_OFFSET,
            CONFIG_SYS_I2C_EEPROM_ADDR_LEN, buf, sizeof(struct m48_board_info));

    if (ret) {
        printf("\nEEPROM @ 0x%02x read FAILED!!!\n",
            CONFIG_SYS_I2C_EEPROM_ADDR);
    } else {
        info->end = 0;

        revision = simple_strtoul((const char*) info->revision, NULL, 10);

        if (memcmp(info->partno, PART_NUMBER_M48_3, sizeof(info->partno)) == 0) 
        {
            if (revision < 2)   board_model = M48_3_PRE;    /* Old PCB: Force to use old RAM timing */
            else                board_model = M48_3_PROD;   /* New PCB */
        }
        else if (memcmp(info->partno, PART_NUMBER_M48_2, sizeof(info->partno)) == 0) 
        {
            if (revision < 2)  board_model = M48_2_PRE;     /* Old PCB: Force to use old RAM timing */
            else               board_model = M48_2_PROD;    /* New PCB */
        } 
        else if (memcmp(info->partno, PART_NUMBER_M48_1, sizeof(info->partno)) == 0) {
            if (revision < 8)  board_model = M48_1_PRE;     /* Old PCB: Force to use old RAM timing */
            else               board_model = M48_1_PROD;    /* New PCB */
        }
        else if (memcmp(info->partno, PART_NUMBER_M48_1S, sizeof(info->partno)) ==0 ) {
            /* For M48.1S there are no "bad" revisions that need old RAM timings */
            board_model = M48_1_PROD;
        }
        else if (memcmp(info->partno, PART_NUMBER_M48_4, sizeof(info->partno == 0))) {
            /* For M48.4 there are no "bad" revisions that need old RAM timings */
            board_model = M48_3_QUADP;
        }
        else /* No known part number. Set the board model based on CPU type */
        {
            if(is_cpu_type(MXC_CPU_MX6Q))           board_model = M48_3_PROD;
            else if(is_cpu_type(MXC_CPU_MX6QP))     board_model = M48_3_QUADP;
            else                                    board_model = M48_1_PROD; /* Always assume we have a board with FPGA (no M48.2!)*/
        }
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
        case M48_3_QUADP:
            configure_registers(ddr_setup_mx6q, ddr_setup_mx6q_size);
            configure_registers(ram_setup_mx6qp, ram_setup_mx6qp_size);
            configure_registers(ram_calibration_mx6q_prod, ram_calibration_mx6q_size_prod);
            break;
        case VALENKA_IMX6Q_SSPPM: /* fallthrough */
        case VALENKA_IMX6Q_MOUNT:
            if (splHandoverData->somVersion == VALENKA_3_2GB_SOM)
            {
                configure_registers(ddr_setup_valenka3, ddr_setup_valenka3_size);
                configure_registers(ram_calibration_valenka3_2gb, ram_calibration_valenka3_2gb_size);
                configure_registers(ram_setup_valenka3_2gb, ram_setup_valenka3_2gb_size);
            }
            else
            if (splHandoverData->somVersion == VALENKA_3_4GB_SOM)
            {
                configure_registers(ddr_setup_valenka3, ddr_setup_valenka3_size);
                configure_registers(ram_calibration_valenka3_4gb, ram_calibration_valenka3_4gb_size);
                configure_registers(ram_setup_valenka3_4gb, ram_setup_valenka3_4gb_size);
            }
            else
            {   /* Valenka 2 */
                configure_registers(ddr_setup_valenka, ddr_setup_valenka_size);
                configure_registers(ram_calibration_valenka, ram_calibration_valenka_size);
                configure_registers(ram_setup_valenka, ram_setup_valenka_size);
            }
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

    i2c_set_bus_num(CONFIG_SYS_EEPROM_BUS_NUM);
    if (i2c_probe(I2C_PMIC_ADDR))
        return 1;

    if (i2c_write(I2C_PMIC_ADDR, PFUZE100_SWBSTCON1, 1, &state, 1))
        return 2;

    return 0;
}

/**
 * @brief Initialize the PMIC DDR3 power rails on the Valenka3 SOM
 *
 * Valenka3 DDR3 is powered via 1.35V's
 */
static int pmic_init_valenka(void)
{
    if ((splHandoverData->somVersion == VALENKA_3_2GB_SOM) ||
        (splHandoverData->somVersion == VALENKA_3_4GB_SOM))
    {
        int ret;
        uchar buf[4];

        /*
         * Please refer to the NXP PMIC (MMPF0100) Power Management IC data sheet for
         * more infomation.
         * Valenka3 uses the MMPF0100F2 variant of the PMIC (see section 6.1.1)
         */

        i2c_set_bus_num(CONFIG_SYS_EEPROM_BUS_NUM);

        ret = i2c_probe(I2C_PMIC_ADDR);
        if (ret != 0)
            return 1;

        if (i2c_read(I2C_PMIC_ADDR, PFUZE100_SW3AVOL, 1, buf, 1))
            return 1;

        /* Enable DDR3 1.35V output on SW3A */
        buf[0] &= ~PMIC_SW34_MASK;
        buf[0] |= PMIC_SW34_1P35V;

        if (i2c_write(I2C_PMIC_ADDR, PFUZE100_SW3AVOL, 1, buf, 1))
            return 1;

        if (i2c_read(I2C_PMIC_ADDR, PFUZE100_SW3BVOL, 1, buf, 1))
            return 1;

        /* Enable DDR3 1.35V output on SW3B */
        buf[0] &= ~PMIC_SW34_MASK;
        buf[0] |= PMIC_SW34_1P35V;

        if (i2c_write(I2C_PMIC_ADDR, PFUZE100_SW3BVOL, 1, buf, 1))
            return 1;
    }

    return 0;
}

static void clock_init_valenka(void)
{
    struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

    if (splHandoverData->boardVersion == VALENKA_IMX6Q_SSPPM ||
        splHandoverData->boardVersion == VALENKA_IMX6Q_MOUNT)
    {
        /* Valenka3 - The clock tree defaults to 528 Mhz */
        if ((splHandoverData->somVersion != VALENKA_3_2GB_SOM) &&
            (splHandoverData->somVersion != VALENKA_3_4GB_SOM))
        {
            /* Valenka2 - Set DDR clock to 400 Mhz */
            writel(0x00060324, &ccm->cbcmr);
        }

        /*
	     * Set CCOSR back to reset value (undo the initialization in clock_init)
         */
        writel(0x000a0001, CCM_CCOSR);
	}

    /* AHB_ROOT_CLK change divide ratio from 4 to 3 for ENET */
    writel(0x00018900, &ccm->cbcdr);

    /* Enable SPI5 clock */
    writel(0x0030FF03, &ccm->CCGR1);
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

    if ( !isValenka() && !(cause & (RST_IPP | RST_POR | RST_WARM )))
    {
        post_boot_mode = post_bootmode_get(0);

        /* do not reset the other controller when in POST */
        if ( post_boot_mode != 0 && ! (post_boot_mode & POST_POWERTEST)) {

            puts("reseting uP2\n");

            regs = (struct gpio_regs *) RESET_GPIO_BASE_ADDR;

            writel((0 << RESET_GPIO_INDEX), &regs->gpio_dr);
            writel((1 << RESET_GPIO_INDEX), &regs->gpio_dir);
            udelay(500);
            writel((1 << RESET_GPIO_INDEX), &regs->gpio_dr);
            writel((0 << RESET_GPIO_INDEX), &regs->gpio_dir);
            puts("reseting uP2 done\n");

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

    pmic_disable_swbst();

    if (isValenka())
    {
        pmic_init_valenka();
        clock_init_valenka();
    }

    /* configure MMDC for SDRAM width/size and calibration */
    spl_dram_init(splHandoverData->boardVersion);

    /* Set up early MMU tables at the beginning of DRAM and start d-cache */
	gd->arch.tlb_addr = MMDC0_ARB_BASE_ADDR + SZ_32M;
	gd->arch.tlb_size = PGTABLE_SIZE;
	enable_caches();
}

void *board_fdt_blob_setup(int *err)
{
    *err = 0;
    void *fdt_blob = _image_binary_end;

    return fdt_blob;
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
void __noreturn jump_to_image_no_args(struct spl_image_info *spl_image)
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
    /* This method is called right before jump_to_image_no_args */
    flush_dcache_all();
}
