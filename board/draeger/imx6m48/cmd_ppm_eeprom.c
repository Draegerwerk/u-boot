/**
 *
 * @file "cmd_ppm_eeprom.c"
 *
 * @brief This provides an implementation of PI board eeprom access routines
 *        for u-boot.
 *
 * @copyright Copyright 2019 Draeger and Licensors. All Rights Reserved.
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

#define AMOUNT_INCREASE_FDT_SIZE 512

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
    PPM_EEPROMINFO_BOARD_ID     board;      /* board  ID */
} ppm_eeprom_board_id_table[] =
{
    {
        "1GB DUAL SOM",
        PPM_EEPROMINFO_BOARD_SOM_1GB_DUAL,
    },
    {
        "2GB DUAL SOM",
        PPM_EEPROMINFO_BOARD_SOM_2GB_DUAL,
    },
    {
        "1GB QUAD SOM",
        PPM_EEPROMINFO_BOARD_SOM_1GB_QUAD,
    },
    {
        "2GB QUAD SOM",
        PPM_EEPROMINFO_BOARD_SOM_2GB_QUAD,
    },
    {
        "SSPPM",
        PPM_EEPROMINFO_BOARD_SSPPM_PI,
    },
    {
        "TSPPM-M",
        PPM_EEPROMINFO_BOARD_TSPPM_M_PI,
    },
    {
        "TSPPM-B",
        PPM_EEPROMINFO_BOARD_TSPPM_B_PI,
    },
    {
        "MOUNT",
        PPM_EEPROMINFO_BOARD_MOUNT_PI,
    },
    {
        "AFE1",
        PPM_EEPROMINFO_BOARD_AFE1    /* AFE = "Analogue Front End" */
    },
    {
        "AFE2",
        PPM_EEPROMINFO_BOARD_AFE2
    },
    {
        "RADIO",
        PPM_EEPROMINFO_BOARD_RADIO
    },
    {
        "UI",
        PPM_EEPROMINFO_BOARD_UI    /* UI = "user interface" */
    },
    {
        "BATTERY",
        PPM_EEPROMINFO_BOARD_BATTERY
    },
};


#define PPM_WLS_ADDR_ENV_NAME "wlsaddr"
/*
 * Save a new env variable, holding the Wireless MAC address
 */
int ppm_setenv_wlsaddr(char *name, const uchar *wls_macaddr)
{
	char buf[20];

	sprintf(buf, "%pM", wls_macaddr);

	return env_set(name, buf);
}


static bool ppm_eeprom_validate_board_id(const char *str, PPM_EEPROMINFO_BOARD_ID *id)
{
    struct board_id_entry *p;
    int idx;

    for (idx = 0; idx < ARRAY_SIZE(ppm_eeprom_board_id_table); ++idx)
    {
        p = &ppm_eeprom_board_id_table[idx];
        if (!strcasecmp(p->name, str))
        {
            *id = p->board;
            return true;
        }
    }
    return false;
}

