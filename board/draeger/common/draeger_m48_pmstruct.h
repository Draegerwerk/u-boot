/*
 * draeger_m48_pmstruct.h
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 * Definition of the persistent memory region that is made accessible to the OS
 */

#ifndef __MX6M48_DRAEGER_M48_PMSTRUCT_H
#define __MX6M48_DRAEGER_M48_PMSTRUCT_H

#define M48_TS_NOT_RUN 	0
#define M48_TS_PASS 	0x4f82b741
#define M48_TS_FAIL 	0xf4b37a12

#define M48_BM_UNKNOWN  0
#define M48_BM_MAJ_MSK  0xFFFF0000
#define M48_BM_MIN_MSK  0x0000FFFF
#define M48_BM_FAT      0x12350000
#define M48_BM_TFTP     0x34530000
#define M48_BM_BOOTP    0x54680000

#define PM_MEMORY_MAGIC        0x25091966

/* start stype */
#define M48_START_TYPE_POWERUP 1
#define M48_START_TYPE_COLD_REBOOT 2
#define M48_START_TYPE_WARM_REBOOT 3

#define M48_UBOOT_VERSION_SIZE 32
#define M48_DEVICE_TYPE_SIZE   14

typedef struct {
    ulong magicConstant;
    ulong	 remoteDelay;
    uint32_t verboseBoot;
    uint32_t checkSum;
    uint32_t data_size; /* size of checksum area (size of data structure without magicConstant and checksum) */
    uint32_t bootmode;
    ulong timestamp_post;
    ulong timestamp_kernelloaded;
    unsigned char uboot_version[M48_UBOOT_VERSION_SIZE];
    unsigned char device_type[M48_DEVICE_TYPE_SIZE];
    struct
    {
        uint32_t result;
        uint32_t magic;
    } post_memtest;
    struct
    {
        uint32_t result;
        uint32_t magic;
    } post_watchdog;
    struct
    {
        uint32_t result;
        uint16_t magic;
        uint16_t state;
    } post_board_reset;
	uint32_t ram_calibration_value; /* recycled location of deprecated post scriptTest */
	uint32_t dummy0;
    struct
    {
        uint32_t result;
        uint32_t magic;
    } post_pmTest;
    uint32_t startType;
} PmBootData;

extern PmBootData* m48PmData;

/* call this after modifying the PmBootData data structure */
extern void updateM48PmStructChecksum(void);

#endif
