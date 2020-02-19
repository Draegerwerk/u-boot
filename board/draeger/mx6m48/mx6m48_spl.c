/*
 * mx6m48_spl.c
 * ideas taken from gw_ventana_spl.c
 * Copyright (C) 2014 Gateworks Corporation
 * Author: Tim Harvey <tharvey@gateworks.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 *
 */
#include <common.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/hab.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/gpio.h>
#include <spl.h>
#include <fsl_esdhc.h>
#include "mx6m48_spl_cfg.h"

extern struct fsl_esdhc_cfg usdhc_cfg[];

#define I2C_PAD_CTRL  (PAD_CTL_PUS_100K_UP |                \
    PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |   \
    PAD_CTL_ODE | PAD_CTL_SRE_FAST)
#define I2C_PAD MUX_PAD_CTRL(I2C_PAD_CTRL)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |            \
        PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |         \
        PAD_CTL_SRE_FAST  | PAD_CTL_HYS)
#define USDHC_PAD MUX_PAD_CTRL(USDHC_PAD_CTRL)


DECLARE_GLOBAL_DATA_PTR;

struct m48_board_info {
    u8 partno[7];        /* 0x1e: part number */
    u8 revision[2];      /* 0x0C: revision */
    u8 end;              /* string delimiter */
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

static iomux_v3_cfg_t const spl_usdhc3_pads[] = {
    IOMUX_PADS(PAD_SD3_CLK__SD3_CLK   | USDHC_PAD),
    IOMUX_PADS(PAD_SD3_CMD__SD3_CMD   | USDHC_PAD),
    IOMUX_PADS(PAD_SD3_DAT0__SD3_DATA0 | USDHC_PAD),
    IOMUX_PADS(PAD_SD3_DAT1__SD3_DATA1 | USDHC_PAD),
    IOMUX_PADS(PAD_SD3_DAT2__SD3_DATA2 | USDHC_PAD),
    IOMUX_PADS(PAD_SD3_DAT3__SD3_DATA3 | USDHC_PAD),
};

typedef enum {
    M48_UNKNOWN,
    M48_IMX6Q_REV06,
    M48_IMX6DL_REV06,
    M48_IMX6Q_REV08,
    M48_IMX6DL_REV08,
} M48_BOARD_VERSION;

#define SLAVE_ADDR 0x7f // does not matter, parameter unused in mxc_i2c driver
static void i2c_setup_iomux(void)
{
    if (is_cpu_type(MXC_CPU_MX6Q)) {
        setup_i2c(1, CONFIG_SYS_I2C_SPEED, SLAVE_ADDR, &mx6q_i2c_pad_info1);
    }
    else {
        setup_i2c(1, CONFIG_SYS_I2C_SPEED, SLAVE_ADDR, &mx6dl_i2c_pad_info1);
    }
}

M48_BOARD_VERSION read_eeprom(int bus, struct m48_board_info *info)
{
    int revision = 0;
    unsigned char *buf = (unsigned char *)info;
    M48_BOARD_VERSION board_model = M48_UNKNOWN;

    if (0 != i2c_set_bus_num(bus) ||
        0 != i2c_probe(CONFIG_SYS_I2C_EEPROM_ADDR) ||
        0 != i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR, CONFIG_SYS_I2C_PART_NO_OFFSET, 1, buf, sizeof(*info)))
    {
        printf("\nEEPROM @ 0x%02x read FAILED!!!\n",
                CONFIG_SYS_I2C_EEPROM_ADDR);
    } else {

        info->end = 0;
        revision = simple_strtoul((const char*) info->revision, NULL, 10);

        if (memcmp(info->partno, "8421921", sizeof(info->partno)) == 0) {

            board_model = M48_IMX6Q_REV08;

        } else if (memcmp(info->partno, "8421901", sizeof(info->partno)) == 0) {

            if (is_cpu_type(MXC_CPU_MX6Q)) board_model = M48_IMX6Q_REV06;
            else {
                if (revision < 8)  board_model = M48_IMX6DL_REV06;
                else               board_model = M48_IMX6DL_REV08;
            }

        } else if (memcmp(info->partno, "8421911", sizeof(info->partno)) == 0) {

            if (revision < 2)  board_model = M48_IMX6DL_REV06;
            else               board_model = M48_IMX6DL_REV08;

        } else {

            if (is_cpu_type(MXC_CPU_MX6Q)) board_model = M48_IMX6Q_REV08;
            else board_model = M48_IMX6DL_REV08;

        }
    }
    return board_model;
}

