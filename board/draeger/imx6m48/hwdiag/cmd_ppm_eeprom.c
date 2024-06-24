/**
 *
 * @file "cmd_ppm_eeprom.c"
 *
 * @brief This provides an implementation of PPM PI board eeprom access routines
 *        for u-boot.
 *
 * @copyright Copyright 2022 Draeger and Licensors. All Rights Reserved.
 *
 */

#include <common.h>
#include <errno.h>
#include <command.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <linux/libfdt.h>
#include <linux/delay.h>
#include <u-boot/crc.h>
#include <fdt_support.h>
#include <env.h>
#include <net.h>
#include <dm/uclass.h>
#include <i2c.h>
#include "cmd_ppm_eeprom.h"

#define PPM_EEPROMINFO_DEBUG_ENABLE 

#ifdef PPM_EEPROMINFO_DEBUG_ENABLE

/**
 * @brief ppm_eeprom debug flag; set to tru to show debug output
 */
bool ppm_eeprom_debug = false;

#define PPM_EEPROMINFO_DEBUG(fmt, ...)          \
    do {                                        \
        if (ppm_eeprom_debug)                   \
        {                                       \
            printf(fmt, ##__VA_ARGS__ );        \
        }                                       \
    } while (0)
#else
#define PPM_EEPROMINFO_DEBUG(fmt, ...)
#endif

/* Fwd reference */
int ppm_eeprom_read_mac(uint32_t eeprom_index, uint8_t *mac);

#define MAX_MUXES (2) /* maximum # of muxes */

#define AMOUNT_INCREASE_FDT_SIZE      (512)
#define PPM_EEPROMINFO_STRING_LENGTH   (32)

#define PPM_EEPROMINFO_ERROR           (-1)

/**
 * @brief I2C access including write protect pin management
 *
 * A global variables is defined that stores all of the data needed to access
 * the current eeprom device.
 */
struct ppm_eeprom_access {
    ppm_eeprom_t                *eeprom;    /* ptr to eeprom */
    PPM_EEPROMINFO_LOCATION_ID  location;   /* location ID   */

    u32     i2c_bus;         /* I2C bus eeprom found on                       */
    u32     i2c_address;     /* I2C address of selected eeprom                */
    u32     i2c_address_len; /* I2c address length for selected eeprom        */
    u32     i2c_page_mask;   /* I2C eeprom page mask (e.g.,0x1f==32 byte page)*/
    u32     i2c_page_size;   /* I2C eeprom max write size (in bytes)          */
    u32     i2c_tWR;         /* I2C EEPROM post-write delay (in microseconds) */
    
    struct ppm_eeprom_mux {
        u32     address;    /* I2C address of mux                    */
        u32     leg;        /* which leg of mux is EEPROM on         */
    } muxes[MAX_MUXES];
    unsigned int numMuxes;  /* # of muxes */

    struct ppm_eeprom_wp {
        bool    has_gpio_wp;            /* true if board has programmable    */
                                        /* EEPROM WP pin                     */
        bool    gpio_configured;        /* true if WP pin is configured      */
        u32     pin;                    /* gpio pin to pull to disable write */
                                        /* protect                           */
        u32     enable_write_value;     /* gpio value to enable writing      */
        u32     disable_write_value;    /* gpio value to disable writing     */
        bool    write_enabled;          /* true if writing is enabled        */
    } wp;
} ppm_eeprom_access;

/**
 * @brief macro to generate CRC of data block
 */
#define GENERATE_CRC_VALUE(block_address)  crc32(0, (const u8 *)&(block_address)->data, sizeof((block_address)->data));

/**
 * @brief eeprom data and valid flag
 */
ppm_eeprom_t    ppm_eeprom_som;         /* EEPROM data for SOM     */
ppm_eeprom_t    ppm_eeprom_pi;          /* EEPROM data for PI      */
ppm_eeprom_t    ppm_eeprom_afe1;        /* EEPROM data for AFE1    */
ppm_eeprom_t    ppm_eeprom_afe2;        /* EEPROM data for AFE2    */
ppm_eeprom_t    ppm_eeprom_radio;       /* EEPROM data for Radio   */
ppm_eeprom_t    ppm_eeprom_ui;          /* EEPROM data for UI      */
ppm_eeprom_t    ppm_eeprom_battery;     /* EEPROM data for Battery */
ppm_eeprom_t    ppm_eeprom_olm;         /* EEPROM data for OLM     */

/**
 * @brief pointer to current selected eeprom
 */
ppm_eeprom_t *eeprom;

/**
 * @brief MAC EEPROM Table
 */
struct {
    char                    *name;                  /* canonical name */
    u32                     i2c_bus;                /* I2C bus */
    u32                     i2c_address;            /* I2C address */
    struct ppm_eeprom_mux   i2c_muxes[MAX_MUXES];   /* I2C multiplexers */
    unsigned int            numMuxes;               /* # of muxes */
} macEeproms[] = {

/* MAC EEPROMs for both Mount and SSPPM */
    {
        /* MAC EEPROM on I2C3-7 */
        "PI",
        2, 0x54,
        { {0x70, 8} }, 1
    },
/* MAC EEPROMs on SSPPM (ssppm can access I2C3) */
    {
        /* WiFi MAC EEPROM on I2C2-1 */
        "WiFi",
        1, 0x51,
        { {0x74, 2} }, 1
    }
};

/* Index definitions for the macEeproms[] array */
#define ENET_MAC_ADDR_INDEX 0
#define WIFI_MAC_ADDR_INDEX 1

/**
 * @brief Table of Devices
 *
 * This is a statically allocated list of devices.  Each device has a name and
 * a board ID.  The board ID is used to lookup I2C device addresses.
 * 
 * @see "cmd_ppm_eeprom.h" for ID macro defines, max string length macros, etc.
 */
struct board_id_entry {
    char                        *name;      /* device name */
    char                        *partNum;   /* Part Number of the board */
    PPM_EEPROMINFO_BOARD_ID     board;      /* board  ID */
} ppm_eeprom_board_id_table[] =
{
    {
        "1GB DUAL SOM",
        "UNKNOWN",
        PPM_EEPROMINFO_BOARD_SOM_1GB_DUAL,
    },
    {
        "2GB DUAL SOM",
        "UNKNOWN",
        PPM_EEPROMINFO_BOARD_SOM_2GB_DUAL,
    },
    {
        "1GB QUAD SOM",
        "UNKNOWN",
        PPM_EEPROMINFO_BOARD_SOM_1GB_QUAD,
    },
    {
        "2GB QUAD SOM",
        "MS40605",
        PPM_EEPROMINFO_BOARD_SOM_2GB_QUAD,
    },
    {
        "SSPPM",
        "MS34821",
        PPM_EEPROMINFO_BOARD_SSPPM_PI,
    },
    {
        "TSPPM-M",
        "TBD",
        PPM_EEPROMINFO_BOARD_TSPPM_M_PI,
    },
    {
        "TSPPM-B",
        "TBD",
        PPM_EEPROMINFO_BOARD_TSPPM_B_PI,
    },
    {
        "MOUNT",
        "MS34381",
        PPM_EEPROMINFO_BOARD_MOUNT_PI,
    },
    {
        "AFE1",
        "MS33121",
        PPM_EEPROMINFO_BOARD_AFE1    /* AFE = "Analogue Front End" */
    },
    {
        "AFE2",
        "MS34631",
        PPM_EEPROMINFO_BOARD_AFE2
    },
    {
        "WCM",
        "MS40251",
        PPM_EEPROMINFO_BOARD_RADIO
    },
    {
        "UIM",
        "MS34711",
        PPM_EEPROMINFO_BOARD_UIM    /* UIM = "user interface module" */
    },
    {
        "BIM",
        "MS34681",
        PPM_EEPROMINFO_BOARD_BIM
    },
    {
        "OLM",
        "MS34691",
        PPM_EEPROMINFO_BOARD_OLM
    },
    {
        "4GB QUAD SOM",
        "MS40411",
        PPM_EEPROMINFO_BOARD_SOM_4GB_QUAD,
    },
};


static bool ppm_eeprom_board_name(const char *partNumber, const char **str)
{
    struct board_id_entry *p;
    int idx;

    for (idx = 0; idx < ARRAY_SIZE(ppm_eeprom_board_id_table); ++idx)
    {
        p = &ppm_eeprom_board_id_table[idx];
        if (strncmp(p->partNum, partNumber, PPM_EEPROMINFO_PN_LENGTH) == 0)
        {
            *str = p->name;
            return true;
        }
    }
    *str = "???";
    return false;
}

struct location_id_entry {
    char *name;
    PPM_EEPROMINFO_LOCATION_ID location;
} ppm_eeprom_location_id_table[] =
{
    {
        "SOM",
        PPM_EEPROMINFO_LOCATION_SOM,
    },
    {
        "PI",
        PPM_EEPROMINFO_LOCATION_PI,
    },
    {
        "AFE1",
        PPM_EEPROMINFO_LOCATION_AFE1
    },
    {
        "AFE2",
        PPM_EEPROMINFO_LOCATION_AFE2
    },
    {
        "WCM",
        PPM_EEPROMINFO_LOCATION_RADIO
    },
    {
        "UIM",
        PPM_EEPROMINFO_LOCATION_UIM
    },
    {
        "BIM",
        PPM_EEPROMINFO_LOCATION_BIM
    },
    {
        "OLM",
        PPM_EEPROMINFO_LOCATION_OLM
    }
};

static bool ppm_eeprom_location_name(PPM_EEPROMINFO_LOCATION_ID location, const char **str)
{
    struct location_id_entry *p;
    int idx;

    for (idx = 0; idx < ARRAY_SIZE(ppm_eeprom_location_id_table); ++idx)
    {
        p = &ppm_eeprom_location_id_table[idx];
        if (p->location == location)
        {
            *str = p->name;
            return true;
        }
    }
    *str = "???";
    return false;
}

/**
 * ppm_eeprom_read() - read data from selected eeprom
 * @offset:	Offset from start of eeprom
 * @ptr:	Pointer to buffer to read into
 * @len:	Number of bytes to read
 *
 * Returns zero on success, negative on error
 */
static int ppm_eeprom_read(u32 offset, void *ptr, u32 len)
{
    struct udevice *i2c_dev;
    struct udevice *i2c_bus;
    int    ret;

    uclass_get_device_by_seq(UCLASS_I2C, ppm_eeprom_access.i2c_bus, &i2c_bus);
    ret = i2c_get_chip(i2c_bus, ppm_eeprom_access.i2c_address, 1, &i2c_dev);
    if (!ret)
    {
		/*
		 * The actual address length is set here rather than in the above
		 * i2c_get_chip call (where it is set to 1) because if this is not
		 * done in 2 steps reading the SOM EEPROM fails!?
		 */
        ret = i2c_set_chip_offset_len(i2c_dev, ppm_eeprom_access.i2c_address_len);
    }
    if (!ret)
    {
        return dm_i2c_read(i2c_dev, offset, ptr, len);
    }
	return ret;
}

/**
 * ppm_eeprom_write() - write data to selected eeprom
 * @offset:	Offset from start of eeprom
 * @ptr:	Pointer to buffer to write
 * @len:	Number of bytes to write
 *
 * Returns zero on success, negative on error
 */
static int ppm_eeprom_write(u32 offset, void *ptr, u32 len)
{
    uchar *curr_ptr = (uchar *)ptr;
    u32 left_to_write;
    u32 left_in_page;
    int ret;
    struct udevice *i2c_dev;
    struct udevice *i2c_bus;

    uclass_get_device_by_seq(UCLASS_I2C, ppm_eeprom_access.i2c_bus, &i2c_bus);
    ret = i2c_get_chip(i2c_bus, ppm_eeprom_access.i2c_address, 1, &i2c_dev);
    if (!ret)
    {
        ret = i2c_set_chip_offset_len(i2c_dev, ppm_eeprom_access.i2c_address_len);
    }
    if (ret)
    {
        return ret;
    }

    PPM_EEPROMINFO_DEBUG("%s:%d offset %u ptr %p len %u\n", __func__, __LINE__, offset, ptr, len);

    do {
        left_to_write = len;
        PPM_EEPROMINFO_DEBUG("%s:%d offset %u len %u => %u ?= %u\n",
                             __func__, __LINE__,
                             offset, left_to_write,
                             (offset & (ppm_eeprom_access.i2c_page_size - 1)),
                             ((offset + left_to_write) & (ppm_eeprom_access.i2c_page_size - 1)));

        /* Trim write to a single page (where upper bits of offset
         * are fixed over all the bytes of a write command) */
        if ((offset & (ppm_eeprom_access.i2c_page_size - 1)) != ((offset + left_to_write) & (ppm_eeprom_access.i2c_page_size - 1)))
        {
            PPM_EEPROMINFO_DEBUG("%s:%d\n", __func__, __LINE__);
            /* data to write spans a page in eeprom, limit write
             * to what fits up to end of page */
            left_in_page = (ppm_eeprom_access.i2c_page_mask + 1) - (offset & ppm_eeprom_access.i2c_page_mask);
            PPM_EEPROMINFO_DEBUG("%s:%d left_to_write %u left_in_page %u\n", __func__, __LINE__, left_to_write, left_in_page);

            if (left_to_write > left_in_page)
            {
                left_to_write = left_in_page;
            }
        }

        PPM_EEPROMINFO_DEBUG("%s:%d offset %u ptr %p len %u\n", __func__, __LINE__, offset, curr_ptr, left_to_write);
        ret = dm_i2c_write(i2c_dev, offset, curr_ptr, left_to_write);
        if (ret)
        {
            if (ret == -ENODEV)
            {
                printf("PPM EEPROM at %s is write-protected\n", ppm_eeprom_access.eeprom->name);
            }
            else
            {
                PPM_EEPROMINFO_DEBUG("%s: error writing to address 0x%02x, offset 0x%04x, len 0x%x return %d\n", __func__, ppm_eeprom_access.i2c_address, offset, left_to_write, ret);
            }
            return ret;
        }

        /* Wait for write to complete (e.g. wait for tWR to pass) */
        udelay(ppm_eeprom_access.i2c_tWR);

        curr_ptr += left_to_write;
        offset += left_to_write;
        len -= left_to_write;
    } while (len);
    return 0;
}

static void ppm_eeprom_reset_layout(ppm_eeprom_layout_crc_t *layout)
{
    uint32_t crc_val;

	/* Provide default items here */
	layout->data.version = __cpu_to_be32(PPM_EEPROMINFO_LAYOUT_VERSION);

	/* Generate CRC of layout */
	crc_val = GENERATE_CRC_VALUE(layout);
	layout->crc = __cpu_to_be32(crc_val);
}

static int ppm_eeprom_validate_layout(ppm_eeprom_t *eeprom)
{
	ppm_eeprom_layout_crc_t *layout;
	bool invalid = false;
    uint32_t crc_val;

	layout = &eeprom->content.layout;

	crc_val = GENERATE_CRC_VALUE(layout);
    PPM_EEPROMINFO_DEBUG("%s:%d %08x ?= %08x\n", __func__, __LINE__, crc_val, __be32_to_cpu(layout->crc));
    if (crc_val != __be32_to_cpu(layout->crc))
	{
		PPM_EEPROMINFO_DEBUG("%s:%d Layout CRC %08x != expected %08x\n",
							 __func__, __LINE__, crc_val, __be32_to_cpu(layout->crc));
		invalid = true;
	}
	else if ((layout->data.version == __cpu_to_be32(PPM_EEPROMINFO_LAYOUT_VERSION_ELV6)) ||
             (layout->data.version == __cpu_to_be32(PPM_EEPROMINFO_LAYOUT_VERSION)))
    {
		PPM_EEPROMINFO_DEBUG("%s:%d Layout version %08x is good\n", 
                             __func__, __LINE__, layout->data.version);
	}
    else if (layout->data.version > __cpu_to_be32(PPM_EEPROMINFO_LAYOUT_VERSION))
    {
		PPM_EEPROMINFO_DEBUG("%s:%d Layout version is new, good, allow booting as this could occur\n", 
                             __func__, __LINE__);
	}
	else
    {
		PPM_EEPROMINFO_DEBUG("%s:%d Unexpected Layout version %08x != expected %08x\n",
                             __func__, __LINE__, layout->data.version, PPM_EEPROMINFO_LAYOUT_VERSION);
		invalid = true;
	}

	if (invalid)
	{
        /* CRC/version is invalid; fill in initial content */
        PPM_EEPROMINFO_DEBUG("PPM EEPROM layout content invalid; initializing\n");

		ppm_eeprom_reset_layout(layout);
		eeprom->dirty.layout = true;
    }
    else
        eeprom->valid.layout = true;

    return 0;
}

static void ppm_eeprom_reset_board(ppm_eeprom_board_crc_t *board)
{
    uint32_t crc_val;

	/* Provide default items here */
	memset(board->data.hwRevision, 0, sizeof(board->data.hwRevision));
	board->data.hwTweakBits = __cpu_to_be32(0x00000000);
	board->data.swCompatibility = __cpu_to_be16(0);
	board->data.partNumber[0] = '\0';
	board->data.serialNumber[0] = '\0';

	/* Generate CRC of board */
	crc_val = GENERATE_CRC_VALUE(board);
	board->crc = __cpu_to_be32(crc_val);
}

static void ppm_eeprom_reset_system(ppm_eeprom_system_crc_t *system)
{
    uint32_t crc_val;

	/* Provide default items here */
	system->data.hwRevision[0] = '-';
	system->data.hwRevision[1] = '0';
	system->data.hwRevision[2] = '0';
	system->data.partNumber[0] = '\0';
	system->data.serialNumber[0] = '\0';

	/* Generate CRC of system */
	crc_val = GENERATE_CRC_VALUE(system);
	system->crc = __cpu_to_be32(crc_val);
}

static int ppm_eeprom_validate_board(ppm_eeprom_t *eeprom)
{
	ppm_eeprom_board_crc_t *board;
	bool invalid = false;
    uint32_t crc_val;

	board = &eeprom->content.board;

	crc_val = GENERATE_CRC_VALUE(board);
    if (crc_val != __be32_to_cpu(board->crc))
    {
        /* CRC is invalid; fill in initial content */
        PPM_EEPROMINFO_DEBUG("%s:%d crc %08x != expected %08x\n", __func__, __LINE__,
							 crc_val, __be32_to_cpu(board->crc));
		invalid = true;
	}

	/* verify content */
    /* revision out of range... */
	if (!invalid)
	{
		if ((board->data.hwRevision[0] != '-') ||
            (board->data.hwRevision[1] < '0')  ||
            (board->data.hwRevision[1] > '9')  ||
            (board->data.hwRevision[2] < '0')  ||
            (board->data.hwRevision[2] > '9'))
		{
			PPM_EEPROMINFO_DEBUG("%s:%d hwRevision is invalid\n", __func__, __LINE__);
			invalid = true;
		}
    }

	if (invalid)
	{
		ppm_eeprom_reset_board(board);
		eeprom->dirty.board = true;
    }
	eeprom->valid.board = true;
    return 0;
}

static int ppm_eeprom_validate_system(ppm_eeprom_t *eeprom)
{
	ppm_eeprom_system_crc_t *system;
	bool invalid = false;
    uint32_t crc_val;

	system = &eeprom->content.system;

	crc_val = GENERATE_CRC_VALUE(system);
    if (crc_val != __be32_to_cpu(system->crc))
    {
        /* CRC is invalid; fill in initial content */
        PPM_EEPROMINFO_DEBUG("%s:%d crc %08x != expected %08x\n", __func__, __LINE__,
							 crc_val, __be32_to_cpu(system->crc));
		invalid = true;
	}

	/* verify content */
    /* revision out of range... */
	if (!invalid)
	{
		if ((system->data.hwRevision[0] != '-') ||
            (system->data.hwRevision[1] < '0')  ||
            (system->data.hwRevision[1] > '9')  ||
            (system->data.hwRevision[2] < '0')  ||
            (system->data.hwRevision[2] > '9'))
		{
			PPM_EEPROMINFO_DEBUG("%s:%d ppmHwRevision is invalid\n", __func__, __LINE__);
			invalid = true;
		}
    }

	eeprom->valid.system = true;

	if (invalid)
	{
		ppm_eeprom_reset_system(system);
		eeprom->dirty.system = true;
        return 0;
    }

    return 1;
}

static int ppm_eeprom_select_mux(int bus_nr, struct ppm_eeprom_mux *mux)
{
    int ret = 0;
    u8 mask;
    struct udevice *i2c_bus;
    struct udevice *i2c_dev;

    PPM_EEPROMINFO_DEBUG("%s:%d bus_nr %d mux->address %d\n", __func__, __LINE__, bus_nr, mux->address);
    /* probe mux device */
    uclass_get_device_by_seq(UCLASS_I2C, bus_nr, &i2c_bus);
    if (dm_i2c_probe(i2c_bus, mux->address, 0, &i2c_dev))
    {
        printf("Failed to probe mux at 0x%02X\n", mux->address);
        return (PPM_EEPROMINFO_ERROR);
    }

    if (mux->leg < 1 || mux->leg > 8)
    {
        printf("Invalid mux leg %u\n", mux->leg);
        return (PPM_EEPROMINFO_ERROR);
    }
    /* Convert from leg into mask */
    mask = 1 << (mux->leg - 1);

    /* first byte of message (no address) sets the mux */
    ret = dm_i2c_write(i2c_dev, 0x00, &mask, sizeof(mask));
    if (ret)
    {
        printf("%s:%d i2c_write 0x%02x to address 0x%x failed with %d\n",
               __func__, __LINE__, mask, mux->address, ret);
    }
    return ret;
}

static int ppm_eeprom_select_muxes(int bus_nr,
                                   struct ppm_eeprom_mux *muxes,
                                   unsigned int numMuxes)
{
    unsigned int i;
    int ret;

    /* select all muxes */
    for (i = 0; i < numMuxes; i++)
    {
        /* select mux */
        ret = ppm_eeprom_select_mux(bus_nr, &muxes[i]);

        /* check value */
        if (ret)
        {
            return ret;
        }
    }

    return 0;
}

static int ppm_eeprom_write_enable(struct ppm_eeprom_wp *wp, bool enable)
{
	if (!wp->has_gpio_wp)
	{
		/* EEPROM has no programmable write protect pin; assume EEPROM
		 * is write enabled; if not, then i2c_write should fail */
		wp->write_enabled = true;
		wp->gpio_configured = true;
		return 0;
	}

	if (!wp->gpio_configured)
	{
		gpio_request(wp->pin, "wp");
		if (enable)
		{
			gpio_direction_output(wp->pin, wp->enable_write_value);
		}
		else
		{
			gpio_direction_output(wp->pin, wp->disable_write_value);
		}
		wp->gpio_configured = true;
		wp->write_enabled = enable;
		return 0;
	}

	if (enable)
	{
		gpio_direction_output(wp->pin, wp->enable_write_value);
	}
	else
	{
		gpio_direction_output(wp->pin, wp->disable_write_value);
	}
	wp->write_enabled = enable;

	return 0;
}

static int ppm_eeprom_init(void)
{
    struct ppm_eeprom_access *access = &ppm_eeprom_access;
    const char *name;
    bool valid;
    int ret;

	PPM_EEPROMINFO_DEBUG("%s:%d access->location %d\n", __func__, __LINE__, access->location);

	valid = ppm_eeprom_location_name(access->location, &name);
	if (!valid)
	{
		printf("%s:%d unknown location %d\n", __func__, __LINE__,
			   (int)access->location);
		return (PPM_EEPROMINFO_ERROR);
	}

	/* Does access to eeprom require a mux? */
	if (access->numMuxes)
    {
	    if (ppm_eeprom_select_muxes(access->i2c_bus, access->muxes, access->numMuxes))
        {
            printf("%s: Failed to probe/configured EEPROM mux on I2C%d\n",
                   __func__,
                   access->i2c_bus + 1 /* I2C ctrls start from zero */);
            return (PPM_EEPROMINFO_ERROR);
        }
    }

    /* Read initial SOM eeprom content */
    eeprom = access->eeprom;
    PPM_EEPROMINFO_DEBUG("%s:%d eeprom->content %p\n", __func__, __LINE__, &eeprom->content);

    eeprom->valid.layout = eeprom->valid.board = eeprom->valid.system = false;
    eeprom->dirty.layout = eeprom->dirty.board = eeprom->dirty.system = false;

    ret = ppm_eeprom_read(0x0, &eeprom->content, sizeof(eeprom->content));
    if (ret)
    {
        printf("%s:%d ppm_eeprom_read failed with %d\n", __func__, __LINE__, ret);
        return ret;
    }

    /* Validate board fields of ppm_eeprom_content */
    ret = ppm_eeprom_validate_layout(eeprom);
    if (!ret && !eeprom->valid.layout)
    {
        valid = ppm_eeprom_location_name(access->location, &name);
        if (!valid)
        {
            printf("%s:%d unknown location %d\n", __func__, __LINE__,
                   (int)access->location);
        }
        printf("PPM %s EEPROM layout content invalid; initializing\n", name);

        /* ... */
    }
    if (ret)
    {
        return ret;
    }

    ret = ppm_eeprom_validate_board(eeprom);
    if (!ret && !eeprom->valid.board)
    {
        valid = ppm_eeprom_location_name(access->location, &name);
        if (!valid)
        {
            printf("%s:%d unknown location %d\n", __func__, __LINE__,
                   (int)access->location);
        }
        printf("PPM %s EEPROM board content invalid; initializing\n", name);
        /* ... */
    }

    /*
     * Set flag for this eeprom to prevent ppm_eeprom_select_location()
     * from attempting to read in again. So we don't overwrite any
     * modified data when selecting different targets before saving.
     */
    eeprom->loaded = true;

    return ret;
}

/**
 * @brief update bus selection and mux selection
 *
 * If EEPROM has already been loaded or initialised, you just need to access
 * its location again via the multiplexers.  You don't need to (and shouldn't)
 * read the EEPROM contents again.
 * @return true on success
 */
static bool ppm_eeprom_update(void)
{
    struct ppm_eeprom_access    *access;    /* EEPROM access           */
    const char                  *name;      /* name of EEPROM location */
    bool                        valid;      /* flag                    */
	struct udevice              *i2c_bus;
	struct udevice              *i2c_dev;

    /* set EEPROM access */
    access = &ppm_eeprom_access;

    /* print debugging info */
    PPM_EEPROMINFO_DEBUG("%s:%d access->location %d\n",
                         __func__, __LINE__, access->location);

    /* get name of EEPROM */
    valid = ppm_eeprom_location_name(access->location, &name);
    if (!valid)
    {
        printf("%s:%d Unknown Location %d\n",
               __func__, __LINE__, (int) access->location);
        return false;
    }

    /* set up I2C multiplexer, if necessary */
    if (access->numMuxes)
    {
        if (ppm_eeprom_select_muxes(access->i2c_bus, access->muxes, access->numMuxes))
        {
            printf("%s: Failed to probe/configured EEPROM mux on I2C%d\n",
                   __func__,
				   access->i2c_bus + 1 /* I2C ctrls start from zero */);
            return false;
        }
    }

    /* probe EEPROM */
    uclass_get_device_by_seq(UCLASS_I2C, ppm_eeprom_access.i2c_bus, &i2c_bus);
    if (dm_i2c_probe(i2c_bus, access->i2c_address, 0, &i2c_dev))
    {
        printf("%s:%d Failed to probe EEPROM at address 0x%X on I2C%d",
               __func__, __LINE__, access->i2c_address, access->i2c_bus);
        return false;
    }

    return true;
}

/**
 * Select which EEPROM to manipulate
 *
 * Note, if eeprom hasn't been read into memory then do so
 *
 * I2C address may need updating for IS8.  See ...
 * >https://confluence.draeger.com/display/~hoenehan/IS8+EEPROM+I2C+Addresses<
 */
static int ppm_eeprom_select_location(PPM_EEPROMINFO_LOCATION_ID location)
{
    int ret = 0;

    PPM_EEPROMINFO_DEBUG("%s:%d location %d\n", __func__, __LINE__, location);

    switch(location)
    {
    case PPM_EEPROMINFO_LOCATION_SOM:
        /* SOM EEPROM is located on I2C3 address 0x50 */
        ppm_eeprom_access.eeprom = &ppm_eeprom_som;
        ppm_eeprom_access.i2c_bus = 2;
        ppm_eeprom_access.i2c_address = 0x50;
        ppm_eeprom_access.i2c_address_len = 2; /* two byte address */
        ppm_eeprom_access.i2c_page_mask = 0x1f;
        ppm_eeprom_access.i2c_tWR = 5000; /* tWR = 5 milliseconds */

        /* SOM EEPROM WC @ jumper X25 */
        ppm_eeprom_access.wp.has_gpio_wp = false;
        ppm_eeprom_access.wp.gpio_configured = false;

        /* SOM EEPROM directly accessible */
        ppm_eeprom_access.numMuxes = 0;
        break;

    case PPM_EEPROMINFO_LOCATION_PI:
        /* PI eeprom is located on I2C3 bus, downstream
         * of PCA9546A expander (address 0x70) on leg 4
         * (SC3/SD3 pins), at address 0x53 */
        ppm_eeprom_access.eeprom = &ppm_eeprom_pi;
        ppm_eeprom_access.i2c_bus = 2;
        ppm_eeprom_access.i2c_address = 0x53;
        ppm_eeprom_access.i2c_address_len = 2; /* two byte address */
        ppm_eeprom_access.i2c_page_mask = 0x1f;
        ppm_eeprom_access.i2c_tWR = 5000; /* tWR = 5 milliseconds */

        /* PI EEPROM WC @ jumper X30 */
        /* SOM must also drive PI_I2C_EEPROM_WP @ GPIO2_0 Low */
        ppm_eeprom_access.wp.has_gpio_wp = true;
        ppm_eeprom_access.wp.gpio_configured = false;
        ppm_eeprom_access.wp.pin = IMX_GPIO_NR(2, 0); /* GPIO2_0 on PI board */
        ppm_eeprom_access.wp.enable_write_value = 0;
        ppm_eeprom_access.wp.disable_write_value = 1;
        ppm_eeprom_access.wp.write_enabled = false;

        /* Setup the PI mux configuration */
        ppm_eeprom_access.numMuxes = 1;
        ppm_eeprom_access.muxes[0].address = 0x70;
        ppm_eeprom_access.muxes[0].leg = 4;

        break;

    case PPM_EEPROMINFO_LOCATION_AFE1:
        /* AFE1 is accessed through PCA9548A I2C3 bus via pins SD5 and SC5
           leg 6).                                                        */
        ppm_eeprom_access.eeprom             = &ppm_eeprom_afe1;
        ppm_eeprom_access.i2c_bus            = 2;
        ppm_eeprom_access.i2c_address        = 0x51;
        ppm_eeprom_access.i2c_address_len    = 2;
        ppm_eeprom_access.i2c_page_mask      = 0x1f; /* page mask   */
        ppm_eeprom_access.i2c_tWR            = 5000; /* tWR = 5 ms  */

        ppm_eeprom_access.wp.has_gpio_wp     = false;
        ppm_eeprom_access.wp.gpio_configured = false;

        ppm_eeprom_access.numMuxes = 1;
        ppm_eeprom_access.muxes[0].address = 0x70;
        ppm_eeprom_access.muxes[0].leg = 6;
        break;

    case PPM_EEPROMINFO_LOCATION_AFE2:
        /* AFE2 is accessed through PCA9548A I2C3 bus via pins SD6 and SC6
           (leg 7).                                                       */
        ppm_eeprom_access.eeprom             = &ppm_eeprom_afe2;
        ppm_eeprom_access.i2c_bus            = 2;
        ppm_eeprom_access.i2c_address        = 0x51;
        ppm_eeprom_access.i2c_address_len    = 2;
        ppm_eeprom_access.i2c_page_mask      = 0x1f; /* page mask   */
        ppm_eeprom_access.i2c_tWR            = 5000; /* tWR = 5 ms  */

        ppm_eeprom_access.wp.has_gpio_wp     = false;
        ppm_eeprom_access.wp.gpio_configured = false;

        ppm_eeprom_access.numMuxes = 1;
        ppm_eeprom_access.muxes[0].address = 0x70;
        ppm_eeprom_access.muxes[0].leg = 7;    /* SD6 and SC6 */
        break;

    case PPM_EEPROMINFO_LOCATION_RADIO:
        /* WiFi CONN is on leg 2 of I2C2 Hub */
        ppm_eeprom_access.eeprom             = &ppm_eeprom_radio;
        ppm_eeprom_access.i2c_bus            = 1;
        ppm_eeprom_access.i2c_address        = 0x50;
        ppm_eeprom_access.i2c_address_len    = 2;
        ppm_eeprom_access.i2c_page_mask      = 0x1f; /* page mask   */
        ppm_eeprom_access.i2c_tWR            = 5000; /* tWR = 5 ms  */

        ppm_eeprom_access.wp.has_gpio_wp     = false;
        ppm_eeprom_access.wp.gpio_configured = false;

        ppm_eeprom_access.numMuxes = 1;
        ppm_eeprom_access.muxes[0].address = 0x74;
        ppm_eeprom_access.muxes[0].leg = 2;    /* SD1 and SC1 */
        break;

    case PPM_EEPROMINFO_LOCATION_UIM:
        /* User Interface module (IS8) is on I2C1 */
        /* User Interface module (IS12/IS14) is only accessible from the UI PIC */
        ppm_eeprom_access.eeprom             = &ppm_eeprom_ui;
        ppm_eeprom_access.i2c_bus            = 0;
        ppm_eeprom_access.i2c_address        = 0x50;
        ppm_eeprom_access.i2c_address_len    = 2;
        ppm_eeprom_access.i2c_page_mask      = 0x1f; /* page mask   */
        ppm_eeprom_access.i2c_tWR            = 5000; /* tWR = 5 ms  */

        ppm_eeprom_access.wp.has_gpio_wp     = false;
        ppm_eeprom_access.wp.gpio_configured = false;

        ppm_eeprom_access.numMuxes = 1;
        ppm_eeprom_access.muxes[0].address = 0x70;
        ppm_eeprom_access.muxes[0].leg = 4;    /* SD3 and SC3 */
        break;

    case PPM_EEPROMINFO_LOCATION_BIM:
        /* Battery Interconnect module (IS8) is on leg 5 of I2C3 Hub */
        /* Battery Interconnect module (IS12/IS14) is only accessible from the UI PIC */
        ppm_eeprom_access.eeprom             = &ppm_eeprom_battery;
        ppm_eeprom_access.i2c_bus            = 2;
        ppm_eeprom_access.i2c_address        = 0x53;
        ppm_eeprom_access.i2c_address_len    = 2;
        ppm_eeprom_access.i2c_page_mask      = 0x1f; /* page mask   */
        ppm_eeprom_access.i2c_tWR            = 5000; /* tWR = 5 ms  */

        ppm_eeprom_access.wp.has_gpio_wp     = false;
        ppm_eeprom_access.wp.gpio_configured = false;

        ppm_eeprom_access.numMuxes = 2;
        ppm_eeprom_access.muxes[0].address = 0x70;
        ppm_eeprom_access.muxes[0].leg = 5;    /* SD4 and SC4 */
        ppm_eeprom_access.muxes[1].address = 0x72;
        ppm_eeprom_access.muxes[1].leg = 4;
        break;

    case PPM_EEPROMINFO_LOCATION_OLM:
        /* Optical Link module is on leg 5 of I2C3 Hub */
        /* Optical Link module (IS12/IS14) is only accessible from the UI PIC */
        ppm_eeprom_access.eeprom             = &ppm_eeprom_olm;
        ppm_eeprom_access.i2c_bus            = 2;
        ppm_eeprom_access.i2c_address        = 0x53;
        ppm_eeprom_access.i2c_address_len    = 2;
        ppm_eeprom_access.i2c_page_mask      = 0x1f; /* page mask   */
        ppm_eeprom_access.i2c_tWR            = 5000; /* tWR = 5 ms  */

        ppm_eeprom_access.wp.has_gpio_wp     = false;
        ppm_eeprom_access.wp.gpio_configured = false;

        ppm_eeprom_access.numMuxes = 2;
        ppm_eeprom_access.muxes[0].address = 0x70;
        ppm_eeprom_access.muxes[0].leg = 5;    /* SD5 and SC5 */
        ppm_eeprom_access.muxes[1].address = 0x72;
        ppm_eeprom_access.muxes[1].leg = 5;
        break;

    case PPM_EEPROMINFO_LOCATION_UNKNOWN:
        ppm_eeprom_access.eeprom = NULL;
        break;

    default:
        printf("Unknown PPM eeprom selected\n");
        return 1;
    }

	/* Setup the location name */
    ppm_eeprom_access.location = location;
	(void)ppm_eeprom_location_name(ppm_eeprom_access.location, &ppm_eeprom_access.eeprom->name);
	if (location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
	{
		/* If selecting unknown, then just return; used at start
		 * to force any commands used after this point to select a valid
		 * location first */
		return 0;
	}

	eeprom = ppm_eeprom_access.eeprom;

    /* If EEPROM has NOT already been loaded, initialise it.  This will set up
     * mux and bus.  Also, this will read contents of EEPROM into memory.  If
     * EEPROM has already been loaded, then just update it.  That will just set
     * up the mux and the bus. */
    if (eeprom->loaded)
    {
        /* only update I2C bus and mux */
        ret = (int) ppm_eeprom_update();
    }
    else
    {
        /* fully load EEPROM over I2C */
        ret = ppm_eeprom_init();
    }

	return ret;
}

/**
 * @brief dump selected EEPROM currently in memory
 */
int ppm_eeprom_dump(void)
{
	u32 ret = 0;
	char tmpStr[PPM_EEPROMINFO_STRING_LENGTH];
	u32 len;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }
	ret = ppm_eeprom_init();
	printf("EEPROM selected: %s\n", eeprom->name);
	if (!eeprom->valid.layout)
	{
		printf("Layout content is invalid!\n");
		return 1;
	}
	else
	{
		printf("Layout:   %s\n", eeprom->dirty.layout ? "dirty" : "");
		printf("          id %04x\n", __be32_to_cpu(eeprom->content.layout.data.version));
		printf("          crc %04x\n", __be32_to_cpu(eeprom->content.layout.crc));
		if (eeprom->content.layout.data.version != __cpu_to_be32(PPM_EEPROMINFO_LAYOUT_VERSION))
		{
			printf("Layout is unknown(%08x != expected %08x); can't decode board/macaddrs!\n", eeprom->content.layout.data.version, PPM_EEPROMINFO_LAYOUT_VERSION);
			return 1;
		}
	}

	/* Now known valid, try to decode board */
	if (!eeprom->valid.board)
	{
		printf("Board content is invalid!\n");
		ret = 1;
	}
	else
	{
		const char *boardName;

		printf("Board:   %s\n", eeprom->dirty.board ? " dirty" : "");
		if (!ppm_eeprom_board_name(eeprom->content.board.data.partNumber, &boardName))
		{
			printf("          Part Number invalid - %s!\n", eeprom->content.board.data.partNumber);
			ret = 1;
		}
		else
		{
			printf("          %s\n", boardName);
		}

		printf("          hwRevision %c%c%c\n",
               eeprom->content.board.data.hwRevision[0],
               eeprom->content.board.data.hwRevision[1],
               eeprom->content.board.data.hwRevision[2]);


		printf("          hwTweakBits 0x%08x\n", __be32_to_cpu(eeprom->content.board.data.hwTweakBits));

		printf("          swCompatibility %04d\n", __be16_to_cpu(eeprom->content.board.data.swCompatibility));
		len = strnlen((char *)eeprom->content.board.data.partNumber, PPM_EEPROMINFO_PN_LENGTH);
		if (len >= PPM_EEPROMINFO_PN_LENGTH)
		{
			printf("          partNumber invalid! (len %u)\n", len);
			ret = 1;
		}
		else
		{
			memcpy(tmpStr, eeprom->content.board.data.partNumber, sizeof(tmpStr));
			printf("          partNumber '%s'\n", tmpStr);
		}

		len = strnlen((char *)eeprom->content.board.data.serialNumber, PPM_EEPROMINFO_SN_LENGTH);
		if (len >= PPM_EEPROMINFO_SN_LENGTH)
		{
			printf("          serialNumber length invalid (%u)!\n", len);
			ret = 1;
		}
		else
		{
			memcpy(tmpStr, eeprom->content.board.data.serialNumber, sizeof(tmpStr));
			printf("          serialNumber '%s'\n", tmpStr);
		}
		printf("          crc %08x\n", eeprom->content.board.crc);
	}

	return ret;
}

/**
 * @brief is eeprom (and its boardId) valid
 */
int ppm_eeprom_board_valid(ppm_eeprom_t *p, const char **boardNamePtr)
{
    const char *boardName;
    if (p->valid.layout && p->valid.board)
    {
        if (ppm_eeprom_board_name(p->content.board.data.partNumber, &boardName))
        {
            *boardNamePtr = boardName;
            return true;
        }
    }
    return false;
}

/**
 * @brief output selected eeprom info
 */
void ppm_eeprom_info(ppm_eeprom_t *p)
{
	char tmpStr[PPM_EEPROMINFO_STRING_LENGTH];
	const char *boardName;
	u32 len;

    if (ppm_eeprom_board_valid(p, &boardName))
    {
        printf("\n");
        printf("%s boardId:         %s\n", p->name, boardName);
        printf("%s hwRevision:      %c%c\n", p->name, p->content.board.data.hwRevision[1], p->content.board.data.hwRevision[2]);
        printf("%s swCompatibility: %u\n", p->name, __be16_to_cpu(p->content.board.data.swCompatibility));
		printf("%s hwTweakBits:     0x%08x\n", p->name, __be32_to_cpu(p->content.board.data.hwTweakBits));

        len = strnlen((char *)p->content.board.data.partNumber, PPM_EEPROMINFO_PN_LENGTH);
        if (len && (len == PPM_EEPROMINFO_PN_LENGTH))
        {
            memcpy(tmpStr, p->content.board.data.partNumber, sizeof(tmpStr));
            tmpStr[len] = '\0';
            printf("%s partNumber:      %s\n", p->name, tmpStr);
        }

        len = strnlen((char *)p->content.board.data.serialNumber, PPM_EEPROMINFO_SN_LENGTH);
        if (len && (len == PPM_EEPROMINFO_SN_LENGTH))
        {
            memcpy(tmpStr, p->content.board.data.serialNumber, sizeof(tmpStr));
            tmpStr[len] = '\0';
            printf("%s serialNumber:    %s\n", p->name, tmpStr);
        }

        if (strncmp(p->name, "PI", 2) == 0)
        {
            if (ppm_eeprom_validate_system(p))
            {
                /* Display the PPM product data, only stored in the PI EEPROM */
                printf("%s ppmHwRevision:   %c%c%c\n",
                       p->name, p->content.system.data.hwRevision[0], p->content.system.data.hwRevision[1], p->content.system.data.hwRevision[2]);

                len = strnlen((char *)p->content.system.data.partNumber, PPM_EEPROMINFO_PN_LENGTH);
                if (len && (len == PPM_EEPROMINFO_PN_LENGTH))
                {
                    memcpy(tmpStr, p->content.system.data.partNumber, sizeof(tmpStr));
                    tmpStr[len] = '\0';
                    printf("%s ppmPartNumber:   %s\n", p->name, tmpStr);
                }

                len = strnlen((char *)p->content.system.data.serialNumber, PPM_EEPROMINFO_SN_LENGTH);
                if (len && (len == PPM_EEPROMINFO_SN_LENGTH))
                {
                    memcpy(tmpStr, p->content.system.data.serialNumber, sizeof(tmpStr));
                    tmpStr[len] = '\0';
                    printf("%s ppmSerialNumber: %s\n", p->name, tmpStr);
                }
            }
        }
    }
}

/**
 * do_ppm_eeprom_dump() - Handle the "ppm_eeprom dump" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, negative on error.
 *
 * Syntax:
 *       ppm_eeprom dump - dump current eeprom content
 */
static int do_ppm_eeprom_dump(struct cmd_tbl *cmdtp, int flag, int argc,
                              char * const argv[])
{
	return ppm_eeprom_dump();
}

/**
 * do_ppm_eeprom_info() - Handle the "ppm_eeprom info" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, negative on error.
 *
 * Syntax:
 *       ppm_eeprom info - dump current eeprom info (used in u-boot startup)
 */
static int do_ppm_eeprom_info(struct cmd_tbl *cmdtp, int flag, int argc,
                              char * const argv[])
{
	int         valid;          /* valid EEPROM found? */
	const char  *boardName;     /* name of board with EEPROM */
    int         i;              /* loop variable */

    ppm_eeprom_t *list[] =       /* EEPROMs to consider */
        {
            &ppm_eeprom_som,
            &ppm_eeprom_pi,
            &ppm_eeprom_afe1,
            &ppm_eeprom_afe2,
            &ppm_eeprom_radio,
            &ppm_eeprom_ui,
            &ppm_eeprom_battery,
            &ppm_eeprom_olm,
        };

    /* no valid EEPROMs found, yet */
    valid = 0;

    /* loop through EEPROMs */
    for (i = 0; i < ARRAY_SIZE(list); i++)
    {
        /* is EEPROM valid? */
        if (ppm_eeprom_board_valid(list[i], &boardName))
        {
            /* display EEPROM info */
            printf("\n");
            ppm_eeprom_info(list[i]);

            /* valid EEPROM found */
            valid = 1;
        }
    }

    /* were any valid EEPROMs found? */
	if (!valid)
	{
		printf("\n");
		printf("No valid EEPROM data detected.\n");
	}
	printf("\n");
	return 0;
}

/**
 * @brief function to read/initalize ppm_eeprom structure and environment
 *
 */
void ppm_eeprom_init_env(PPM_UNIT_TYPE ppmUnitType)
{
    unsigned int    i;
    int             nrOfEeproms;

    struct {
        PPM_EEPROMINFO_LOCATION_ID  locationId;     /* EEPROM location ID   */
        ppm_eeprom_t                *eeprom;        /* EEPROM               */
        char                        *ethaddr_src;   /* Ethernet Address Src */
    } eeproms[] = {
        {
            PPM_EEPROMINFO_LOCATION_PI,
            &ppm_eeprom_pi,
            "PI"
        },
        {
            PPM_EEPROMINFO_LOCATION_SOM,
            &ppm_eeprom_som,
            "SOM"
        },
        {
            PPM_EEPROMINFO_LOCATION_AFE1,
            &ppm_eeprom_afe1,
            "AFE1"
        },
        {
            PPM_EEPROMINFO_LOCATION_AFE2,
            &ppm_eeprom_afe2,
            "AFE2"
        },
        {
            PPM_EEPROMINFO_LOCATION_RADIO,
            &ppm_eeprom_radio,
            "WCM"
        },
        {
            PPM_EEPROMINFO_LOCATION_UIM,
            &ppm_eeprom_ui,
            "UIM"
        },
        {
            PPM_EEPROMINFO_LOCATION_BIM,
            &ppm_eeprom_battery,
            "BIM"
        },
        {
            PPM_EEPROMINFO_LOCATION_OLM,
            &ppm_eeprom_olm,
            "OLM"
        }
    };

    if (ppmUnitType == PPM_UNIT_MOUNT)
    {
        nrOfEeproms = 2; /* Only PI and SOM */
    }
    else
    {
        nrOfEeproms = ARRAY_SIZE(eeproms);
    }

    /* Dump the data */
    for (i = 0; i < nrOfEeproms; i++)
    {
        ppm_eeprom_select_location(eeproms[i].locationId);
    }

    for (i = 0; i < nrOfEeproms; i++)
    {
        ppm_eeprom_info(eeproms[i].eeprom);
    }

	/* Deselect eeprom */
	ppm_eeprom_select_location(PPM_EEPROMINFO_LOCATION_UNKNOWN);
}

/**
 * do_ppm_eeprom_debug() - Handle the "ppm_eeprom debug" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom debug [true|false] - enable/disable ppm_eeprom debug state
 */
static int do_ppm_eeprom_debug(struct cmd_tbl *cmdtp, int flag, int argc,
                               char * const argv[])
{
    if (argc != 2)
    {
        printf("Missing arg\n");
        return CMD_RET_USAGE;
    }

    if ((strcasecmp(argv[1], "true") == 0)
		|| (strcasecmp(argv[1], "on") == 0))
    {
        ppm_eeprom_debug = true;
    }
	else if ((strcasecmp(argv[1], "false") == 0)
			 || (strcasecmp(argv[1], "off") == 0))
    {
        ppm_eeprom_debug = false;
    }
    else
    {
        printf("Only 'true' or 'false' are only allow arg\n");
        return CMD_RET_USAGE;
    }
    return 0;
}

/**
 * do_ppm_eeprom_write_enable() - Handle the "ppm_eeprom write_enable" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom debug [true|false] - enable/disable ppm_eeprom debug state
 */
static int do_ppm_eeprom_write_enable(struct cmd_tbl *cmdtp, int flag, int argc,
                                      char * const argv[])
{
	int ret;

    if (argc != 2)
    {
        printf("Missing arg\n");
        return CMD_RET_USAGE;
    }

    if ((strcasecmp(argv[1], "true") == 0)
		|| (strcasecmp(argv[1], "on") == 0))
    {
		ret = ppm_eeprom_write_enable(&ppm_eeprom_access.wp, true);
    }
	else if ((strcasecmp(argv[1], "false") == 0)
			 || (strcasecmp(argv[1], "off") == 0))
    {
		ret = ppm_eeprom_write_enable(&ppm_eeprom_access.wp, false);
    }
    else
    {
        printf("Only 'true' or 'false' are only allow arg\n");
        return CMD_RET_USAGE;
    }
	if (ret)
	{
		printf("Failed to change eeprom write enable for %s\n", ppm_eeprom_access.eeprom->name);
		return (PPM_EEPROMINFO_ERROR);
	}
    return 0;
}

/**
 * do_ppm_eeprom_version() - Handle the "ppm_eeprom version" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom version        - show eeprom layout version
 *       ppm_eeprom version 0x0001 - set eeprom layout version
 */
static int do_ppm_eeprom_version(struct cmd_tbl *cmdtp, int flag, int argc,
                                 char * const argv[])
{
	ppm_eeprom_layout_crc_t *layout;
    uint32_t version;
	uint32_t crc_val;
	char *end;
    int ret = 0;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

	layout = &eeprom->content.layout;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

    if (argc == 2)
    {
        version = simple_strtoul(argv[1], &end, 10);
		if (end && *end != '\0')
		{
			printf("Invalid version '%s'\n", argv[1]);
			return CMD_RET_USAGE;
		}
        layout->data.version = version;

        /* Since layout version changed, recalculate
         * layout CRC */
        crc_val = GENERATE_CRC_VALUE(layout);
        layout->crc = __be32_to_cpu(crc_val);
        eeprom->dirty.layout = true;
    }

    printf("version 0x%x\n", __be32_to_cpu(layout->data.version));
    return ret;
}

#define PREAMBLE        { 0x00, 0x30, 0xE6 }
#define TEN_BIT_MASK    ((1 << 10) - 1)         /* 0x03FF */
/**
 * ppm_eeprom_read_mac() - read MAC address data from MAC EEPROM
 * @brief read MAC address from MAC EEPROM
 * @param[in]  "uint32_T eeprom_index"
 * @param[out] "uint8_t *mac"
 * @return zero (0) on success, (-1) on failure.
 * 
 * The MAC EEPROM is new for IS8, and there are two, one on the SSPPM PI for copper ethernet and optical ethernet,
 * and one on the Redpind card, for the WIFI interface.
 *
 * The MAC address is stored in the MAC EEPROMs first six (6) bytes.
 * Read the address, validate it, and if valid pass the address back to the caller.
 */
int ppm_eeprom_read_mac(uint32_t eeprom_index, uint8_t *mac)
{
    unsigned int    i;      /* loop variable */
    int             err;    /* error flag */
    uint8_t         data[PPM_EEPROMINFO_MAC_ADDR_LENGTH];
    struct udevice *i2c_dev;

    if ((eeprom_index != ENET_MAC_ADDR_INDEX) && (eeprom_index != WIFI_MAC_ADDR_INDEX))
    {        
        printf("%s:%d MAC EEPROM bad index\n", __func__, __LINE__);
        return (PPM_EEPROMINFO_ERROR);
    }

    if (!strlen(macEeproms[eeprom_index].name))
    {
        printf("%s:%d MAC EEPROM bad name\n", __func__, __LINE__);
        return (PPM_EEPROMINFO_ERROR);
    }

    /* select mux */
    err = ppm_eeprom_select_muxes(macEeproms[eeprom_index].i2c_bus,
                                  macEeproms[eeprom_index].i2c_muxes,
                                  macEeproms[eeprom_index].numMuxes);
    if (err)
    {
        printf("%s:%d MAC EEPROM bad\n", __func__, __LINE__);

        /* failed, error message already printed from inside function */
        return err;
    }

    /* get and probe eeprom device */
    if (i2c_get_chip_for_busnum(macEeproms[eeprom_index].i2c_bus,
                                macEeproms[eeprom_index].i2c_address,
								1, &i2c_dev))
    {
        printf("Failed to get/probe eeprom at 0x%02X\n", macEeproms[eeprom_index].i2c_address);
        return (PPM_EEPROMINFO_ERROR);
    }

    /* read first six bytes of EEPROM */
    err = dm_i2c_read(i2c_dev, 0x00, data, PPM_EEPROMINFO_MAC_ADDR_LENGTH);
    if (err)
    {
        printf("Error: %s:%d - Could not read from EEPROM\n", __func__, __LINE__);
        return err;
    }

    /* The I2C read was successful, error check the address format */
    if (!is_valid_ethaddr(data))
    {
        printf("%s:%d MAC EEPROM bad\n", __func__, __LINE__);
        return(PPM_EEPROMINFO_ERROR);
    }

    /*** validate MAC Address ***/
    if (data[3] < 0x01 || data[3] > 0x0F)
    {
        printf("%s:%d MAC EEPROM illegal format\n", __func__, __LINE__);
        return(PPM_EEPROMINFO_ERROR);
    }

    /* separate last two bytes into two bit fields: 6-bits and 10-bits.  The
     * 10-bit field may not be zero. */
    if (!(((data[4] << 8) | data[5]) & TEN_BIT_MASK))
    {
        printf("%s:%d MAC EEPROM illegal format\n", __func__, __LINE__);
        return(PPM_EEPROMINFO_ERROR);
    }

    /* return the address to the caller */
    for (i=0; i<PPM_EEPROMINFO_MAC_ADDR_LENGTH; i++)
	{
	    mac[i] = data[i];
	}

    return 0;
}

/**
 * do_ppm_eeprom_mac() - read data from MAC EEPROM
 * @brief get MAC address from MAC EEPROM
 * @param[in] "struct cmd_tbl *cmdtp"
 * @param[in] "int flag"
 * @param[in] "int argc"
 * @param[in] "char * const argv[]"
 * @return zero (0) on success
 * 
 * The MAC address is stored in the MAC EEPROMs first six (6) bytes.  Treat
 * these bytes as read-only.  Do not overwrite these values.  There are several
 * MAC EEPROMs.  Read the desired EEPROM in the first argument.  Then, access
 * it, and re-selct the previously selected EEPROM.
 */
int do_ppm_eeprom_mac(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int    sel;    /* index of MAC EEPROM in table */
    unsigned int    i;      /* loop variable */
    int             err;    /* error flag */
    bool            found;  /* found flag */
    uint8_t         preamble[]  = PREAMBLE;
    struct udevice *i2c_dev;

    uint8_t data[PPM_EEPROMINFO_MAC_ADDR_LENGTH]; /* bfr for MAC address */

    /* verify # of args */
    if (argc != 2)
    {
        printf("Error: invalid number of arguments\n");
        return 1;
    }

    /* identify MAC EEPROM by name */
    found = false; /* no match found */
    for (i = 0; i < ARRAY_SIZE(macEeproms) && !found; i++)
    {
        if (!strcasecmp(argv[1], macEeproms[i].name))
        {
            sel = i; /* select MAC EEPROM */
            found = true; /* match found */
        }
    }

    /* print error if no match found */
    if (!found)
    {
        printf("Error: MAC EEPROM not listed.  "
               "The only choices are as follows: [");
        for (i = 0; i < ARRAY_SIZE(macEeproms); i++)
        {
            printf("%s%c ", macEeproms[i].name,
                   (i < (ARRAY_SIZE(macEeproms) - 1)) ? ',' : ']');
        }
        printf("\n");
        return 1;
    }

    /* select mux */
    err = ppm_eeprom_select_muxes(macEeproms[sel].i2c_bus,
                                  macEeproms[sel].i2c_muxes,
                                  macEeproms[sel].numMuxes);
    if (err)
    {
        /* failed, error message already printed from inside function */
        ppm_eeprom_update(); /* re-select previously selected device */
        return err;
    }

    /* get and probe eeprom device */
    if (i2c_get_chip_for_busnum(macEeproms[sel].i2c_bus,
                                macEeproms[sel].i2c_address,
								1, &i2c_dev))
    {
        printf("Failed to get/probe eeprom at 0x%02X\n", macEeproms[sel].i2c_address);
        return (PPM_EEPROMINFO_ERROR);
    }

    /* read first six bytes of EEPROM */
    err = dm_i2c_read(i2c_dev, 0x00, data, PPM_EEPROMINFO_MAC_ADDR_LENGTH);
    if (err)
    {
        printf("Error: %s@%u, Could not read from EEPROM\n",
               __func__, __LINE__);
        ppm_eeprom_update(); /* re-select previously selected device */
        return err;
    }

    /* print result */
    printf("EEPROM MAC Address: %pM\n", data);

    /*** validate MAC Address ***/
    err = 0;

    /* validate preamble */
    for (i = 0; i < ARRAY_SIZE(preamble); i++)
    {
        if (data[i] != preamble[i])
        {
            printf("Error: MAC address byte %u expected to be 0x%02X\n",
                   i, preamble[i]);
            err = (PPM_EEPROMINFO_ERROR);
        }
    }

    /* validate fourth byte */
    if (data[3] < 0x01 || data[3] > 0x0F)
    {
        printf("Error: MAC Address 4th byte must be in range 0x01-0x0F\n");
        err = (PPM_EEPROMINFO_ERROR);
    }

    /* separate last two bytes into two bit fields: 6-bits and 10-bits.  The
     * 10-bit field may not be zero. */
    if (!(((data[4] << 8) | data[5]) & TEN_BIT_MASK))
    {
        printf("Error: the last ten bytes of the MAC address may not equal "
               "zero\n");
        err = (PPM_EEPROMINFO_ERROR);
    }

    if (err)
    {
        printf("Bad MAC Address\n");
    }
    else
    {
        printf("Good MAC Address\n");
    }

    /* re-select previously selected device */
    ppm_eeprom_update();

    return err;
}
#undef PREAMBLE
#undef TEN_BIT_MASK

/**
 * ppm_eeprom_write_enabled() - Indicate if EEPROM is write enabled
 * @eeprom_wp:	EEPROM WP struct pointer
 *
 * @return int - non-zero on error, zero if enabled, positive if not
 */
int ppm_eeprom_write_disabled(struct ppm_eeprom_wp *wp)
{
    if (!wp->has_gpio_wp)
    {
        /* Can't tell if write enabled; assume it is */
        return 0;
    }

    /* Return error if WP gpio not configured */
    if (!wp->gpio_configured)
    {
        return (PPM_EEPROMINFO_ERROR);
    }

    if (wp->write_enabled)
    {
        return 0;
    }
    return 1;
}

/**
 * do_ppm_eeprom_save() - Handle the "ppm_eeprom save" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom save        - save modified eeprom content to EEPROM
 */
static int do_ppm_eeprom_save(struct cmd_tbl *cmdtp, int flag, int argc,
                              char * const argv[])
{
	int ret = 1;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

    if (argc > 1)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

	if (!eeprom)
	{
		printf("No EEPROM selected.\n");
	}

	if (eeprom->dirty.layout)
	{
        /* If not write enabled then fail */
        if (ppm_eeprom_write_disabled(&ppm_eeprom_access.wp))
        {
            printf("EEPROM not write enabled\n");
            return 1;
        }
		/* EEPROM board content in ram is dirty; write to memory */
		ret = ppm_eeprom_write(offsetof(ppm_eeprom_data_cmv1_t, layout), &eeprom->content.layout, sizeof(eeprom->content.layout));
		if (ret)
		{
            /* ppm_eeprom_write already complained */
            return 1;
		}
		printf("%s layout section written to EEPROM.\n", ppm_eeprom_access.eeprom->name);
		eeprom->dirty.layout = false;
	}

	if (eeprom->dirty.board)
	{
		/* EEPROM board content in ram is dirty; write to memory */
		ret = ppm_eeprom_write(offsetof(ppm_eeprom_data_cmv1_t, board), &eeprom->content.board, sizeof(eeprom->content.board));
		if (ret)
		{
            /* ppm_eeprom_write already complained */
            return 1;
		}
		printf("%s board section written to EEPROM.\n", ppm_eeprom_access.eeprom->name);
		eeprom->dirty.board = false;
	}

	if (eeprom->dirty.system)
	{
		/* EEPROM system content in ram is dirty; write to memory */
		ret = ppm_eeprom_write(offsetof(ppm_eeprom_data_cmv1_t, system), &eeprom->content.system, sizeof(eeprom->content.system));
		if (ret)
		{
            /* ppm_eeprom_write already complained */
            return 1;
		}
		printf("%s system section written to EEPROM.\n", ppm_eeprom_access.eeprom->name);
		eeprom->dirty.system = false;
	}

	/* If we get here and ret==1 then there was nothing to write. */
	if (ret)
	{
		printf("Nothing written to %s EEPROM.\n", ppm_eeprom_access.eeprom->name);
	}

	return 0;
}

/**
 * do_ppm_eeprom_select() - Handle the "ppm_eeprom select" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom select        - show which eeprom is selected
 *       ppm_eeprom select som    - select the SOM eeprom
 *       ppm_eeprom select pi     - select the PI eeprom
 *       ppm_eeprom select afe1   - select the AFE1 eeprom
 */
static int do_ppm_eeprom_select(struct cmd_tbl *cmdtp, int flag, int argc,
                                char * const argv[])
{
	int             ret = 0;    /* return value           */
    unsigned int    i;          /* loop index             */
    int             found;      /* boolean: found result? */

    /* define possible locations */
    /* A similar table exists in ppm_eeprom_location_id_table[], ...
       However, the strings are uppercase instead of lowercase       */
    struct {
        char                        *locationStr;
        PPM_EEPROMINFO_LOCATION_ID  locationId;
    } locations[] = {
        { PPM_EEPROMINFO_LOCATION_KEY_SOM,     PPM_EEPROMINFO_LOCATION_SOM    },
        { PPM_EEPROMINFO_LOCATION_KEY_PI,      PPM_EEPROMINFO_LOCATION_PI     },
        { PPM_EEPROMINFO_LOCATION_KEY_AFE1,    PPM_EEPROMINFO_LOCATION_AFE1   },
        { PPM_EEPROMINFO_LOCATION_KEY_AFE2,    PPM_EEPROMINFO_LOCATION_AFE2   },
        { PPM_EEPROMINFO_LOCATION_KEY_RADIO,   PPM_EEPROMINFO_LOCATION_RADIO  },
        { PPM_EEPROMINFO_LOCATION_KEY_UIM,     PPM_EEPROMINFO_LOCATION_UIM    },
        { PPM_EEPROMINFO_LOCATION_KEY_BIM,     PPM_EEPROMINFO_LOCATION_BIM    },
        { PPM_EEPROMINFO_LOCATION_KEY_OLM,     PPM_EEPROMINFO_LOCATION_OLM    },
    };

    /* check # of arguments */
    if (argc != 2)
    {
        if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
        {
			printf("No EEPROM location selected\n");
			return 0;
        }
    }

    /* search locations */
    found = 0; /* not found */
    for (i = 0; i < ARRAY_SIZE(locations) && !found; i++)
    {
        if (!strcasecmp(argv[1], locations[i].locationStr))
        {
            ret = ppm_eeprom_select_location(locations[i].locationId);
            found = 1; /* found */
        }
    }

    /* if no results found, error */
    if (!found)
    {
        printf("The only selectable eeprom devices are ");
        for (i = 0; i < ARRAY_SIZE(locations); i++)
        {
            printf("'%s'%s",
                   locations[i].locationStr,
                   (i < (ARRAY_SIZE(locations) - 1)) ? ", " : "\n");
        }
        return CMD_RET_USAGE;
    }

    /* check for failure */
    if (!ret)
    {
		printf("Failed to select %s EEPROM\n", ppm_eeprom_access.eeprom->name);
    }

	printf("Selected EEPROM location: %s\n", ppm_eeprom_access.eeprom->name);
    return ret;
}

/**
 * do_ppm_eeprom_reset() - Handle the "ppm_eeprom reset" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom reset      - reset in-memory eeprom content
 */
static int do_ppm_eeprom_reset(struct cmd_tbl *cmdtp, int flag, int argc,
                               char * const argv[])
{
    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

	ppm_eeprom_reset_layout(&eeprom->content.layout);
	ppm_eeprom_reset_board(&eeprom->content.board);

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_PI)
    {
        ppm_eeprom_reset_system(&eeprom->content.system);
    }

	eeprom->dirty.layout = true;
	eeprom->dirty.board = true;
	eeprom->dirty.system = true;

	eeprom->valid.layout = true;
	eeprom->valid.board = true;
	eeprom->valid.system = true;

	printf("%s all sections reset.\n", ppm_eeprom_access.eeprom->name);

	return 0;
}

