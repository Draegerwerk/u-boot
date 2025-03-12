# m48_default_system_timing.sdc
# (C) 2017 by Drägerwerk AG & Co. KGaA
#
# Timing constrains file for M48 module FPGA

# Main FPGA clock
set clk_66MHz   {my_clkdivider_inst|my_clkdivider_inst|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk}

# FPGA: SDIO clocks and virtual SDIO clocks (HS: 20ns = 50 MHz, SDR50: 9.6ns = 104 MHz)
set SDIO_CLK_PERIOD_HS    20.0
set SDIO_CLK_PERIOD_SDR50  9.6

create_clock -name fpga_sdio1_clk      -period $SDIO_CLK_PERIOD_HS [ get_ports { fpga_sdio1_clk } ]
create_clock -name fpga_sdio2_clk      -period $SDIO_CLK_PERIOD_HS [ get_ports { fpga_sdio2_clk } ]
create_clock -name iMX6_sdio1_clk_virt -period $SDIO_CLK_PERIOD_HS
create_clock -name iMX6_sdio2_clk_virt -period $SDIO_CLK_PERIOD_HS

# FPGA: EIM clock and virtual EIM clock (9.6ns = 104 MHz, 18ns = 52 MHz)
set EIM_CLK_PERIOD 18

create_clock -name fpga_eim_bclk -period $EIM_CLK_PERIOD [ get_ports { fpga_eim_bclk } ]
create_clock -name iMX6_eim_bclk_virt -period $EIM_CLK_PERIOD

# HPS: I2C clock
create_clock -name i2c0_clock -period "400 kHz"  [ get_ports { hps_i2c0_scl } ]
create_clock -name i2c3_clock -period "400 kHz"  [ get_ports { hps_loanio21 } ]

# PLL clock definition and jitter behaviour of the PLLs
derive_pll_clocks -create_base_clocks
derive_clock_uncertainty

# Split input clock domain all other clocks
set_clock_groups -asynchronous -group [ get_clocks { fpga_mclk_25mhz } ]

# Split clock domains between 66 MHZ clock and EIM and SDIO clocks
set_clock_groups -asynchronous -group [ get_clocks $clk_66MHz ] -group [ get_clocks { fpga_eim_bclk  } ] 
set_clock_groups -asynchronous -group [ get_clocks $clk_66MHz ] -group [ get_clocks { fpga_sdio1_clk } ] 
set_clock_groups -asynchronous -group [ get_clocks $clk_66MHz ] -group [ get_clocks { fpga_sdio2_clk } ]

# Split JTAG clock from all other clocks
set_clock_groups -asynchronous -group [ get_clocks { altera_reserved_tck } ]

# EIM interface
# Parameters according document IMX6SDLCEC Rev. 7, 10/2016, section 4.9.3.2 
# General EIM Timing - Synchronous mode, Synchronous access, BCD=0 (52 MHz and 104 MHz)
#
set eim_tcomin -1.25
set eim_tcomax  1.75
set eim_th      2.0
set eim_tsu     2.0

set_input_delay  -clock iMX6_eim_bclk_virt -min  $eim_tcomin -clock_fall [ get_ports { fpga_eim_cs* fpga_eim_lba fpga_eim_oe fpga_eim_rw fpga_eim_ad* fpga_eim_eb* } ]
set_input_delay  -clock iMX6_eim_bclk_virt -max  $eim_tcomax -clock_fall [ get_ports { fpga_eim_cs* fpga_eim_lba fpga_eim_oe fpga_eim_rw fpga_eim_ad* fpga_eim_eb* } ]
set_output_delay -clock iMX6_eim_bclk_virt -min -$eim_th                 [ get_ports { fpga_eim_wait fpga_eim_ad* } ]
set_output_delay -clock iMX6_eim_bclk_virt -max  $eim_tsu                [ get_ports { fpga_eim_wait fpga_eim_ad* } ]

# SDIO interfaces
# Parameters according document IMX6SDLIEC Rev. 7, 10/2016
# section 4.11.4.1 SDIO High Speed AC Timing
# section 4.11.4.3 SDR50 AC Timing
#
set sdio_tcomin_SDR50  -3.0
set sdio_tcomax_SDR50   1.0
set sdio_tcomin_HS     -6.6
set sdio_tcomax_HS      3.6
set sdio_th             1.5
set sdio_tsu            2.5

set_input_delay  -clock iMX6_sdio1_clk_virt -min  $sdio_tcomin_HS -clock_fall [ get_ports { fpga_sdio1_cmd fpga_sdio1_d* } ]
set_input_delay  -clock iMX6_sdio1_clk_virt -max  $sdio_tcomax_HS -clock_fall [ get_ports { fpga_sdio1_cmd fpga_sdio1_d* } ]
set_output_delay -clock iMX6_sdio1_clk_virt -min -$sdio_th                    [ get_ports { fpga_sdio1_cmd fpga_sdio1_d* } ]
set_output_delay -clock iMX6_sdio1_clk_virt -max  $sdio_tsu                   [ get_ports { fpga_sdio1_cmd fpga_sdio1_d* } ]

