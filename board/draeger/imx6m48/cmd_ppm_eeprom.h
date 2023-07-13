/**
*
* @file
*
* @brief This provides prototypes for PI board eeprom access
*        routines for u-boot.
*
* @copyright Copyright 2019 Draeger and Licensors. All Rights Reserved.
*
*/

#ifndef __CMD_PI_EEPROM_H__
#define __CMD_PI_EEPROM_H__

/*
 * Note that the properties vxbFdtFslPpmEepromInfo are looking for in
 * the FDT (Flattened Device Tree) added by u-boot to describe content
 * of the HW Identification EEPROMs is:
 *
 * /imx6q/ppmEeprom
 *   {
 *      compatible = "draeger,usrspceeprominfo";
 *      <location> {
 *        boardId = <id>;
 *        hwVersion = <Mfg Hw Version number>;
 *        swCompatibility = <SW Compatibility Level>;
 *        partNumber = "string";
 *        serialNumber = "string";
 *        ethAddr = [ <mac address> ];
 *        wifiAddr = [ <mac address> ];
 *      };
 *      <location> = { ... };
 *      <location> = { ... };
 *  };
 *
 * Where:
 *   <location> states which board the EEPROM info is from, one of:
 *      som,
 *      pi,
 *      afe1,
 *      afe2
 *   <id> is:
 *      1 for 1GB Dual SOM,
 *      2 for 2GB Dual SOM,
 *      3 for 1GB Quad SOM,
 *      4 for 2GB Quat SOM,
 *      5 for SSPPM Product Integration (PI),
 *      6 for TSPPM_M PI,
 *      7 for TSPPM_B PI,
 *      8 for MOUNT PI
 *   <Mfg Hw Version Number> is the manufacturing HW version
 *   <SW Compatibility Level> is the Software compatibility of this board
 *   <mac address> is six hex bytes, not all zeros or 0xff, and not multicast
 */

/* Strings for location keys */
#define PPM_EEPROMINFO_LOCATION_KEY_SOM         "som"
#define PPM_EEPROMINFO_LOCATION_KEY_PI          "pi"
#define PPM_EEPROMINFO_LOCATION_KEY_AFE1        "afe1"
#define PPM_EEPROMINFO_LOCATION_KEY_AFE2        "afe2"
#define PPM_EEPROMINFO_LOCATION_KEY_RADIO       "radio"
#define PPM_EEPROMINFO_LOCATION_KEY_UI          "ui"
#define PPM_EEPROMINFO_LOCATION_KEY_BATTERY     "battery"

/* Strings for property keys */
#define PPM_EEPROMINFO_PROPERTY_BOARD_ID         "boardId"
#define PPM_EEPROMINFO_PROPERTY_HW_REVISION      "hwRevision"
#define PPM_EEPROMINFO_PROPERTY_HW_TWEAK_BITS    "hwTweakBits"
#define PPM_EEPROMINFO_PROPERTY_SW_COMPATIBILITY "swCompatibility"
#define PPM_EEPROMINFO_PROPERTY_PART_NUMBER      "partNumber"
#define PPM_EEPROMINFO_PROPERTY_SERIAL_NUMBER    "serialNumber"
#define PPM_EEPROMINFO_PROPERTY_ETH_ADDR         "ethAddr"
#define PPM_EEPROMINFO_PROPERTY_WIFI_ADDR        "wifiAddr"

/**
 * @brief GCC attribute to pack a structure
 */
#define PACKED_ATTRIBUTE __attribute__ ((__packed__))

/* Errors */
#define PPM_EEPROMINFO_BASE_ERROR 0xE0000000
#define PPM_EEPROMINFO_ERROR_EEPROM (PPM_EEPROMINFO_BASE_ERROR + 1)

/* EEPROM layout version identifier ('ELV6' = 'EEPROM Layout Version 6') */
/*
 * ELV6 - Removed the ethAddr and wifiAddr from the structure, IS8 uses two additional MAC only EEPROM's
 */
#define PPM_EEPROMINFO_LAYOUT_VERSION \
    (('E' << 24) | ('L' << 16) | ('V' << 8) | ('6' << 0))


/**
 * @brief maximum value of hwRevision or swCompatibility
 */
#define PPM_EEPROMINFO_MAX_REVISION 9999

/**
 * @brief enum used to identify the I2C location of devices
 *
 * This enumeration is used a location identifier.  This enumeration is used in
 * switch statements to select the proper I2C address in order to access a
 * particular device.  These identifiers do not <i>directly</i> imply an
 * address or a location.
 */
typedef enum
{
    PPM_EEPROMINFO_LOCATION_UNKNOWN = 0,    /* EEPROM location is unknown */
    PPM_EEPROMINFO_LOCATION_SOM     = 1,    /* SOM module */
    PPM_EEPROMINFO_LOCATION_PI      = 2,    /* PI board */
    PPM_EEPROMINFO_LOCATION_AFE1    = 3,    /* AFE1 */
    PPM_EEPROMINFO_LOCATION_AFE2    = 4,    /* AFE2 */
    PPM_EEPROMINFO_LOCATION_RADIO   = 5,    /* Radio/WiFi module */
    PPM_EEPROMINFO_LOCATION_UI      = 6,    /* UI module */
    PPM_EEPROMINFO_LOCATION_BATTERY = 7     /* battery interconnect module */
} PPM_EEPROMINFO_LOCATION_ID;

/**
 * @brief enum for board IDs
 * 
 * This enumeration is used as a board identifier.
 */