/**
 * do_ppm_eeprom_board_hw_revision() - Handle the "ppm_eeprom hw_revision" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom hw_revision      - show eeprom hw revison
 *       ppm_eeprom hw_revision 99   - set eeprom hw revision to 99
 */
static int do_ppm_eeprom_board_hw_revision(struct cmd_tbl *cmdtp, int flag, int argc,
                                           char * const argv[])
{
	ppm_eeprom_board_crc_t *board;
    char hwRevision[PPM_EEPROMINFO_HWREV_LENGTH-1];
    uint32_t crc_val;
    int ret = 0;
	uint32_t len;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

	board = &eeprom->content.board;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

    if (argc == 2)
    {
		len = strlen(argv[1]);

        if ((len != 1) && (len != 2))
        {
            printf("HW Revision '%s' is too long, maximum of two digits in the range 00 - 99\n", argv[1]);
			return CMD_RET_USAGE;
        }

        strncpy((char *)hwRevision, argv[1], sizeof(board->data.hwRevision) - 1);

        if (((len == 2) &&
            ((hwRevision[0] < '0')  ||
             (hwRevision[0] > '9')  ||
             (hwRevision[1] < '0')  ||
             (hwRevision[1] > '9')))
            ||
            ((len == 1) &&
             ((hwRevision[0] < '0')  ||
              (hwRevision[0] > '9'))))
        {
            printf("Valid HW Revision values range from 00 - 99 (decimal)\n");
			return CMD_RET_USAGE;
        }

        board->data.hwRevision[0] = '-';
        if (len == 2)
        {
            board->data.hwRevision[1] = argv[1][0];
            board->data.hwRevision[2] = argv[1][1];
        }
        else
        {
            board->data.hwRevision[1] = '0';
            board->data.hwRevision[2] = argv[1][0];
        }

        /* Since board hwVersion changed, recalculate
         * board CRC and write content back to memory */
        crc_val = GENERATE_CRC_VALUE(board);
        board->crc = __cpu_to_be32(crc_val);

        eeprom->dirty.board = true;
        return 0;
    }

    printf("HW revision %c%c\n", board->data.hwRevision[1], board->data.hwRevision[2]);
    return ret;
}

