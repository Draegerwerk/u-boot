#
# (C) Copyright 2015 Draeger and Licensors, info@draeger.com
#
# SPDX-License-Identifier:        GPL-2.0+
#

include $(OBJTREE)/include/autoconf.mk

LIB	= libpostsocfpga_m48.o

COBJS-$(CONFIG_HAS_POST)	+= watchdog.o board_reset.o m48_board_specifics.o  memory.o

include $(TOPDIR)/post/rules.mk
