/**
*
* @file
*
* @brief This provides prototypes for PPM PI board eeprom access
*        routines for u-boot.
*
* @copyright Copyright 2022 Draeger and Licensors. All Rights Reserved.
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
#define PPM_EEPROMINFO_LOCATION_KEY_RADIO       "wcm"
#define PPM_EEPROMINFO_LOCATION_KEY_UIM         "uim"
#define PPM_EEPROMINFO_LOCATION_KEY_BIM         "bim"
#define PPM_EEPROMINFO_LOCATION_KEY_OLM         "olm"

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

/* EEPROM layout version identifier ('ELV8' = 'EEPROM Layout Version 8') */

/*
 * ELV6 - Removed the ethAddr and wifiAddr from the structure, IS8 uses two additional MAC only EEPROM's
 * ELV7 - Removed the board_id byte, which was redundant with the PartNumber. ELV7 was never implemented.
 * ELV8 - Added three new SSPPM/UMM Product Fields (not board) PartNumber, SerialNumber and RevIndex
 *
 * Support booting with either ELV6 or ELV8, and anything greater than ELV8, as long as the checksums match
 */
#define PPM_EEPROMINFO_LAYOUT_LATEST_VERS  ('8')
#define PPM_EEPROMINFO_LAYOUT_ELV7_VERS    ('7')
#define PPM_EEPROMINFO_LAYOUT_ELV6_VERS    ('6')

#define PPM_EEPROMINFO_LAYOUT_VERSION \
    (('E' << 24) | ('L' << 16) | ('V' << 8) | (PPM_EEPROMINFO_LAYOUT_LATEST_VERS << 0))

#define PPM_EEPROMINFO_LAYOUT_VERSION_ELV7			\
    (('E' << 24) | ('L' << 16) | ('V' << 8) | (PPM_EEPROMINFO_LAYOUT_ELV7_VERS << 0))

#define PPM_EEPROMINFO_LAYOUT_VERSION_ELV6			\
    (('E' << 24) | ('L' << 16) | ('V' << 8) | (PPM_EEPROMINFO_LAYOUT_ELV6_VERS << 0))

#define PPM_EEPROMINFO_LAYOUT_PREFIX    (('E' << 24) | ('L' << 16) | ('V' << 8))

#define PPM_EEPROMINFO_LAYOUT_MASK	 (0xFFFFFF00)


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
    PPM_EEPROMINFO_LOCATION_RADIO   = 5,    /* Radio/WiFi WCM module */
    PPM_EEPROMINFO_LOCATION_UIM     = 6,    /* UI module */
    PPM_EEPROMINFO_LOCATION_BIM     = 7,    /* battery interconnect module */
    PPM_EEPROMINFO_LOCATION_OLM     = 8     /* optical link module */
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
    PPM_EEPROMINFO_BOARD_SOM_2GB_DUAL = 2,  /* board is 2GB Dual i.mx6 SOM */
    PPM_EEPROMINFO_BOARD_SOM_1GB_QUAD = 3,  /* board is 1GB Quad i.mx6 SOM */
    PPM_EEPROMINFO_BOARD_SOM_2GB_QUAD = 4,  /* board is 2GB Quad i.mx6 SOM */
    PPM_EEPROMINFO_BOARD_SSPPM_PI     = 5,  /* board is the SSPPM PI */
    PPM_EEPROMINFO_BOARD_TSPPM_M_PI   = 6,  /* board is the TSPPM Medium PI */
    PPM_EEPROMINFO_BOARD_TSPPM_B_PI   = 7,  /* board is the TSPPM Basic PI */
    PPM_EEPROMINFO_BOARD_MOUNT_PI     = 8,  /* board is the MOUNT PI */
    PPM_EEPROMINFO_BOARD_AFE1         = 9,  /* board is the AFE1 */
    PPM_EEPROMINFO_BOARD_AFE2         = 10, /* board is the AFE2 */
    PPM_EEPROMINFO_BOARD_RADIO        = 11, /* board is the Radio */
    PPM_EEPROMINFO_BOARD_UIM          = 12, /* board is the UI */
    PPM_EEPROMINFO_BOARD_BIM          = 13, /* board is the battery */
    PPM_EEPROMINFO_BOARD_OLM          = 14, /* board is the OLM */
    PPM_EEPROMINFO_BOARD_SOM_4GB_QUAD = 15  /* board is 4GB Quad i.mx6 SOM (Valenka3) */
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

