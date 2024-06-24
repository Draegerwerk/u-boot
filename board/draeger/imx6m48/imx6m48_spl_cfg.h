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

extern m48_configuration ram_setup_mx6qp [];
extern int ram_setup_mx6qp_size;

extern m48_configuration ram_calibration_mx6dl_pre [];
extern int ram_calibration_mx6dl_size_pre;

extern m48_configuration ram_calibration_mx6dl_prod [];
extern int ram_calibration_mx6dl_size_prod;

extern m48_configuration ram_calibration_mx6q_pre [];
extern int ram_calibration_mx6q_size_pre;

extern m48_configuration ram_calibration_mx6q_prod [];
extern int ram_calibration_mx6q_size_prod;

extern m48_configuration ddr_setup_valenka[];
extern int ddr_setup_valenka_size;

extern m48_configuration ram_setup_valenka [];
extern int ram_setup_valenka_size;

extern m48_configuration ram_calibration_valenka [];
extern int ram_calibration_valenka_size;

extern m48_configuration ddr_setup_valenka3[];
extern int ddr_setup_valenka3_size;

extern m48_configuration ram_calibration_valenka3_2gb [];
extern int ram_calibration_valenka3_2gb_size;

extern m48_configuration ram_calibration_valenka3_4gb [];
extern int ram_calibration_valenka3_4gb_size;

extern m48_configuration ram_setup_valenka3_2gb [];
extern int ram_setup_valenka3_2gb_size;

extern m48_configuration ram_setup_valenka3_4gb [];
extern int ram_setup_valenka3_4gb_size;

#endif /* SRC_U_BOOT_2014_07_BOARD_DRAEGER_MX6M48_MX6M48_SPL_CFG_H_ */
