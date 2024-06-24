/*
 *  (C) Copyright 2020 Draeger and Licensors, info@draeger.com
 *
 *  SPDX-License-Identifier: GPL-2.0+
 */

#ifndef U_BOOT_BOARD_DRAEGER_COMMON_DRAEGER_COMMON_H_
#define U_BOOT_BOARD_DRAEGER_COMMON_DRAEGER_COMMON_H_

#include <command.h>

#define IMAGE_PATH "boot"



#define BRESET_MAGIC                    0xfdb9
#define POST_BRESET_STATE_UP1_RESET     0
#define POST_BRESET_STATE_UP2_RESET     1
#define POST_BRESET_STATE_BOTH_DONE     2
#define SYS_WATCHDOG_MAGIC      0x12480000
#define SYS_WATCHDOG_MAGIC_MASK 0xFFFF0000
#define POST_REMOTE_WAIT_INTERVAL 2000

#define CMD_BUF_SIZE 256
extern char cmdbuf[];

void updateM48PmStructChecksum(void);

ulong post_word_load(void);
void post_word_store(ulong);

int check_name(char*, int);
char* get_image_path(const char*);
char* get_image_name(const char*);
char* get_legacy_image_name(const char*);

void fdt_fixup_serial_output(void *);

char* getPmMemoryRegionInDDR3(void);
void initializePMSaveMemory(void);
void checkPMSaveMemory(void);
unsigned int silentBootIsRequired(void);
int board_test_run_always (int);
int start_kernel_image(ulong, ulong);
int startKernel (const char*, const char*, bool);
int start_itb_image(void);
int start_legacy_image(void);

int getBootMmcDevice(void);
void setEnvBootArgs(const char*);
char* get_conf_name(void);
const char* get_ftd_name(void);
int load_extra_images_from_partition(int partition);
int load_extra_images_from_tftp(void);
int display_error_message(void);

uint64_t getPhysRamSize(void);

#endif /* U_BOOT_BOARD_DRAEGER_COMMON_DRAEGER_COMMON_H_ */
