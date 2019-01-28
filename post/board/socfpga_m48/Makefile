#
# (C) Copyright 2015 Draegerwerk AG
#

include $(OBJTREE)/include/autoconf.mk

LIB	= libpostsocfpga_m48.o

COBJS-$(CONFIG_HAS_POST)	+= watchdog.o board_reset.o m48_board_specifics.o  memory.o

include $(TOPDIR)/post/rules.mk