/**
 * do_ppm_eeprom_board_hw_tweak_bits() - Handle the "ppm_eeprom hw_tweak_bits" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom hw_tweak_bits      - show eeprom hw tweak bits
 *       ppm_eeprom hw_tweak_bits 0x101   - set eeprom hw tweak bits to 0x101
 */
static int do_ppm_eeprom_board_hw_tweak_bits(struct cmd_tbl *cmdtp, int flag, int argc,
                                             char * const argv[])
{
	ppm_eeprom_board_crc_t *board;
    uint32_t hwTweakBits;
    uint32_t crc_val;
	char *end;
    int ret = 0;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

	board = &eeprom->content.board;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

	hwTweakBits = __be32_to_cpu(board->data.hwTweakBits);

    if (argc == 2)
    {
        hwTweakBits = simple_strtoul(argv[1], &end, 0);
		board->data.hwTweakBits = __cpu_to_be32(hwTweakBits);

        /* Since board hw tweak bits changed, recalculate
         * board CRC and mark as dirty */
        crc_val = GENERATE_CRC_VALUE(board);
        board->crc = __be32_to_cpu(crc_val);

        eeprom->dirty.board = true;
        return 0;
    }

    printf("HW tweak bits 0x%08x\n", __be32_to_cpu(board->data.hwTweakBits));
    return ret;
}

