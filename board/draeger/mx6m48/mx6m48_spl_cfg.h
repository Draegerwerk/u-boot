/*
 * mx6m48_spl_cfg.h
 *
 * SPDX-License-Identifier:        GPL-2.0+
 *
 * (C) Copyright 2015 Draeger and Licensors, info@draeger.com
 *
 */

#ifndef SRC_U_BOOT_2014_07_BOARD_DRAEGER_MX6M48_MX6M48_SPL_CFG_H_
#define SRC_U_BOOT_2014_07_BOARD_DRAEGER_MX6M48_MX6M48_SPL_CFG_H_

typedef struct {
    u32 address;
    u32 value;
} m48_configuration;

extern m48_configuration ddr_setup_mx6dl[];
extern int ddr_setup_mx6dl_size;

extern m48_configuration ram_setup_mx6dl [];
extern int ram_setup_mx6dl_size;

extern m48_configuration ddr_setup_mx6q[];
extern int ddr_setup_mx6q_size;

extern m48_configuration ram_setup_mx6q [];
extern int ram_setup_mx6q_size;

extern m48_configuration ram_calibration_mx6dl_06 [];
extern int ram_calibration_mx6dl_size_06;

extern m48_configuration ram_calibration_mx6dl_08 [];
extern int ram_calibration_mx6dl_size_08;

extern m48_configuration ram_calibration_mx6q_06 [];
extern int ram_calibration_mx6q_size_06;

extern m48_configuration ram_calibration_mx6q_08 [];
extern int ram_calibration_mx6q_size_08;

#endif /* SRC_U_BOOT_2014_07_BOARD_DRAEGER_MX6M48_MX6M48_SPL_CFG_H_ */
