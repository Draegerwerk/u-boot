#
# (C) Copyright 2021 Draeger and Licensors, info@draeger.com
#
# SPDX-License-Identifier:        GPL-2.0+
#

ifdef CONFIG_SPL_BUILD
CFLAGS_imx6m48_spl.o := $(if $(BUILD_TAG),-DBUILD_TAG='"$(BUILD_TAG)"')
obj-y += imx6m48_spl.o imx6m48dl_spl_cfg.o imx6m48q_spl_cfg.o imx6m48qp_spl_cfg.o imx6valenka_spl_cfg.o
else
obj-y += imx6_dp.o
obj-$(CONFIG_HW_DIAG) += hwdiag/
endif

CFLAGS_imx6m48.o := $(if $(BUILD_TAG),-DBUILD_TAG='"$(BUILD_TAG)"')
obj-y += imx6m48.o