/**
 * do_ppm_eeprom_board_sw_compatibility() - Handle the "ppm_eeprom sw_compatibility" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom sw_compatibility      - show eeprom sw revision
 *       ppm_eeprom sw_compatibility 9999   - set eeprom sw revision to 9999
 */
static int do_ppm_eeprom_board_sw_compatibility(struct cmd_tbl *cmdtp, int flag, int argc,
                                                char * const argv[])
{
	ppm_eeprom_board_crc_t *board;
    uint32_t swCompatibility;
    uint32_t crc_val;
	char *end;
    int ret = 0;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

	board = &eeprom->content.board;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

	swCompatibility = board->data.swCompatibility;

    if (argc == 2)
    {
        swCompatibility = simple_strtoul(argv[1], &end, 10);
		/* SW Compatibiliy index range is 0000 .. 9999 (decimal) */
		if ((end && (*end != '\0')) || (swCompatibility > PPM_EEPROMINFO_MAX_REVISION))
		{
			printf("swCompatibility '%s' invalid\n", argv[1]);
			return CMD_RET_USAGE;
		}
        board->data.swCompatibility = __cpu_to_be16(swCompatibility);

        /* Since board SW compatibility changed, recalculate
         * board CRC and write content back to memory */
        crc_val = GENERATE_CRC_VALUE(board);
        board->crc = __be32_to_cpu(crc_val);

        eeprom->dirty.board = true;
        return 0;
    }

    printf("SW compatibility index %04u\n", __be16_to_cpu(board->data.swCompatibility));
    return ret;
}