set_input_delay  -clock iMX6_sdio2_clk_virt -min  $sdio_tcomin_HS -clock_fall [ get_ports { fpga_sdio2_cmd fpga_sdio2_d* } ]
set_input_delay  -clock iMX6_sdio2_clk_virt -max  $sdio_tcomax_HS -clock_fall [ get_ports { fpga_sdio2_cmd fpga_sdio2_d* } ]
set_output_delay -clock iMX6_sdio2_clk_virt -min -$sdio_th                    [ get_ports { fpga_sdio2_cmd fpga_sdio2_d* } ]
set_output_delay -clock iMX6_sdio2_clk_virt -max  $sdio_tsu                   [ get_ports { fpga_sdio2_cmd fpga_sdio2_d* } ]

# Quick hack for direct IO
set_false_path -from [ get_ports { fpga_fpctrl* } ] -to *
set_false_path -to   [ get_ports { fpga_fpctrl* } ] -from *

# HPS: emac0
#
set_false_path -from [ get_ports { hps_emac0_rx_ctl } ] -to *
set_false_path -from [ get_ports { hps_emac0_rx_clk } ] -to *
set_false_path -from [ get_ports { hps_emac0_rxd*   } ] -to *
set_false_path -from [ get_ports { hps_emac0_mdio   } ] -to *

set_false_path -to [ get_ports { hps_emac0_tx_ctl } ] -from *
set_false_path -to [ get_ports { hps_emac0_tx_clk } ] -from *
set_false_path -to [ get_ports { hps_emac0_txd*   } ] -from *
set_false_path -to [ get_ports { hps_emac0_mdc    } ] -from *
set_false_path -to [ get_ports { hps_emac0_mdio   } ] -from *

# HPS: qspi
#
set_false_path -from [ get_ports { hps_qspi_io* } ] -to *

set_false_path -to [ get_ports { hps_qspi_clk } ] -from *
set_false_path -to [ get_ports { hps_qspi_ss0 } ] -from *
set_false_path -to [ get_ports { hps_qspi_io* } ] -from *

# HPS: trace
#
set_false_path -to [ get_ports { hps_trace_clk } ] -from *
set_false_path -to [ get_ports { hps_trace_d*  } ] -from *

# HPS: sdio
#
set_false_path -from [ get_ports { hps_sdio_cmd } ] -to *
set_false_path -from [ get_ports { hps_sdio_d*  } ] -to *

set_false_path -to [get_ports { hps_sdio_clk } ] -from *
set_false_path -to [get_ports { hps_sdio_cmd } ] -from *
set_false_path -to [get_ports { hps_sdio_d*  } ] -from *

# HPS: i2c0
#
set_false_path -from [ get_ports { hps_i2c0_sda } ] -to *
set_false_path -from [ get_ports { hps_i2c0_scl } ] -to *

set_false_path -to [ get_ports { hps_i2c0_sda } ] -from *
set_false_path -to [ get_ports { hps_i2c0_scl } ] -from *

# HPS: i2c3
#
set_false_path -from [ get_ports { hps_loanio20 } ] -to *
set_false_path -from [ get_ports { hps_loanio21 } ] -to *

set_false_path -to [ get_ports { hps_loanio20 } ] -from *
set_false_path -to [ get_ports { hps_loanio21 } ] -from *

# HPS: uart0
#
set_false_path -from [ get_ports { hps_uart0_rx } ] -to *
set_false_path -to [ get_ports { hps_uart0_tx } ] -from *

# USB BlasterII to be reliable, 25MHz
#
# create_clock -name { altera_reserved_tck } -period 40 { altera_reserved_tck }
set_input_delay  -clock altera_reserved_tck -clock_fall 3 [ get_ports altera_reserved_tdi ]
set_input_delay  -clock altera_reserved_tck -clock_fall 3 [ get_ports altera_reserved_tms ]
set_output_delay -clock altera_reserved_tck 3 [ get_ports altera_reserved_tdo ]

# APPLICATION SPECIFIC constraints
#

# HPS: gpio

#set_false_path -from [ get_ports { hps_gpio37 } ] -to *
#set_false_path -to   [ get_ports { hps_gpio37 } ] -from *

#set_false_path -from [ get_ports { hps_gpio40 } ] -to *
#set_false_path -to   [ get_ports { hps_gpio40 } ] -from *

#set_false_path -from [ get_ports { hps_gpio41 } ] -to *
#set_false_path -to   [ get_ports { hps_gpio41 } ] -from *

#set_false_path -from [ get_ports { hps_gpio42 } ] -to *
#set_false_path -to   [ get_ports { hps_gpio42 } ] -from *

