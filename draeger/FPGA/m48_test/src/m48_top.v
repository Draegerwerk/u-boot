// m48_top.v
// Top level of M48 generic and function test FPGA
//
// (C) 2017 Draegerwerk AG & Co. KGaA
//          Moislinger Allee 53-55
//          D-23542 Luebeck
//
// AUTHOR:
// Heiko Dickfeld
// Peter Bach
//

module m48_top (

	// Clock and Reset
	input        fpga_mclk_25mhz, //fpga_clk_50,
	
	// HPS memory controller ports
	output [14:0] hps_memory_mem_a,
	output  [2:0] hps_memory_mem_ba,
	output        hps_memory_mem_ck,
	output        hps_memory_mem_ck_n,
	output        hps_memory_mem_cke,
	output        hps_memory_mem_cs_n,
	output        hps_memory_mem_ras_n,
	output        hps_memory_mem_cas_n,
	output        hps_memory_mem_we_n,
	output        hps_memory_mem_reset_n,
	inout  [31:0] hps_memory_mem_dq,
	inout   [3:0] hps_memory_mem_dqs,
	inout   [3:0] hps_memory_mem_dqs_n,
	output        hps_memory_mem_odt,
	output  [3:0] hps_memory_mem_dm,
	input         hps_memory_oct_rzqin,
	
	// HPS peripherals used by M48 definition
	//
	
	// HPS - EMAC interface used by M48
	output        hps_emac0_tx_ctl,
	output        hps_emac0_tx_clk,
	output  [3:0] hps_emac0_txd,
	input         hps_emac0_rx_ctl,
	input         hps_emac0_rx_clk,
	input   [3:0] hps_emac0_rxd,
	inout         hps_emac0_mdio,
	output        hps_emac0_mdc,
	
	// HPS - QSPI interface used by M48
	output        hps_qspi_clk,
	output        hps_qspi_ss0,
	inout   [3:0] hps_qspi_io,
	
	// HPS - SD card interface used by M48 (possible boot device)
	output        hps_sdio_clk,
	inout         hps_sdio_cmd,
	inout   [3:0] hps_sdio_d,
	
	// HPS - UART interface used by M48 (possible boot device)
	input         hps_uart0_rx,
	output        hps_uart0_tx,
	
	// HPS - I2C interface used by M48
	inout         hps_i2c0_sda,
	inout         hps_i2c0_scl,
	
	// HPS - TRACE interface used by M48
	output        hps_trace_clk,
	output  [7:0] hps_trace_d,
	
	// HPS - GPIO interface used by M48
	inout         hps_reset_out,   // gpio61 (output)
	inout         hps_safety_trig, // gpio62 (output)
	
	// FPGA peripherals used by M48 definition
	//
	
	// FPGA - SD card interface 1 used by M48
	input         fpga_sdio1_clk,
	inout         fpga_sdio1_cmd,
	inout   [3:0] fpga_sdio1_d,
	
	// FPGA - SD card interface 2 used by M48
	input         fpga_sdio2_clk,
	inout         fpga_sdio2_cmd,
	inout   [3:0] fpga_sdio2_d,
	
	// FPGA - EIM interface used by M48
	input         fpga_eim_bclk,
	inout  [15:0] fpga_eim_ad,
	input   [1:0] fpga_eim_cs,
	input   [1:0] fpga_eim_eb,
	input         fpga_eim_lba,
	input         fpga_eim_oe, // not used
	input         fpga_eim_rw,
	output        fpga_eim_wait,
	
	// FPGA - i.MX6 to FPGA general purpose
	input         fpga_fpctrl0,
	input         fpga_fpctrl1,
	output        fpga_fpctrl2,
	input         fpga_fpctrl3,
	output        fpga_fpctrl4,
	output        fpga_fpctrl5,
	input         fpga_fpctrl6,
	output        fpga_fpctrl7,
	
	// Application peripherals
	//
	
	// HPS - GPI interface
	// Attention: hps_gpi input signals are sharing its pins with the DDR3 bank
	//            DDR3 IO specification is SSTL-15 Class I (1.5 Volt)
	//            be sure to include a level shifter to use these pins
	
	input         hps_gpi0,
	input         hps_gpi1,
	input         hps_gpi2,
	input         hps_gpi3,
	input         hps_gpi4,
	input         hps_gpi5,
	input         hps_gpi6,
	input         hps_gpi7,
	input         hps_gpi8,
	input         hps_gpi9,
	input         hps_gpi10,
	input         hps_gpi11,
	input         hps_gpi12,
	input         hps_gpi13,
	
	// HPS - GPIO interface
	inout         hps_gpio37,
	inout         hps_gpio40,
	inout         hps_gpio41,
	inout         hps_gpio42,
	inout         hps_gpio43,
	inout         hps_gpio44,
	
	// HPS - LOAN IO 
	inout         hps_loanio14,
	inout         hps_loanio15,
	inout         hps_loanio16,
	inout         hps_loanio17,
	inout         hps_loanio18,
	inout         hps_loanio19,
	inout         hps_loanio20,
	inout         hps_loanio21,
	inout         hps_loanio22,
	inout         hps_loanio23,
	inout         hps_loanio24,
	inout         hps_loanio25,
	inout         hps_loanio26,
	inout         hps_loanio27,
	inout         hps_loanio35,
	inout         hps_loanio57, // Input: HPS_JUMPER3
	inout         hps_loanio58, // Input: HPS_JUMPER2
	inout         hps_loanio59, // Input: HPS_JUMPER1
	
	inout         fpga_diffio_rx_b1n,
	inout         fpga_diffio_rx_b1p,
	inout         fpga_diffio_rx_b3n,
	inout         fpga_diffio_rx_b3p,
	inout         fpga_diffio_rx_b5n,
	inout         fpga_diffio_rx_b5p,
	inout         fpga_diffio_rx_b7n,
	inout         fpga_diffio_rx_b7p,
	inout         fpga_diffio_tx_b2n,
	inout         fpga_diffio_tx_b2p,
	inout         fpga_diffio_tx_b4n,
	inout         fpga_diffio_tx_b4p,
	inout         fpga_diffio_tx_b6n,
	inout         fpga_diffio_tx_b6p,
	inout         fpga_diffio_tx_b8n,
	inout         fpga_diffio_tx_b8p,
	
	inout         fpga_clk_diffio_rx_b23p,
	inout         fpga_clk_diffio_rx_b15p,
	inout         fpga_diffio_rx_b10n,
	inout         fpga_diffio_rx_b10p,
	
	inout         fpga_diffio_rx_b11n,
	inout         fpga_diffio_rx_b11p,
	inout         fpga_diffio_rx_b14n,
	inout         fpga_diffio_rx_b14p,
	
	inout         fpga_diffio_rx_b15n,
	inout         fpga_diffio_rx_b18n, 
	inout         fpga_diffio_rx_b18p,
	inout         fpga_diffio_rx_b19n,
	inout         fpga_diffio_rx_b19p,
	inout         fpga_diffio_rx_b22n,
	inout         fpga_diffio_rx_b22p,
	inout         fpga_diffio_rx_b23n,
	inout         fpga_diffio_tx_b12n,
	inout         fpga_diffio_tx_b12p,
	inout         fpga_diffio_tx_b13n,
	inout         fpga_diffio_tx_b13p,
	inout         fpga_diffio_tx_b16n,
	inout         fpga_diffio_tx_b16p,
	inout         fpga_diffio_tx_b17n,
	inout         fpga_diffio_tx_b17p,
	inout         fpga_diffio_tx_b20n,
	inout         fpga_diffio_tx_b20p,
	inout         fpga_diffio_tx_b21n,
	inout         fpga_diffio_tx_b21p,
	inout         fpga_diffio_tx_b24n,
	inout         fpga_diffio_tx_b24p,
	inout         fpga_diffio_tx_b9n,
	inout         fpga_diffio_tx_b9p,
	
	inout         fpga_diffio_rx_b26n,
	inout         fpga_diffio_rx_b26p,
	inout         fpga_diffio_rx_b27n,
	inout         fpga_diffio_rx_b27p,
	inout         fpga_diffio_rx_b30n,
	output        fpga_diffio_rx_b30p,		// out: Scard: SMARTCARD_SCK
	input         fpga_diffio_rx_b31n,		// in:  Scard: SMARTCARD_D
	output        fpga_diffio_rx_b34n,		// out: Scard: SMARTCARD_DOE
	output        fpga_diffio_rx_b34p,		// out: Scard: SMARTCARD_Q
	output        fpga_diffio_rx_b35n,		// out: Scard: SMARTCARD_RST
	inout         fpga_diffio_tx_b25n,
	inout         fpga_diffio_tx_b25p,
	inout         fpga_diffio_tx_b28n,
	inout         fpga_diffio_tx_b28p,
	
	inout         fpga_diffio_tx_b29n,
	inout         fpga_diffio_tx_b29p,
	inout         fpga_diffio_tx_b32n,
	inout         fpga_diffio_tx_b32p,
	
	inout         fpga_diffio_tx_b33p,
	inout         fpga_diffio_tx_b36n,
	inout         fpga_diffio_tx_b36p,
	inout         fpga_diffio_rx_r4n,
	inout         fpga_diffio_rx_r4p,
	inout         fpga_diffio_rx_r6n,
	inout         fpga_diffio_rx_r6p,
	inout         fpga_diffio_rx_r8n,
	inout         fpga_diffio_rx_r8p,
	inout         fpga_diffio_tx_r1n,
	inout         fpga_diffio_tx_r1p,
	inout         fpga_diffio_tx_r3n,
	inout         fpga_diffio_tx_r3p,
	inout         fpga_diffio_tx_r5n,
	inout         fpga_diffio_tx_r7n,
	inout         fpga_diffio_tx_r7p,
	inout         fpga_diffio_rx_t1n,
	inout         fpga_diffio_rx_t21n,
	inout         fpga_diffio_rx_t21p,
	inout         fpga_diffio_rx_t23n,
	inout         fpga_diffio_rx_t23p,
	inout         fpga_diffio_rx_t9n,
	inout         fpga_diffio_tx_t22n,
	inout         fpga_diffio_tx_t22p,
	inout         fpga_diffio_tx_t24n,
	inout         fpga_diffio_tx_t4n,
	inout         fpga_diffio_tx_t4p,
	inout         fpga_clk_diffio_rx_t9p,
	inout         fpga_clk_diffio_rx_t1p
);

	// Internal wires and registers declaration
	wire          hps_fpga_reset_n;
	
	wire          pll_reset_n;
	wire          clk_66MHz;
	wire          clk_1MHz;
	wire          clk_locked;
	
	wire          clk_locked_debounced;
	wire          sys_reset_n;
	
	wire    [2:0] hps_reset_req;
	wire          hps_cold_reset;
	wire          hps_warm_reset;
	wire          hps_debug_reset;
	
	wire    [1:0] sdio_sync_in;
	wire    [1:0] sdio_sync_out;
	
	wire          clkgen2_signals_clk1;
	wire          clkgen2_signals_clk2;
	wire          clkgen2_signals_trig;
	
	wire   [31:0] gpio1_gpo_out;
	wire   [31:0] gpio1_gpi_in;
	
	wire   [31:0] gpio2_gpo_out;
	wire   [31:0] gpio2_gpi_in;
	
	wire          irq_out;
	
	// Connection of internal logics
	assign sdio_sync_in[0]	= fpga_fpctrl1; // DIFFIO_RX_B46P
	assign sdio_sync_in[1]	= fpga_fpctrl3; // DIFFIO_RX_B46N
	
	assign fpga_fpctrl2		= sdio_sync_out[0];
	assign fpga_fpctrl4		= sdio_sync_out[1];
	
	// Test loopback and interrupt to i.MX6
	assign fpga_fpctrl5		= fpga_fpctrl0 | irq_out; // Combined loopback and interrupt
	assign fpga_fpctrl7		= fpga_fpctrl6;
	
	// SoC Sub-system module
	m48_soc m48_soc_inst
	(
		
		.clk_clk								(clk_66MHz),
		.reset_reset_n							(sys_reset_n),
		
		.hps_0_f2h_cold_reset_req_reset_n		(~hps_cold_reset),
		.hps_0_f2h_debug_reset_req_reset_n		(~hps_debug_reset),
		.hps_0_f2h_warm_reset_req_reset_n		(~hps_warm_reset),
		
		.hps_0_h2f_loan_io_in					(), // [66:0]
		.hps_0_h2f_loan_io_out					(), // [66:0]
		.hps_0_h2f_loan_io_oe					(), // [66:0]
		.hps_0_h2f_reset_reset_n				(hps_fpga_reset_n), // Output
		
		// M48 - Memory
		.memory_mem_a							(hps_memory_mem_a),
		.memory_mem_ba							(hps_memory_mem_ba),
		.memory_mem_ck							(hps_memory_mem_ck),
		.memory_mem_ck_n						(hps_memory_mem_ck_n),
		.memory_mem_cke							(hps_memory_mem_cke),
		.memory_mem_cs_n						(hps_memory_mem_cs_n),
		.memory_mem_ras_n						(hps_memory_mem_ras_n),
		.memory_mem_cas_n						(hps_memory_mem_cas_n),
		.memory_mem_we_n						(hps_memory_mem_we_n),
		.memory_mem_reset_n						(hps_memory_mem_reset_n),
		.memory_mem_dq							(hps_memory_mem_dq),
		.memory_mem_dqs							(hps_memory_mem_dqs),
		.memory_mem_dqs_n						(hps_memory_mem_dqs_n),
		.memory_mem_odt							(hps_memory_mem_odt),
		.memory_mem_dm							(hps_memory_mem_dm),
		.memory_oct_rzqin						(hps_memory_oct_rzqin),
		
		// M48 - RGMII0
		.hps_0_hps_io_hps_io_emac0_inst_TX_CTL	(hps_emac0_tx_ctl),
		.hps_0_hps_io_hps_io_emac0_inst_TX_CLK	(hps_emac0_tx_clk),
		.hps_0_hps_io_hps_io_emac0_inst_TXD0	(hps_emac0_txd[0]),
		.hps_0_hps_io_hps_io_emac0_inst_TXD1	(hps_emac0_txd[1]),
		.hps_0_hps_io_hps_io_emac0_inst_TXD2	(hps_emac0_txd[2]),
		.hps_0_hps_io_hps_io_emac0_inst_TXD3	(hps_emac0_txd[3]),
		.hps_0_hps_io_hps_io_emac0_inst_RX_CTL	(hps_emac0_rx_ctl),
		.hps_0_hps_io_hps_io_emac0_inst_RX_CLK	(hps_emac0_rx_clk),
		.hps_0_hps_io_hps_io_emac0_inst_RXD0	(hps_emac0_rxd[0]),
		.hps_0_hps_io_hps_io_emac0_inst_RXD1	(hps_emac0_rxd[1]),
		.hps_0_hps_io_hps_io_emac0_inst_RXD2	(hps_emac0_rxd[2]),
		.hps_0_hps_io_hps_io_emac0_inst_RXD3	(hps_emac0_rxd[3]),
		.hps_0_hps_io_hps_io_emac0_inst_MDIO	(hps_emac0_mdio),
		.hps_0_hps_io_hps_io_emac0_inst_MDC		(hps_emac0_mdc),
		
		// M48 - QSPI
		.hps_0_hps_io_hps_io_qspi_inst_CLK		(hps_qspi_clk),
		.hps_0_hps_io_hps_io_qspi_inst_SS0		(hps_qspi_ss0),
		.hps_0_hps_io_hps_io_qspi_inst_IO0		(hps_qspi_io[0]),
		.hps_0_hps_io_hps_io_qspi_inst_IO1		(hps_qspi_io[1]),
		.hps_0_hps_io_hps_io_qspi_inst_IO2		(hps_qspi_io[2]),
		.hps_0_hps_io_hps_io_qspi_inst_IO3		(hps_qspi_io[3]),
		
		// M48 - SDIO
		.hps_0_hps_io_hps_io_sdio_inst_CLK		(hps_sdio_clk),
		.hps_0_hps_io_hps_io_sdio_inst_CMD		(hps_sdio_cmd),
		.hps_0_hps_io_hps_io_sdio_inst_D0		(hps_sdio_d[0]),
		.hps_0_hps_io_hps_io_sdio_inst_D1		(hps_sdio_d[1]),
		.hps_0_hps_io_hps_io_sdio_inst_D2		(hps_sdio_d[2]),
		.hps_0_hps_io_hps_io_sdio_inst_D3		(hps_sdio_d[3]),
		
		// M48 - UART0
		.hps_0_hps_io_hps_io_uart0_inst_RX		(hps_uart0_rx),
		.hps_0_hps_io_hps_io_uart0_inst_TX		(hps_uart0_tx),
		
		// M48 - I2C0
		.hps_0_hps_io_hps_io_i2c0_inst_SDA		(hps_i2c0_sda),
		.hps_0_hps_io_hps_io_i2c0_inst_SCL		(hps_i2c0_scl),
		
		// M48 - Trace port
		.hps_0_hps_io_hps_io_trace_inst_CLK		(hps_trace_clk),
		.hps_0_hps_io_hps_io_trace_inst_D0		(hps_trace_d[0]),
		.hps_0_hps_io_hps_io_trace_inst_D1		(hps_trace_d[1]),
		.hps_0_hps_io_hps_io_trace_inst_D2		(hps_trace_d[2]),
		.hps_0_hps_io_hps_io_trace_inst_D3		(hps_trace_d[3]),
		.hps_0_hps_io_hps_io_trace_inst_D4		(hps_trace_d[4]),
		.hps_0_hps_io_hps_io_trace_inst_D5		(hps_trace_d[5]),
		.hps_0_hps_io_hps_io_trace_inst_D6		(hps_trace_d[6]),
		.hps_0_hps_io_hps_io_trace_inst_D7		(hps_trace_d[7]),
		
		// M48 - SDIO to i.MX6
		.sdio_1_sdio_interface_sdio_clk			(fpga_sdio1_clk),
		.sdio_1_sdio_interface_sdio_cmd			(fpga_sdio1_cmd),
		.sdio_1_sdio_interface_sdio_data		(fpga_sdio1_d),
		.sdio_2_sdio_interface_sdio_clk			(fpga_sdio2_clk),
		.sdio_2_sdio_interface_sdio_cmd			(fpga_sdio2_cmd),
		.sdio_2_sdio_interface_sdio_data		(fpga_sdio2_d),
		
		.sdio_sync_in_export					(sdio_sync_in),
		.sdio_sync_out_export					(sdio_sync_out),
		
		// M48 - i.MX6 eim interface
		.eim_eim_clk							(fpga_eim_bclk),
		.eim_eim_cs_n							(fpga_eim_cs[0]),
		.eim_eim_lba_n							(fpga_eim_lba),
		.eim_eim_ad								(fpga_eim_ad),
		.eim_eim_we_n							(fpga_eim_rw),
		.eim_eim_wait_n							(fpga_eim_wait),
		// Interrupt from FPGA to i.MX6
		.int1_signals_export					(irq_out),
		
		// M48 - GPIO fixed definition
		.hps_0_hps_io_hps_io_gpio_inst_GPIO61	(hps_reset_out),  // Output
		.hps_0_hps_io_hps_io_gpio_inst_GPIO62	(hps_safety_trig), // Output
		
		
		// M48 User Part
		//
		
		.hps_pio_io_0_external_export
		({ // (31..0)
			fpga_clk_diffio_rx_b15p,	// 31
			fpga_clk_diffio_rx_b23p,	// 30
			fpga_clk_diffio_rx_t1p,		// 29
			fpga_clk_diffio_rx_t9p,		// 28
			fpga_diffio_rx_b10n,		// 27
			fpga_diffio_rx_b10p,		// 26
			fpga_diffio_rx_b11n,		// 25
			fpga_diffio_rx_b11p,		// 24
			fpga_diffio_rx_b14n,		// 23
			fpga_diffio_rx_b14p,		// 22
			fpga_diffio_rx_b15n,		// 21
			fpga_diffio_rx_b18n,		// 20
			fpga_diffio_rx_b18p,		// 19
			fpga_diffio_rx_b19n,		// 18
			fpga_diffio_rx_b19p,		// 17
			fpga_diffio_rx_b1n,			// 16
			fpga_diffio_rx_b1p,			// 15
			fpga_diffio_rx_b22n,		// 14
			fpga_diffio_rx_b22p,		// 13
			fpga_diffio_rx_b23n,		// 12
			fpga_diffio_rx_b26n,		// 11
			fpga_diffio_rx_b26p,		// 10
			fpga_diffio_rx_b27n,		//  9
			fpga_diffio_rx_b27p,		//  8
			fpga_diffio_rx_b30n,		//  7
			5'bzzzzz,					//  6..2 not connected
			fpga_diffio_rx_b35p,		//  1
			fpga_diffio_rx_b3n,			//  0
		}),
		
		.hps_pio_io_1_external_export
		({ // (31..0)
			fpga_diffio_rx_b3p,			// 31
			fpga_diffio_rx_b5n,			// 30
			fpga_diffio_rx_b5p,			// 29
			fpga_diffio_rx_b7n,			// 28
			fpga_diffio_rx_b7p,			// 27
			fpga_diffio_rx_r4n,			// 26
			fpga_diffio_rx_r4p,			// 25
			fpga_diffio_rx_r6n,			// 24
			fpga_diffio_rx_r6p,			// 23
			fpga_diffio_rx_r8n,			// 22
			fpga_diffio_rx_r8p,			// 21
			fpga_diffio_rx_t1n,			// 20
			fpga_diffio_rx_t21n,		// 19
			fpga_diffio_rx_t21p,		// 18
			fpga_diffio_rx_t23n,		// 17
			fpga_diffio_rx_t23p,		// 16
			fpga_diffio_rx_t9n,			// 15
			fpga_diffio_tx_b12n,		// 14
			fpga_diffio_tx_b12p,		// 13
			fpga_diffio_tx_b13n,		// 12
			fpga_diffio_tx_b13p,		// 11
			fpga_diffio_tx_b16n,		// 10
			fpga_diffio_tx_b16p,		//  9
			fpga_diffio_tx_b17n,		//  8
			fpga_diffio_tx_b17p,		//  7
			fpga_diffio_tx_b20n,		//  6
			fpga_diffio_tx_b20p,		//  5
			fpga_diffio_tx_b21n,		//  4
			fpga_diffio_tx_b21p,		//  3
			fpga_diffio_tx_b24n,		//  2
			fpga_diffio_tx_b24p,		//  1
			fpga_diffio_tx_b25n,		//  0
		}),
		
		.hps_pio_io_2_external_export
			({ // (31..0)
			fpga_diffio_tx_b25p,		// 31
			fpga_diffio_tx_b28n,		// 30
			fpga_diffio_tx_b28p,		// 29
			fpga_diffio_tx_b29n,		// 28
			fpga_diffio_tx_b29p,		// 27
			fpga_diffio_tx_b2n,			// 26
			fpga_diffio_tx_b2p,			// 25
			fpga_diffio_tx_b32n,		// 24
			fpga_diffio_tx_b32p,		// 23
			fpga_diffio_tx_b33p,		// 22
			fpga_diffio_tx_b36n,		// 21
			fpga_diffio_tx_b36p,		// 20
			fpga_diffio_tx_b4n,			// 19
			fpga_diffio_tx_b4p,			// 18
			fpga_diffio_tx_b6n,			// 17
			fpga_diffio_tx_b6p,			// 16
			fpga_diffio_tx_b8n,			// 15
			fpga_diffio_tx_b8p,			// 14
			fpga_diffio_tx_b9n,			// 13
			fpga_diffio_tx_b9p,			// 12
			fpga_diffio_tx_r1n,			// 11
			fpga_diffio_tx_r1p,			// 10
			fpga_diffio_tx_r3n,			//  9
			fpga_diffio_tx_r3p,			//  8
			fpga_diffio_tx_r5n,			//  7
			fpga_diffio_tx_r7n,			//  6
			fpga_diffio_tx_r7p,			//  5
			fpga_diffio_tx_t22n,		//  4
			fpga_diffio_tx_t22p,		//  3
			fpga_diffio_tx_t24n,		//  2
			fpga_diffio_tx_t4n,			//  1
			fpga_diffio_tx_t4p			//  0
		}),
		
		// M48 - GPI for user proposes
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI0	(hps_gpi0),  // (hps_hlgpi0),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI1	(hps_gpi1),  // (hps_hlgpi1),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI2	(hps_gpi2),  // (hps_hlgpi2),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI3	(hps_gpi3),  // (hps_hlgpi3),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI4	(hps_gpi4),  // (hps_hlgpi4),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI5	(hps_gpi5),  // (hps_hlgpi5),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI6	(hps_gpi6),  // (hps_hlgpi6),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI7	(hps_gpi7),  // (hps_hlgpi7),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI8	(hps_gpi8),  // (hps_hlgpi8),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI9	(hps_gpi9),  // (hps_hlgpi9),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI10	(hps_gpi10), // (hps_hlgpi10),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI11	(hps_gpi11), // (hps_hlgpi11),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI12	(hps_gpi12), // (hps_hlgpi12),
		.hps_0_hps_io_hps_io_gpio_inst_HLGPI13	(hps_gpi13), // (hps_hlgpi13),
		
		// M48 - GPIO for user proposes
		.hps_0_hps_io_hps_io_gpio_inst_GPIO37	(hps_gpio37),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO40	(hps_gpio40),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO41	(hps_gpio41),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO42	(hps_gpio42),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO43	(hps_gpio43),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO44	(hps_gpio44),
		
		// M48 - Loan IOs for user proposes (connected to GPIOs)
		.hps_0_hps_io_hps_io_gpio_inst_GPIO14	(hps_loanio14),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO15	(hps_loanio15),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO16	(hps_loanio16),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO17	(hps_loanio17),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO18	(hps_loanio18),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO19	(hps_loanio19),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO20	(hps_loanio20),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO21	(hps_loanio21),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO22	(hps_loanio22),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO23	(hps_loanio23),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO24	(hps_loanio24),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO25	(hps_loanio25),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO26	(hps_loanio26),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO27	(hps_loanio27),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO35	(hps_loanio35),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO57	(hps_loanio57),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO58	(hps_loanio58),
		.hps_0_hps_io_hps_io_gpio_inst_GPIO59	(hps_loanio59),
		
		// M48 - Smart card
		.scard_signals_data_in					(fpga_diffio_rx_b31n),		// SMARTCARD_D
		.scard_signals_data_out					(fpga_diffio_rx_b34p),		// SMARTCARD_Q
		.scard_signals_data_en_out				(fpga_diffio_rx_b34n),		// SMARTCARD_DOE
		.scard_signals_rst_out					(fpga_diffio_rx_b35n),		// SMARTCARD_RST
		.scard_signals_clk_out					(fpga_diffio_rx_b30p)		// SMARTCARD_SCK
	);
	
	// Generate power_on_reset and synchronize hps_fpga_reset_n
	pipe sync_hps_reset
	(
		.clk		(fpga_mclk_25mhz),
		.reset_n	(hps_fpga_reset_n),
		.data_in	(1'b1),
		.data_out	(pll_reset_n)
	);
	defparam sync_hps_reset.STAGES = 10;
	
	// Generate avalon clock
	my_ClkDivider my_clkdivider_inst
	(
		.refclk		(fpga_mclk_25mhz),
		.rst		(~pll_reset_n),
		.outclk_0	(clk_66MHz),
		.outclk_1	(clk_1MHz),
		.locked		(clk_locked)
	);
	
	// Debounce m48_clk_divider.locked port
	`define  PLL_LOCK_TIMEOUT 100000  // at 25 Mhz this equals to 4 ms
	
	debounce debounce_pll_lock
	(
		.clk		(fpga_mclk_25mhz),
		.reset_n	(pll_reset_n),
		.data_in 	(clk_locked),
		.data_out	(clk_locked_debounced)
	);
	defparam debounce_pll_lock.WIDTH = 1;
	defparam debounce_pll_lock.POLARITY = "HIGH";
	defparam debounce_pll_lock.TIMEOUT = `PLL_LOCK_TIMEOUT;
	defparam debounce_pll_lock.TIMEOUT_WIDTH = $clog2(`PLL_LOCK_TIMEOUT);
	
	// Synchronize clk_locked_debounced and generate sys_reset_n
	pipe sync_pll_lock
	(
		.clk		(clk_66MHz),
		.reset_n	(hps_fpga_reset_n),
		.data_in	(clk_locked_debounced),
		.data_out	(sys_reset_n)
	);
	defparam sync_pll_lock.STAGES = 3;
	
	// Generate HPS resets
	hps_reset hps_reset_inst
	(
		.source_clk	(clk_66MHz),
		.source		(hps_reset_req)
	);
	
	altera_edge_detector pulse_cold_reset
	(
		.clk		(clk_66MHz),
		.rst_n		(sys_reset_n),
		.signal_in	(hps_reset_req[0]),
		.pulse_out	(hps_cold_reset)
	);
	defparam pulse_cold_reset.PULSE_EXT = 6;
	defparam pulse_cold_reset.EDGE_TYPE = 1;
	defparam pulse_cold_reset.IGNORE_RST_WHILE_BUSY = 1;
	
	altera_edge_detector pulse_warm_reset
	(
		.clk		(clk_66MHz),
		.rst_n		(sys_reset_n),
		.signal_in	(hps_reset_req[1]),
		.pulse_out	(hps_warm_reset)
	);
	defparam pulse_warm_reset.PULSE_EXT = 2;
	defparam pulse_warm_reset.EDGE_TYPE = 1;
	defparam pulse_warm_reset.IGNORE_RST_WHILE_BUSY = 1;
	
	altera_edge_detector pulse_debug_reset
	(
		.clk		(clk_66MHz),
		.rst_n		(sys_reset_n),
		.signal_in	(hps_reset_req[2]),
		.pulse_out	(hps_debug_reset)
	);
	defparam pulse_debug_reset.PULSE_EXT = 32;
	defparam pulse_debug_reset.EDGE_TYPE = 1;
	defparam pulse_debug_reset.IGNORE_RST_WHILE_BUSY = 1;
	
endmodule