typedef enum
{
    PPM_EEPROMINFO_BOARD_UNKNOWN      = 0,
    PPM_EEPROMINFO_BOARD_SOM_1GB_DUAL = 1,  /* board is 1GB Dual i.mx6 SOM */
    PPM_EEPROMINFO_BOARD_SOM_2GB_DUAL = 2,  /* board is 1GB Dual i.mx6 SOM */
    PPM_EEPROMINFO_BOARD_SOM_1GB_QUAD = 3,  /* board is 1GB Dual i.mx6 SOM */
    PPM_EEPROMINFO_BOARD_SOM_2GB_QUAD = 4,  /* board is 1GB Dual i.mx6 SOM */
    PPM_EEPROMINFO_BOARD_SSPPM_PI     = 5,  /* board is the SSPPM PI */
    PPM_EEPROMINFO_BOARD_TSPPM_M_PI   = 6,  /* board is the TSPPM Medium PI */
    PPM_EEPROMINFO_BOARD_TSPPM_B_PI   = 7,  /* board is the TSPPM Basic PI */
    PPM_EEPROMINFO_BOARD_MOUNT_PI     = 8,  /* board is the MOUNT PI */
    PPM_EEPROMINFO_BOARD_AFE1         = 9,  /* board is the AFE1 */
    PPM_EEPROMINFO_BOARD_AFE2         = 10, /* board is the AFE2 */
    PPM_EEPROMINFO_BOARD_RADIO        = 11, /* board is the Radio */
    PPM_EEPROMINFO_BOARD_UI           = 12, /* board is the UI */
    PPM_EEPROMINFO_BOARD_BATTERY      = 13  /* board is the battery */
} PPM_EEPROMINFO_BOARD_ID;

/**
 * @brief enum for PPM unit types
 *
 * This enumeration is used as a unit type identifier.
 */
typedef enum
{
	PPM_UNIT_SSPPM,
	PPM_UNIT_MOUNT
} PPM_UNIT_TYPE;

#define PPM_EEPROMINFO_STRING_LENGTH   32   /* max length of eeprom strings */
#define PPM_EEPROMINFO_MAC_ADDR_LENGTH  6

typedef struct PACKED_ATTRIBUTE ppm_eeprom_layout {
    uint32_t version; /* layout version of eeprom content (0x4e4c5635  ELV5) */
} ppm_eeprom_layout_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_layout_crc {
    ppm_eeprom_layout_t data;   /* eeprom layout */
    uint32_t crc;                    /* CRC of layout content */
} ppm_eeprom_layout_crc_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_board {
    uint8_t  boardId;                /* PPM_EEPROMINFO_LAYOUT_VERSION_x identifier */
    uint16_t  hwRevision;            /* 0x0000='0000' .. 0x270f='9999' (Mfg build revision)*/
    uint32_t hwTweakBits;            /* hw tweak bits(sub hwRevision info) */
    uint16_t  swCompatibility;       /* 0x0000='0000' .. 0x270f='9999' (SW compatibility)*/
    uint8_t partNumber[PPM_EEPROMINFO_STRING_LENGTH];
    uint8_t serialNumber[PPM_EEPROMINFO_STRING_LENGTH];
} ppm_eeprom_board_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_board_crc {
    ppm_eeprom_board_t data;	/* board content */
    uint32_t crc;			/* board content CRC */
} ppm_eeprom_board_crc_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_macaddrs {
    uint8_t ethAddr[PPM_EEPROMINFO_MAC_ADDR_LENGTH];	/* MAC addr of copper/optical interface */
    uint8_t wifiAddr[PPM_EEPROMINFO_MAC_ADDR_LENGTH];	/* MAC addr of WiFi interface */
} ppm_eeprom_macaddrs_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_macaddrs_crc {
    ppm_eeprom_macaddrs_t data; /* Mac addresses */
    u32 crc;			/* CRC of macAddrs content */
} ppm_eeprom_macaddrs_crc_t;

/* EEPROM layout for both CPU and PI boards CMV1 version... */
typedef struct PACKED_ATTRIBUTE ppm_eeprom_data {
    ppm_eeprom_layout_crc_t layout;	/* eeprom layout */
    ppm_eeprom_board_crc_t board;	/* board content */
#ifndef CONFIG_DRAEGERPI_IS8
    ppm_eeprom_macaddrs_crc_t macaddrs;
#endif
} ppm_eeprom_data_cmv1_t;

typedef struct ppm_eeprom_bits {
    bool layout;    /* true if layout is valid/dirty (depending how used) */
    bool board;     /* true if board is valid/dirty (depending how used) */
#ifndef CONFIG_DRAEGERPI_IS8
    bool macaddrs;  /* true if macaddrs is valid/dirty (depending how used) */
#endif
} ppm_eeprom_bits_t;

typedef struct ppm_eeprom {
    ppm_eeprom_data_cmv1_t content; /* data read/written to EEPROM */
    bool loaded;                /* true if data loaded from eeprom */
    ppm_eeprom_bits_t valid;    /* flags for valid content */
    ppm_eeprom_bits_t dirty;    /* flags for dirty content(needs flushing) */
    const char *name;           /* name of location of EEPROM */
} ppm_eeprom_t;

/**
 * @brief function to read/initalize ppm_eeprom structure and environment
 *
 * @return bool - true if valid eeprom info found in SOM/PI EEPROMs
 */
extern void ppm_eeprom_init_env(PPM_UNIT_TYPE ppmUnitType);
extern int ftd_fixup_ppm_eeprom(PPM_UNIT_TYPE ppmUnitType, void *blob);

#endif /* __CMD_PI_EEPROM_H__ */

