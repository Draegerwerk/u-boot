# Copyright (C) 1991-2014 Altera Corporation. All rights reserved.
# Your use of Altera Corporation's design tools, logic functions 
# and other software and tools, and its AMPP partner logic 
# functions, and any output files from any of the foregoing 
# (including device programming or simulation files), and any 
# associated documentation or information are expressly subject 
# to the terms and conditions of the Altera Program License 
# Subscription Agreement, the Altera Quartus II License Agreement,
# the Altera MegaCore Function License Agreement, or other 
# applicable license agreement, including, without limitation, 
# that your use is for the sole purpose of programming logic 
# devices manufactured by Altera and sold by Altera or its 
# authorized distributors.  Please refer to the applicable 
# agreement for further details.

# Quartus II: Generate Tcl File for Project
# File: m48_default_system.tcl
# Generated on: Thu Aug 13 12:11:12 2015

# Load Quartus II Tcl Project package
package require ::quartus::project

set need_to_close_project 0
set make_assignments 1

# Check that the right project is open
if {[is_project_open]} {
	if {[string compare $quartus(project) "m48_default_system"]} {
		puts "Project m48_default_system is not open"
		set make_assignments 0
	}
} else {
	# Only open if not already open
	if {[project_exists m48_default_system]} {
		project_open -revision m48_default_system m48_default_system
	} else {
		project_new -revision m48_default_system m48_default_system
	}
	set need_to_close_project 1
}