/**
 * do_ppm_eeprom_board_part_number() - Handle the "ppm_eeprom board_part_number" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom board_part_number         - show board part number
 *       ppm_eeprom board_part_number "<partname/number>" - set board part number
 */
static int do_ppm_eeprom_board_part_number(struct cmd_tbl *cmdtp, int flag, int argc,
                                           char * const argv[])
{
	ppm_eeprom_board_crc_t *board;
	char tmpStr[PPM_EEPROMINFO_PN_LENGTH];
	uint32_t len;
    uint32_t crc_val;
    int ret = 0;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

	board = &eeprom->content.board;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

    if (argc == 2)
    {
        /* Validate argv[1] */
		len = strlen(argv[1]);
        if (len > PPM_EEPROMINFO_PN_LENGTH)
        {
            printf("part number '%s' is too long\n", argv[1]);
			return 1;
        }
        strcpy((char *)&board->data.partNumber, argv[1]);

        /* Since board partNumber changed, recalculate
         * board CRC and write content back to memory */
		crc_val = GENERATE_CRC_VALUE(board);
        board->crc = __be32_to_cpu(crc_val);

        eeprom->dirty.board = true;
        return 0;
    }

	memcpy(tmpStr, board->data.partNumber, sizeof(tmpStr));
    printf("part_number '%s'\n", tmpStr);
    return ret;
}

