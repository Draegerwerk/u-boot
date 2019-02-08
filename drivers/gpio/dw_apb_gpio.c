/* dw_apb_gpio.c
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 * Driver for Synopsys® DesignWare® APB General Purpose Programming I/O
 *
 * Note: partially implemented as far as needed for Draeger M48 board_reset POST
 */

#include <common.h>
#include <asm/gpio.h>

#define NUM_GPIO_CTRL        3
#define PINS_PER_CONTROLLER	29

#define NUM_SYNC_CTRL        2
#define PINS_PER_SYNC_CTRLR  2
#define SWPORTA_DR_OFFSET               (0x00)
#define SWPORTA_DDR_OFFSET              (0x04)
#define EXT_PORTA_OFFSET                (0x50)

#define GPIO3_FPGA_BASE_ADDR 0xFF202000 /* gpioDpramSyncIn */
#define GPIO4_FPGA_BASE_ADDR 0xFF202020 /* gpioDpramSyncOut */
/* GPIO port description */
static unsigned long gpio_ports[] = {
    [0] = GPIO0_BASE_ADDR,
    [1] = GPIO1_BASE_ADDR,
    [2] = GPIO2_BASE_ADDR,
    [3] = GPIO3_FPGA_BASE_ADDR,
    [4] = GPIO4_FPGA_BASE_ADDR,
};

int get_gpio_port(unsigned gpio)
{
    unsigned int port = gpio / PINS_PER_CONTROLLER;
    unsigned int sync_port;

    if (port >= NUM_GPIO_CTRL) {
        sync_port = (gpio - (NUM_GPIO_CTRL * PINS_PER_CONTROLLER)) / PINS_PER_SYNC_CTRLR;
        port = NUM_GPIO_CTRL + sync_port;
    }

    if (port >= NUM_GPIO_CTRL + NUM_SYNC_CTRL)
        port = -1;

    return port;
}

unsigned int get_pin_mask (unsigned gpio)
{
    unsigned int pinMask;
    int port = get_gpio_port (gpio);

    if (port < NUM_GPIO_CTRL) {
        pinMask = 1 << (gpio % PINS_PER_CONTROLLER);
    } else {
        pinMask = 1 << ((gpio - (NUM_GPIO_CTRL * PINS_PER_CONTROLLER)) % PINS_PER_SYNC_CTRLR);
    }

    return pinMask;
}

int gpio_request(unsigned gpio, const char *label)
{
    int port = get_gpio_port (gpio);

    if (port < 0)
        return -1;

    return 0;
}

int gpio_set_value(unsigned gpio, int value)
{
    unsigned long baseAddr;
    unsigned int  oldVal;
    unsigned int  pinMask;
    int  port     = get_gpio_port (gpio);

    if (port < 0)
        return -1;

    pinMask  = get_pin_mask (gpio);

    baseAddr = gpio_ports[port];
    /* write output */
    oldVal = readl (baseAddr + SWPORTA_DR_OFFSET);
    if (value == 0) { /*low*/
        oldVal &= ~pinMask;
    } else {
        oldVal |= pinMask;
    }
    writel (oldVal, baseAddr + SWPORTA_DR_OFFSET);

    return 0;
}

int gpio_get_value(unsigned gpio)
{
    unsigned long baseAddr;
    unsigned int  val;
    unsigned int  pinMask;
    int  port     = get_gpio_port (gpio);

    if (port < 0)
        return -1;

    baseAddr = gpio_ports[port];

    if (port < NUM_GPIO_CTRL)
        val = readl (baseAddr + EXT_PORTA_OFFSET);
    else
        val = readl (baseAddr + SWPORTA_DR_OFFSET);

    pinMask  = get_pin_mask (gpio);

    if  (val & pinMask) return 1;

    return 0;
}

int gpio_direction_input(unsigned gpio)
{
    unsigned long baseAddr;
    unsigned int  pinMask;
    int  port     = get_gpio_port (gpio);

    if (port < 0)
        return -1;

    pinMask  = get_pin_mask (gpio);
    baseAddr = gpio_ports[port];

    /* set to output */
    writel (readl (baseAddr + SWPORTA_DDR_OFFSET) & ~(pinMask), baseAddr + SWPORTA_DDR_OFFSET);

    return 0;
}


int gpio_direction_output(unsigned gpio, int value)
{
    unsigned long baseAddr;
    unsigned int  pinMask;
    int  port     = get_gpio_port (gpio);

    if (port < 0)
        return -1;

    if (gpio_set_value(gpio, value) < 0)
        return -1;

    pinMask  = get_pin_mask (gpio);
    baseAddr = gpio_ports[port];

    /* set to output */
    writel (readl (baseAddr + SWPORTA_DDR_OFFSET) | pinMask, baseAddr + SWPORTA_DDR_OFFSET);

    return 0;
}


int gpio_free(unsigned gpio)
{
    return 0;
}