# Make assignments
if {$make_assignments} {
	set_global_assignment -name FAMILY "Cyclone V"
	set_global_assignment -name DEVICE 5CSEBA2U23C7S
	set_global_assignment -name TOP_LEVEL_ENTITY m48_top
	set_global_assignment -name ORIGINAL_QUARTUS_VERSION 13.1
	set_global_assignment -name PROJECT_CREATION_TIME_DATE "08:49:17  SEPTEMBER 11, 2013"
	set_global_assignment -name LAST_QUARTUS_VERSION 14.1.0
	set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files
	set_global_assignment -name EDA_SIMULATION_TOOL "ModelSim (Verilog)"
	set_global_assignment -name DEVICE_FILTER_PIN_COUNT 672
	set_global_assignment -name DEVICE_FILTER_SPEED_GRADE 7
	set_global_assignment -name MAX_CORE_JUNCTION_TEMP 85
	set_global_assignment -name MIN_CORE_JUNCTION_TEMP 0
	set_global_assignment -name USE_DLL_FREQUENCY_FOR_DQS_DELAY_CHAIN ON
	set_global_assignment -name UNIPHY_SEQUENCER_DQS_CONFIG_ENABLE ON
	set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING ON
	set_global_assignment -name ECO_REGENERATE_REPORT ON
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 4A
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 5A
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 3A
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 3B
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 7A
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 7B
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 7C
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 7D
	set_global_assignment -name IOBANK_VCCIO 3.3V -section_id 8A
	set_global_assignment -name STRATIX_DEVICE_IO_STANDARD "3.3-V LVTTL"
	set_global_assignment -name POWER_PRESET_COOLING_SOLUTION "28 MM HEAT SINK WITH STILL AIR"
	set_global_assignment -name POWER_BOARD_THERMAL_MODEL "NONE (CONSERVATIVE)"
	set_global_assignment -name ENABLE_SIGNALTAP OFF
	set_global_assignment -name PARTITION_NETLIST_TYPE SOURCE -section_id Top
	set_global_assignment -name PARTITION_FITTER_PRESERVATION_LEVEL PLACEMENT_AND_ROUTING -section_id Top
	set_global_assignment -name PARTITION_COLOR 16764057 -section_id Top
	set_global_assignment -name VERILOG_FILE $quartus(args)/src/m48_top.v
	set_global_assignment -name QIP_FILE m48_soc/synthesis/m48_soc.qip
	set_global_assignment -name QIP_FILE ../../../../../m48_fpga/ip_library/platform_ip/reset/hps_reset.qip
	set_global_assignment -name VERILOG_FILE ../../../../../m48_fpga/ip_library/platform_ip/reset/altera_edge_detector.v
	set_global_assignment -name VERILOG_FILE ../../../../../m48_fpga/ip_library/platform_ip/reset/debounce.v
	set_global_assignment -name VERILOG_FILE ../../../../../m48_fpga/ip_library/platform_ip/reset/pipe.v
	set_global_assignment -name QIP_FILE $quartus(args)/src/my_ClkDivider/my_ClkDivider.qip
	set_global_assignment -name SDC_FILE $quartus(args)/src/m48_default_system_timing.sdc
	set_global_assignment -name STRATIXV_CONFIGURATION_SCHEME "PASSIVE PARALLEL X16"
	set_global_assignment -name USE_CONFIGURATION_DEVICE OFF
	set_global_assignment -name CRC_ERROR_OPEN_DRAIN OFF
	set_global_assignment -name ENABLE_CRC_ERROR_PIN ON
	set_global_assignment -name ERROR_CHECK_FREQUENCY_DIVISOR 2
	set_global_assignment -name GENERATE_RBF_FILE ON
	set_global_assignment -name ON_CHIP_BITSTREAM_DECOMPRESSION OFF
	set_global_assignment -name RELEASE_CLEARS_BEFORE_TRI_STATES ON
	set_global_assignment -name ENABLE_INIT_DONE_OUTPUT ON
	set_global_assignment -name EDA_TIME_SCALE "1 ps" -section_id eda_simulation
	set_global_assignment -name EDA_OUTPUT_DATA_FORMAT "VERILOG HDL" -section_id eda_simulation
	set_location_assignment PIN_J15 -to hps_loanio14
	set_location_assignment PIN_A16 -to hps_loanio15
	set_location_assignment PIN_J14 -to hps_loanio16
	set_location_assignment PIN_A15 -to hps_loanio17
	set_location_assignment PIN_D17 -to hps_loanio18
	set_location_assignment PIN_A14 -to hps_loanio19
	set_location_assignment PIN_E16 -to hps_loanio20
	set_location_assignment PIN_A13 -to hps_loanio21
	set_location_assignment PIN_J13 -to hps_loanio22
	set_location_assignment PIN_A12 -to hps_loanio23
	set_location_assignment PIN_J12 -to hps_loanio24
	set_location_assignment PIN_A11 -to hps_loanio25
	set_location_assignment PIN_C15 -to hps_loanio26
	set_location_assignment PIN_A9 -to hps_loanio27
	set_location_assignment PIN_B14 -to hps_loanio35
	set_location_assignment PIN_A18 -to hps_loanio57
	set_location_assignment PIN_C17 -to hps_loanio58
	set_location_assignment PIN_B18 -to hps_loanio59
	set_location_assignment PIN_AF27 -to fpga_eim_ad[0]
	set_location_assignment PIN_AF28 -to fpga_eim_ad[1]
	set_location_assignment PIN_AG26 -to fpga_eim_ad[10]
	set_location_assignment PIN_AE23 -to fpga_eim_ad[11]
	set_location_assignment PIN_AG24 -to fpga_eim_ad[12]
	set_location_assignment PIN_AH24 -to fpga_eim_ad[13]
	set_location_assignment PIN_AG23 -to fpga_eim_ad[14]
	set_location_assignment PIN_AH23 -to fpga_eim_ad[15]
	set_location_assignment PIN_AF25 -to fpga_eim_ad[2]
	set_location_assignment PIN_AG28 -to fpga_eim_ad[3]
	set_location_assignment PIN_AG25 -to fpga_eim_ad[4]
	set_location_assignment PIN_AH27 -to fpga_eim_ad[5]
	set_location_assignment PIN_AC22 -to fpga_eim_ad[6]
	set_location_assignment PIN_AH26 -to fpga_eim_ad[7]
	set_location_assignment PIN_AC23 -to fpga_eim_ad[8]
	set_location_assignment PIN_AE24 -to fpga_eim_ad[9]
	set_location_assignment PIN_AF23 -to fpga_eim_bclk
	set_location_assignment PIN_AH22 -to fpga_eim_cs[0]
	set_location_assignment PIN_AD23 -to fpga_eim_cs[1]
	set_location_assignment PIN_AH21 -to fpga_eim_eb[0]
	set_location_assignment PIN_AE22 -to fpga_eim_eb[1]
	set_location_assignment PIN_AF22 -to fpga_eim_lba
	set_location_assignment PIN_AG21 -to fpga_eim_oe
	set_location_assignment PIN_AF21 -to fpga_eim_rw
	set_location_assignment PIN_AF20 -to fpga_eim_wait
	set_location_assignment PIN_AG20 -to fpga_fpctrl0
	set_location_assignment PIN_AE20 -to fpga_fpctrl1
	set_location_assignment PIN_AG19 -to fpga_fpctrl2
	set_location_assignment PIN_AD20 -to fpga_fpctrl3
	set_location_assignment PIN_AH19 -to fpga_fpctrl4
	set_location_assignment PIN_AG18 -to fpga_fpctrl5
	set_location_assignment PIN_AA19 -to fpga_fpctrl6
	set_location_assignment PIN_AH18 -to fpga_fpctrl7
	set_location_assignment PIN_AA18 -to fpga_sdio1_clk
	set_location_assignment PIN_AE19 -to fpga_sdio1_cmd
	set_location_assignment PIN_AF18 -to fpga_sdio1_d[0]
	set_location_assignment PIN_AD19 -to fpga_sdio1_d[1]
	set_location_assignment PIN_AH17 -to fpga_sdio1_d[2]
	set_location_assignment PIN_Y15 -to fpga_sdio1_d[3]
	set_location_assignment PIN_AH16 -to fpga_sdio2_clk
	set_location_assignment PIN_AA15 -to fpga_sdio2_cmd
	set_location_assignment PIN_AD17 -to fpga_sdio2_d[0]
	set_location_assignment PIN_AG15 -to fpga_sdio2_d[1]
	set_location_assignment PIN_AE17 -to fpga_sdio2_d[2]
	set_location_assignment PIN_AH14 -to fpga_sdio2_d[3]
	set_location_assignment PIN_Y13 -to fpga_mclk_25mhz
	set_location_assignment PIN_C28 -to hps_memory_mem_a[0]
	set_location_assignment PIN_B28 -to hps_memory_mem_a[1]
	set_location_assignment PIN_E26 -to hps_memory_mem_a[2]
	set_location_assignment PIN_D26 -to hps_memory_mem_a[3]
	set_location_assignment PIN_J21 -to hps_memory_mem_a[4]
	set_location_assignment PIN_J20 -to hps_memory_mem_a[5]
	set_location_assignment PIN_C26 -to hps_memory_mem_a[6]
	set_location_assignment PIN_B26 -to hps_memory_mem_a[7]
	set_location_assignment PIN_F26 -to hps_memory_mem_a[8]
	set_location_assignment PIN_F25 -to hps_memory_mem_a[9]
	set_location_assignment PIN_A24 -to hps_memory_mem_a[10]
	set_location_assignment PIN_B24 -to hps_memory_mem_a[11]
	set_location_assignment PIN_D24 -to hps_memory_mem_a[12]
	set_location_assignment PIN_C24 -to hps_memory_mem_a[13]
	set_location_assignment PIN_G23 -to hps_memory_mem_a[14]
	set_location_assignment PIN_A27 -to hps_memory_mem_ba[0]
	set_location_assignment PIN_H25 -to hps_memory_mem_ba[1]
	set_location_assignment PIN_G25 -to hps_memory_mem_ba[2]
	set_location_assignment PIN_N21 -to hps_memory_mem_ck
	set_location_assignment PIN_N20 -to hps_memory_mem_ck_n
	set_location_assignment PIN_L28 -to hps_memory_mem_cke
	set_location_assignment PIN_L21 -to hps_memory_mem_cs_n
	set_location_assignment PIN_A25 -to hps_memory_mem_ras_n
	set_location_assignment PIN_A26 -to hps_memory_mem_cas_n
	set_location_assignment PIN_E25 -to hps_memory_mem_we_n
	set_location_assignment PIN_V28 -to hps_memory_mem_reset_n
	set_location_assignment PIN_D28 -to hps_memory_mem_odt
	set_location_assignment PIN_G28 -to hps_memory_mem_dm[0]
	set_location_assignment PIN_P28 -to hps_memory_mem_dm[1]
	set_location_assignment PIN_W28 -to hps_memory_mem_dm[2]
	set_location_assignment PIN_AB28 -to hps_memory_mem_dm[3]
	set_location_assignment PIN_C6 -to hps_emac0_tx_ctl
	set_location_assignment PIN_E4 -to hps_emac0_tx_clk
	set_location_assignment PIN_C10 -to hps_emac0_txd[0]
	set_location_assignment PIN_F5 -to hps_emac0_txd[1]
	set_location_assignment PIN_C9 -to hps_emac0_txd[2]
	set_location_assignment PIN_C4 -to hps_emac0_txd[3]
	set_location_assignment PIN_C7 -to hps_emac0_mdc
	set_location_assignment PIN_A6 -to hps_qspi_ss0
	set_location_assignment PIN_C14 -to hps_qspi_clk
	set_location_assignment PIN_B8 -to hps_sdio_clk
	set_location_assignment PIN_B19 -to hps_uart0_rx
	set_location_assignment PIN_C16 -to hps_uart0_tx
	set_location_assignment PIN_C21 -to hps_trace_clk
	set_location_assignment PIN_A22 -to hps_trace_d[0]
	set_location_assignment PIN_B21 -to hps_trace_d[1]
	set_location_assignment PIN_A21 -to hps_trace_d[2]
	set_location_assignment PIN_K18 -to hps_trace_d[3]
	set_location_assignment PIN_A20 -to hps_trace_d[4]
	set_location_assignment PIN_J18 -to hps_trace_d[5]
	set_location_assignment PIN_A19 -to hps_trace_d[6]
	set_location_assignment PIN_C18 -to hps_trace_d[7]
	set_location_assignment PIN_A17 -to hps_reset_out
	set_location_assignment PIN_H17 -to hps_safety_trig
	set_location_assignment PIN_J25 -to hps_memory_mem_dq[0]
	set_location_assignment PIN_J24 -to hps_memory_mem_dq[1]
	set_location_assignment PIN_E28 -to hps_memory_mem_dq[2]
	set_location_assignment PIN_D27 -to hps_memory_mem_dq[3]
	set_location_assignment PIN_J26 -to hps_memory_mem_dq[4]
	set_location_assignment PIN_K26 -to hps_memory_mem_dq[5]
	set_location_assignment PIN_G27 -to hps_memory_mem_dq[6]
	set_location_assignment PIN_F28 -to hps_memory_mem_dq[7]
	set_location_assignment PIN_K25 -to hps_memory_mem_dq[8]
	set_location_assignment PIN_L25 -to hps_memory_mem_dq[9]
	set_location_assignment PIN_J27 -to hps_memory_mem_dq[10]
	set_location_assignment PIN_J28 -to hps_memory_mem_dq[11]
	set_location_assignment PIN_M27 -to hps_memory_mem_dq[12]
	set_location_assignment PIN_M26 -to hps_memory_mem_dq[13]
	set_location_assignment PIN_M28 -to hps_memory_mem_dq[14]
	set_location_assignment PIN_N28 -to hps_memory_mem_dq[15]
	set_location_assignment PIN_N24 -to hps_memory_mem_dq[16]
	set_location_assignment PIN_N25 -to hps_memory_mem_dq[17]
	set_location_assignment PIN_T28 -to hps_memory_mem_dq[18]
	set_location_assignment PIN_U28 -to hps_memory_mem_dq[19]
	set_location_assignment PIN_N26 -to hps_memory_mem_dq[20]
	set_location_assignment PIN_N27 -to hps_memory_mem_dq[21]
	set_location_assignment PIN_R27 -to hps_memory_mem_dq[22]
	set_location_assignment PIN_V27 -to hps_memory_mem_dq[23]
	set_location_assignment PIN_R26 -to hps_memory_mem_dq[24]
	set_location_assignment PIN_R25 -to hps_memory_mem_dq[25]
	set_location_assignment PIN_AA28 -to hps_memory_mem_dq[26]
	set_location_assignment PIN_W26 -to hps_memory_mem_dq[27]
	set_location_assignment PIN_R24 -to hps_memory_mem_dq[28]
	set_location_assignment PIN_T24 -to hps_memory_mem_dq[29]
	set_location_assignment PIN_Y27 -to hps_memory_mem_dq[30]
	set_location_assignment PIN_AA27 -to hps_memory_mem_dq[31]
	set_location_assignment PIN_R17 -to hps_memory_mem_dqs[0]
	set_location_assignment PIN_R19 -to hps_memory_mem_dqs[1]
	set_location_assignment PIN_T19 -to hps_memory_mem_dqs[2]
	set_location_assignment PIN_U19 -to hps_memory_mem_dqs[3]
	set_location_assignment PIN_R16 -to hps_memory_mem_dqs_n[0]
	set_location_assignment PIN_R18 -to hps_memory_mem_dqs_n[1]
	set_location_assignment PIN_T18 -to hps_memory_mem_dqs_n[2]
	set_location_assignment PIN_T20 -to hps_memory_mem_dqs_n[3]
	set_location_assignment PIN_D4 -to hps_emac0_mdio
	set_location_assignment PIN_A8 -to hps_qspi_io[0]
	set_location_assignment PIN_H16 -to hps_qspi_io[1]
	set_location_assignment PIN_A7 -to hps_qspi_io[2]
	set_location_assignment PIN_J16 -to hps_qspi_io[3]
	set_location_assignment PIN_D14 -to hps_sdio_cmd
	set_location_assignment PIN_C13 -to hps_sdio_d[0]
	set_location_assignment PIN_B6 -to hps_sdio_d[1]
	set_location_assignment PIN_B11 -to hps_sdio_d[2]
	set_location_assignment PIN_B9 -to hps_sdio_d[3]
	set_location_assignment PIN_C19 -to hps_i2c0_sda
	set_location_assignment PIN_B16 -to hps_i2c0_scl
	set_location_assignment PIN_A5 -to hps_gpio37
	set_location_assignment PIN_H13 -to hps_gpio40
	set_location_assignment PIN_A4 -to hps_gpio41
	set_location_assignment PIN_H12 -to hps_gpio42
	set_location_assignment PIN_B4 -to hps_gpio43
	set_location_assignment PIN_B12 -to hps_gpio44
	set_location_assignment PIN_C8 -to hps_emac0_rxd[0]
	set_location_assignment PIN_C5 -to hps_emac0_rxd[1]
	set_location_assignment PIN_E5 -to hps_emac0_rxd[2]
	set_location_assignment PIN_D5 -to hps_emac0_rxd[3]
	set_location_assignment PIN_G4 -to hps_emac0_rx_clk
	set_location_assignment PIN_F4 -to hps_emac0_rx_ctl
	set_location_assignment PIN_D25 -to hps_memory_oct_rzqin
	set_location_assignment PIN_AC7 -to altera_reserved_tms
	set_location_assignment PIN_AB5 -to altera_reserved_tck
	set_location_assignment PIN_W10 -to altera_reserved_tdi
	set_location_assignment PIN_Y9 -to altera_reserved_tdo
	set_location_assignment PIN_V11 -to fpga_clk_diffio_rx_b15p
	set_location_assignment PIN_V12 -to fpga_clk_diffio_rx_b23p
	set_location_assignment PIN_D12 -to fpga_clk_diffio_rx_t1p
	set_location_assignment PIN_E11 -to fpga_clk_diffio_rx_t9p
	set_location_assignment PIN_AE9 -to fpga_diffio_rx_b10n
	set_location_assignment PIN_AD10 -to fpga_diffio_rx_b10p
	set_location_assignment PIN_U11 -to fpga_diffio_rx_b11n
	set_location_assignment PIN_T11 -to fpga_diffio_rx_b11p
	set_location_assignment PIN_AE11 -to fpga_diffio_rx_b14n
	set_location_assignment PIN_AD11 -to fpga_diffio_rx_b14p
	set_location_assignment PIN_W11 -to fpga_diffio_rx_b15n
	set_location_assignment PIN_AF10 -to fpga_diffio_rx_b18n
	set_location_assignment PIN_AF11 -to fpga_diffio_rx_b18p
	set_location_assignment PIN_T12 -to fpga_diffio_rx_b19n
	set_location_assignment PIN_T13 -to fpga_diffio_rx_b19p
	set_location_assignment PIN_Y8 -to fpga_diffio_rx_b1n
	set_location_assignment PIN_W8 -to fpga_diffio_rx_b1p
	set_location_assignment PIN_AD12 -to fpga_diffio_rx_b22n
	set_location_assignment PIN_AE12 -to fpga_diffio_rx_b22p
	set_location_assignment PIN_W12 -to fpga_diffio_rx_b23n
	set_location_assignment PIN_AF13 -to fpga_diffio_rx_b26n
	set_location_assignment PIN_AG13 -to fpga_diffio_rx_b26p
	set_location_assignment PIN_U13 -to fpga_diffio_rx_b27n
	set_location_assignment PIN_U14 -to fpga_diffio_rx_b27p
	set_location_assignment PIN_AE15 -to fpga_diffio_rx_b30n
	set_location_assignment PIN_AF15 -to fpga_diffio_rx_b30p
	set_location_assignment PIN_AA13 -to fpga_diffio_rx_b31n
	set_location_assignment PIN_AG16 -to fpga_diffio_rx_b34n
	set_location_assignment PIN_AF17 -to fpga_diffio_rx_b34p
	set_location_assignment PIN_V13 -to fpga_diffio_rx_b35n
	set_location_assignment PIN_W14 -to fpga_diffio_rx_b35p
	set_location_assignment PIN_T8 -to fpga_diffio_rx_b3n
	set_location_assignment PIN_U9 -to fpga_diffio_rx_b3p
	set_location_assignment PIN_V10 -to fpga_diffio_rx_b5n
	set_location_assignment PIN_U10 -to fpga_diffio_rx_b5p
	set_location_assignment PIN_AA11 -to fpga_diffio_rx_b7n
	set_location_assignment PIN_Y11 -to fpga_diffio_rx_b7p
	set_location_assignment PIN_Y18 -to fpga_diffio_rx_r4n
	set_location_assignment PIN_Y17 -to fpga_diffio_rx_r4p
	set_location_assignment PIN_W15 -to fpga_diffio_rx_r6n
	set_location_assignment PIN_Y16 -to fpga_diffio_rx_r6p
	set_location_assignment PIN_V15 -to fpga_diffio_rx_r8n
	set_location_assignment PIN_V16 -to fpga_diffio_rx_r8p
	set_location_assignment PIN_C12 -to fpga_diffio_rx_t1n
	set_location_assignment PIN_L9 -to fpga_diffio_rx_t21n
	set_location_assignment PIN_L10 -to fpga_diffio_rx_t21p
	set_location_assignment PIN_K8 -to fpga_diffio_rx_t23n
	set_location_assignment PIN_L8 -to fpga_diffio_rx_t23p
	set_location_assignment PIN_D11 -to fpga_diffio_rx_t9n
	set_location_assignment PIN_AF8 -to fpga_diffio_tx_b12n
	set_location_assignment PIN_AE7 -to fpga_diffio_tx_b12p
	set_location_assignment PIN_AF9 -to fpga_diffio_tx_b13n
	set_location_assignment PIN_AE8 -to fpga_diffio_tx_b13p
	set_location_assignment PIN_AF6 -to fpga_diffio_tx_b16n
	set_location_assignment PIN_AF5 -to fpga_diffio_tx_b16p
	set_location_assignment PIN_AG6 -to fpga_diffio_tx_b17n
	set_location_assignment PIN_AF7 -to fpga_diffio_tx_b17p
	set_location_assignment PIN_AH2 -to fpga_diffio_tx_b20n
	set_location_assignment PIN_AH3 -to fpga_diffio_tx_b20p
	set_location_assignment PIN_AH4 -to fpga_diffio_tx_b21n
	set_location_assignment PIN_AG5 -to fpga_diffio_tx_b21p
	set_location_assignment PIN_AH5 -to fpga_diffio_tx_b24n
	set_location_assignment PIN_AH6 -to fpga_diffio_tx_b24p
	set_location_assignment PIN_AH7 -to fpga_diffio_tx_b25n
	set_location_assignment PIN_AG8 -to fpga_diffio_tx_b25p
	set_location_assignment PIN_AH8 -to fpga_diffio_tx_b28n
	set_location_assignment PIN_AG9 -to fpga_diffio_tx_b28p
	set_location_assignment PIN_AH9 -to fpga_diffio_tx_b29n
	set_location_assignment PIN_AG10 -to fpga_diffio_tx_b29p
	set_location_assignment PIN_Y4 -to fpga_diffio_tx_b2n
	set_location_assignment PIN_Y5 -to fpga_diffio_tx_b2p
	set_location_assignment PIN_AH11 -to fpga_diffio_tx_b32n
	set_location_assignment PIN_AG11 -to fpga_diffio_tx_b32p
	set_location_assignment PIN_AH12 -to fpga_diffio_tx_b33p
	set_location_assignment PIN_AH13 -to fpga_diffio_tx_b36n
	set_location_assignment PIN_AG14 -to fpga_diffio_tx_b36p
	set_location_assignment PIN_AB4 -to fpga_diffio_tx_b4n
	set_location_assignment PIN_AA4 -to fpga_diffio_tx_b4p
	set_location_assignment PIN_AD4 -to fpga_diffio_tx_b6n
	set_location_assignment PIN_AC4 -to fpga_diffio_tx_b6p
	set_location_assignment PIN_AE6 -to fpga_diffio_tx_b8n
	set_location_assignment PIN_AD5 -to fpga_diffio_tx_b8p
	set_location_assignment PIN_AF4 -to fpga_diffio_tx_b9n
	set_location_assignment PIN_AE4 -to fpga_diffio_tx_b9p
	set_location_assignment PIN_AE26 -to fpga_diffio_tx_r1n
	set_location_assignment PIN_AF26 -to fpga_diffio_tx_r1p
	set_location_assignment PIN_AD26 -to fpga_diffio_tx_r3n
	set_location_assignment PIN_AE25 -to fpga_diffio_tx_r3p
	set_location_assignment PIN_AB23 -to fpga_diffio_tx_r5n
	set_location_assignment PIN_AA23 -to fpga_diffio_tx_r7n
	set_location_assignment PIN_AA24 -to fpga_diffio_tx_r7p
	set_location_assignment PIN_H5 -to fpga_diffio_tx_t22n
	set_location_assignment PIN_H6 -to fpga_diffio_tx_t22p
	set_location_assignment PIN_H4 -to fpga_diffio_tx_t24n
	set_location_assignment PIN_D8 -to fpga_diffio_tx_t4n
	set_location_assignment PIN_E8 -to fpga_diffio_tx_t4p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_wait
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_rw
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_oe
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_lba
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_eb[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_eb[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_cs[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_cs[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_bclk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[15]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[14]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[12]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[11]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[13]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[10]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[9]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[8]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[7]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[6]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[5]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[4]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_eim_ad[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio1_clk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio2_clk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio1_cmd
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio2_cmd
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_clk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_uart0_rx
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_uart0_tx
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_i2c0_scl
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_i2c0_sda
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_mdc
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_mdio
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_rx_clk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_rx_ctl
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_rxd[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_rxd[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_rxd[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_rxd[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_tx_clk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_tx_ctl
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_txd[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_txd[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_txd[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_emac0_txd[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_sdio_clk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_sdio_cmd
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_sdio_d[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_sdio_d[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_sdio_d[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_sdio_d[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_gpio37
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_gpio40
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_gpio41
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_gpio42
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_gpio43
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_gpio44
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio14
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio15
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio16
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio17
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio18
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio19
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio20
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio21
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio22
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio23
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio24
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio25
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio26
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio27
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio35
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio57
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio58
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_loanio59
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi1
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi2
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi3
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi4
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi5
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi6
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi7
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi8
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi9
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi10
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi11
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi12
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_gpi13
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_safety_trig
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_oct_rzqin -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[0] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[0] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[0] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[1] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[1] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[1] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[2] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[2] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[2] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[3] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[3] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[3] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[4] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[4] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[4] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[5] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[5] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[5] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[6] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[6] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[6] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[7] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[7] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[7] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[8] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[8] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[8] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[9] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[9] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[9] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[10] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[10] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[10] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[11] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[11] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[11] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[12] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[12] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[12] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[13] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[13] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[13] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[14] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[14] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[14] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[15] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[15] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[15] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[16] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[16] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[16] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[17] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[17] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[17] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[18] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[18] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[18] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[19] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[19] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[19] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[20] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[20] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[20] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[21] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[21] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[21] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[22] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[22] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[22] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[23] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[23] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[23] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[24] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[24] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[24] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[25] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[25] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[25] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[26] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[26] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[26] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[27] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[27] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[27] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[28] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[28] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[28] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[29] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[29] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[29] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[30] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[30] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[30] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dq[31] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[31] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dq[31] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs[0] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[0] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[0] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs[1] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[1] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[1] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs[2] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[2] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[2] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs[3] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[3] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs[3] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs_n[0] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[0] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[0] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs_n[1] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[1] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[1] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs_n[2] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[2] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[2] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_dqs_n[3] -tag __hps_sdram_p0
	set_instance_assignment -name INPUT_TERMINATION "PARALLEL 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[3] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dqs_n[3] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_ck -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITHOUT CALIBRATION" -to hps_memory_mem_ck -tag __hps_sdram_p0
	set_instance_assignment -name D5_DELAY 2 -to hps_memory_mem_ck -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "DIFFERENTIAL 1.5-V SSTL CLASS I" -to hps_memory_mem_ck_n -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITHOUT CALIBRATION" -to hps_memory_mem_ck_n -tag __hps_sdram_p0
	set_instance_assignment -name D5_DELAY 2 -to hps_memory_mem_ck_n -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[0] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[0] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[10] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[10] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[11] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[11] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[12] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[12] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[13] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[13] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[14] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[14] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[1] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[1] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[2] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[2] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[3] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[3] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[4] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[4] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[5] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[5] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[6] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[6] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[7] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[7] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[8] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[8] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_a[9] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_a[9] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_ba[0] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_ba[0] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_ba[1] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_ba[1] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_ba[2] -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_ba[2] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_cas_n -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_cas_n -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_cke -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_cke -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_cs_n -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_cs_n -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_odt -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_odt -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_ras_n -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_ras_n -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_we_n -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_we_n -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_reset_n -tag __hps_sdram_p0
	set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to hps_memory_mem_reset_n -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dm[0] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dm[0] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dm[1] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dm[1] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dm[2] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dm[2] -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "SSTL-15 CLASS I" -to hps_memory_mem_dm[3] -tag __hps_sdram_p0
	set_instance_assignment -name OUTPUT_TERMINATION "SERIES 50 OHM WITH CALIBRATION" -to hps_memory_mem_dm[3] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[0] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[1] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[2] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[3] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[4] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[5] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[6] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[7] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[8] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[9] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[10] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[11] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[12] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[13] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[14] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[15] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[16] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[17] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[18] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[19] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[20] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[21] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[22] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[23] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[24] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[25] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[26] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[27] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[28] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[29] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[30] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dq[31] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dm[0] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dm[1] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dm[2] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dm[3] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs[0] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs[1] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs[2] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs[3] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs_n[0] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs_n[1] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs_n[2] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_dqs_n[3] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[0] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[10] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[11] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[12] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[13] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[14] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[1] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[2] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[3] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[4] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[5] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[6] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[7] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[8] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_a[9] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_ba[0] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_ba[1] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_ba[2] -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_cas_n -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_cke -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_cs_n -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_odt -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_ras_n -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_we_n -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_reset_n -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_ck -tag __hps_sdram_p0
	set_instance_assignment -name PACKAGE_SKEW_COMPENSATION OFF -to hps_memory_mem_ck_n -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|ureset|phy_reset_mem_stable_n -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|ureset|phy_reset_n -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uio_pads|dq_ddio[0].read_capture_clk_buffer -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_write_side[0] -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_wraddress[0] -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uio_pads|dq_ddio[1].read_capture_clk_buffer -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_write_side[1] -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_wraddress[1] -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uio_pads|dq_ddio[2].read_capture_clk_buffer -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_write_side[2] -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_wraddress[2] -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uio_pads|dq_ddio[3].read_capture_clk_buffer -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_write_side[3] -tag __hps_sdram_p0
	set_instance_assignment -name GLOBAL_SIGNAL OFF -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|p0|umemphy|uread_datapath|reset_n_fifo_wraddress[3] -tag __hps_sdram_p0
	set_instance_assignment -name ENABLE_BENEFICIAL_SKEW_OPTIMIZATION_FOR_NON_GLOBAL_CLOCKS ON -to soc_inst|hps_0|hps_io|border|hps_sdram_inst -tag __hps_sdram_p0
	set_instance_assignment -name PLL_COMPENSATION_MODE DIRECT -to soc_inst|hps_0|hps_io|border|hps_sdram_inst|pll0|fbout -tag __hps_sdram_p0
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_reset_out
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_mclk_25mhz
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_qspi_ss0
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_qspi_clk
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[4]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[5]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[6]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_trace_d[7]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio1_d[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio1_d[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio1_d[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio1_d[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio2_d[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio2_d[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio2_d[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_sdio2_d[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl0
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl1
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl2
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl3
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl4
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl5
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl6
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_fpctrl7
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_qspi_io[0]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_qspi_io[1]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_qspi_io[2]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_qspi_io[3]
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_clk_diffio_rx_b15p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_clk_diffio_rx_b23p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_clk_diffio_rx_t1p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_clk_diffio_rx_t9p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b10n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b10p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b11n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b11p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b14n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b14p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b15n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b18n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b18p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b19n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b19p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b1n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b1p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b22n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b22p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b23n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b26n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b26p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b27n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b27p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b30n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b30p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b31n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b34n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b34p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b35n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b35p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b3n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b3p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b5n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b5p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b7n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_b7p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_r4n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_r4p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_r6n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_r6p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_r8n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_r8p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_t1n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_t21n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_t21p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_t23n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_t23p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_rx_t9n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b12n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b12p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b13n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b13p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b16n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b16p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b17n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b17p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b20n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b20p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b21n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b21p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b24n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b24p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b25n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b25p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b28n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b28p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b29n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b29p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b2n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b2p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b32n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b32p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b33p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b36n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b36p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b4n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b4p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b6n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b6p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b8n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b8p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b9n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_b9p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_r1n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_r1p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_r3n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_r3p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_r5n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_r7n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_r7p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_t22n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_t22p
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_t24n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_t4n
	set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to fpga_diffio_tx_t4p
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_eim_lba
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_eim_cs[0]
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_eim_rw
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_eim_ad[*]
	set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to fpga_eim_ad[*]
	set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to fpga_eim_ad[*]
	set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to fpga_eim_wait
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_sdio1_cmd
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_sdio1_d[*]
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_sdio2_cmd
	set_instance_assignment -name FAST_INPUT_REGISTER ON -to fpga_sdio2_d[*]
	set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to fpga_sdio1_cmd
	set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to fpga_sdio1_d[*]
	set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to fpga_sdio2_cmd
	set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to fpga_sdio2_d[*]
	set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to fpga_sdio1_cmd
	set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to fpga_sdio1_d[*]
	set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to fpga_sdio2_cmd
	set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to fpga_sdio2_d[*]
	set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to fpga_sdio1_cmd
	set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to fpga_sdio1_d[*]
	set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to fpga_sdio2_cmd
	set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to fpga_sdio2_d[*]
	set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to fpga_eim_ad[*]
	set_instance_assignment -name CURRENT_STRENGTH_NEW 8MA -to hps_sdio_clk
	set_instance_assignment -name CURRENT_STRENGTH_NEW 8MA -to hps_sdio_cmd
	set_instance_assignment -name CURRENT_STRENGTH_NEW 8MA -to hps_sdio_d[*]
	set_instance_assignment -name CURRENT_STRENGTH_NEW 8MA -to hps_qspi_clk
	set_instance_assignment -name CURRENT_STRENGTH_NEW 8MA -to hps_qspi_ss0
	set_instance_assignment -name CURRENT_STRENGTH_NEW 8MA -to hps_qspi_io[*]
	set_instance_assignment -name SLEW_RATE 1 -to fpga_sdio1_cmd
	set_instance_assignment -name SLEW_RATE 1 -to fpga_sdio1_d[*]
	set_instance_assignment -name SLEW_RATE 1 -to fpga_sdio2_cmd
	set_instance_assignment -name SLEW_RATE 1 -to fpga_sdio2_d[*]
	set_instance_assignment -name SLEW_RATE 0 -to hps_sdio_clk
	set_instance_assignment -name SLEW_RATE 0 -to hps_sdio_cmd
	set_instance_assignment -name SLEW_RATE 0 -to hps_sdio_d[*]
	set_instance_assignment -name SLEW_RATE 1 -to fpga_eim_ad[*]
	set_instance_assignment -name SLEW_RATE 0 -to hps_qspi_clk
	set_instance_assignment -name SLEW_RATE 0 -to hps_qspi_ss0
	set_instance_assignment -name SLEW_RATE 0 -to hps_qspi_io[*]
	set_global_assignment   -name TIMEQUEST_MULTICORNER_ANALYSIS ON
	set_global_assignment   -name NUM_PARALLEL_PROCESSORS 10
	set_global_assignment   -name OPTIMIZATION_TECHNIQUE BALANCED
	set_global_assignment   -name OPTIMIZATION_MODE BALANCED
	set_instance_assignment -name PARTITION_HIERARCHY root_partition -to | -section_id Top
	# Commit assignments
	export_assignments

	#hdi
	package require ::quartus::flow
	execute_flow -compile
	#end hdi

	# Close project
	if {$need_to_close_project} {
		project_close
	}
}