/**
 * do_ppm_eeprom_board_serial_number() - Handle the "ppm_eeprom board_serial_number" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom board_serial_number         - show board serial number
 *       ppm_eeprom board_serial_number "<serialname/number>" - set board serial number
 */
static int do_ppm_eeprom_board_serial_number(struct cmd_tbl *cmdtp, int flag, int argc,
                                             char * const argv[])
{
	ppm_eeprom_board_crc_t *board;
	char tmpStr[PPM_EEPROMINFO_SN_LENGTH];
	uint32_t len;
    uint32_t crc_val;
    int ret = 0;

    if (ppm_eeprom_access.location == PPM_EEPROMINFO_LOCATION_UNKNOWN)
    {
        printf("No EEPROM location selected\n");
        return 1;
    }

	board = &eeprom->content.board;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

    if (argc == 2)
    {
        /* Validate argv[1] */
		len = strlen(argv[1]);
        if (len > PPM_EEPROMINFO_SN_LENGTH)
        {
            printf("serial number '%s' is too long\n", argv[1]);
			return 1;
        }
        strcpy((char *)&board->data.serialNumber, argv[1]);

        /* Since board serialNumber changed, recalculate
         * board CRC and write content back to memory */
		crc_val = GENERATE_CRC_VALUE(board);
        board->crc = __be32_to_cpu(crc_val);

        eeprom->dirty.board = true;
        return 0;
    }

	memcpy(tmpStr, board->data.serialNumber, sizeof(tmpStr));
    printf("serial_number '%s'\n", tmpStr);
    return ret;
}