#set_false_path -from [ get_ports { hps_gpio43 } ] -to *
#set_false_path -to   [ get_ports { hps_gpio43 } ] -from *

#set_false_path -from [ get_ports { hps_gpio44 } ] -to *
#set_false_path -to   [ get_ports { hps_gpio44 } ] -from *

# HPS: loan IO

#set_false_path -from [ get_ports { hps_loanio14 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio14 } ] -from *

#set_false_path -from [ get_ports { hps_loanio15 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio15 } ] -from *

#set_false_path -from [ get_ports { hps_loanio16 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio16 } ] -from *

#set_false_path -from [ get_ports { hps_loanio17 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio17 } ] -from *

#set_false_path -from [ get_ports { hps_loanio18 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio18 } ] -from *

#set_false_path -from [ get_ports { hps_loanio19 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio19 } ] -from *

#set_false_path -from [ get_ports { hps_loanio22 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio22 } ] -from *

#set_false_path -from [ get_ports { hps_loanio23 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio23 } ] -from *

#set_false_path -from [ get_ports { hps_loanio24 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio24 } ] -from *

#set_false_path -from [ get_ports { hps_loanio25 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio25 } ] -from *

#set_false_path -from [ get_ports { hps_loanio26 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio26 } ] -from *

#set_false_path -from [ get_ports { hps_loanio27 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio27 } ] -from *

#set_false_path -from [ get_ports { hps_loanio35 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio35 } ] -from *

#set_false_path -from [ get_ports { hps_loanio57 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio57 } ] -from *

#set_false_path -from [ get_ports { hps_loanio58 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio58 } ] -from *

#set_false_path -from [ get_ports { hps_loanio59 } ] -to *
#set_false_path -to   [ get_ports { hps_loanio59 } ] -from *

# FPGA: IO

set_false_path -from [ get_ports { fpga_clk_diffio_rx_b15p } ] -to *
set_false_path -to   [ get_ports { fpga_clk_diffio_rx_b15p } ] -from *

set_false_path -from [ get_ports { fpga_clk_diffio_rx_b23p } ] -to *
set_false_path -to   [ get_ports { fpga_clk_diffio_rx_b23p } ] -from *

set_false_path -from [ get_ports { fpga_clk_diffio_rx_t1p } ] -to *
set_false_path -to   [ get_ports { fpga_clk_diffio_rx_t1p } ] -from *

set_false_path -from [ get_ports { fpga_clk_diffio_rx_t9p } ] -to *
set_false_path -to   [ get_ports { fpga_clk_diffio_rx_t9p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b10n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b10n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b10p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b10p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b11n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b11n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b11p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b11p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b14n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b14n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b14p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b14p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b15n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b15n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b18n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b18n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b18p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b18p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b19n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b19n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b19p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b19p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b1n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b1n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b1p }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b1p }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b22n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b22n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b22p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b22p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b23n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b23n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b26n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b26n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b26p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b26p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b27n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b27n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b27p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b27p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b30n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b30n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b30p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b30p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b31n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b31n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b34n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b34n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b34p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b34p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b35n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b35n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b35p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b35p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b3n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b3n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b3p }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b3p }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b5n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b5n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b5p }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b5p }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b7n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b7n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_b7p }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_b7p }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_r4n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_r4n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_r4p }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_r4p }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_r6n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_r6n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_r6p }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_r6p }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_r8n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_r8n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_r8p }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_r8p }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_t1n }  ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_t1n }  ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_t21n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_t21n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_t21p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_t21p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_t23n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_t23n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_t23p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_t23p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_rx_t9n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_rx_t9n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b12n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b12n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b12p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b12p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b13n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b13n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b13p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b13p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b16n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b16n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b16p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b16p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b17n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b17n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b17p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b17p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b20n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b20n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b20p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b20p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b21n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b21n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b21p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b21p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b24n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b24n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b24p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b24p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b25n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b25n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b25p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b25p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b28n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b28n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b28p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b28p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b29n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b29n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b29p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b29p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b2n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b2n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b2p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b2p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b32n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b32n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b32p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b32p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b33p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b33p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b36n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b36n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b36p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b36p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b4n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b4n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b4p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b4p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b6n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b6n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b6p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b6p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b8n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b8n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b8p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b8p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b9n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b9n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_b9p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_b9p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_r1n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_r1n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_r1p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_r1p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_r3n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_r3n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_r3p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_r3p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_r5n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_r5n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_r7n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_r7n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_r7p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_r7p  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_t22n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_t22n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_t22p } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_t22p } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_t24n } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_t24n } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_t4n  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_t4n  } ] -from *

set_false_path -from [ get_ports { fpga_diffio_tx_t4p  } ] -to *
set_false_path -to   [ get_ports { fpga_diffio_tx_t4p  } ] -from *