int board_mmc_getcd(struct mmc *mmc)
{
    return 1;
}

int board_mmc_init(bd_t *bis)
{
    s32 status = 0;

    SETUP_IOMUX_PADS(spl_usdhc3_pads);

    usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
    usdhc_cfg[0].max_bus_width = 4;

    status = fsl_esdhc_initialize(bis, &usdhc_cfg[0]);

    return status;
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
        case M48_IMX6Q_REV08:
            configure_registers(ddr_setup_mx6q, ddr_setup_mx6q_size);
            configure_registers(ram_calibration_mx6q_08, ram_calibration_mx6q_size_08);
            configure_registers(ram_setup_mx6q, ram_setup_mx6q_size);
            break;
        case M48_IMX6DL_REV08:
            configure_registers(ddr_setup_mx6dl, ddr_setup_mx6dl_size);
            configure_registers(ram_calibration_mx6dl_08, ram_calibration_mx6dl_size_08);
            configure_registers(ram_setup_mx6dl, ram_setup_mx6dl_size);
            break;
        case M48_IMX6Q_REV06:
            configure_registers(ddr_setup_mx6q, ddr_setup_mx6q_size);
            configure_registers(ram_calibration_mx6q_06, ram_calibration_mx6q_size_06);
            configure_registers(ram_setup_mx6q, ram_setup_mx6q_size);
            break;
        case M48_IMX6DL_REV06:
            configure_registers(ddr_setup_mx6dl, ddr_setup_mx6dl_size);
            configure_registers(ram_calibration_mx6dl_06, ram_calibration_mx6dl_size_06);
            configure_registers(ram_setup_mx6dl, ram_setup_mx6dl_size);
            break;
        default:
            break;
    }

    /* wait for one complete refresh cycle */
    mdelay(70);
}

void board_init_f(ulong dummy)
{
    struct m48_board_info m48_info;
    M48_BOARD_VERSION board_model;
    char revision_0;

#ifdef DEBUG_SPL_WAIT_FOR_JTAG_DEBUGGER
    volatile ulong debugger_is_attached;
    debugger_is_attached = 0;
    while(debugger_is_attached == 0)
    { /* attach with your debugger and manually set variable to 1 */ }
#endif
    /*
     * Zero out global data:
     *  - this shoudl be done by crt0.S
     *  - failure to zero it will cause i2c_setup to fail
     */
    memset((void *)gd, 0, sizeof(struct global_data));

    get_clocks();

    /* setup AIPS and disable watchdog */
    arch_cpu_init();

    board_early_init_f();

    /* setup GP timer */
    timer_init();

    /* UART clocks enabled and gd valid - init serial console */
    preloader_console_init();


    /* iomux and setup of i2c */
    i2c_setup_iomux();


    /* read/validate EEPROM info to determine board model and SDRAM cfg */
    memset(&m48_info, '?', sizeof(m48_info));
    m48_info.end = 0;
    board_model = read_eeprom(1, &m48_info);

    if (board_model == M48_UNKNOWN) {
        /* As already stated in read_eeprom, the read of eeprom failed! */
        printf("Unknown board model! Will halt now.\n");
        reset_cpu(0);
    }

    /* configure MMDC for SDRAM width/size and calibration */
    spl_dram_init(board_model);

    revision_0 = m48_info.revision[0];
    m48_info.revision[0] = 0;
    printf("PartNo: %s Rev.%c%c model %d\n",
        m48_info.partno,
        revision_0,
        m48_info.revision[1],
        board_model);

    /* Clear the BSS. */
    memset(__bss_start, 0, __bss_end - __bss_start);

    /* load/boot image from boot device */
    board_init_r(NULL, 0);
}

void __noreturn jump_to_image_no_args(struct spl_image_info *spl_image)
{
    typedef void __noreturn (*image_entry_noargs_t)(void);

    image_entry_noargs_t image_entry =
            (image_entry_noargs_t) spl_image->entry_point;

    debug("M48 image entry point: 0x%X\n", spl_image->entry_point);

    if ( ! authenticate_image(spl_image->load_addr, spl_image->size)) {
        printf("M48 image authentication failed\n");
        reset_cpu(0);
    }

    image_entry();
}

void reset_cpu(ulong addr)
{
    /* this will reset the cpu when watchdog is enabled, if not, it stops here */
    while (1) {
    }
}