/*
 * PPM Product field handlers
 *
 * These are only stored in the PI EEPROM, all other EEPROM's have these
 * fields set to 0xFF's.
 *
 *  - PPM product revision - string (three bytes "-XX")
 *  - PPM product part number 
 *  - PPM product serial number 
 */

/**
 * do_ppm_eeprom_ppm_hw_revision() - Handle the "ppm_eeprom ppm_hw_revision" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom ppm_hw_revision      - show eeprom hw revison
 *       ppm_eeprom ppm_hw_revision 99   - set eeprom hw revision to 99
 */
static int do_ppm_eeprom_ppm_hw_revision(struct cmd_tbl *cmdtp, int flag, int argc,
                                         char * const argv[])
{
    ppm_eeprom_system_crc_t *system;
    char hwRevision[PPM_EEPROMINFO_HWREV_LENGTH-1];
    uint32_t crc_val;
    int ret = 0;
	uint32_t len;

    if (ppm_eeprom_access.location != PPM_EEPROMINFO_LOCATION_PI)
    {
        printf("PPM Product fields must select the PI EEPROM location\n");
        return 1;
    }

	system = &eeprom->content.system;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

    if (argc == 2)
    {
		len = strlen(argv[1]);

        if ((len != 1) && (len != 2))
        {
            printf("HW Revision '%s' is too long, maximum of two digits in the range 00 - 99\n", argv[1]);
			return CMD_RET_USAGE;
        }

        strncpy((char *)hwRevision, argv[1], sizeof(system->data.hwRevision) - 1);

        if (((len == 2) &&
            ((hwRevision[0] < '0')  ||
             (hwRevision[0] > '9')  ||
             (hwRevision[1] < '0')  ||
             (hwRevision[1] > '9')))
            ||
            ((len == 1) &&
             ((hwRevision[0] < '0')  ||
              (hwRevision[0] > '9'))))
        {
            printf("Valid HW Revision values range from 00 - 99 (decimal)\n");
			return CMD_RET_USAGE;
        }

        system->data.hwRevision[0] = '-';
        if (len == 2)
        {
            system->data.hwRevision[1] = argv[1][0];
            system->data.hwRevision[2] = argv[1][1];
        }
        else
        {
            system->data.hwRevision[1] = '0';
            system->data.hwRevision[2] = argv[1][0];
        }

        /* Since board hwVersion changed, recalculate
         * system CRC and write content back to memory */
        crc_val = GENERATE_CRC_VALUE(system);
        system->crc = __cpu_to_be32(crc_val);

        eeprom->dirty.system = true;
        return 0;
    }

    printf("PPM HW revision %c%c\n", system->data.hwRevision[1], system->data.hwRevision[2]);
    return ret;
}