#define PPM_EEPROMINFO_PN_LENGTH        (7)          /* MSxxxxx */
#define PPM_EEPROMINFO_SN_LENGTH        (9)          /* ABCD-WXYZ */
#define PPM_EEPROMINFO_MAC_ADDR_LENGTH  (6)
#define PPM_EEPROMINFO_HWREV_LENGTH     (3)

#define PPM_BOARD_TOTAL_RESERVED_SIZE    (256)                                               /* 256 bytes reserved for PCS info */
#define PPM_LAYOUT_DATA_CRC_SIZE         (sizeof(uint32_t) + sizeof(uint32_t))               /* Compute the size of the ppm_eeprom_layout_t and it's CRC */

/* Compute the sizeof the ppm_eeprom_board_crc_t */
#define PPM_BOARD_DATA_CRC_SIZE          (PPM_EEPROMINFO_HWREV_LENGTH + sizeof(uint32_t) + sizeof(uint16_t) + PPM_EEPROMINFO_PN_LENGTH + PPM_EEPROMINFO_SN_LENGTH + sizeof(uint32_t))

/* Compute the sizeof the unused bytes within the PCS reserved area (first 256 bytes) */
#define PPM_BOARD_RESERVED_SIZE          (PPM_BOARD_TOTAL_RESERVED_SIZE - (PPM_LAYOUT_DATA_CRC_SIZE + PPM_BOARD_DATA_CRC_SIZE))

typedef struct PACKED_ATTRIBUTE ppm_eeprom_layout {
    uint32_t version; /* layout version of eeprom content (0x4e4c5638  ELV8) */
} ppm_eeprom_layout_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_layout_crc {
    ppm_eeprom_layout_t data;   /* eeprom layout */
    uint32_t crc;               /* CRC of layout content */
} ppm_eeprom_layout_crc_t;

/* PPM per board/PCBA information */
typedef struct PACKED_ATTRIBUTE ppm_eeprom_board {
    uint8_t  hwRevision[PPM_EEPROMINFO_HWREV_LENGTH];          /* HW Revision Index string "-XX" where XX is "00"-"99" */
    uint32_t hwTweakBits;            /* hw tweak bits(sub hwRevision info) */
    uint16_t swCompatibility;        /* 0x0000='0000' .. 0x270f='9999' (SW compatibility)*/
    uint8_t  partNumber[PPM_EEPROMINFO_PN_LENGTH];
    uint8_t  serialNumber[PPM_EEPROMINFO_SN_LENGTH];
    uint8_t  reserved[PPM_BOARD_RESERVED_SIZE];      /* PCS reserved data, for future use */
} ppm_eeprom_board_t;

/*
 * PPM per system/product information, a label with this information, will be printed
 * and attached to back of the system/product (SSPPM, SMM, UMM, TSPPM-B, TSPPM-M, etc...)
 *
 * This is new to ELV8
 */
typedef struct PACKED_ATTRIBUTE ppm_eeprom_system {
    uint8_t partNumber[PPM_EEPROMINFO_PN_LENGTH];
    uint8_t hwRevision[PPM_EEPROMINFO_HWREV_LENGTH];          /* HW Revision Index string "-XX" where XX is "00"-"99" */
    uint8_t serialNumber[PPM_EEPROMINFO_SN_LENGTH];
} ppm_eeprom_system_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_board_crc {
    ppm_eeprom_board_t data;	/* board content */
    uint32_t crc;			    /* board content CRC */
} ppm_eeprom_board_crc_t;

typedef struct PACKED_ATTRIBUTE ppm_eeprom_system_crc {
    ppm_eeprom_system_t data;	/* system content */
    uint32_t crc;			    /* system content CRC */
} ppm_eeprom_system_crc_t;

/* EEPROM layout for both CPU and PI boards CMV1 version... */
typedef struct PACKED_ATTRIBUTE ppm_eeprom_data {
    ppm_eeprom_layout_crc_t layout;	/* eeprom layout */
    ppm_eeprom_board_crc_t  board;	/* board content */
    ppm_eeprom_system_crc_t system;	/* Product PI/system content (PI EEPROM only) starts at eeprom offset 0x100*/
} ppm_eeprom_data_cmv1_t;

typedef struct ppm_eeprom_bits {
    bool layout;    /* true if layout is valid/dirty (depending how used) */
    bool board;     /* true if board is valid/dirty (depending how used) */
    bool system;    /* true if system is valid/dirty (depending how used) */
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

#endif /* __CMD_PI_EEPROM_H__ */