static bool ppm_eeprom_board_name(PPM_EEPROMINFO_BOARD_ID board, const char **str)
{
    struct board_id_entry *p;
    int idx;

    for (idx = 0; idx < ARRAY_SIZE(ppm_eeprom_board_id_table); ++idx)
    {
        p = &ppm_eeprom_board_id_table[idx];
        if (p->board == board)
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
        "RADIO",
        PPM_EEPROMINFO_LOCATION_RADIO
    },
    {
        "UI",
        PPM_EEPROMINFO_LOCATION_UI
    },
    {
        "BATTERY",
        PPM_EEPROMINFO_LOCATION_BATTERY
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

#if 0

/* This function is currently not used */

/**
 * ppm_eeprom_parse_ethaddr() - parse ethernet MAC address
 * @addr: input string to parse
 * @macaddr: output binary MAC address
 *
 * Return true if addr is parsable as valid MAC address
 */
static bool ppm_eeprom_parse_ethaddr(const char *addr, uchar macaddr[PPM_EEPROMINFO_MAC_ADDR_LENGTH])
{
    int idx;
    char *end;

    for (idx=0; idx < 6; ++idx)
    {
        u32 byte = simple_strtoul(addr, &end, 16);
        if (byte > 0xff)
        {
            return false;
        }

        macaddr[idx] = byte;
        if (idx == 5 && (end && (*end == '\0')))
        {

            if (is_valid_ethaddr(macaddr))
                return true;
            else
                return false;
        }
        if (end && (*end != ':'))
        {
            return false;
        }
        /* Skip over colon */
        addr = end + 1;
    }
    return false;
}
#endif

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

	if (layout->data.version != __cpu_to_be32(PPM_EEPROMINFO_LAYOUT_VERSION))
    {
		PPM_EEPROMINFO_DEBUG("%s:%d Layout version %08x != expected %08x\n",
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
	eeprom->valid.layout = true;
    return 0;
}

static void ppm_eeprom_reset_board(ppm_eeprom_board_crc_t *board)
{
    uint32_t crc_val;

	/* Provide default items here */
	board->data.boardId = PPM_EEPROMINFO_BOARD_UNKNOWN;
	board->data.hwRevision = __cpu_to_be16(0);
	board->data.hwTweakBits = __cpu_to_be32(0x00000000);
	board->data.swCompatibility = __cpu_to_be16(0);
	board->data.partNumber[0] = '\0';
	board->data.serialNumber[0] = '\0';

	/* Generate CRC of board */
	crc_val = GENERATE_CRC_VALUE(board);
	board->crc = __cpu_to_be32(crc_val);
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
	if (!invalid)
	{
		/* boardId invalid ... */
		if ((__be16_to_cpu(board->data.hwRevision) > PPM_EEPROMINFO_MAX_REVISION)
			|| (__be16_to_cpu(board->data.swCompatibility) > PPM_EEPROMINFO_MAX_REVISION))
		{
			PPM_EEPROMINFO_DEBUG("%s:%d hwRevision or swCompatibility are invalid\n", __func__, __LINE__);
			invalid = true;
		}

		/* revision out of range... */
		/* revisionIndex out of range... */
		if (!invalid)
		{
			if ((strnlen((char *)board->data.partNumber, PPM_EEPROMINFO_STRING_LENGTH)>= PPM_EEPROMINFO_STRING_LENGTH)
				|| (strnlen((char *)board->data.serialNumber, PPM_EEPROMINFO_STRING_LENGTH)>= PPM_EEPROMINFO_STRING_LENGTH))
			{
				PPM_EEPROMINFO_DEBUG("%s:%d part/serial number length invalid\n", __func__, __LINE__);
				invalid = true;
			}
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
        return (-1);
    }

    if (mux->leg < 1 || mux->leg > 8)
    {
        printf("Invalid mux leg %u\n", mux->leg);
        return -1;
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
		return -1;
	}

	/* Does access to eeprom require a mux? */
    if (access->numMuxes)
    {
        if (ppm_eeprom_select_muxes(access->i2c_bus, access->muxes, access->numMuxes))
        {
            printf("%s: Failed to probe/configured EEPROM mux on I2C%d\n",
                   __func__,
				   access->i2c_bus + 1 /* I2C ctrls start from zero */);
            return -1;
        }
    }

    /* Read initial SOM eeprom content */
    eeprom = access->eeprom;
    PPM_EEPROMINFO_DEBUG("%s:%d eeprom->content %p\n", __func__, __LINE__, &eeprom->content);

    eeprom->valid.layout = eeprom->valid.board = false;
    eeprom->dirty.layout = eeprom->dirty.board = false;

    ret = ppm_eeprom_read(0x0, &eeprom->content, sizeof(eeprom->content));
    if (ret)
    {
        printf("%s:%d ppm_eeprom_read failed with %d\n", __func__, __LINE__, ret);
        return ret;
    }
    /* Validate both board and macaddr parts of ppm_eeprom_content */
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

        case PPM_EEPROMINFO_LOCATION_UI:
        /* I2C1 */
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

        case PPM_EEPROMINFO_LOCATION_BATTERY:
        /* Battery Interconnect module is on leg 5 of I2C3 Hub */
        /* Battery Interconnect board is pending design.  I2C address not
           known.  Assumed to be 0x40. Please change if incorrect.       */
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
	char tmpStr[PPM_EEPROMINFO_STRING_LENGTH+1];
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
		if (!ppm_eeprom_board_name(eeprom->content.board.data.boardId, &boardName))
		{
			printf("          id %d invalid!\n", eeprom->content.board.data.boardId);
			ret = 1;
		}
		else
		{
			printf("          id %d (%s)\n", eeprom->content.board.data.boardId, boardName);
		}
		printf("          hwRevision %04d\n", __be16_to_cpu(eeprom->content.board.data.hwRevision));

		printf("          hwTweakBits 0x%08x\n", __be32_to_cpu(eeprom->content.board.data.hwTweakBits));

		printf("          swCompatibility %04d\n", __be16_to_cpu(eeprom->content.board.data.swCompatibility));
		len = strnlen((char *)eeprom->content.board.data.partNumber, PPM_EEPROMINFO_STRING_LENGTH);
		if (len >= PPM_EEPROMINFO_STRING_LENGTH)
		{
			printf("          partNumber invalid! (len %u)\n", len);
			ret = 1;
		}
		else
		{
			memcpy(tmpStr, eeprom->content.board.data.partNumber, sizeof(tmpStr));
			printf("          partNumber '%s'\n", tmpStr);
		}

		len = strnlen((char *)eeprom->content.board.data.serialNumber, PPM_EEPROMINFO_STRING_LENGTH);
		if (len >= PPM_EEPROMINFO_STRING_LENGTH)
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
            if (ppm_eeprom_board_name(p->content.board.data.boardId, &boardName))
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
	char tmpStr[PPM_EEPROMINFO_STRING_LENGTH+1];
	const char *boardName;
	u32 len;

    if (ppm_eeprom_board_valid(p, &boardName))
    {
        printf("%s boardId:      %s(%u) hwRevision %u (swCompatibility %u)\n", p->name, boardName, p->content.board.data.boardId, __be16_to_cpu(p->content.board.data.hwRevision), __be16_to_cpu(p->content.board.data.swCompatibility));
		printf("%s hwTweakBits:  0x%08x\n", p->name, __be32_to_cpu(p->content.board.data.hwTweakBits));
        len = strnlen((char *)p->content.board.data.partNumber, PPM_EEPROMINFO_STRING_LENGTH);
        if (len && (len < PPM_EEPROMINFO_STRING_LENGTH))
        {
            memcpy(tmpStr, p->content.board.data.partNumber, sizeof(tmpStr));
            tmpStr[len] = '\0';
            printf("%s partNumber:   %s\n", p->name, tmpStr);
        }

        len = strnlen((char *)p->content.board.data.serialNumber, PPM_EEPROMINFO_STRING_LENGTH);
        if (len && (len < PPM_EEPROMINFO_STRING_LENGTH))
        {
            memcpy(tmpStr, p->content.board.data.serialNumber, sizeof(tmpStr));
            tmpStr[len] = '\0';
            printf("%s serialNumber: %s\n", p->name, tmpStr);
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
 * @return bool - true if valid eeprom info found in SOM/PI EEPROMs
 */
void ppm_eeprom_init_env(PPM_UNIT_TYPE ppmUnitType)
{
    /* Inspect PI and SOM eeproms */
    char            *env_ethaddr;
    u8              ethaddr[PPM_EEPROMINFO_MAC_ADDR_LENGTH];
    u8              wlsaddr[PPM_EEPROMINFO_MAC_ADDR_LENGTH];
    char            ethaddr_str[PPM_EEPROMINFO_MAC_ADDR_LENGTH * 3];
    /* ^ string to hold MAC address from eeprom */
    const char      *ethaddr_src = NULL;
    const char      *wlsaddr_src = NULL;
    bool            found_valid_ethaddr = false;
    bool            found_valid_wlsaddr = false;

    unsigned int    i;
    int             nrOfEeproms;

    /* Set the eth MAC addr to all 0xff to be invalid */
    memset(ethaddr, 0xff, sizeof(ethaddr));

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
            "RADIO"
        },
        {
            PPM_EEPROMINFO_LOCATION_UI,
            &ppm_eeprom_ui,
            "UI"
        },
        {
            PPM_EEPROMINFO_LOCATION_BATTERY,
            &ppm_eeprom_battery,
            "BATTERY"
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

    if (!ppm_eeprom_read_mac(ENET_MAC_ADDR_INDEX, ethaddr))
	{
	    found_valid_ethaddr = true;
	    ethaddr_src = "ENET MAC EEPROM";
	}

    if (ppmUnitType == PPM_UNIT_SSPPM)
    {
        if (!ppm_eeprom_read_mac(WIFI_MAC_ADDR_INDEX, wlsaddr))
	    {
	        found_valid_wlsaddr = true;
            wlsaddr_src = "WIFI MAC EEPROM";
        }
    }

	/* If have a valid ethaddr */
	if (found_valid_ethaddr)
	{
		/* Have an ethernet address; check if same as what's in envirionment */
		snprintf(ethaddr_str, sizeof(ethaddr_str), "%pM", ethaddr);

		env_ethaddr = env_get("ethaddr");
		if (!env_ethaddr)
		{
			printf("Setting ethaddr to %s %pM\n", ethaddr_src, ethaddr);
			eth_env_set_enetaddr("ethaddr", ethaddr);
		}
		else if (strcasecmp(env_ethaddr, ethaddr_str))
		{
			/* Environment doesn't match; override environment with eeprom */
			printf("Override ethaddr to %s %pM\n", ethaddr_src, ethaddr);
			eth_env_set_enetaddr("ethaddr", ethaddr);
		}
		else
		{
			printf("ethaddr is %s %pM\n", ethaddr_src, ethaddr);
		}
	}
	else
	{
		printf("No valid Ethernet MAC address found in PI MAC EEPROM\n");
	}

	/* If have a valid wls addr */
	if (found_valid_wlsaddr)
	{
		/* Have an wls address; check if same as what's in envirionment */
		snprintf(ethaddr_str, sizeof(ethaddr_str), "%pM", wlsaddr);

		env_ethaddr = env_get("wlsaddr");
		if (!env_ethaddr)
		{
			printf("Setting wlsaddr to %s %pM\n", wlsaddr_src, wlsaddr);
			ppm_setenv_wlsaddr(PPM_WLS_ADDR_ENV_NAME, wlsaddr);
		}
		else if (strcasecmp(env_ethaddr, ethaddr_str))
		{
			/* Environment doesn't match; override environment with eeprom */
			printf("Override wlsaddr to %s %pM\n", ethaddr_src, wlsaddr);
			ppm_setenv_wlsaddr(PPM_WLS_ADDR_ENV_NAME, wlsaddr);
		}
		else
		{
			printf("wlsaddr is %s %pM\n", wlsaddr_src, wlsaddr);
		}
	}
	else if (ppmUnitType == PPM_UNIT_SSPPM)
	{
		printf("No valid Wireless MAC address found in WIFI MAC EEPROM\n");
	}

	/* Deselect eeprom */
	ppm_eeprom_select_location(PPM_EEPROMINFO_LOCATION_UNKNOWN);
}

#ifdef PPM_EEPROMINFO_DEBUG_ENABLE
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
		return -1;
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

    printf("version 0x%x\n", layout->data.version);
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
            printf("%s:%d IS8 MAC EEPROM bad index\n", __func__, __LINE__);
            return -1;
        }

    if (!strlen(macEeproms[eeprom_index].name))
        {
            printf("%s:%d IS8 MAC EEPROM bad name\n", __func__, __LINE__);
            return -1;
        }

    /* select mux */
    err = ppm_eeprom_select_muxes(macEeproms[eeprom_index].i2c_bus,
                                  macEeproms[eeprom_index].i2c_muxes,
                                  macEeproms[eeprom_index].numMuxes);
    if (err)
    {
        printf("%s:%d IS8 MAC EEPROM bad\n", __func__, __LINE__);

        /* failed, error message already printed from inside function */
        return err;
    }

    /* get and probe eeprom device */
    if (i2c_get_chip_for_busnum(macEeproms[eeprom_index].i2c_bus,
                                macEeproms[eeprom_index].i2c_address,
								1, &i2c_dev))
    {
        printf("Failed to get/probe eeprom at 0x%02X\n", macEeproms[eeprom_index].i2c_address);
        return (-1);
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
        printf("%s:%d IS8 MAC EEPROM bad\n", __func__, __LINE__);
        return(-1);
    }

    /*** validate MAC Address ***/
    if (data[3] < 0x01 || data[3] > 0x0F)
    {
        printf("%s:%d IS8 MAC EEPROM illegal format\n", __func__, __LINE__);
        return(-1);
    }

    /* separate last two bytes into two bit fields: 6-bits and 10-bits.  The
     * 10-bit field may not be zero. */
    if (!(((data[4] << 8) | data[5]) & TEN_BIT_MASK))
    {
        printf("%s:%d IS8 MAC EEPROM illegal format\n", __func__, __LINE__);
        return(-1);
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
        return (-1);
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
            err = -1;
        }
    }

    /* validate fourth byte */
    if (data[3] < 0x01 || data[3] > 0x0F)
    {
        printf("Error: MAC Address 4th byte must be in range 0x01-0x0F\n");
        err = -1;
    }

    /* separate last two bytes into two bit fields: 6-bits and 10-bits.  The
     * 10-bit field may not be zero. */
    if (!(((data[4] << 8) | data[5]) & TEN_BIT_MASK))
    {
        printf("Error: the last ten bytes of the MAC address may not equal "
               "zero\n");
        err = -1;
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
        return -1;
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

	/* If we get here and ret==1 then there was nothing to write. */
	if (ret)
	{
		printf("Nothing written to %s EEPROM.\n", ppm_eeprom_access.eeprom->name);
	}

	return 0;
}
#endif

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
        { PPM_EEPROMINFO_LOCATION_KEY_UI,      PPM_EEPROMINFO_LOCATION_UI     },
        { PPM_EEPROMINFO_LOCATION_KEY_BATTERY, PPM_EEPROMINFO_LOCATION_BATTERY},
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

	eeprom->dirty.layout = true;
	eeprom->dirty.board = true;
	eeprom->valid.layout = true;
	eeprom->valid.board = true;

	printf("%s all sections reset.\n", ppm_eeprom_access.eeprom->name);

	return 0;
}

/**
 * do_ppm_eeprom_board_id() - Handle the "ppm_eeprom id" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Argument count (ppm_eeprom notwithstanding)
 * @argv:	Arguments (ppm_eeprom notwithstanding)
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *       ppm_eeprom id        - show board id
 *       ppm_eeprom id [1gb dual som|2gb dual som|1gb quad som|2gb quad som|ssppm|tsppm-m|tsppm-b|mount] - set board id
 */
static int do_ppm_eeprom_board_id(struct cmd_tbl *cmdtp, int flag, int argc,
				char * const argv[])
{
	ppm_eeprom_board_crc_t *board;
    PPM_EEPROMINFO_BOARD_ID board_id = PPM_EEPROMINFO_BOARD_UNKNOWN;
	const char *boardName;
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
        ret = ppm_eeprom_validate_board_id(argv[1], &board_id);
        if (!ret)
        {
            printf("Board ID '%s' is invalid\n", argv[1]);
            return CMD_RET_USAGE;
        }

        /* Since board id changes, recalculate
         * board CRC and write content back to memory */
        board->data.boardId = board_id;
        crc_val = GENERATE_CRC_VALUE(board);
        board->crc = __be32_to_cpu(crc_val);

        eeprom->dirty.board = true;
    }

	/* Display board ID */
	printf("%s board is", ppm_eeprom_access.eeprom->name);
	if (!ppm_eeprom_board_name(eeprom->content.board.data.boardId, &boardName))
	{
		printf(" UNKNOWN");
	}
	else
	{
		printf(" '%s'", boardName);
	}
	printf(" (%d)\n", eeprom->content.board.data.boardId);
    return ret;
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
 *       ppm_eeprom hw_revision 9999   - set eeprom hw revision to 9999
 */
static int do_ppm_eeprom_board_hw_revision(struct cmd_tbl *cmdtp, int flag, int argc,
				char * const argv[])
{
	ppm_eeprom_board_crc_t *board;
    uint32_t hwRevision;
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

	hwRevision = __be16_to_cpu(board->data.hwRevision);

    if (argc == 2)
    {
        hwRevision = simple_strtoul(argv[1], &end, 10);
		/* hwRevision range is 0000 .. 9999 (decimal) */
		if ((end && (*end != '\0')) || (hwRevision > PPM_EEPROMINFO_MAX_REVISION))
		{
			printf("HW revision '%s' invalid\n", argv[1]);
			return CMD_RET_USAGE;
		}
        board->data.hwRevision = __cpu_to_be16(hwRevision);

        /* Since board sw compatibility changed, recalculate
         * board CRC and write content back to memory */
        crc_val = GENERATE_CRC_VALUE(board);
        board->crc = __cpu_to_be32(crc_val);

        eeprom->dirty.board = true;
        return 0;
    }

    printf("HW revision %04u\n", __be16_to_cpu(board->data.hwRevision));
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
	char tmpStr[PPM_EEPROMINFO_STRING_LENGTH+1];
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
        if (len >= PPM_EEPROMINFO_STRING_LENGTH)
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
	char tmpStr[PPM_EEPROMINFO_STRING_LENGTH+1];
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
        if (len >= PPM_EEPROMINFO_STRING_LENGTH)
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

static struct cmd_tbl cmd_ppm_eeprom_sub[] = {
#ifdef PPM_EEPROMINFO_DEBUG_ENABLE
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
#endif
    U_BOOT_CMD_MKENT(ppm_eeprom dump, 2, 1, do_ppm_eeprom_dump,
                     "dump content of eeprom",
                     NULL),
    U_BOOT_CMD_MKENT(ppm_eeprom info, 2, 1, do_ppm_eeprom_info,
                     "display info content of eeprom",
                     NULL),
    U_BOOT_CMD_MKENT(ppm_eeprom select, 2, 1, do_ppm_eeprom_select,
                     "set/show eeprom location",
                     "[som|pi] - set/show eeprom location"),
    U_BOOT_CMD_MKENT(ppm_eeprom id, 2, 1, do_ppm_eeprom_board_id,
                     "set or show board id",
                     "[1gb dual som|2gb dual som|1gb quad som|2gb quad som|ssppm|tsppm-m|tsppm-b|mount|afe1|afe2|ui|radio|battery] - set/show eeprom board id"),
    U_BOOT_CMD_MKENT(ppm_eeprom hw_revision, 2, 1, do_ppm_eeprom_board_hw_revision,
                     "set/show board HW revision",
                     "[0-9999] - set/show board HW revision"),
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
    "select [som|pi|afe1|afe2] - select which eeprom\n"
    "id [1gb dual som|2gb dual som|1gb quad som|2gb quad som|ssppm|tsppm-m|tsppm-b|mount|afe1|afe2] - set/show board id\n"
    "hw_revision [number] - set/show board hw revision\n"
    "hw_tweak_bits [bitmask] - set/show board hw tweak bits\n"
    "sw_compatibility [number] - set/show sw compatibility index\n"
    "part_number [string] - set/show part number\n"
    "serial_number [string] - set/show serial number\n"
    "eth_addr [MAC addr] - set/show ethernet MAC address\n"
    "wifi_addr [MAC addr] - set/show WiFi MAC address\n"
    "\n"
    "mac [MAC EEPROM name] - show MAC EEPROM data\n"
    "\n"
#ifdef PPM_EEPROMINFO_DEBUG_ENABLE
	"\n"
    "debug [true|false] - enable/disable debug output\n"
	"write_enable [true|on|false|off] - enable/disable EEPROM writing\n"
    "version [number] - set/show eeprom layout version\n"
    "save - save any changes to eeprom\n"
    "reset - reset in-memory eeprom content\n"
#endif
;


U_BOOT_CMD(
	ppm_eeprom,7,0,do_ppm_eeprom,
	"PPM EEPROMs control",
	ppm_eeprom_help_text
);


#ifdef CONFIG_OF_BOARD_SETUP

/**
 * Node to create in root path to hold each eeprom information
 */
#define EEPROM_INFO_NODE "eepromInfo"

/**
 * root path where to create EEPROM_INFO_NODE
 */
#define ROOT_IMX6Q_FDT_PATH "/imx6"

/**
 * compatible string in EEPROM_INFO_NODE
 */
#define EEPROM_INFO_COMPATIBLE_STR "draeger,usrspceeprominfo"

/**
 * ft_add_eeprom_info() - add particular eeprom information to the FDT
 */
int ft_add_eeprom_info(struct fdt_header *fdt, ppm_eeprom_t *eeprom, const char *nodeName, uint8_t *ethAddrPtr, uint8_t *wifiAddrPtr)
{
    char *tmpStr;
    int eepromInfoOffset;
    int rootNodeOffset;
    int infoOffset;
    uint32_t tmp_u32, tmp2_u32;
    int err;
    char tmpBuf[PPM_EEPROMINFO_STRING_LENGTH + 1];

    /* 1) if ROOT_IMX6Q_FDT_PATH doesn't exist, fail */
	rootNodeOffset = fdt_path_offset(fdt, ROOT_IMX6Q_FDT_PATH);
    if (rootNodeOffset == -FDT_ERR_NOTFOUND)
    {
		printf("%s is not found in Device Tree Blob\n", ROOT_IMX6Q_FDT_PATH);
		return rootNodeOffset;
	}
    if (rootNodeOffset < 0)
    {
        printf("%s:%d nodeName %s rootNodeOffset %d\n", __func__, __LINE__, nodeName, rootNodeOffset);
        return rootNodeOffset;
    }

	/* 2) If EEPROM_INFO_NODE doesn't exist in /imx6q then create it */
    eepromInfoOffset = fdt_subnode_offset(fdt, rootNodeOffset, EEPROM_INFO_NODE);
    if (eepromInfoOffset == -FDT_ERR_NOTFOUND)
    {
        printf("%s: %s not found; creating\n", __func__, EEPROM_INFO_NODE);
      add_subnode:
        eepromInfoOffset = fdt_add_subnode(working_fdt, rootNodeOffset, EEPROM_INFO_NODE);
        if (eepromInfoOffset == -FDT_ERR_NOSPACE)
        {
            int ret;
            printf("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
            ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
            if (!ret)
            {
                goto add_subnode;
            }
        }
        if (eepromInfoOffset < 0)
        {
            printf("%s:%d failed to create /%s eepromInfoOffset %d\n", __func__, __LINE__, EEPROM_INFO_NODE, eepromInfoOffset);
            return eepromInfoOffset;
        }

        /* Create compatible string in /imx6q/eepromInfo */
        tmpStr = EEPROM_INFO_COMPATIBLE_STR;
      add_compatible:
        err = fdt_setprop(fdt, eepromInfoOffset, "compatible", tmpStr, strlen(tmpStr) + 1);
        if (err == -FDT_ERR_NOSPACE)
        {
            int ret;
            PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
            ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
            if (!ret)
            {
                goto add_compatible;
            }
        }
        if (err < 0)
        {
            printf("%s:%d\n err %d\n", __func__, __LINE__, err);
            return err;
        }
    }
    else if (eepromInfoOffset < 0)
    {
        printf("%s:%d eepromInfoOffset %d\n", __func__, __LINE__, eepromInfoOffset);
        return eepromInfoOffset;
        
    }

    /* 3) create nodeName in EEPROM_INFO_NODE */
  add_nodeName:
    infoOffset = fdt_add_subnode(fdt, eepromInfoOffset, nodeName);
    if (infoOffset == -FDT_ERR_NOSPACE)
    {
        int ret;
        PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
        ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
        if (!ret)
        {
            goto add_nodeName;
        }
    }
    if (infoOffset < 0)
    {
        printf("%s:%d infoOffset %d\n", __func__, __LINE__, infoOffset);
        return infoOffset;
    }

    /* 4) populate nodeName node with content of eeprom */
    tmp2_u32 = eeprom->content.board.data.boardId;
    tmp_u32 = cpu_to_fdt32(tmp2_u32);
  add_boardId:
    err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_BOARD_ID, &tmp_u32, sizeof(tmp_u32));
    if (err == -FDT_ERR_NOSPACE)
    {
        int ret;
        PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
        ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
        if (!ret)
        {
            goto add_boardId;
        }
    }
    if (err < 0)
    {
        printf("%s:%d\n err %d\n", __func__, __LINE__, err);
        return err;
    }

    tmp2_u32 = __be16_to_cpu(eeprom->content.board.data.hwRevision);
    tmp_u32 = cpu_to_fdt32(tmp2_u32);
  add_hwRevision:
    err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_HW_REVISION, &tmp_u32, sizeof(tmp_u32));
    if (err == -FDT_ERR_NOSPACE)
    {
        int ret;
        PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
        ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
        if (!ret)
        {
            goto add_hwRevision;
        }
    }
    if (err < 0)
    {
        printf("%s:%d\n err %d\n", __func__, __LINE__, err);
        return err;
    }

    tmp2_u32 = __be32_to_cpu(eeprom->content.board.data.hwTweakBits);
    tmp_u32 = cpu_to_fdt32(tmp2_u32);
  add_hwTweakBits:
    err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_HW_TWEAK_BITS, &tmp_u32, sizeof(tmp_u32));
    if (err == -FDT_ERR_NOSPACE)
    {
        int ret;
        PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
        ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
        if (!ret)
        {
            goto add_hwTweakBits;
        }
    }
    if (err < 0)
    {
        printf("%s:%d\n err %d\n", __func__, __LINE__, err);
        return err;
    }

    tmp2_u32 = __be16_to_cpu(eeprom->content.board.data.swCompatibility);
    tmp_u32 = cpu_to_fdt32(tmp2_u32);
  add_swCompatibility:
    err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_SW_COMPATIBILITY, &tmp_u32, sizeof(tmp_u32));
    if (err == -FDT_ERR_NOSPACE)
    {
        int ret;
        PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
        ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
        if (!ret)
        {
            goto add_swCompatibility;
        }
    }
    if (err < 0)
    {
        printf("%s:%d\n err %d\n", __func__, __LINE__, err);
        return err;
    }

    if (strlen((char *)eeprom->content.board.data.partNumber))
    {
        memcpy(tmpBuf, eeprom->content.board.data.partNumber, sizeof(tmpBuf));
        tmpBuf[sizeof(tmpBuf) - 1] = 0x00;
      add_partNumber:
        err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_PART_NUMBER, tmpBuf, strlen(tmpBuf) + 1);
        if (err == -FDT_ERR_NOSPACE)
        {
            int ret;
            PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
            ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
            if (!ret)
            {
                goto add_partNumber;
            }
        }
        if (err < 0)
        {
            printf("%s:%d\n err %d\n", __func__, __LINE__, err);
            return err;
        }
    }

    if (strlen((char *)eeprom->content.board.data.serialNumber))
    {
        memcpy(tmpBuf, eeprom->content.board.data.serialNumber, sizeof(tmpBuf));
        tmpBuf[sizeof(tmpBuf) - 1] = 0x00;
      add_serialNumber:
        err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_SERIAL_NUMBER, tmpBuf, strlen(tmpBuf) + 1);
        if (err == -FDT_ERR_NOSPACE)
        {
            int ret;
            PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
            ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
            if (!ret)
            {
                goto add_serialNumber;
            }
        }
        if (err < 0)
        {
            printf("%s:%d\n err %d\n", __func__, __LINE__, err);
            return err;
        }
    }

    if (ethAddrPtr && is_valid_ethaddr(ethAddrPtr))
    {
      add_ethAddr:
        err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_ETH_ADDR, ethAddrPtr, PPM_EEPROMINFO_MAC_ADDR_LENGTH);
        if (err == -FDT_ERR_NOSPACE)
        {
            int ret;
            PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
            ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
            if (!ret)
            {
                goto add_ethAddr;
            }
        }
        if (err < 0)
        {
            printf("%s:%d\n err %d\n", __func__, __LINE__, err);
            return err;
        }
    }

    if (wifiAddrPtr && is_valid_ethaddr(wifiAddrPtr))
    {
      add_wifiAddr:
        err = fdt_setprop(fdt, infoOffset, PPM_EEPROMINFO_PROPERTY_WIFI_ADDR, wifiAddrPtr, PPM_EEPROMINFO_MAC_ADDR_LENGTH);
        if (err == -FDT_ERR_NOSPACE)
        {
            int ret;
            PPM_EEPROMINFO_DEBUG("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
            ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
            if (!ret)
            {
                goto add_wifiAddr;
            }
        }
        if (err < 0)
        {
            printf("%s:%d\n err %d\n", __func__, __LINE__, err);
            return err;
        }
    }

    return err;
}

/*
 * Pass the Wireless MAC address into the FDT for VxWorks WIFI driver access.
 */
void fdt_fixup_wls(void *fdt)
{
    int node, i;
    char wls[10], *tmp;
    const char *path;
    char redpinepath[64];
    unsigned char mac_addr[6];
    int ret;

	node = fdt_path_offset(fdt, "/aliases");
	if (node < 0)
		return;

	if (!env_get("wlsaddr")) {
                debug("No wireless MAC Address env variable defined\n");
                return;
	}

        i = 0;
	if ((tmp = env_get("wlsaddr")) != NULL) {
                snprintf(wls, sizeof(wls), "wireless%d", i);
		path = fdt_getprop(fdt, node, wls, NULL);
		if (!path) {
			debug("No alias for %s\n", wls);
			return;
                }		

                string_to_enetaddr(tmp, mac_addr);

                strcpy(redpinepath, path);
                strcat(redpinepath, "/redpine@0");

        add_space:
                ret = fdt_find_and_setprop(fdt, redpinepath, "local-mac-address", &mac_addr, 6, 1);

                if (ret == -FDT_ERR_NOSPACE)
                    {
                        printf("%s:%d fdt_increase_size by %d\n", __func__, __LINE__, AMOUNT_INCREASE_FDT_SIZE);
                        ret = fdt_increase_size(fdt, AMOUNT_INCREASE_FDT_SIZE);
                        if (!ret)
                            {
                                goto add_space;
                            }
                    }
        }
}

/**
 * ftd_fixup_ppm_eeprom() - perform board setup in flattened device tree
 * @blob: device tree blob in RAM
 *
 * Perform flattend device tree modifications. In our case we
 * want to add /eepromInformation that contains 'som' and 'pi'
 * nodes (if valid eeprom information exists) which hold:
 *   boardId = <board id integer>;
 *   hwRevision = <hw revision>;
 *   hwTweakBits = <hw tweak bits>;
 *   swCompatibility = <sw compatibility index>;
 *   partNumber = "<part number string>";
 *   serialNumber = "<serial number string>";
 *   ethAddr = [ xx xx xx xx xx xx ]; 
 *   wifiAddr = [ xx xx xx xx xx xx ]; 
 *
 * Note fields are supplied if the data in the eeprom is valid.  If
 * no field in the SOM eeprom is valid then /eepromInformation/som will
 * not be added. Same for PI eeprom and /eepromInformation/pi.
 */

int ftd_fixup_ppm_eeprom(PPM_UNIT_TYPE ppmUnitType, void *blob)
{
    struct fdt_header   *working_fdt = blob;    /* FDT            */
    unsigned int        i;                      /* index variable */
    int                 flag;                   /* error flag     */
	int                 nrOfBoards;
    ppm_eeprom_macaddrs_t macAddress;
    uint8_t *ethMacAddrPtr;
    uint8_t *wifiMacAddrPtr;

    /* array with board information */
    struct {
        ppm_eeprom_t    *eeprom;        /* EEPROM       */
        const char      *locationKey;   /* location key */
        PPM_EEPROMINFO_LOCATION_ID  locationId;
        const char      *name;          /* board name   */
    } boards[] = {
        {
            &ppm_eeprom_som,
            PPM_EEPROMINFO_LOCATION_KEY_SOM,
            PPM_EEPROMINFO_LOCATION_SOM,
            NULL
        },
        {
            &ppm_eeprom_pi,
            PPM_EEPROMINFO_LOCATION_KEY_PI,
            PPM_EEPROMINFO_LOCATION_PI,
            NULL
        },
        {
            &ppm_eeprom_afe1,
            PPM_EEPROMINFO_LOCATION_KEY_AFE1,
            PPM_EEPROMINFO_LOCATION_AFE1,
            NULL
        },
        {
            &ppm_eeprom_afe2,
            PPM_EEPROMINFO_LOCATION_KEY_AFE2,
            PPM_EEPROMINFO_LOCATION_AFE2,
            NULL
        },
        {
            &ppm_eeprom_radio,
            PPM_EEPROMINFO_LOCATION_KEY_RADIO,
            PPM_EEPROMINFO_LOCATION_RADIO,
            NULL
        },
        {
            &ppm_eeprom_ui,
            PPM_EEPROMINFO_LOCATION_KEY_UI,
            PPM_EEPROMINFO_LOCATION_UI,
            NULL
        },
        {
            &ppm_eeprom_battery,
            PPM_EEPROMINFO_LOCATION_KEY_BATTERY,
            PPM_EEPROMINFO_LOCATION_BATTERY,
            NULL
        },
    };

    printf("%s: working_fdt %p\n", __func__, working_fdt);

	if (ppmUnitType == PPM_UNIT_MOUNT)
	{
		nrOfBoards = 2;
	}
	else
	{
		nrOfBoards = ARRAY_SIZE(boards);
	}

    /* make sure at least one EEPROM is loaded */
    flag = 0;
    for (i = 0; i < nrOfBoards && !flag; i++)
    {
        if (boards[i].eeprom->loaded)
            flag = 1;
    }
    if (!flag)
    {
        printf("%s: Neither SOM, PI, AFE1, AFE2, RADIO, UI, nor BATTERY"
               " EEPROM is loaded.\n", __func__);
        return -ENODEV;
    }

    /* make sure at least one EEPROM is valid */
    flag = 0;
    for (i = 0; i < nrOfBoards && !flag; i++)
    {
        if (ppm_eeprom_board_valid(boards[i].eeprom, &boards[i].name))
            flag = 1;
    }
    if (!flag)
    {
        printf("%s: Neither SOM, PI, AFE1, AFE2, RADIO, UI, nor BATTERY"
               " EEPROM is valid\n", __func__);
        return -EINVAL;
    }

    memset(&macAddress, 0, sizeof(macAddress));

    if (ppm_eeprom_read_mac(ENET_MAC_ADDR_INDEX, macAddress.ethAddr) != 0)
	{
	    printf("%s - Error reading IS8 eth MAC address\n", __func__);
	}

    if (ppmUnitType == PPM_UNIT_SSPPM)
    {
        if (ppm_eeprom_read_mac(WIFI_MAC_ADDR_INDEX, macAddress.wifiAddr) != 0)
        {
            printf("%s - Error reading IS8 wifi MAC address\n", __func__);
        }

        /* Add WLS address to the FDT */
        fdt_fixup_wls(working_fdt);
    }

    /* Resize the fdt to allow room for changes */
    fdt_shrink_to_minimum(working_fdt, 0);

    /* add /eepromInfo/[...] nodes */
    for (i = 0; i < nrOfBoards; i++)
    {
        if (boards[i].locationId == PPM_EEPROMINFO_LOCATION_PI)
            {
                ethMacAddrPtr = macAddress.ethAddr;
                wifiMacAddrPtr = NULL;
            }
        else if (boards[i].locationId == PPM_EEPROMINFO_LOCATION_RADIO)
            {
                ethMacAddrPtr = NULL;
                wifiMacAddrPtr = macAddress.wifiAddr;
            }
        else
            {
                ethMacAddrPtr = NULL;
                wifiMacAddrPtr = NULL;
            }
	    
        flag = ft_add_eeprom_info(working_fdt,
                                  boards[i].eeprom,
                                  boards[i].locationKey,
                                  ethMacAddrPtr,
                                  wifiMacAddrPtr);
        if (flag)
        {
            printf("%s:%d ft_add_eprom_info(%s) failed %d\n",
                   __func__, __LINE__, boards[i].locationKey, flag);
            return flag;
        }
    }

    return flag;
}
#endif /* CONFIG_OF_BOARD_SETUP */