/**
 * do_ppm_eeprom_ppm_part_number() - Handle the "ppm_eeprom ppm_part_number" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom ppm_part_number         - show product part number
 *       ppm_eeprom ppm_part_number "<partname/number>" - set product part number
 */
static int do_ppm_eeprom_ppm_part_number(struct cmd_tbl *cmdtp, int flag, int argc,
                                         char * const argv[])
{
	ppm_eeprom_system_crc_t *system;
	char tmpStr[PPM_EEPROMINFO_PN_LENGTH];
	uint32_t len;
    uint32_t crc_val;
    int ret = 0;

    if (ppm_eeprom_access.location != PPM_EEPROMINFO_LOCATION_PI)
    {
        printf("PPM Product fields must select the PI EEPROM location\n");
        return 1;
    }

	system = &eeprom->content.system;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

    if (argc == 2)
    {
        /* Validate argv[1] */
		len = strlen(argv[1]);
        if (len > PPM_EEPROMINFO_PN_LENGTH)
        {
            printf("part number '%s' is too long\n", argv[1]);
			return 1;
        }
        strcpy((char *)&system->data.partNumber, argv[1]);

        /* Since system partNumber changed, recalculate
         * system CRC and write content back to memory */
		crc_val = GENERATE_CRC_VALUE(system);
        system->crc = __be32_to_cpu(crc_val);

        eeprom->dirty.system = true;
        return 0;
    }

	memcpy(tmpStr, system->data.partNumber, sizeof(tmpStr));
    printf("part_number '%s'\n", tmpStr);
    return ret;
}

/**
 * do_ppm_eeprom_ppm_serial_number() - Handle the "ppm_eeprom ppm_serial_number" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom ppm_serial_number         - show PPM product serial number
 *       ppm_eeprom ppm_serial_number "<serialname/number>" - set PPM product serial number
 */
static int do_ppm_eeprom_ppm_serial_number(struct cmd_tbl *cmdtp, int flag, int argc,
                                           char * const argv[])
{
	ppm_eeprom_system_crc_t *system;
	char tmpStr[PPM_EEPROMINFO_SN_LENGTH];
	uint32_t len;
    uint32_t crc_val;
    int ret = 0;

    if (ppm_eeprom_access.location != PPM_EEPROMINFO_LOCATION_PI)
    {
        printf("PPM Product fields must select the PI EEPROM location\n");
        return 1;
    }

	system = &eeprom->content.system;

    if (argc > 2)
    {
        printf("Too many args\n");
        return CMD_RET_USAGE;
    }

    if (argc == 2)
    {
        /* Validate argv[1] */
		len = strlen(argv[1]);
        if (len > PPM_EEPROMINFO_SN_LENGTH)
        {
            printf("serial number '%s' is too long\n", argv[1]);
			return 1;
        }
        strcpy((char *)&system->data.serialNumber, argv[1]);

        /* Since system serialNumber changed, recalculate
         * system CRC and write content back to memory */
		crc_val = GENERATE_CRC_VALUE(system);
        system->crc = __be32_to_cpu(crc_val);

        eeprom->dirty.system = true;
        return 0;
    }

	memcpy(tmpStr, system->data.serialNumber, sizeof(tmpStr));
    printf("serial_number '%s'\n", tmpStr);
    return ret;
}


static struct cmd_tbl cmd_ppm_eeprom_sub[] = {
    U_BOOT_CMD_MKENT(ppm_eeprom debug, 2, 1, do_ppm_eeprom_debug,
                     "ppm_eeprom enable debug",
                     "ppm_eeprom debug true/on/false/off - enable/disable debug"),
    U_BOOT_CMD_MKENT(ppm_eeprom write_enable, 2, 1, do_ppm_eeprom_write_enable, "", ""),
    U_BOOT_CMD_MKENT(ppm_eeprom version, 2, 1, do_ppm_eeprom_version,
					 "ppm_eeprom ", ""),
    U_BOOT_CMD_MKENT(ppm_eeprom save, 2, 1, do_ppm_eeprom_save, "", ""),
    U_BOOT_CMD_MKENT(ppm_eeprom reset, 2, 1, do_ppm_eeprom_reset,
                     "reset in-memory eeprom content",
                     ""),
    U_BOOT_CMD_MKENT(ppm_eeprom dump, 2, 1, do_ppm_eeprom_dump,
                     "dump content of eeprom",
                     NULL),
    U_BOOT_CMD_MKENT(ppm_eeprom info, 2, 1, do_ppm_eeprom_info,
                     "display info content of eeprom",
                     NULL),
    U_BOOT_CMD_MKENT(ppm_eeprom select, 2, 1, do_ppm_eeprom_select,
                     "set/show eeprom location",
                     "[som|pi] - set/show eeprom location"),
    U_BOOT_CMD_MKENT(ppm_eeprom hw_revision, 2, 1, do_ppm_eeprom_board_hw_revision,
                     "set/show board HW revision",
                     "[0-99] - set/show board HW revision"),
    U_BOOT_CMD_MKENT(ppm_eeprom hw_tweak_bits, 2, 1, do_ppm_eeprom_board_hw_tweak_bits,
                     "set/show board HW tweak bits",
                     "[0-0xFFFFFFFF] - set/show board HW tweak bits"),
    U_BOOT_CMD_MKENT(ppm_eeprom sw_compatibility, 2, 1, do_ppm_eeprom_board_sw_compatibility,
                     "set/show eeprom board sw compatibility",
                     "[0-9999] - set/show board sw compatibility"),
    U_BOOT_CMD_MKENT(ppm_eeprom part_number, 2, 1, do_ppm_eeprom_board_part_number,
                     "set/show eeprom board part number",
                     "[part number] set/show board part number"),
    U_BOOT_CMD_MKENT(ppm_eeprom serial_number, 2, 1, do_ppm_eeprom_board_serial_number,
                     "set/show eeprom board serial number",
                     "[part number] set/show board serial number"),
    U_BOOT_CMD_MKENT(ppm_eeprom mac, 2, 1, do_ppm_eeprom_mac,
                     "show MAC address from a MAC EEPROM",
                     "[MAC EEPROM] show MAC address from MAC EEPROM"),
    U_BOOT_CMD_MKENT(ppm_eeprom ppm_hw_revision, 2, 1, do_ppm_eeprom_ppm_hw_revision,
                     "set/show PPM product HW revision (PI EEPROM only)",
                     "[0-99] - set/show PPM product HW revision"),
    U_BOOT_CMD_MKENT(ppm_eeprom ppm_part_number, 2, 1, do_ppm_eeprom_ppm_part_number,
                     "set/show eeprom PPM product part number",
                     "[part number] set/show PPM product part number"),
    U_BOOT_CMD_MKENT(ppm_eeprom ppm_serial_number, 2, 1, do_ppm_eeprom_ppm_serial_number,
                     "set/show eeprom PPM product serial number",
                     "[part number] set/show PPM product serial number"),
};


static int do_ppm_eeprom(struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
    struct cmd_tbl *c;
    int rc;
    int len;
    char cmd_string[32];


	/* If first time command run, initialize */
	if (PPM_EEPROMINFO_LOCATION_UNKNOWN == ppm_eeprom_access.location)
	{
		/*
		 * Select SOM EEPROM as default.
		 * If SOM eeprom not programmed that is OK.
		 */
		ppm_eeprom_select_location(PPM_EEPROMINFO_LOCATION_SOM);
	}


	if (argc < 2)
	{
		/* No args, default to 'info' sub-command. */
		do_ppm_eeprom_info(cmdtp, flag, argc, argv);
		return(CMD_RET_SUCCESS);
	}

    /* Concatenate "ppm_eeprom" with subcommand to produce fully
     * qualified command - needed to make subcommand help on error
	 * to print correctly.
	 * "ppm_eeprom" must be used because uBoot supports partial
	 * command matching. */
    len = snprintf(cmd_string, sizeof(cmd_string), "%s %s", "ppm_eeprom", argv[1]);
    if (len >= sizeof(cmd_string))
    {
        printf("ppm_eeprom command '%s' too long!\n", argv[1]);
        return CMD_RET_USAGE;
    }

	/* Strip off leading 'ppm_eeprom' command argument */
	argc--;
	argv++;


	c = find_cmd_tbl(cmd_string, &cmd_ppm_eeprom_sub[0], ARRAY_SIZE(cmd_ppm_eeprom_sub));

	if (!c)
    {
        rc = CMD_RET_USAGE;
    }
    else
    {
		rc = c->cmd(cmdtp, flag, argc, argv);
        if (rc == CMD_RET_USAGE)
        {
            rc = cmd_usage(c);
        }
    }

    return rc;
}

static char ppm_eeprom_help_text[] =
    "\n"
    "****************************************************************\n"
    "WARNING: Most of the EEPROM configuration values are set\n"
    "         during manufacturing.  Changing these values\n"
    "         will overwrite the default values\n"
    "****************************************************************\n"
    "info - display all known/valid eeprom content\n"
    "select [som|pi|afe1|afe2|wcm] - select which eeprom\n"
    "hw_revision [number] - set/show board hw revision\n"
    "hw_tweak_bits [bitmask] - set/show board hw tweak bits\n"
    "sw_compatibility [number] - set/show sw compatibility index\n"
    "part_number [string] - set/show part number\n"
    "serial_number [string] - set/show serial number\n"
    "ppm_hw_revision [number] - set/show PPM product revision\n"
    "ppm_part_number [string] - set/show PPM product part number\n"
    "ppm_serial_number [string] - set/show PPM product serial number\n"
    "\n"
    "mac [MAC EEPROM name] - show MAC EEPROM data\n"
    "\n"
	"\n"
    "debug [true|false] - enable/disable debug output\n"
	"write_enable [true|on|false|off] - enable/disable EEPROM writing\n"
    "version [number] - set/show eeprom layout version\n"
    "save - save any changes to eeprom\n"
    "reset - reset in-memory eeprom content\n"
    ;


U_BOOT_CMD(
	ppm_eeprom,7,0,do_ppm_eeprom,
	"PPM EEPROMs control",
	ppm_eeprom_help_text
    );





